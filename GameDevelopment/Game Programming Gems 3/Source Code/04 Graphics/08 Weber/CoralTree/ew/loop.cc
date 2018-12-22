/******************************************************************************

Coral Tree ew/loop.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

/*******************************************************************************

					Event loops and Event Context

******************************************************************************/



#include "ew.h"


#define EW_EVENT_DEBUG				FALSE	// TRUE = event verbosity
#define EW_EVENT_DEBUG_IGNORED		FALSE	// also mention ignored events from system
#define EW_EVENT_NO_IDLEMOUSE		FALSE	// TRUE = block idlemouse events
#define EW_WIN32_TIMER_DEBUG		FALSE	// TRUE = Win32 Timer/Work-function/Idlemouse verbosity
#define EW_ENV_DEBUG				FALSE	// TRUE = environment verbosity

#define EW_DRAG_OUTSIDE				TRUE	// allow mouse drag out of window (and still get events under Win32)
#define EW_RELEASE_KEYS				TRUE	// TRUE = send release events on focus change
#define EW_EVENT_REDIRECTION		TRUE	// allow context to alter target of events

#define EW_REJECT_REENTRANT_EXPO	FALSE
#define EW_DIRECT_PROCESSING		FALSE	// TRUE = don't use event queue
#define	EW_MOUSE_POS_PRECOGNITION	FALSE	// querys X directly, significant speed loss (seems to get next event's position)
#define EW_KEY_REPEAT_INTERNAL		FALSE	// TRUE = do key repeating with internal timing (not through X's mechanism)
#define EW_PAINT_DIRECT				FALSE	// TRUE = redraw Win32 paint events immediately instead of processing them
#define EW_EVENT_KILLTEST			FALSE	// do EndLoop() on PAUSE key

#define EW_KEY_DELAY1				(long)(0.30 *1e3)	// initial delay
#define EW_KEY_DELAY2				(long)(0.05 *1e3)	// repeating delay

#define EW_COMPACT_MOUSE			TRUE	// TRUE = compact sequential mouse motion events
#define EW_COMPACT_DEBUG			FALSE

#define EW_TARGET_ON_FOCUS			TRUE



// Sun lacks
#ifndef XK_ISO_Left_Tab
	#define	XK_ISO_Left_Tab			0xFE20
#endif



WBA_INIT_DEFINITION(EW);


/******************************************************************************
Abstract out main() because Win32 requires the use of WinMain as the
entry point.

Also Win32 requires the HINSTANCE from WinMain in its CreateWindow call.
Therefore, the global EW_HINSTANCE is set here.  (its Win32's fault I need
a global, not mine). Same for EW_NCMDSHOW;
******************************************************************************/
#if EW_WL==EW_WIN32

extern int main(int argc,char **argv,char **envp);

void ewTranslateEnvironmentVariables(char **envp);


/* redirect ridiculous Windows psuedo-main to the real main */
HINSTANCE EW_HINSTANCE;
int EW_NCMDSHOW;
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
	{
	int argc;
	char **argv;
	char *envp[1000];

	argc = ewStringToArgv(lpCmdLine,&argv);

	EW_NCMDSHOW = nCmdShow;
	EW_HINSTANCE = hInstance;

	/* translate environment variables */
	ewTranslateEnvironmentVariables(envp);

	// hide console
	HWND console=EW_Window::GetConsoleHWND();
	if(console)
		EW_Window::ShowConsole(console,FALSE);

	return main(argc,argv,envp);
	}



/******************************************************************************
void ewTranslateEnvironmentVariables(char **envp)

	also copy
		PATH		=	Path
		USER		=	USERNAME
		HOME		=	HOMEDRIVE + HOMEPATH
		HOST		=	COMPUTERNAME
		HOSTTYPE	=	OS	( or PROCESSOR_ARCHITECTURE )

	NOTE: HOST and HOSTTYPE are apparently only in tcsh

	Note the convenient little translation table.
	The UNIX variable on the left is the concatenation of the Win32
	variables after it.

	for EW_WL==EW_WIN32 only

******************************************************************************/
void ewTranslateEnvironmentVariables(char **envp)
	{
	const long translations=5;
	char translation[translations][3][50]=
		{
		"HOST",		"COMPUTERNAME",				"",
		"HOSTTYPE",	"PROCESSOR_ARCHITECTURE",	"",
		"PATH",		"Path",						"",
		"USER",		"USERNAME",					"",
		"HOME",		"HOMEDRIVE",				"HOMEPATH",
		};

	char newentry[10000];
	char *partentry;
	char *penv;

	long entries=0;
	long m,n;

	/* get double-NULL-terminated concatenation of NULL-terminated strings */
	penv=GetEnvironmentStrings();

#if EW_ENV_DEBUG

	char message[10000];

	GPL_GLOBALOUT.SetFlags(GPL_OUT_MESSAGEBOX);

	n=0;
	while( penv[n] || penv[n+1] )
		{
		if(penv[n])
			message[n]=penv[n];
		else
			message[n]='\n';
		n++;
		}
	message[n]=0;

	EW_PRINT(EW_OS,EW_WARN,message);

#endif

	/* copy Win32 variables */
	while(*penv)
		if(penv[0]=='=')
			{
			/* skip over "=?????" entries that lack a variable name */
			penv+=strlen(penv)+1;
			}
		else
			{
			envp[entries]=new char[strlen(penv)+1];

			strcpy(envp[entries],penv);
			penv+=strlen(envp[entries])+1;
			entries++;
			}

	envp[entries]=0;

	/* translate some Win32 variables to UNIX names and append to the end of **envp */
	for(m=0;m<translations;m++)
		if( !ewGetEnvironmentVariable(envp,translation[m][0]) )
			{
			sprintf(newentry,"%s=",translation[m][0]);

			for(n=1;n<3;n++)
				if( partentry=ewGetEnvironmentVariable(envp,translation[m][n]) )
					strcat(newentry,partentry);

			envp[entries]=new char[strlen(newentry)+1];
			strcpy(envp[entries],newentry);
			entries++;
			envp[entries]=0;
			}
	}

#endif



/******************************************************************************
char	*ewGetEnvironmentVariable(char **envp,char *name)

	returns pointer to actual value-string in environment variable
		(do not alter)

	returns NULL if name not found

Keywords:
******************************************************************************/
/*PROTOTYPE*/
char *ewGetEnvironmentVariable(char **envp,char *name)
	{
	char entryname[256];

	long entries=0;

	while(envp[entries])
		{
		sscanf(envp[entries],"%[^=]",entryname);
		if( !strcmp(entryname,name) )
			return &envp[entries][strlen(entryname)+1];

		entries++;
		}

	return NULL;
	}



/******************************************************************************
long	ewDebugShowEnvironment(char **envp)

	print out environment variables
	returns number of variables

Keywords:
******************************************************************************/
/*PROTOTYPE*/
long ewDebugShowEnvironment(char **envp)
	{
	char message[1024];

	long entries=0;

	EW_PRINT(EW_OS,EW_LOG,"ewDebugShowEnvironment():");
	while(envp[entries])
		{
		sprintf(message,"  %d %s",(int)entries,envp[entries]);
		EW_PRINT(EW_OS,EW_LOG,message);

		entries++;
		}

	return entries;
	}



#if EW_WL==EW_XWIN
/******************************************************************************
void	ewSetXKeyboardState(Display *display,long restore)

	clear or restore key repeating

	if display=NULL, use display given on previous call

Keywords:
******************************************************************************/
void ewSetXKeyboardState(Display *display,long restore)
	{
	static XKeyboardState	keyboard_state;
	static long				keyboard_state_set=FALSE;
	static Display			*last_display;

	XKeyboardControl		keyboard_control;

	/* store display pointer for future */
	if(display)
		last_display=display;
	else if(!last_display)
		{
#if EW_EVENT_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"ewSetXKeyboardState() no last display stored");
#endif

		return;
		}

#if FALSE
	// by individual keys (but it still effects all of X, not just this app)
	long m;

	XGetKeyboardControl(last_display,&keyboard_state);
	for(m=0;m<32;m++)
		printf("%d %d\n",m,keyboard_state.auto_repeats[m]);

	keyboard_control.auto_repeat_mode=AutoRepeatModeOn;
	for(m=0;m<256;m++)
		{
printf("repeat on %d\n",m);
		keyboard_control.key=m;
		XChangeKeyboardControl(last_display,KBKey|KBAutoRepeatMode,&keyboard_control);
		}

	return;
#endif

	if(restore)
		{
#if EW_EVENT_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"ewSetXKeyboardState() restoring repeat state");
#endif

		/* restore auto-repeat state */
		if(keyboard_state_set)
			{
#if EW_EVENT_DEBUG
	printf("repeat state was set to %d\n",keyboard_state.global_auto_repeat);
#endif

			keyboard_control.auto_repeat_mode=keyboard_state.global_auto_repeat;
			XChangeKeyboardControl(last_display,KBAutoRepeatMode,&keyboard_control);
			}
		}
	else
		{
#if EW_EVENT_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"ewSetXKeyboardState() storing repeat state");
#endif

		/* get keyboard state */
		XGetKeyboardControl(last_display,&keyboard_state);
		keyboard_state_set=TRUE;

		/* turn off auto-repeat */
		XAutoRepeatOff(last_display);
		}
	}
#endif



/******************************************************************************
void	ewExit(long value)

	official way to shut down an EW program

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void ewExit(long value)
	{
#if EW_EVENT_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"ewExit() called to terminate EW");
#endif

	ewExitFunction();

	exit(value);
	}



/******************************************************************************
void	ewExitFunction()

	post-exit() function registered by atexit()

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void ewExitFunction()
	{
	static long down=FALSE;

	if(down)
		{
#if EW_EVENT_DEBUG
		EW_PRINT(EW_EVENT,EW_WARN,"ewExitFunction() already called");
#endif
		return;
		}
	else
		down=TRUE;

#if EW_EVENT_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"ewExitFunction()");
#endif


#if EW_WL==EW_XWIN

	// restore keyboard repeating if it was on

#if EW_KEY_REPEAT_INTERNAL

	#if EW_EVENT_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"restoring key repeat state");
	#endif

	ewSetXKeyboardState(NULL,TRUE);
#endif

#endif
	}



/******************************************************************************
void	ewRegisterExitCallback( void (*function)(void) )

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void ewRegisterExitCallback( void (*function)(void) )
	{
/* OSDEP */

#if defined __sgi || ( OSD_OS == OSD_SUNOS && OSD_OSVER == 5 ) || OSD_OS==OSD_LINUX

	if( atexit(function) )
		EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::EW_EventContext(): SGI atexit() could not register exit function");

#elif OSD_OS == OSD_SUNOS && OSD_OSVER == 4

	if( on_exit((void (*)(int ...))function,NULL) )
		EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::EW_EventContext(): Sun on_exit() could not register exit function");

#elif EW_WL!=EW_WIN32

	EW_PRINT(EW_EVENT,EW_WARN,"EW_EventContext::EW_EventContext(): do not know how to register exit function on this platform");

#endif
	}



/******************************************************************************
void	EW_EventContext::AddWindow(EW_Window *window)

Keywords:
******************************************************************************/
void EW_EventContext::AddWindow(EW_Window *window)
	{
	DB("EW_EventContext::AddWindow(EW_Window *window)");

	winlist.AppendNode(window);
	window->SetEventContext(this);

	// set for windows, including this new one
	if(window!=GetPhantom())
		ActivateCursor(cursorindex);
	}



/******************************************************************************
void	EW_EventContext::RemoveWindow(EW_Window *window)

Keywords:
******************************************************************************/
void EW_EventContext::RemoveWindow(EW_Window *window)
	{
	// remove any timers
	ClearTimerIntervals(window);

	winlist.RemoveNode(window);
	window->SetEventContext(NULL);
	}



/******************************************************************************
void	EW_EventContext::Dump(long code,long brief)

Keywords:
******************************************************************************/
void EW_EventContext::Dump(long code,long brief)
	{
	char message[256];

	sprintf(message,"Event Context 0x%x dump %d windows",(int)this,(int)(GetWindowList()->GetNumberNodes()));
	EW_PRINT(EW_WINDOW,EW_LOG,message);

	EW_Window *node;

	GetWindowList()->ToHead();
	while((node=GetWindowList()->PostIncrement())!=NULL)
		node->Dump(brief);
	}



/******************************************************************************
void	EW_EventContext::EndLoop()

Keywords:
******************************************************************************/
void EW_EventContext::EndLoop(void)
	{
	exit_loop=TRUE;
	}



#if EW_WL==EW_MEX
/******************************************************************************
*******************************************************************************
									MEX
*******************************************************************************
******************************************************************************/



/******************************************************************************
void	EW_EventContext::BeginLoop()

Keywords:
******************************************************************************/
void EW_EventContext::BeginLoop()
	{
	DB("EW_EventContext::BeginLoop()");

	EW_Window *to_refresh;
	EW_Event event;
	EW_Redraw redraw;

	short keymapping[EW_MAX_MEX_KEY];		// fixed mapping
	short devicestate[EW_MAX_MEX_DEVICE];	// set after qread()
	short keystate[EW_MAX_MEX_KEY];			// controlled by key ops

	long holding_key=0;	// key that is repeating 0=none
	long entry;
	long mx,my;
	long m;

	short data;
	short quit=FALSE;
	short allow_buttons = TRUE;

	qdevice(WINQUIT);
	qdevice(LEFTMOUSE);
	qdevice(MIDDLEMOUSE);
	qdevice(RIGHTMOUSE);
	
	qdevice(KEYBD);

	/* queue up the whole keyboard */
	for(m=BUT0;m<=MAXKBDBUT;m++)
		qdevice(m);
	for(m=XKBDOFFSET;m<=XKBDOFFSET+XKBDCOUNT;m++)
		qdevice(m);

/*
	qdevice(UPARROWKEY);
	qdevice(DOWNARROWKEY);
	qdevice(LEFTARROWKEY);
	qdevice(RIGHTARROWKEY);

	qdevice(F9KEY);
	qdevice(F10KEY);
*/

	/* create key mapping */
	for(m=0;m<EW_MAX_MEX_KEY;m++)
		{
		keymapping[m]=0;
		keystate[m]=FALSE;
		}
	for(m=0;m<EW_MAX_MEX_CROSS_KEY;m++)
		keymapping[ ewMexCrossMapping[m][0] ] = ewMexCrossMapping[m][1];

	for(m=0;m<EW_MAX_MEX_DEVICE;m++)
		devicestate[m]=FALSE;


	/* initial draw */
	winlist.Rewind();
	while((to_refresh = winlist.Next())!=NULL)
		{
		to_refresh->WinSet();

		ewMexGetWinMouse(&mx,&my);
		redraw.SetMousePosition(mx,my);
		redraw.SetForce(TRUE);

		to_refresh->Refresh(&redraw);
		}

	while(!quit)
		{
#if EW_EVENT_DEBUG
		char message[200];
#endif

		event.SetSource(EW_EVENT_NONE);
		event.SetUsed(FALSE);

		entry=qread(&data);

		if( ISBUTTON(entry) && !allow_buttons && data!= -1)
			{
			/* block button pending a focus change */
			entry=NULLDEV;
			}
		else
			devicestate[entry]=data;

		switch(entry)
			{
			case NULLDEV:
				break;
			case REDRAW:
				{

#if EW_EVENT_DEBUG
				sprintf(message,"MEX REDRAW %d",data);
				EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

				winlist.Rewind();

				while((to_refresh = winlist.Next())!=NULL)
					{

					if(to_refresh->wid == data)
						{
						to_refresh->UpdateSize();
						to_refresh->Resize();
						to_refresh->WinSet();

						ewMexGetWinMouse(&mx,&my);
						redraw.SetMousePosition(mx,my);
						redraw.SetForce(TRUE);

#if EW_EVENT_DEBUG
				sprintf(message," -> REFRESH %d",data);
				EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

						to_refresh->Refresh(&redraw);
						}
					}
				}
				break;

			case INPUTCHANGE:
				{
				short roll_over=FALSE;

				allow_buttons=FALSE;

#if EW_RELEASE_KEYS
				/* release all device buttons first */
				for(m=1;m<VALOFFSET;m++)
					if( devicestate[m] != 0 )
						{

#if EW_EVENT_DEBUG
						sprintf(message,"waiting on %d=%d",m,devicestate[m]);
						EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
						/* -1 to force release below */
						if( devicestate[m] == 1 )
							qenter(m,-1);

						/* don't repeat entry */
						devicestate[m]= -1;

						roll_over=TRUE;
						}
#endif

				if(roll_over)
					{
#if EW_EVENT_DEBUG
					sprintf(message,"MEX INPUTCHANGE %d -> rolled over",data);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					/* re-enter INPUTCHANGE after releasing buttons */
					qenter(entry,data);
					}
				else
					{
					/* do the event now */

#if EW_EVENT_DEBUG
					sprintf(message,"MEX INPUTCHANGE %d",data);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					// if data==0, focus is being lost,
					// but we don't clear focus_win until EW_EVENT_FOCUSCHANGE event is processed

					if(data)
						{
						focus_win = NULL;
						winlist.Rewind();
						while((focus_win = winlist.Next())!=NULL)
							if(focus_win->wid == data)
								break;

						if(focus_win == NULL)
							EW_PRINT(EW_EVENT,EW_ERROR,"focus on unregistered window");
						}

					event.SetSIS(EW_EVENT_FOCUS,EW_ITEM_FOCUSCHANGE, data? long( (void *)focus_win ): EW_STATE_NULL);
					event.SetMousePosition(	-1,-1);

					allow_buttons=TRUE;
					}

				holding_key=0;
				}

				break;

			case LEFTMOUSE:
				event.SetSource(	EW_EVENT_MOUSEBUTTON);
				event.SetItem(		EW_ITEM_LEFT);
				event.SetState(		data? EW_STATE_PRESS: EW_STATE_RELEASE);

				event.SetMouseButtons((event.GetMouseButtons()&6)|data);
				ewMexQueMouse(event.GetMouseButtons()!=0);
				break;

			case MIDDLEMOUSE:
				event.SetSource(	EW_EVENT_MOUSEBUTTON);
				event.SetItem(		EW_ITEM_MIDDLE);
				event.SetState(		data? EW_STATE_PRESS: EW_STATE_RELEASE);

				event.SetMouseButtons((event.GetMouseButtons()&5)|(data<<1));
				ewMexQueMouse(event.GetMouseButtons()!=0);
				break;

			case RIGHTMOUSE:
				event.SetSource(	EW_EVENT_MOUSEBUTTON);
				event.SetItem(		EW_ITEM_RIGHT);
				event.SetState(		data? EW_STATE_PRESS: EW_STATE_RELEASE);

				event.SetMouseButtons((event.GetMouseButtons()&3)|(data<<2));
				ewMexQueMouse(event.GetMouseButtons()!=0);
				break;

			/* each MOUSEX should be immediately followed by a MOUSEY */
			case MOUSEX:
				break;

			case MOUSEY:
				{
				if( !EW_COMPACT_MOUSE || qtest()!=MOUSEX )
					{
					event.SetSIS(EW_EVENT_MOUSEPOSITION,(EW_ITEM_X | EW_ITEM_Y),EW_STATE_REPEAT);
					}

				break;
				}

			/* temporary kill */
			case F1KEY:
				quit = TRUE;
				break;

			case KEYBD:
#if EW_EVENT_DEBUG
				sprintf(message,"MEX KEYBD %d '%c' (ignored)",data,data);
				EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

/*
				if(holding_key == -data)
					holding_key=data;
				else if(holding_key == data)
					{
					event.SetSource(	EW_EVENT_KEYBOARD);
					event.SetItem(		data);
					event.SetState(		EW_STATE_REPEAT);
					}
*/
				break;

			case WINQUIT:
				break;

			default:
				if(ISBUTTON(entry))
					{
					long gb=getbutton(entry);

					short matched=TRUE;

					/* can fill in whatever other keys that all the window systems support */
					switch(entry)
						{
						case UPARROWKEY:
							event.SetItem(EW_KEY_CURSOR_UP);
							break;
						case DOWNARROWKEY:
							event.SetItem(EW_KEY_CURSOR_DOWN);
							break;
						case LEFTARROWKEY:
							event.SetItem(EW_KEY_CURSOR_LEFT);
							break;
						case RIGHTARROWKEY:
							event.SetItem(EW_KEY_CURSOR_RIGHT);
							break;

						default:
							matched=FALSE;

							if(entry>=0 && entry<EW_MAX_MEX_KEY)
								{
								unsigned char ascii=keymapping[entry];

								if(ascii>0 && ascii<256)
									{
									event.SetItem(ascii);
									matched=TRUE;
									}
								}

							break;
						}

/*
					printf("data=%d gb=%d keystate[entry]=%d holding_key=%d matched=%d allow_buttons=%d\n",
																data,gb,keystate[entry],holding_key,matched,allow_buttons);
*/

					if(matched)
						{
						/* data of -1 indicates forced release upon pending loss of focus */
/*
						if( (allow_buttons && (data || holding_key || !EW_RELEASE_KEYS) &&
																data==gb && (special_key||keystate[entry]!=gb)) || data== -1 )
*/
						if( (allow_buttons && (data || holding_key || !EW_RELEASE_KEYS) && data==gb) || data== -1 )
							{
							event.SetSource(EW_EVENT_KEYBOARD);

							if(!keystate[entry])
								event.SetState(EW_STATE_PRESS);
							else
								event.SetState(data>0? EW_STATE_REPEAT: EW_STATE_RELEASE);

							if(data!= -1)
								{
								/* negative to skip first EW_STATE_REPEAT (which coincides with EW_STATE_PRESS) */
								if(gb)
									holding_key= -(event.GetItem());
								else if(abs(holding_key) == event.GetItem())
									holding_key=0;
								}
							}

						if(data== -1)
							{
							devicestate[entry]=0;
							keystate[entry]=FALSE;
							}
						else
							keystate[entry]=gb;
						}
					else
						devicestate[entry]=0;
					}

				break;
			}

		if( event.GetSource()!=EW_EVENT_NONE && focus_win!=NULL )
			{
			long mx,my;

			ewMexGetWinMouse(&mx,&my);
			event.SetMousePosition(mx,my);

#if EW_EVENT_DEBUG
			event.Print(focus_win->wid);
#endif

			focus_win->ProcessEvent(&event,TRUE);

			/* if focus was just lost, clear focus */
//			if(event.GetSource()==EW_EVENT_FOCUS && event.GetItem()==EW_ITEM_FOCUSCHANGE && event.GetState()==EW_STATE_NULL)
			if( event.IsOnly(EW_EVENT_FOCUS,EW_ITEM_FOCUSCHANGE,EW_STATE_NULL) )
				focus_win=NULL;
			}
		}
	}



/******************************************************************************
void	ewMexQueMouse(long on)

Keywords: IrisGL
******************************************************************************/
/*PROTOTYPE*/
void ewMexQueMouse(long on)
	{
	DB("ewMexQueMouse(long on)");

	if(on)
		{
		qdevice(MOUSEX);
		qdevice(MOUSEY);
		}
	else
		{
		unqdevice(MOUSEX);
		unqdevice(MOUSEY);
		}
	}



/******************************************************************************
void	ewMexGetMouse(long *x,long *y)

	sets x,y to the screen coordinates of the mouse

Keywords: IrisGL
******************************************************************************/
/*PROTOTYPE*/
void ewMexGetMouse(long *x,long *y)
	{
	*x=getvaluator(MOUSEX);
	*y=getvaluator(MOUSEY);
	}



/******************************************************************************
void	ewMexGetWinMouse(long *x,long *y)

	calls getorigin()
	calls ewMexGetMouse()

	sets x,y to window coordinates relative to lower left corner of window

Keywords: IrisGL
******************************************************************************/
/*PROTOTYPE*/
void ewMexGetWinMouse(long *x,long *y)
	{
	long wx,wy;	/* window origin */
	long mx,my;	/* mouse position in screen coordinates */

	getorigin(&wx,&wy);
	ewMexGetMouse(&mx,&my);

	*x=mx-wx;
	*y=my-wy;
	}



#elif EW_WL==EW_XWIN
/******************************************************************************
*******************************************************************************
								X Windows
*******************************************************************************
******************************************************************************/



#define MAX_MAPPED_STRING_LENGTH 10



int		(*ewOriginalXErrorHandler)(Display *,XErrorEvent *);
int		(*ewOriginalXIOErrorHandler)(Display *);
long	ewXGetMousePosition(EW_Window *target,long *mousex,long *mousey);



/******************************************************************************
int		ewXErrorHandler(Display *display,XErrorEvent *)

	for non-fatal errors

Keywords:
******************************************************************************/
int ewXErrorHandler(Display *display,XErrorEvent *error_event)
	{
	const long messagelen=256;
	//char message[messagelen];
	char buffer[messagelen],function[messagelen];
	char request_number[16];

	sprintf(request_number,"%d",error_event->request_code);
	sprintf(buffer,"UNKNOWN");
	XGetErrorDatabaseText(display,"XRequest",request_number,buffer,function,messagelen);

	EW_PRINT(EW_WINDOW,EW_ERROR,"non-fatal X error as follows, display 0x%x op %d:%d \"%s\" serial %d error %d",
			display,error_event->request_code,error_event->minor_code,function,error_event->serial,error_event->error_code);
		

	XGetErrorText(display,error_event->error_code,buffer,messagelen);
	EW_PRINT(EW_WINDOW,EW_ERROR,"%s",buffer);

#if FALSE
	EW_PRINT(EW_WINDOW,EW_LOG,"calling original X error handler");
	ewOriginalXErrorHandler(display,error_event);
#endif

	return FALSE;
	}



/******************************************************************************
int		ewXIOErrorHandler(Display *display)

	for fatal errors
	should not return

Keywords:
******************************************************************************/
int ewXIOErrorHandler(Display *display)
	{
	EW_PRINT(EW_WINDOW,EW_ERROR,"fatal IO Error on display 0x%x",display);

	ewExitFunction();

#if EW_EVENT_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"calling original X IO error handler");
#endif

	ewOriginalXIOErrorHandler(display);

	EW_PRINT(EW_WINDOW,EW_FATAL,"original X IO handler should not have returned");

	// should never get this far
	return FALSE;
	}



/******************************************************************************
long	ewXGetMousePosition(EW_Window *target,long *mousex,long *mousey)

	sets mouse position in top-down coordinates (inverted)

	returns TRUE if mouse is in window

Keywords:
******************************************************************************/
/*PROTOTYPE*/
long ewXGetMousePosition(EW_Window *target,long *mousex,long *mousey)
	{
	if(target && target->GetOpened())
		{
		Window			dummy_win,dummy_win2;
		unsigned int	dummy_uint;
		int				dummy_int,dummy_int2;
		int				mx,my;

		long result=XQueryPointer(EW_Window::GetDisplay(),target->GetXWindow(),&dummy_win,&dummy_win2,&dummy_int,&dummy_int2,
																										&mx,&my,&dummy_uint);

		if(result==True)
			{
#if FALSE
			// un-invert
			long sx,sy;
			target->GetSize(&sx,&sy);

			*mousex=mx;
			*mousey=sy-my;
#else
			*mousex=mx;
			*mousey=my;
#endif

			return TRUE;
			}
		}

	*mousex= -1;
	*mousey= -1;

	return FALSE;
	}




/******************************************************************************
void	EW_EventContext::BeginLoop()

Keywords:
******************************************************************************/
void EW_EventContext::BeginLoop()
	{
	DB("EW_EventContext::BeginLoop()");

	static EW_Event event;

	EW_Window *target=NULL;	// destination of event being created

	Display *display;
	XEvent report;
	KeySym keysym;
	XComposeStatus compose;

	const short keystates=256;
	short ascii_keystate[keystates];
	short special_keystate[keystates];
	short key_stack[1000];
	short keys_stacked=0;

	long do_work;
	long last_key_time=osdTimer();
	long last_timer_time=osdTimer();
	long key_delay;
	long mx,my,mousebuttons;
	long m;
	long last_mx=0,last_my=0;
	long motionx=0,motiony=0;
	EW_Window *motionwin=NULL;
	long force_idlemouse=TRUE;

	long repeating_key=0;

	int count;
	char buffer[MAX_MAPPED_STRING_LENGTH];

#if EW_EVENT_DEBUG
	char message[256];
#endif

	// new error handlers
	ewOriginalXIOErrorHandler=XSetIOErrorHandler(ewXIOErrorHandler);
	if(!ewOriginalXIOErrorHandler)
		EW_PRINT(EW_WINDOW,EW_WARN,"could not replace IOErrorHandler");

	ewOriginalXErrorHandler=XSetErrorHandler(ewXErrorHandler);
	if(!ewOriginalXErrorHandler)
		EW_PRINT(EW_WINDOW,EW_WARN,"could not replace ErrorHandler");

	for(m=0;m<keystates;m++)
		{
		ascii_keystate[m]=0;
		special_keystate[m]=0;
		}

	if(EW_Window::GetDisplay() == NULL)
		{
		EW_Window::SetDisplay(XOpenDisplay(NULL));
		if(EW_Window::GetDisplay() == NULL)
			EW_PRINT(EW_WINDOW,EW_FATAL,"cannot connect to X server");

		EW_Window::SetScreen(DefaultScreen(EW_Window::GetDisplay()));
		}

	display = EW_Window::GetDisplay();

	// initialize for Purify
	report.xkey.x=0;
	report.xkey.y=0;

	// Give the event loop at least one attempt.
	exit_loop=FALSE;

	bool allow_delays=(maxevents<1);

	long eventcount=0;
	while(!exit_loop && (maxevents<1 || eventcount<maxevents) )
		{
		eventcount++;

		long ix,iy,sx,sy;

		if(GetWorkFunction())
			do_work=TRUE;
		else
			{
			do_work=FALSE;
			winlist.ToHead();
			while((target=winlist.PostIncrement())!=NULL)
				if(target->GetDoWork())
					{
					do_work=TRUE;
					break;
					}
			}

		target=focus_win;

		// reset event, preserving mouse data (probably shouldn't store them here)
		event.GetMousePosition(&mx,&my);
		mousebuttons=event.GetMouseButtons();
		event.Reset();
		event.SetMousePosition(mx,my);
		event.SetMouseButtons(mousebuttons);

		if(keys_stacked)
			{
#if EW_EVENT_DEBUG
			EW_PRINT(EW_EVENT,EW_LOG,"KEY STACK");
#endif
			target=last_focus_win;

			event.SetSIS(EW_EVENT_KEYBOARD,key_stack[--keys_stacked],EW_STATE_RELEASE);
			}
		else if(XPending(display))
			{
			long press=FALSE;

			XNextEvent(display,&report);

			winlist.Rewind();
			while((target=winlist.Next())!=NULL)
				{
				if(target->GetXWindow() == report.xany.window)
					{
					if(!target->GetOpened())
						target=NULL;

					break;
					}
				}

//			printf("new event on 0x%x (%s)\n",target,target? target->GetTitle(): "null");

			char keys[32];
			switch (report.type)
				{
				case MapNotify:
					// auto-mapping and unmapping can happen when iconizing
#if EW_EVENT_DEBUG
					sprintf(message,"X Map 0x%x",report.xexpose.window);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					if(target)
						target->SetMapped(TRUE);
					break;

				case UnmapNotify:
#if EW_EVENT_DEBUG
					sprintf(message,"X Unmap 0x%x",report.xexpose.window);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					if(target)
						target->SetMapped(FALSE);
					break;

				case Expose:
					{
#if EW_EVENT_DEBUG
					sprintf(message,"X Expose 0x%x",report.xexpose.window);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
					if(target)
						{
						//* force viewport/clip reset
						EW_Window::SetSetWindow(NULL);

						// event will delete
						EW_ClipRegion *exposure=new EW_ClipRegion;

						long winsx,winsy;
						target->GetSize(&winsx,&winsy);

						long ex,ey,esx,esy,ecount;
						// this looks ahead and uses future redraws
						// alternatively, maybe we should just skip this one and leave others for later
						do
							{
							esx=report.xexpose.width;
							esy=report.xexpose.height;
							ex=report.xexpose.x;
							ey=EW_INVERT_Y(report.xexpose.y+esy-1,winsy);
							ecount=report.xexpose.count;

							exposure->AddRectangle(ex,ey,esx,esy);
#if EW_EVENT_DEBUG
							sprintf(message,"  rect %d,%d %d,%d count=%d winsize=%d,%d",ex,ey,esx,esy,ecount,winsx,winsy);
							EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

							} while(XCheckTypedWindowEvent(display,target->GetXWindow(),Expose,&report));

//* TODO check if stenciling available
#if EW_GL==EW_OPENGL
						// under OpenGL without stenciling, only can use single-rectangular clipping "scissor"
						// so have to redraw a potentially larger area
						// switch sub rectangles for one that is a total bounds
						exposure->GetBounds(&ex,&ey,&esx,&esy);
						exposure->DeleteRectangles();
						exposure->AddRectangle(ex,ey,esx,esy);
#endif

//						XQueryPointer(display,target->GetXWindow(),&dummy_win,&dummy_win2,&dummy_int,&dummy_int2,
//									&qx,&qy,&dummy_uint);

						event.SetClipRegion(exposure);

/*
						if(target->GetIgnoreNextExposure())
							{
							target->SetIgnoreNextExposure(FALSE);
#if EW_EVENT_DEBUG
							EW_PRINT(EW_EVENT,EW_LOG,"ignoring exposure on target 0x%x (%s)",target,target->GetTitle());
#endif
							}
						else
*/
							{
#if FALSE
							ewXGetMousePosition(target,&qx,&qy);
							redraw.SetMousePosition(qx,qy);

							redraw.SetForce(TRUE);

							target->WinSet();
							target->Refresh(&redraw);
#else
							event.SetMousePosition(-1,-1);
//							target->SetDirty(EW_DIRTY_COMPLETE);
							event.SetSIS(EW_EVENT_SYSTEM,EW_ITEM_EXPOSURE,(long)target);
#endif
							}

						RestackModalWindows();
						}
					break;
					}

				case DestroyNotify:
#if EW_EVENT_DEBUG
					sprintf(message,"X Destroy 0x%x",report.xdestroywindow.window);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
					break;

				case ConfigureNotify:
#if EW_EVENT_DEBUG
					sprintf(message,"X ConfigureNotify 0x%x (%s)",
							report.xconfigure.window,target? target->GetTitle(): NULL);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
					if(target && target->GetMapped())
						{
						target->ConfirmSize();
						target->UpdateSize();
						target->Resize();

						if(target!=GetTopMappedModalWindow())
							RestackModalWindows();
						}
					break;

				case EnterNotify:
#if EW_EVENT_DEBUG
					sprintf(message,"X EnterNotify 0x%x",report.xcrossing.window);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					focus_win = target;

					if(!focus_win)
					{
						//EW_PRINT(EW_EVENT,EW_WARN,"EnterNotify for unrecognized window");
						break;
					};

					event.SetSIS(EW_EVENT_FOCUS,EW_ITEM_FOCUSCHANGE,(long)target);
					event.SetMousePosition(	-1,-1);

#if EW_KEY_REPEAT_INTERNAL
					ewSetXKeyboardState(display,FALSE);
#endif

#if FALSE
					if(target->Flags() & EW_BORDERLESS)
					{
						if(target->Mapped())
						{
							XSetInputFocus(display, target->GetXWindow(), RevertToParent, CurrentTime);
						}
					}
#endif
					break;

				case LeaveNotify:
					{
#if EW_EVENT_DEBUG
					sprintf(message,"X LeaveNotify 0x%x",report.xcrossing.window);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					if( event.GetMouseButtons() != 0x0 )
						{
						/* skip extra LeaveNotify when mouse buttons being pressed */
						break;
						}
					short put_back=FALSE;

#if EW_RELEASE_KEYS
					long special;
					short *pointer;

					for(special=0;special<2;special++)
						for(m=0;m<keystates;m++)
							{
							pointer= special? &special_keystate[m]: &ascii_keystate[m];

							if(*pointer)
								{
								*pointer=0;
								key_stack[keys_stacked++]= special? 0xFF00|m: m;
								put_back=FALSE;
								}
							}
#endif

					/* if key releases were stacked up, put LeaveNotify back on X queue and process later */
					if(put_back)
						{
						XPutBackEvent(display,&report);
						}
					else
						{
						event.SetSIS(EW_EVENT_FOCUS,EW_ITEM_FOCUSCHANGE,EW_STATE_NULL);
						event.SetMousePosition(	-1,-1);
						}

					repeating_key=0;

#if EW_KEY_REPEAT_INTERNAL
					ewSetXKeyboardState(display,TRUE);
#endif

					last_focus_win=focus_win;
					focus_win=NULL;
					motionwin=NULL;
					motionx= -1;
					motiony= -1;
					}

					break;

				case ButtonPress:
					press=TRUE;
				case ButtonRelease:
					event.SetSource(	EW_EVENT_MOUSEBUTTON);
					event.SetState(		press? EW_STATE_PRESS: EW_STATE_RELEASE);

					event.SetMousePosition(report.xbutton.x,report.xbutton.y);

#if EW_TARGET_ON_FOCUS
					focus_win=target;
#endif

					if(focus_win==NULL)
						{
						EW_PRINT(EW_EVENT,EW_ERROR,"button without window focus");
						}

					switch(report.xbutton.button)
						{
						case Button1:
							event.SetItem(EW_ITEM_LEFT);
							event.SetMouseButtons((event.GetMouseButtons()&6) | (press));
							break;

						case Button2:
							event.SetItem(EW_ITEM_MIDDLE);
							event.SetMouseButtons((event.GetMouseButtons()&5) | (press<<1));
							break;

						case Button3:
							event.SetItem(EW_ITEM_RIGHT);
							event.SetMouseButtons((event.GetMouseButtons()&3) | (press<<2));
							break;

						case Button4:
							event.SetItem(EW_ITEM_WHEEL);
							event.SetState(120);
							break;

						case Button5:
							event.SetItem(EW_ITEM_WHEEL);
							event.SetState(-120);
							break;

						default:
							EW_PRINT(EW_EVENT,EW_ERROR,"release without button");
							break;
						}
					break;

				case MotionNotify:
					{
#if EW_COMPACT_MOUSE
					/* this way may take beyond another type of event (BAD) */
					// while(XCheckMaskEvent(display,ButtonMotionMask, &report));

					/* this way checks the queue, peeks at the next event, and uses it instead if it is another motion */
					XEvent peek_event;
					long pending;
					while(( pending=XPending(display) && XPeekEvent(display,&peek_event) && peek_event.type==MotionNotify ))
						{
#if EW_COMPACT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"COMPACT");
#endif
						XNextEvent(display,&report);
						}
#if EW_COMPACT_DEBUG
					if(pending)
						EW_PRINT(EW_EVENT,EW_LOG,"OTHER EVENT PENDING");
					else
						EW_PRINT(EW_EVENT,EW_LOG,"QUEUE EMPTY");
#endif

#endif

					motionwin=target;
					motionx=report.xmotion.x;
					motiony=report.xmotion.y;

					event.SetMousePosition(motionx,motiony);

#if EW_TARGET_ON_FOCUS
					focus_win=target;
#endif

					if(focus_win == NULL)
						EW_PRINT(EW_EVENT,EW_WARN,"motion without window focus");

					// redetermine state of mouse buttons
					unsigned int button_state=report.xmotion.state;
					mousebuttons=0;

					mousebuttons|= (button_state&Button1Mask)? 1: 0;
					mousebuttons|= (button_state&Button2Mask)? 2: 0;
					mousebuttons|= (button_state&Button3Mask)? 4: 0;

					event.SetMouseButtons(mousebuttons);

					if(event.GetMouseButtons())
						{
						event.SetSIS(EW_EVENT_MOUSEPOSITION,(EW_ITEM_X | EW_ITEM_Y),EW_STATE_REPEAT);
#if EW_EVENT_DEBUG && !EW_EVENT_NO_IDLEMOUSE
						printf("motion with mouse buttons\n");
#endif
						}
					else
						{
#if EW_EVENT_DEBUG && !EW_EVENT_NO_IDLEMOUSE
						printf("motion without mouse buttons\n");
#endif
						}

					break;
					}

				case KeyPress:
				case KeyRelease:
					{

					long keycode=report.xkey.keycode;
					long modifier=report.xkey.state;

					XQueryKeymap(display,keys);

					long keydown;
					keydown = (keys[keycode/8] & 1<<(keycode%8) );

#if EW_TARGET_ON_FOCUS
					focus_win=target;
#endif

					if(target==NULL)
						EW_PRINT(EW_EVENT,EW_ERROR,"key without window focus");

#if FALSE
					for(m=32;m>=0;m--)
						printf("%2x",keys[m]);

					printf(" %x %d",keycode,keycode);
					printf(" %s\n", keydown? "ON": "OFF");
#endif
					// dispose of releases from X key repeats
					if(report.type==KeyRelease && keydown)
						{
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"%sdispose%s KeyRelease && keydown",ewColorCode(3),ewColorCode(0));
#endif
						break;
						}

					// dispose of false presses from X key repeats
					if(report.type==KeyPress && !keydown)
						{
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"%sNOT dispose%s KeyPress && !keydown",ewColorCode(3),ewColorCode(0));
#endif

						// This can happen legally, don't dispose
//						break;
						}

					// dispose of extra presses after leaving window
					if(report.type==KeyPress && !focus_win)
						{
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"%sdispose%s KeyPress && !focus_win",ewColorCode(3),ewColorCode(0));
#endif
						break;
						}

					event.SetSource(	EW_EVENT_KEYBOARD);
					event.SetState(		(report.type==KeyPress)? EW_STATE_PRESS: EW_STATE_RELEASE);

					event.SetMousePosition(report.xkey.x,report.xkey.y);

					count = XLookupString(&(report.xkey), buffer, MAX_MAPPED_STRING_LENGTH, &keysym, &compose);

#if EW_EVENT_DEBUG
					sprintf(message,"X %s 0x%x keycode=0x%x state=0x%x keydown=0x%x count=%d keysym=0x%x",
							(report.type==KeyPress)? "KeyPress": "KeyRelease",
							report.xcrossing.window,keycode,
							report.xkey.state,keydown,count,keysym);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					if(keysym >= XK_F1 && keysym <= XK_F10)
						{
						// can this questionable mapping be assumed?
						event.SetItem(	EW_KEY_F(keysym-XK_F1 + 1) );
						}
					else if( count>0 && keysym>=XK_space && keysym<=XK_asciitilde)
						{
						// consolidated ascii
						event.SetItem(buffer[0]);
						}
					else
						{
						switch(keysym)
							{
							case XK_Left:
								event.SetItem(EW_KEY_CURSOR_LEFT);
								break;

							case XK_Up:
								event.SetItem(EW_KEY_CURSOR_UP);
								break;

							case XK_Right:
								event.SetItem(EW_KEY_CURSOR_RIGHT);
								break;

							case XK_Down:
								event.SetItem(EW_KEY_CURSOR_DOWN);
								break;

							case XK_Home:
								event.SetItem(EW_KEY_HOME);
								break;

							case XK_End:
								event.SetItem(EW_KEY_END);
								break;

							case XK_Insert:
								event.SetItem(EW_KEY_INSERT);
								break;

							case XK_Tab:
								event.SetItem(EW_KEY_TAB);
								break;

							case XK_ISO_Left_Tab:
								event.SetItem(EW_KEY_SHIFT|EW_KEY_TAB);
								break;

							case XK_Pause:
								event.SetItem(EW_KEY_PAUSE);
								break;

							case XK_Page_Up:
								event.SetItem(EW_KEY_PAGE_UP);
								break;

							case XK_Page_Down:
								event.SetItem(EW_KEY_PAGE_DOWN);
								break;

							default:
								if( count>0 && (keysym >= XK_BackSpace) && (keysym <= XK_Delete) )
									{
									// scattered ascii
									event.SetItem(buffer[0]);
									}
								else
									event.SetSource(EW_EVENT_NONE);

								break;
							}
						}

					// convert CR into LF
					if(event.GetItem()==13)
						event.SetItem(EW_KEY_RETURN);

					// shift, caplock, and control modifiers
					long item=event.GetItem();

					if(modifier&ShiftMask)
						item|=EW_KEY_SHIFT;
					if(modifier&LockMask)
						item|=EW_KEY_CAPS_LOCK;
					if(modifier&ControlMask)
						item|=EW_KEY_CONTROL;

					event.SetItem(item);

					event.MapControlCharacters();

					if(report.type==KeyPress)
						{
						if(keysym<256)
							ascii_keystate[keysym]=TRUE;
						else
							special_keystate[keysym&0xFF]=TRUE;

						// convert repeated KeyPress's from X into EW_STATE_REPEAT events
						if(repeating_key==event.GetItem())
							event.SetState(EW_STATE_REPEAT);

						repeating_key=event.GetItem();

						key_delay=EW_KEY_DELAY1;
						last_key_time=osdTimer();
						}
					else
						{
						long old_state;

						if(keysym<256)
							{
							old_state=ascii_keystate[keysym];
							ascii_keystate[keysym]=FALSE;
							}
						else
							{
							old_state=special_keystate[keysym&0xFF];
							special_keystate[keysym&0xFF]=FALSE;
							}

//						if(!repeating_key && EW_RELEASE_KEYS)
						if(!old_state && EW_RELEASE_KEYS)
							event.SetSource(EW_EVENT_NONE);

						repeating_key=0;
						}

					break;
					}

				case SelectionClear:
					// lost clipboard ownership
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"SelectionClear");
#endif

					break;

				case SelectionNotify:
					// clipboard data has been received
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"SelectionNotify");
#endif

					if(report.xselection.property == None)
						{
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"no result");
#endif
						}
					else if( (target=clipboard_requestor) != NULL)
						{
						event.SetSIS(EW_EVENT_CLIPBOARD,EW_ITEM_CLIPBOARD_READY,EW_STATE_NULL);

						Window win=GetPhantom()->GetXWindow();
						Display *display=GetPhantom()->GetDisplay();

						char *property=XGetAtomName(display,report.xselection.property);
#if EW_EVENT_DEBUG
						char *type=XGetAtomName(display,report.xselection.target);
						EW_PRINT(EW_EVENT,EW_LOG,"  property (%s) type (%s)",property,type);
#endif

						if(strcmp(property,"STRING"))
							event.SetItem(EW_ITEM_CLIPBOARD_FAIL);
						else
							{
							XTextProperty textproperty;
							XGetTextProperty(display,win,&textproperty,report.xselection.property);

							XFree(property);

							// does atom property need to be deleted?

							char *textbuffer=(char *)osdMalloc(textproperty.nitems+1);

							strncpy(textbuffer,(char *)textproperty.value,textproperty.nitems);
							textbuffer[textproperty.nitems]=0;

#if EW_EVENT_DEBUG
							EW_PRINT(EW_EVENT,EW_LOG,"  value (%s)",textbuffer);
#endif

							GetClipboardString()->NewBuffer((char *)textproperty.value);

							osdFree(textbuffer);
							}

						clipboard_requestor=NULL;
						}
					break;

				case SelectionRequest:
					// clipboard data should be sent
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"SelectionRequest");
#endif

					{
					Window win=report.xselectionrequest.requestor;
					Display *display=report.xselectionrequest.display;

					long ok=FALSE;

					if(report.xselectionrequest.property!=None)
						{
						char *property=XGetAtomName(display,report.xselectionrequest.target);
#if EW_EVENT_DEBUG
						char *type=XGetAtomName(display,report.xselectionrequest.property);
						EW_PRINT(EW_EVENT,EW_LOG,"  property (%s) type (%s)",property,type);
#endif

						ok=(!strcmp(property,"STRING"));
						}

					char *pointer=GetClipboardString()->Buffer();
					XTextProperty textproperty;

					if(ok)
						{
						if(!XStringListToTextProperty(&pointer,1,&textproperty))
							EW_PRINT(EW_EVENT,EW_ERROR,"XStringListToTextProperty() failed");

						XSetTextProperty(display,win,&textproperty,report.xselectionrequest.property);
						}

					// send event
					XEvent xevent;

					xevent.type=						SelectionNotify;
//					xevent.xselection.serial=			report.xselectionrequest.serial;
//					xevent.xselection.send_event=		True;
//					xevent.xselection.display=			report.xselectionrequest.display;
					xevent.xselection.selection=		report.xselectionrequest.selection;
					xevent.xselection.target=			report.xselectionrequest.target;
					xevent.xselection.property=			ok? report.xselectionrequest.property: None;
//					xevent.xselection.time=				CurrentTime;

#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG," Send: ok=%d (%s) display=0x%x win=0x%x/0x%x",
																	ok,pointer,display,win,report.xselectionrequest.owner);
#endif

					XSendEvent(display,win,True,0,&xevent);
					}
					break;

				case ClientMessage:
#if EW_EVENT_DEBUG
					sprintf(message,"ClientMessage format %d on 0x%x (%s)",report.xclient.format,target,
							target? target->GetTitle(): "NULL");
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					if(target)
						{
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,
											"target->GetDeleteAtom()=%d target->GetKillAtom()=%d report.xclient.message_type=%d",
											target->GetDeleteAtom(),target->GetKillAtom(),report.xclient.message_type);

						for(m=0;m<5;m++)
							EW_PRINT(EW_EVENT,EW_LOG,"report.xclient.data.l[%d]=%d 0x%x",m,
									report.xclient.data.l[m],report.xclient.data.l[m]);
#endif

						long do_delete = FALSE;
						long quit_app = FALSE;
						switch(report.xclient.format)
							{
							case 8:
								if((Atom)(report.xclient.data.b[0]) == target->GetDeleteAtom())
									do_delete = TRUE;
								break;
							case 16:
								if((Atom)(report.xclient.data.s[0]) == target->GetDeleteAtom())
									do_delete = TRUE;
								break;
							case 32:
								if((Atom)(report.xclient.data.l[0]) == target->GetDeleteAtom())
									do_delete = TRUE;
								if((Atom)(report.xclient.data.l[0]) == target->GetKillAtom())
									{
									do_delete = TRUE;
									quit_app = TRUE;
									}
								break;
							default:
								EW_PRINT(EW_EVENT,EW_WARN,"invalid X event format");
								break;
							}

/*
if(report.xclient.data.l[0]==0x1007B2D8)
	printf("CLOSE\n");
if(report.xclient.data.l[3]==0x1007B3A0)
	printf("QUIT\n");
*/

						if(do_delete)
							{
							if(quit_app)
								{
#if EW_EVENT_DEBUG
								EW_PRINT(EW_EVENT,EW_LOG,"EW_ITEM_QUIT_APP");
#endif
								target->DoKillCallback(quit_app);
								event.SetSIS(EW_EVENT_SYSTEM,EW_ITEM_QUIT_APP,(long)target);
								}
							else
								{
#if EW_EVENT_DEBUG
								EW_PRINT(EW_EVENT,EW_LOG,"EW_ITEM_CLOSEWINDOW");
#endif
								event.SetSIS(EW_EVENT_SYSTEM,EW_ITEM_CLOSEWINDOW,(long)target);
								}

							focus_win = target;
							}
						else
							{
							event.SetSource(EW_EVENT_NONE);
							}
						}
					break;
				default:
#if EW_EVENT_DEBUG_IGNORED
					sprintf(message,"Unrecognized X Event type 0x%x",(long)report.type);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
					break;
				}

			if(report.type==ButtonPress || report.type==ButtonRelease || report.type==MotionNotify)
				{
				long item=event.GetItem();

				long modifier= (report.type==MotionNotify)? report.xmotion.state: report.xbutton.state;

				if(modifier&ShiftMask)
					item|=EW_KEY_SHIFT;
				if(modifier&LockMask)
					item|=EW_KEY_CAPS_LOCK;
				if(modifier&ControlMask)
					item|=EW_KEY_CONTROL;

				event.SetItem(item);
				}
			}
		else
			{
			/* work function (when no X events pending) */

			long currenttimer=osdTimer();

			long key_timediff=currenttimer-last_key_time;
			long timer_timediff=currenttimer-last_timer_time;

			long sleep_me=FALSE;
			long timing=FALSE;

/*
			printf("last_key_time=%d currenttimer=%d timediff=%d/%d repeating_key=%d\n",
												last_key_time,currenttimer,timediff,key_delay,repeating_key);
*/

			/* negative check for possible wrap around */
			if(key_timediff<0)
				last_key_time=currenttimer;
			if(timer_timediff<0)
				last_timer_time=currenttimer;

			/* key repeater */
			if(EW_KEY_REPEAT_INTERNAL && repeating_key)
				{
				timing=TRUE;

				if(key_timediff>key_delay )
					{
					key_delay=EW_KEY_DELAY2;

					/* synthesize a key event */
					event.SetSource(	EW_EVENT_KEYBOARD);
					event.SetItem(		repeating_key);
					event.SetState(		EW_STATE_REPEAT);

					/* last reported position */
					event.SetMousePosition(report.xkey.x,report.xkey.y);

					last_key_time=currenttimer;
					}
				else
					sleep_me=TRUE;
				}
			else
				{
				EW_TimerIntervalList *intervallist=GetTimerIntervalList();
				EW_TimerInterval *tint;

				long timediff;

				sleep_me=TRUE;

				intervallist->ToHead();
				while( (tint=(EW_TimerInterval *)intervallist->PostIncrement()) != NULL )
					{
					timing=TRUE;

					timediff=currenttimer-tint->GetLastEventTime();

//printf("0x%x currenttimer=%d~%d timediff=%d/%d\n",(long)tint,currenttimer,tint->GetLastEventTime(),timediff,tint->GetInterval());

					if( timediff>tint->GetInterval() || timediff<0 )
						{
						/* synthesize a timer event */
						event.SetSIS(EW_EVENT_PERIODIC,EW_ITEM_TIMER,tint->GetIdentifier());

						target=tint->GetTargetWindow();

						tint->SetLastEventTime(currenttimer);
						sleep_me=FALSE;
						break;
						}
					}
				}

			if(!do_work)
				{
				if(!timing && !force_idlemouse && event.GetSource()==EW_EVENT_NONE)
					{
					// block waiting on next event
					XPeekEvent(display,&report);
					}
				else if(sleep_me && allow_delays)
					osdMilliSleep(10);
				}
			}

		// idlemouse or work event
		if(event.GetSource()==EW_EVENT_NONE)
			{
#if EW_MOUSE_POS_PRECOGNITION
			ewXGetMousePosition(target,&mx,&my);

			printf("last=%d,%d report=%d,%d get=%d,%d\n",last_mx,last_my,motionx,motiony,mx,my);
#else
			if(motionwin==target)
				{
				mx=motionx;
				my=motiony;
				}
			else
				{
				mx= -1;
				my= -1;
				}
#endif

//			printf("check mouse move %d %d,%d->%d,%d\n",force_idlemouse,last_mx,last_my,mx,my);

#if TRUE
			if( last_mx>=0 && last_my>=0 && mx>=0 && my>=0 && ( force_idlemouse || last_mx!=mx || last_my!=my ) )
				{
//				printf("mouse moved %d %d,%d->%d,%d\n",force_idlemouse,last_mx,last_my,mx,my);

				force_idlemouse=FALSE;
				last_mx=mx;
				last_my=my;

				event.SetSIS(EW_EVENT_FOCUS,EW_ITEM_IDLEMOUSE,EW_STATE_NULL);
				if(allow_delays)
					osdMilliSleep(10);
				}
			else if(do_work)
				{
				event.SetSIS(EW_EVENT_PERIODIC,EW_ITEM_WORK,EW_STATE_NULL);
				}

			if( last_mx>=0 && last_my>=0 && mx>=0 && my>=0 )
				event.SetMousePosition(mx,my);

#else
			if( last_mx>=0 && last_my>=0 && mx>=0 && my>=0 )
				{
				if( force_idlemouse || last_mx!=mx || last_my!=my )
					{
//					printf("mouse moved %d %d,%d->%d,%d\n",force_idlemouse,last_mx,last_my,mx,my);

					force_idlemouse=FALSE;
					last_mx=mx;
					last_my=my;

					event.SetSIS(EW_EVENT_FOCUS,EW_ITEM_IDLEMOUSE,EW_STATE_NULL);
					if(allow_delays)
						osdMilliSleep(10);
					}
				else if(do_work)
					{
					event.SetSIS(EW_EVENT_PERIODIC,EW_ITEM_WORK,EW_STATE_NULL);
					}

				event.SetMousePosition(mx,my);
				}
#endif
			}

#if EW_EVENT_NO_IDLEMOUSE
		if(event.IsIdleMouse())
			event.SetSource(EW_EVENT_NONE);
#endif

		// check for invalid target (work events excepted)
		if( !GetWindowList()->SetCurrent(target) && !event.IsWork() )
			{
			// if target not on event-context's list, nullify

#if FALSE && EW_EVENT_DEBUG
			EW_PRINT(EW_EVENT,EW_WARN,"Ignoring event targeted to unlisted window:");
			event.Print((long)target);
#endif
			event.SetSource(EW_EVENT_NONE);
			}

		if( event.GetSource()!=EW_EVENT_NONE )
			{
#if EW_EVENT_DEBUG
			EW_PRINT(EW_EVENT,EW_LOG,"%sPreProcess%s",ewColorCode(6),ewColorCode(0));
#endif
			if(event.IsWork())
				{
				CallWorkFunction();

#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG,"Multicasting EW_ITEM_WORK to all applicable windows");
#endif

				winlist.Rewind();
				while((target=winlist.PostIncrement())!=NULL)
					if(target->GetDoWork())
						{
						event.SetMousePosition(-1,-1);

#if EW_EVENT_DEBUG
						event.Print(target->GetXWindow());
#endif

#if EW_DIRECT_PROCESSING
						target->ProcessEvent(&event,TRUE);
#else
						event.SetDestination(target);
						eventqueue.AppendCopyOf(&event);
#endif
						}


				// is 0 ok??
				if(allow_delays)
					osdMilliSleep(0);
				}
			else if(target!=NULL)
				{
				/* invert y location to bottom up */
				target->GetGeometry(&ix,&iy,&sx,&sy);
				event.InvertMousePosition(sy);
				event.GetMousePosition(&mx,&my);

#if EW_EVENT_REDIRECTION
				if(GetEventRedirection())
					{
					EW_Window *new_target=GetEventRedirection();

					if(target!=new_target)
						{
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"Event Redirect 0x%x (%s) to 0x%x (%s)",
										(long)target,target->GetTitle(),
										(long)new_target,new_target->GetTitle()
										);
#endif

						long newx,newy;
						new_target->GetOrigin(&newx,&newy);

						mx+=ix-newx;
						my+=iy-newy;

						event.SetMousePosition(mx,my);
						target=new_target;
						}
					}
#endif

				SetScreenMouse(ix+mx,iy+my);

#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG," In window 0x%x (%s)",(long)target,target->GetTitle());
				event.Print(target->GetXWindow());
#endif

				EW_Window *modalwindow=GetTopMappedModalWindow();
				if(modalwindow && modalwindow!=target && event.IsModal() &&
													(!target->GetModal() || modalwindow->GetModal()==EW_MODAL_EXCLUSIVE))
					{
					if(event.IsModalBeep())
						{
						target->RingBell();
						RestackModalWindows();
						}
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG," Modal event BLOCKED");
#endif
					}
				else
					{
#if EW_DIRECT_PROCESSING
					target->ProcessEvent(&event,TRUE);
#else
					event.SetDestination(target);
					eventqueue.AppendCopyOf(&event);
#endif

					// cause an IDLEMOUSE after any trigger event (not necessarily next)
					if(!event.IsIdleMouse() && event.Is(EW_EVENT_IDLESOURCE,EW_ITEM_ALL,EW_STATE_ANY))
						{
						force_idlemouse=TRUE;
#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"Setting force_idlemouse for next loop");
#endif
						}
					}
				}

#if !EW_DIRECT_PROCESSING
			eventqueue.ProcessAllEvents();
#endif

			if(event.IsCloseWindow())
				target->DoKillCallback(FALSE);
			}

		DeleteDelayedWindows();
		}
	}



#elif EW_WL==EW_WIN32
/******************************************************************************
*******************************************************************************
									Win32
*******************************************************************************
******************************************************************************/



/******************************************************************************
void	EW_EventContext::BeginLoop()

Keywords:
******************************************************************************/
void EW_EventContext::BeginLoop()
	{
	MSG msg;

	EW_Window *target;
	RECT rect;

	exit_loop=FALSE;

	GetWindowList()->Rewind();
	while((target=GetWindowList()->PostIncrement())!=NULL)
		{
		if(!GetClientRect(target->GetHWND(),&rect))
			EW_PRINT(EW_WINDOW,EW_ERROR,"EW_EventContext::BeginLoop() GetClientRect() failed");

		if(!InvalidateRect(target->GetHWND(),&rect,FALSE))
			EW_PRINT(EW_WINDOW,EW_ERROR,"EW_EventContext::BeginLoop() InvalidateRect() failed");

		if(!UpdateWindow(target->GetHWND()))
			EW_PRINT(EW_WINDOW,EW_ERROR,"EW_EventContext::BeginLoop() UpdateWindow() failed");

		if(!PostMessage(target->GetHWND(),WM_PAINT,(WPARAM)target->GetHDC(),NULL))
			EW_PRINT(EW_WINDOW,EW_ERROR,"EW_EventContext::BeginLoop() PostMessage() failed");
		}

	while(!exit_loop && GetMessage(&msg, (HWND) NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	}



/******************************************************************************
LRESULT CALLBACK ewWindowProcedure(HWND hwnd,UINT msg,WPARAM wparam,
																LPARAM lparam)
	The Win32 window procedure

	Note that unlike the X and MEX loops, this call does not loop.
	It is called (indirectly) from each event (message)
		in EW_EventContext::BeginLoop().

	This could be altered to handle mouse/key drags to outside the window
		more like the X loop does.

******************************************************************************/
LRESULT CALLBACK ewWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
	char message[256];

	EW_Window *window=NULL;
	short press=FALSE;

	long is_down,was_down;
	long lastmx,lastmy;
	long mx,my,mousebuttons;

	long tmp_long = GetWindowLong(hwnd,0);
	window = *(EW_Window **)&tmp_long;
//	window = (EW_Window *)GetWindowLong(hwnd,0);

	if(!window)
		return (DefWindowProc(hwnd,msg,wparam,lparam));

	// make sure Win32 doesn't try to keep sending events after loop is done (especially bad during deconstruction)
	EW_EventContext *context=window->GetEventContext();
	if(!context)
		{
		EW_PRINT(EW_WINDOW,EW_WARN,"ewWindowProcedure() window has no context");
		return 0;
		}

	// make sure Win32 doesn't try to keep sending events after loop is done (especially bad during deconstruction)
	if(context->IsEndingLoop())
		{
		EW_PRINT(EW_WINDOW,EW_WARN,"ewWindowProcedure() called after finished loop");
		return 0;
		}

	window->GetLastMouse(&lastmx,&lastmy);
//	HWND hwnd=window->GetHWND();
	HDC hdc=window->GetHDC();

/*
	if(window->GetMapped() && IsIconic(hwnd))
		window->SetMapped(FALSE);
	if(!window->GetMapped() && !IsIconic(hwnd))
		window->SetMapped(TRUE);
*/

	EW_Event localevent;
	EW_Event *event=&localevent;
	EW_Event *stored_event=window->GetStoredEvent();

	// restore mouse data from previous run
	stored_event->GetMousePosition(&mx,&my);
	mousebuttons=stored_event->GetMouseButtons();
	event->SetMousePosition(mx,my);
	event->SetMouseButtons(mousebuttons);

	if(ewGlobalOutput.GetFlags() & OSD_OUT_STDERR)
		fflush(stderr);

	if(ewGlobalOutput.GetFlags() & OSD_OUT_STDOUT)
		fflush(stdout);

	long shifted=		( GetKeyState(VK_SHIFT) & 0x8000 ) != 0;
	long controlled=	( GetKeyState(VK_CONTROL) & 0x8000 ) != 0;
	long locked=		( GetKeyState(VK_CAPITAL) & 0x1 ) != 0;

	switch(msg)
		{
//		case WM_MOUSEACTIVATE:
		case WM_ACTIVATE:
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_ACTIVATE %d to %d",tmp_long,wparam);
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			// if wparam is zero, window was deactivated
			event->SetSIS(EW_EVENT_FOCUS,EW_ITEM_FOCUSCHANGE, (wparam)? (long)window: NULL);

			event->SetMousePosition(-1,-1);
			event->SetMouseButtons(0x0);

			// if one of app's windows selected other than modal
			if(wparam)
				{
				if(context)
					{
					EW_Window *modalwindow=context->GetTopMappedModalWindow();

/*
					if(modalwindow && window)
						EW_PRINT(EW_EVENT,EW_LOG,"modalwindow=0x%x %d window=0x%x %d",modalwindow,modalwindow->GetModal(),
																								window,window->GetModal());
*/

					if(modalwindow && window!=modalwindow &&
													(!window->GetModal() || modalwindow->GetModal()==EW_MODAL_EXCLUSIVE))
						{
//						EW_PRINT(EW_EVENT,EW_LOG,"BEEP");
						window->RingBell();
						context->RestackModalWindows();
						}

					}
				}
			break;

		case WM_PAINT:
			{
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_PAINT %d",tmp_long);
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			EW_Redraw redraw;
			redraw.SetMousePosition(lastmx,lastmy);
			redraw.SetForce(TRUE);

			if(RealizePalette(hdc)==GDI_ERROR)
				EW_PRINT(EW_EVENT,EW_WARN," RealizePalette() failed");

			RECT bounds;
			if(!GetUpdateRect(hwnd,&bounds,FALSE))
				{
				EW_PRINT(EW_EVENT,EW_WARN," GetUpdateRect() no update region");
				return 0;
				}

#if TRUE
			HRGN updateregion=CreateRectRgn(0,0,1,1);
			if(updateregion==NULL)
				EW_PRINT(EW_EVENT,EW_WARN," CreateRectRgn() failed");
			else
				{
				long rx,ry,rsx,rsy;

				rx=bounds.left;
				ry=bounds.top;
				rsx=bounds.right-rx;
				rsy=bounds.bottom-ry;
//				EW_PRINT(EW_EVENT,EW_LOG," bound %d,%d %d,%d",rx,ry,rsx,rsy);

				long result;
				if( (result=GetUpdateRgn(hwnd,updateregion,FALSE)) == ERROR)
					EW_PRINT(EW_EVENT,EW_WARN," GetUpdateRgn() returned ERROR");
				else
					{
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG," GetUpdateRgn() returned %d (Null=%d Simple=%d Complex=%d)",
							result,NULLREGION,SIMPLEREGION,COMPLEXREGION);
#endif

					long length=GetRegionData(updateregion,0,NULL);

					RGNDATA *regiondata=(RGNDATA *)osdMalloc(length);

					// man page says GetRegionData() 0 on failure, 1 on success, and the number of required bytes
					// if length is too small or RGNDATA pointer is NULL
					//
					// in reality, it appears to return the number of bytes on success (never 1)
					if( (result=GetRegionData(updateregion,length,regiondata)) == 0 )
						EW_PRINT(EW_EVENT,EW_WARN," GetRegionData() failed %d, length=%d buffer=0x%x",
																						result,length,regiondata);
					else
						{
						RECT *rectangle;
						long rects=regiondata->rdh.nCount;

#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG," %d rectangle%s",rects,rects==1? "": "s");
#endif

						EW_ClipRegion *exposure=new EW_ClipRegion;

						long winsx,winsy;
						window->GetSize(&winsx,&winsy);

						long m;
						for(m=0;m<rects;m++)
							{
							rectangle=(RECT *)(&regiondata->Buffer[m*sizeof(RECT)]);
							
							rx=rectangle->left;
							rsx=rectangle->right-rx;
							rsy=rectangle->bottom-rectangle->top;
							ry=EW_INVERT_Y(rectangle->top+rsy-1,winsy);

							exposure->AddRectangle(rx,ry,rsx,rsy);

#if EW_EVENT_DEBUG
							EW_PRINT(EW_EVENT,EW_LOG," rect %d,%d %d,%d",rx,ry,rsx,rsy);
#endif
							}

						event->SetClipRegion(exposure);
						}

					osdFree(regiondata);
					}

				if(!DeleteObject(updateregion))
					EW_PRINT(EW_EVENT,EW_WARN," DeleteObject() failed");
				}
#endif

#if EW_PAINT_DIRECT
			if(BeginPaint(hwnd, &ps) != hdc)
				EW_PRINT(EW_EVENT,EW_ERROR,"BeginPaint() failed");
			else
				{
				window->Resize();
				window->Refresh(&redraw);
				window->WinSet();

				EndPaint(hwnd,&ps);
				}
			return 0;
#else
/*
			if(window->GetIgnoreNextExposure())
				{
				window->SetIgnoreNextExposure(FALSE);
#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG,"ignoring exposure on window 0x%x (%s)",window,window->GetTitle());
#endif
				}
			else
*/
				{
				event->SetMousePosition(-1,-1);
//				window->SetDirty(EW_DIRTY_COMPLETE);
				event->SetSIS(EW_EVENT_SYSTEM,EW_ITEM_EXPOSURE,(long)window);
				}

			PAINTSTRUCT ps;
			if(BeginPaint(hwnd, &ps) != hdc)
				EW_PRINT(EW_EVENT,EW_ERROR,"BeginPaint() failed");
			EndPaint(hwnd,&ps);
#endif
			}
			break;

		case WM_SIZE:
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_SIZE %d",tmp_long);
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			window->UpdateSize();
			window->Resize();
			break;

		case WM_MOVE:
			window->UpdateSize();
			window->Resize();
			break;

		case WM_LBUTTONDOWN:
			press=TRUE;
		case WM_LBUTTONUP:
			event->SetSource(	EW_EVENT_MOUSEBUTTON);
			event->SetItem(		EW_ITEM_LEFT);
			event->SetState(	press? EW_STATE_PRESS: EW_STATE_RELEASE);

			event->SetMousePosition(MAKEPOINTS(lparam).x,MAKEPOINTS(lparam).y);
			event->GetMousePosition(&lastmx,&lastmy);
			window->SetLastMouse(lastmx,lastmy);

			event->SetMouseButtons( (event->GetMouseButtons()&6)|press );
			break;

		case WM_MBUTTONDOWN:
			press=TRUE;
		case WM_MBUTTONUP:
			event->SetSource(	EW_EVENT_MOUSEBUTTON);
			event->SetItem(		EW_ITEM_MIDDLE);
			event->SetState(	press? EW_STATE_PRESS: EW_STATE_RELEASE);

			event->SetMousePosition(MAKEPOINTS(lparam).x,MAKEPOINTS(lparam).y);
			event->GetMousePosition(&lastmx,&lastmy);
			window->SetLastMouse(lastmx,lastmy);

			event->SetMouseButtons( (event->GetMouseButtons()&5)|(press<<1) );
			break;

		case WM_RBUTTONDOWN:
			press=TRUE;
		case WM_RBUTTONUP:
			event->SetSource(	EW_EVENT_MOUSEBUTTON);
			event->SetItem(		EW_ITEM_RIGHT);
			event->SetState(	press? EW_STATE_PRESS: EW_STATE_RELEASE);

			event->SetMousePosition(MAKEPOINTS(lparam).x,MAKEPOINTS(lparam).y);
			event->GetMousePosition(&lastmx,&lastmy);
			window->SetLastMouse(lastmx,lastmy);

			event->SetMouseButtons( (event->GetMouseButtons()&3)|(press<<2) );
			break;

		case WM_MOUSEMOVE:
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_MOUSEMOVE %d button=%d",tmp_long,event->GetMouseButtons());
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			event->SetMousePosition(MAKEPOINTS(lparam).x,MAKEPOINTS(lparam).y);
			event->GetMousePosition(&lastmx,&lastmy);
			window->SetLastMouse(lastmx,lastmy);

			if(event->GetMouseButtons())
				{
				event->SetSIS(EW_EVENT_MOUSEPOSITION,(EW_ITEM_X | EW_ITEM_Y),EW_STATE_REPEAT);
				}
			else
				event->SetSource(EW_EVENT_NONE);
			break;

		case WM_KEYDOWN:
			press=TRUE;
		case WM_KEYUP:
			event->SetSource(EW_EVENT_KEYBOARD);

			if(wparam>=VK_F1 && wparam<=VK_F10)
				event->SetItem(EW_KEY_F(wparam-VK_F1 + 1));
			else
				{
				switch(wparam)
					{
					case VK_LEFT:
						event->SetItem(EW_KEY_CURSOR_LEFT);
						break;

					case VK_UP:
						event->SetItem(EW_KEY_CURSOR_UP);
						break;

					case VK_RIGHT:
						event->SetItem(EW_KEY_CURSOR_RIGHT);
						break;

					case VK_DOWN:
						event->SetItem(EW_KEY_CURSOR_DOWN);
						break;

					case VK_HOME:
						event->SetItem(EW_KEY_HOME);
						break;

					case VK_END:
						event->SetItem(EW_KEY_END);
						break;

					case VK_INSERT:
						event->SetItem(EW_KEY_INSERT);
						break;

					case VK_TAB:
						event->SetItem(EW_KEY_TAB);
						break;

					case VK_PRIOR:
						event->SetItem(EW_KEY_PAGE_UP);
						break;

					case VK_NEXT:
						event->SetItem(EW_KEY_PAGE_DOWN);

					/* temporary kill */
					case VK_PAUSE:
#if EW_EVENT_KILLTEST
						if(context)
							EndLoop();

						event->SetSource(EW_EVENT_NONE);
#endif
						event->SetItem(EW_KEY_PAUSE);
						break;

					case VK_DELETE:
						event->SetItem(EW_KEY_DELETE);
						break;

					case 0x10:
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 SHIFT press/release (ignored)");
#endif
						event->SetSource(EW_EVENT_NONE);
						break;

					case 0x11:
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 CTRL press/release (ignored)");
#endif
						event->SetSource(EW_EVENT_NONE);
						break;

					case 0x14:
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 CAPS-LOCK press/release (ignored)");
#endif
						event->SetSource(EW_EVENT_NONE);
						break;

					default:
						{
						unsigned char keystate[256];
						unsigned short trans[3];

						if(!GetKeyboardState(keystate))
							EW_PRINT(EW_EVENT,EW_ERROR,"GetKeyboardState() failed");

						trans[0]=0;
						long result=ToAscii(wparam,lparam,keystate,trans,FALSE);

						if(result!=1)
							{
							EW_PRINT(EW_EVENT,EW_WARN,"ToAscii() failed: result=%d trans[0]='%c'",result,trans[0]);
							event->SetSource(EW_EVENT_NONE);
							}
						else
							event->SetItem(trans[0]&255);

#if FALSE
						if(FALSE)
//						if(press)
							{
#if EW_EVENT_DEBUG
							sprintf(message,"Win32 WM_KEY%s 0x%x,0x%x recycle",press? "DOWN": "UP",tmp_long,wparam);
							EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
							return (DefWindowProc(hwnd,msg,wparam,lparam));
							}

						event->SetItem((long)wparam);
#endif
						}
						break;
					}
				}

			event->MapControlCharacters();

#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_KEY%s 0x%x,0x%x->0x%x '%c'->",press? "DOWN": "UP",tmp_long,wparam,event->GetItem());
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			break;

		case WM_CHAR:
			if(TRUE)
				{
#if EW_EVENT_DEBUG      
				EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_CHAR event ignored");
#endif              
				event->SetSource(EW_EVENT_NONE);
				}
			else
				{
				event->SetSource(	EW_EVENT_KEYBOARD);
				event->SetItem(		(long)wparam);

				event->MapControlCharacters();

#if EW_EVENT_DEBUG
				sprintf(message,"Win32 WM_CHAR 0x%x,0x%x->0x%x (RET=0x%x)",tmp_long,wparam,event->GetItem(),EW_KEY_RETURN);
				EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
				}
			break;

		case WM_CLOSE:
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_CLOSE %d",tmp_long);
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif
			event->SetSIS(EW_EVENT_SYSTEM,EW_ITEM_CLOSEWINDOW,EW_STATE_NULL);
			window->DoKillCallback(FALSE);

			context->RestackModalWindows();
			break;

		case WM_DESTROY:
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_DESTROY %d",tmp_long);
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			break;

		case WM_ERASEBKGND:
#if EW_EVENT_DEBUG
			sprintf(message,"Win32 WM_ERASEBKGND %d",tmp_long);
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			/* DO NOT LET WIN32 CLEAR BG AUTOMATICALLY */
			break;

		case WM_TIMER:
			{
			long *id=(long *)&wparam;

#if EW_WIN32_TIMER_DEBUG
			sprintf(message,"Win32 WM_TIMER %d (hwnd %d) of %d do-work=%d",tmp_long,hwnd,*id,window->GetDoWork());
			EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

			if(*id>1)
				{
				// Programable Timer (id in wparam should always be >0)

				event->SetSIS(EW_EVENT_PERIODIC,EW_ITEM_TIMER,wparam);
				}
			else if(window->GetDoWork() || EW_EVENT_BUTTONLESS_MOTION)
				{
				// Work Function (these are high frequency timed events of id=1)
				// (used to be synthetically generated events of id=-1)

				MSG msg;

				// remove any following idling messages from queue
				while( PeekMessage(&msg,hwnd,EW_WINDOW_WIN32_IDLE_MESSAGE,EW_WINDOW_WIN32_IDLE_MESSAGE,PM_NOREMOVE) )
					{
					long *id2=(long *)&msg.wParam;

					if(*id2==1)
						{
						PeekMessage(&msg,hwnd,EW_WINDOW_WIN32_IDLE_MESSAGE,EW_WINDOW_WIN32_IDLE_MESSAGE,PM_REMOVE);

#if EW_EVENT_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"deleted excess Win32 idle timer-message");
#endif
						}
					else
						{
						sprintf(message,"ignored real timer %d %d",msg.wParam,msg.lParam);
						EW_PRINT(EW_EVENT,EW_LOG,message);

						break;
						}
					}

				// if no other messages pending (in any window of thread), do work
				if( !PeekMessage(&msg,(HWND)NULL,0,EW_WINDOW_WIN32_IDLE_MESSAGE-1,PM_NOREMOVE) &&
					!PeekMessage(&msg,(HWND)NULL,EW_WINDOW_WIN32_IDLE_MESSAGE+1,0xFFFFFFFF,PM_NOREMOVE) )
					{
					long idlemx,idlemy;

					window->GetLastMouse(&lastmx,&lastmy);
					window->GetIdleMouse(&idlemx,&idlemy);

#if EW_WIN32_TIMER_DEBUG
					sprintf(message,"check mouse move %d %d -> %d %d",idlemx,idlemy,lastmx,lastmy);
					EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

					if( (window->GetForceIdleMouse() || idlemx!=lastmx || idlemy!=lastmy) && lastmx>=0 && lastmy>=0)
						{
#if EW_EVENT_DEBUG || EW_WIN32_TIMER_DEBUG
						if(window->GetForceIdleMouse())
							EW_PRINT(EW_EVENT,EW_LOG,"force_idlemouse, last idle %d,%d last mouse %d %d",
																					idlemx,idlemy,lastmx,lastmy);
						else
							EW_PRINT(EW_EVENT,EW_LOG,"mouse moved %d,%d -> %d %d",idlemx,idlemy,lastmx,lastmy);
#endif

						window->SetForceIdleMouse(FALSE);
						window->SetIdleMouse(lastmx,lastmy);

						event->SetSIS(EW_EVENT_FOCUS,EW_ITEM_IDLEMOUSE,EW_STATE_NULL);
						}
					else if(window->GetDoWork())
						{
#if EW_WIN32_TIMER_DEBUG
						EW_PRINT(EW_EVENT,EW_LOG,"work function");
#endif

						event->SetSIS(EW_EVENT_PERIODIC,EW_ITEM_WORK,EW_STATE_NULL);
						}

					event->SetMousePosition(lastmx,lastmy);
					}
#if EW_WIN32_TIMER_DEBUG
				else
					{
					sprintf(message,"other pending in %d of %d %d %d (WM_TIMER=%d)",msg.hwnd,msg.message,
																								msg.wParam,msg.lParam,WM_TIMER);
					EW_PRINT(EW_EVENT,EW_LOG,message);
					}
#endif
				}
			}
			break;

		case WM_SHOWWINDOW:
#if EW_EVENT_DEBUG
			EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_SHOWWINDOW %d",tmp_long);
#endif

			// if OS activated a window, grab is no longer valid
#if EW_DRAG_OUTSIDE
			if(GetCapture() && hwnd!=GetCapture() && window->GetWinDrag())
				{
#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG,"Leave -> Release");
#endif
				window->ReleasePointer();
				window->SetWinDrag(FALSE);
				}
#endif
			break;

		default:
			switch(msg)
				{
				case WM_NCHITTEST:
					{
					// use very common event to watch for boundary crossing
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_NCHITTEST %d",tmp_long);
#endif

					long ix,iy,sx,sy,dx,dy;

					window->GetGeometry(&ix,&iy,&sx,&sy);
					window->GetScreenSize(&dx,&dy);

					mx=MAKEPOINTS(lparam).x-ix;
					my=EW_INVERT_Y(MAKEPOINTS(lparam).y,dy) - iy;

					// if NOT mouse with window, clear the mouse buttons
					if( !EW_WITHIN( mx,my, 0,0,sx,sy ) )
						event->SetMouseButtons(NULL);
					}
					break;

#if EW_EVENT_DEBUG_IGNORED
				case WM_SETFOCUS:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_SETFOCUS %d ignored",tmp_long);
					break;

				case WM_KILLFOCUS:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_KILLFOCUS %d ignored",tmp_long);
					break;

				case WM_QUERYOPEN:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_QUERYOPEN %d ignored",tmp_long);
					break;

				case WM_GETTEXT:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_GETTEXT %d ignored",tmp_long);
					break;

				case WM_ACTIVATEAPP:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_ACTIVATEAPP %d ignored",tmp_long);
					break;

				case WM_GETMINMAXINFO:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_GETMINMAXINFO %d ignored",tmp_long);
					break;

				case WM_NCPAINT:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_NCPAINT %d ignored",tmp_long);
					break;

				case WM_MOUSEACTIVATE:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_MOUSEACTIVATE %d ignored",tmp_long);
					break;

				case WM_WINDOWPOSCHANGING:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_WINDOWPOSCHANGING %d ignored",tmp_long);
					break;

				case WM_WINDOWPOSCHANGED:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_WINDOWPOSCHANGED %d ignored",tmp_long);
					break;

				case WM_NCCALCSIZE:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_NCCALCSIZE %d ignored",tmp_long);
					break;

				case WM_NCACTIVATE:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_NCACTIVATE %d ignored",tmp_long);
					break;

				case WM_NCLBUTTONDOWN:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_NCLBUTTONDOWN %d ignored",tmp_long);
					break;

				case WM_NCMOUSEMOVE:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_NCMOUSEMOVE %d ignored",tmp_long);
					break;

				case WM_SYSCOMMAND:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_SYSCOMMAND %d ignored",tmp_long);
					break;

				case WM_CAPTURECHANGED:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_CAPTURECHANGED %d ignored",tmp_long);
					break;

				case WM_SETCURSOR:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 WM_SETCURSOR %d ignored",tmp_long);
					break;

				default:
					EW_PRINT(EW_EVENT,EW_LOG,"Win32 unused event (message 0x%x:%d) %d",msg,msg,tmp_long);
					break;
#endif
				}

			event->SetSIS(EW_EVENT_NONE,EW_ITEM_NONE,EW_STATE_NULL);
			stored_event->CopyFrom(event);

			// don't actually need to do anything, so it is inherently ready
			if(context && context->GetClipboardRequestor())
				{
				window=context->GetClipboardRequestor();
				context->SetClipboardRequestor(NULL);

				event->SetSIS(EW_EVENT_CLIPBOARD,EW_ITEM_CLIPBOARD_READY,EW_STATE_NULL);
				}

			// pass event on (may reoccur in a new form)
			if(event->GetSource()==EW_EVENT_NONE)
				return (DefWindowProc(hwnd,msg,wparam,lparam));

			break;
		}

	// grab events where mouse was dragged out of window with some button(s) down
#if EW_DRAG_OUTSIDE
	if(event->GetSource()==EW_EVENT_MOUSEBUTTON)
		{
		if(event->GetState()==EW_STATE_PRESS)
			{
			if(!window->GetWinDrag() && !GetCapture())
				{
#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG,"Grab");
#endif
				window->GrabPointer();
				window->SetWinDrag(TRUE);
				}
			}
		else if(!event->GetMouseButtons())
			{
			if(window->GetWinDrag() && hwnd==GetCapture())
				{
#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG,"Release");
#endif
				window->ReleasePointer();
				}

			window->SetWinDrag(FALSE);
			}
		}
#endif

	if(event->GetSource()==EW_EVENT_KEYBOARD)
		{
		is_down=	( lparam & (1<<31) ) == 0;
		was_down=	( lparam & (1<<30) ) != 0;

		/* make sure non-shifted chars are not uppercase (as with WM_KEYUP) */
		if(!shifted && !locked && event->GetItem()>='A' && event->GetItem()<='Z')
			event->SetItem( event->GetItem() + 'a'-'A');

		if(is_down)
			{
			if(was_down)
				event->SetState(EW_STATE_REPEAT);
			else
				event->SetState(EW_STATE_PRESS);
			}
		else
			event->SetState(EW_STATE_RELEASE);

		event->SetMousePosition(lastmx,lastmy);

#if EW_EVENT_DEBUG
		sprintf(message,"  0x%x (%d %d->%d) shifted=%d/0x%x controlled=%d/0x%x locked=%d/0x%x",
					lparam,is_down,was_down,event->GetState(),
					shifted,GetKeyState(VK_SHIFT),
					controlled,GetKeyState(VK_CONTROL),
					locked,GetKeyState(VK_CAPITAL)
					);
		EW_PRINT(EW_EVENT,EW_LOG,message);
#endif

		}

	if(event->GetSource()==EW_EVENT_KEYBOARD ||
									event->GetSource()==EW_EVENT_MOUSEPOSITION || event->GetSource()==EW_EVENT_MOUSEBUTTON)
		{
		if(shifted)
			event->SetItem( event->GetItem() | EW_KEY_SHIFT );
		if(controlled)
			event->SetItem( event->GetItem() | EW_KEY_CONTROL );
		if(locked)
			event->SetItem( event->GetItem() | EW_KEY_CAPS_LOCK );
		}

	long iconic=IsIconic(window->GetHWND());
	if(iconic)
		window->SetWasIconic(TRUE);

	// prevent redraw on first focus change after de-iconization because an exposure will follow
	// otherwise you will get a double redraw
	if(event->IsFocusChange() && !iconic && window->GetWasIconic())
		{
#if EW_EVENT_DEBUG
		EW_PRINT(EW_EVENT,EW_LOG,"first FocusChange after iconic made not dirty in anticipation of exposure");
#endif
		window->SetDirty(0,FALSE);
		window->SetWasIconic(FALSE);
		}

	if(event->GetSource()==EW_EVENT_NONE && window->GetEventContext()->GetWorkFunction())
		{
#if EW_WIN32_TIMER_DEBUG
		EW_PRINT(EW_EVENT,EW_LOG,"external work function");
#endif

		event->SetSIS(EW_EVENT_PERIODIC,EW_ITEM_WORK,EW_STATE_NULL);
		}

	if(event->GetSource()!=EW_EVENT_NONE )
		{
		long ix,iy,sx,sy;

		if(event->IsWork())
			{
			window->GetEventContext()->CallWorkFunction();

#if EW_EVENT_DEBUG
			EW_PRINT(EW_EVENT,EW_LOG,"Multicasting EW_ITEM_WORK to all applicable windows");
#endif

			EW_Window *target;

			window->GetEventContext()->GetWindowList()->Rewind();
			while((target=window->GetEventContext()->GetWindowList()->PostIncrement())!=NULL)
				if(target->GetDoWork())
					{
					event->SetMousePosition(-1,-1);

#if EW_EVENT_DEBUG
					event->Print((long)target);
#endif

					target->ProcessEvent(event,TRUE);
					}
			}
		else if(window!=NULL)
			{
			/* invert y location to bottom up */
			window->GetGeometry(&ix,&iy,&sx,&sy);
			event->InvertMousePosition(sy);

#if EW_EVENT_REDIRECTION
			if(context && context->GetEventRedirection())
				{
				EW_Window *new_target=context->GetEventRedirection();

				if(window!=new_target)
					{
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"Event Redirect 0x%x (%s) to 0x%x (%s)",
									(long)window,window->GetTitle(),
									(long)new_target,new_target->GetTitle()
									);
#endif

					event->GetMousePosition(&mx,&my);

					long newx,newy;
					new_target->GetOrigin(&newx,&newy);

					mx+=ix-newx;
					my+=iy-newy;

					event->SetMousePosition(mx,my);
					window=new_target;
					}
				}
#endif

			stored_event->CopyFrom(event);

#if EW_EVENT_DEBUG
			EW_PRINT(EW_EVENT,EW_LOG," In window 0x%x (%s) IsIconic()=%d",(long)window,window->GetTitle(),iconic);
			event->Print(tmp_long);
#endif

			window->GetLastMouse(&lastmx,&lastmy);
			if(context)
				context->SetScreenMouse(ix+lastmx,iy+lastmy);

#if FALSE
			EW_PRINT(EW_EVENT,EW_LOG,"    BEFORE %d,%d 0x%x",lastmx,lastmy,tmp_long);
#endif
			long was_reentrant=context? context->GetReEntrant(): FALSE;

/*
			EW_PRINT(EW_EVENT,EW_LOG,"GetReEntrant()=%d",was_reentrant);
			event->Print(0);
*/

			if(context && !was_reentrant)
				context->SetReEntrant(TRUE);

#if EW_REJECT_REENTRANT_EXPO
			if(was_reentrant && event->IsExposure())
				{
#if EW_EVENT_DEBUG
				EW_PRINT(EW_EVENT,EW_LOG," Reject Re-Entrant Exposure");
#endif
				}
			else
#endif
				{
				EW_Window *modalwindow=context? context->GetTopMappedModalWindow(): NULL;
				if(modalwindow && modalwindow!=window && event->IsModal() &&
													(!window->GetModal() || modalwindow->GetModal()==EW_MODAL_EXCLUSIVE))
					{
					if(event->IsModalBeep())
						{
						window->RingBell();
						context->RestackModalWindows();
						}
#if EW_EVENT_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG," Modal event BLOCKED");
#endif
					}
				else if(!iconic)
					{
#if EW_DIRECT_PROCESSING
					window->ProcessEvent(event,TRUE);
#else
					event->SetDestination(window);
					if(context)
					context->GetEventQueue()->AppendCopyOf(event);
#endif

					// cause an IDLEMOUSE after any triggering event (not necessarily next)
					if(!event->IsIdleMouse() && event->Is(EW_EVENT_IDLESOURCE,EW_ITEM_ALL,EW_STATE_ANY))
						window->SetForceIdleMouse(TRUE);
					}
				}

			if(context && !was_reentrant)
				context->SetReEntrant(FALSE);
#if FALSE
			window->GetLastMouse(&lastmx,&lastmy);
			EW_PRINT(EW_EVENT,EW_LOG,"    AFTER %d,%d 0x%x",lastmx,lastmy,tmp_long);
#endif
			}
		}

	if(context && !context->GetReEntrant())
		{
#if !EW_DIRECT_PROCESSING
		context->SetReEntrant(TRUE);
		context->GetEventQueue()->ProcessAllEvents();
		context->SetReEntrant(FALSE);
#endif

		context->DeleteDelayedWindows();
		}

	return 0;
	}



/******************************************************************************
void	EW_Window::SetLastMouse(long mx,long my)

******************************************************************************/
void EW_Window::SetLastMouse(long mx,long my)
		{
//		EW_PRINT(EW_EVENT,EW_LOG,"EW_Window::SetLastMouse(%d,%d) from %d,%d",mx,my,last_mx,last_my);

		last_mx=mx;
		last_my=my;
		}

#else
	#error NO HOST EVENT WINDOW SYSTEM SPECIFIED
#endif
