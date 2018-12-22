
/******************************************************************************

Coral Tree ew/eventcxt.cc
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



#include "ew.h"



#define WDS_QUERY_TEST	1
#define WDS_QUERY_GRAIN	100



/******************************************************************************
		EW_EventContext::EW_EventContext(void)

******************************************************************************/
EW_EventContext::EW_EventContext(void)
	{
	cursors_initialized=FALSE;
	cursorindex=EW_CURSOR_NORMAL;
	focus_win = NULL;
	last_focus_win = NULL;
	exit_loop=FALSE;
	SetWorkFunction(NULL);
	SetEventRedirection(NULL);
	SetReEntrant(FALSE);
	SetScreenMouse(-1,-1);
	maxevents=0;
	clipboard_requestor=NULL;

//	SetEnvironment(set_envp);
//	SetArguments(set_argc,set_argv);

#if EW_WL==EW_WIN32

	WNDCLASS wc;
	BOOL return_val;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = ewWindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(EW_Window *);
	wc.hInstance = EW_HINSTANCE;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = NULL;
	wc.hbrBackground = (struct HBRUSH__ *)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "EW_WClass";

	return_val = RegisterClass(&wc);
	if(!return_val)
		EW_PRINT(EW_EVENT,EW_ERROR,"could not register class");

#endif

	ewRegisterExitCallback(ewExitFunction);

	AddWindow(&phantom);
	phantom.SetFlags(EW_WINDOW_PHANTOM);
	phantom.Open("Phantom");

	InitCursors();
	}



/******************************************************************************
		EW_EventContext::~EW_EventContext(void)

******************************************************************************/
EW_EventContext::~EW_EventContext(void)
	{
	ClearTimerIntervals(NULL);
	modalstack.Clear();

	RemoveWindow(&phantom);

	EW_Window *node;

	// remove all non-newed pointers that will otherwise be deleted elsewhere, presumably
	GetWindowList()->ToHead();
	while((node = GetWindowList()->PostIncrement())!=NULL)
		{
		RemoveWindow(node);

		if(!node->GetNewed())
			{
#if EW_NUDE_WARN
			EW_PRINT(EW_APP,EW_WARN,"~EW_EventContext() 0x%x not auto-deleting non-newed window 0x%x (%s%s%s)",
														this,node,ewColorCode(3),node->GetTitle(),ewColorCode(0));
#endif
			}
		else
			delete node;
		}
	}



/******************************************************************************
long	EW_EventContext::AppendTimerInterval(long identifier,long interval,
															EW_Window *target)

******************************************************************************/
long EW_EventContext::AppendTimerInterval(long identifier,long interval,EW_Window *target)
	{
	EW_TimerInterval *tint;

	if(!target)
		{
		EW_PRINT(EW_APP,EW_ERROR,"EW_EventContext::AppendTimerInterval() given null target window");
		return 0;
		}

	if(identifier<2)
		identifier=2;

	// make sure it's unique
	long repeat=TRUE;
	while(repeat)
		{
		repeat=FALSE;

		timerlist.ToHead();
		while( (tint=(EW_TimerInterval *)timerlist.PostIncrement()) != NULL )
			{
			if(tint->GetIdentifier()==identifier)
				{
//				EW_PRINT(EW_EVENT,EW_WARN,"EW_EventContext::AppendTimerInterval() identifier collision");

				identifier++;
				repeat=TRUE;
				break;
				}
			}
		}

#if EW_WL==EW_WIN32
	identifier=SetTimer(target->GetHWND(),identifier,interval,(TIMERPROC)NULL);
#endif

	tint=new EW_TimerInterval;

	tint->SetTargetWindow(target);
	tint->SetIdentifier(identifier);
	tint->SetInterval(interval);

	timerlist.AppendNode(tint);

	return identifier;
	}



/******************************************************************************
long	EW_EventContext::RemoveTimerInterval(long identifier)

******************************************************************************/
long EW_EventContext::RemoveTimerInterval(long identifier)
	{
	EW_TimerInterval *tint;
	long found=FALSE;

	// clear ALL instances of given identifier
	// (even though they should be unique)
	timerlist.ToHead();
	while( (tint=(EW_TimerInterval *)timerlist.PostIncrement()) != NULL )
		{
		if(tint->GetIdentifier()==identifier)
			{
			found=TRUE;

#if EW_WL==EW_WIN32
			KillTimer(tint->GetTargetWindow()->GetHWND(),identifier);
#endif

			timerlist.RemoveNode(tint);
			delete tint;
			}
		}

	return found;
	}



/******************************************************************************
void	EW_EventContext::ClearTimerIntervals(EW_Window *target)

******************************************************************************/
void EW_EventContext::ClearTimerIntervals(EW_Window *target)
	{
	EW_TimerInterval *tint;

	timerlist.ToHead();
	while( (tint=(EW_TimerInterval *)timerlist.Current()) != NULL )
		{
		// if target==NULL, remove ALL intervals
		if(target==NULL || tint->GetTargetWindow()==target)
			RemoveTimerInterval(tint->GetIdentifier());
		else
			timerlist.PostIncrement();
		}
	}



/******************************************************************************
EW_Window	*EW_EventContext::GetEventRedirection(void)

******************************************************************************/
EW_Window *EW_EventContext::GetEventRedirection(void)
	{
	if(redirection && !winlist.SetCurrent(redirection))
		redirection=NULL;

	return redirection;
	}



/******************************************************************************
void	EW_EventContext::DeleteDelayedWindows(void)

******************************************************************************/
void EW_EventContext::DeleteDelayedWindows(void)
	{
	EW_Window *window;

	winlist.ToHead();
	while((window=winlist.PostIncrement())!=NULL)
		{
		if(window->GetDelayedDelete())
			{
			// prevent re-entrant loops
			window->SetDelayedDelete(FALSE);

			// window should auto-remove on its destruction
			// RemoveWindow(window);

#if !EW_RELEASE_GRADE
			EW_PRINT(EW_EVENT,EW_LOG,"EW_EventContext::DeleteDelayedWindows() found one -> deleting");
#endif

			delete window;
			}
		}
	}



/******************************************************************************
long	EW_EventContext::AppendWindowToModalStack(EW_Window *window,long exclusive)

	if already on stack, moved to top

	returns non-zero if was already on stack

******************************************************************************/
long EW_EventContext::AppendWindowToModalStack(EW_Window *window,long exclusive)
	{
	long result=FALSE;

	window->SetModal(exclusive? EW_MODAL_EXCLUSIVE: EW_MODAL_NONEXCLUSIVE);

	if(modalstack.SetCurrent(window))
		{
		RemoveWindowFromModalStack(window);
		result=TRUE;
		}

	modalstack.AppendNode(window);
	window->Modalize();
	window->Raise(TRUE);
	return result;
	}



/******************************************************************************
long	EW_EventContext::RemoveWindowFromModalStack(EW_Window *window)

	returns non-zero if wasn't on the stack

******************************************************************************/
long EW_EventContext::RemoveWindowFromModalStack(EW_Window *window)
	{
	window->SetModal(EW_MODAL_NOT);

	if(!modalstack.SetCurrent(window))
		return TRUE;

	modalstack.RemoveNode(window);
	window->DeModalize();

	EW_Window *other=GetTopMappedModalWindow();
	if(other)
		other->Modalize();

	RestackModalWindows();

	return FALSE;
	}



/******************************************************************************
EW_Window	*EW_EventContext::GetTopMappedModalWindow(void)

	returns tailmost mapped window, potentially NULL

******************************************************************************/
EW_Window *EW_EventContext::GetTopMappedModalWindow(void)
	{
	EW_Window *node;

	modalstack.ToTail();
	while((node = modalstack.PostDecrement())!=NULL)
		{
		if(node->GetMapped())
			return node;
		}

	return NULL;
	}



/******************************************************************************
void	EW_EventContext::RestackModalWindows(void)

******************************************************************************/
void EW_EventContext::RestackModalWindows(void)
	{
	long reset=TRUE;

#if FALSE && EW_WL==EW_XWIN

	reset=FALSE;

	EW_Window *node;
	Display *display=NULL;
	int screen;

	node=GetTopMappedModalWindow();
	if(node)
		{
		display=node->GetDisplay();
		screen=node->GetScreen();
		}
	else
		return;

	Window thiswin=node->GetXWindow(),rootwin,parentwin,*childwin;
	unsigned int uchildren;
	long children;

	// parent to grandparent
	long pass;
	for(pass=0;pass<3;pass++)
		{
		if(!XQueryTree(display,thiswin,&rootwin,&parentwin,&childwin,&uchildren))
			{
			EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::RestackModalWindows() XQueryTree() failed");
			return;
			}

		XFree(childwin);

		thiswin=parentwin;
		}

	QueryTreeRecursively(parentwin,&childwin,&children);

	long m,max= -1;

	// find max of mapped non-modals
	winlist.ToHead();
	while((node = winlist.PostIncrement())!=NULL)
		{
		if(node->GetMapped() && !node->GetModal())
			{
#if WDS_QUERY_TEST
			printf("non-modal (%s)\n",node->GetTitle());
#endif
			for(m=0;m<children;m++)
				{
#if WDS_QUERY_TEST
				printf("non-modal %d  0x%x:0x%x\n",m,node->GetXWindow(),childwin[m]);
#endif
				if(node->GetXWindow()==childwin[m])
					{
#if WDS_QUERY_TEST
					printf("MATCH XXXXXXXXXXXXXXXXX\n");
#endif
					if(max<m)
						max=m;

					break;
					}
				}
			}
		}

#if WDS_QUERY_TEST
#endif
	printf("1 max=%d/%d\n",max,children);

	// make sure rest of modals follow in order
	modalstack.ToHead();
	while((node = modalstack.PostIncrement())!=NULL)
		{
		if(node->GetMapped())
			{
#if WDS_QUERY_TEST
			printf("modal (%s)\n",node->GetTitle());
#endif
			for(m=0;m<children;m++)
				{
#if WDS_QUERY_TEST
				printf("modal %d  0x%x:0x%x\n",m,node->GetXWindow(),childwin[m]);
#endif
				if(node->GetXWindow()==childwin[m])
					{
#if WDS_QUERY_TEST
					printf("MATCH XXXXXXXXXXXXXXXXX\n");
#endif

					if(max<m)
						max=m;
					else
						reset=TRUE;

					break;
					}
				}
			}
		}

#if WDS_QUERY_TEST
#endif
	printf("2 max=%d/%d reset=%d\n",max,children,reset);

	osdFree(childwin);
#endif

	EW_Window *node;

#if EW_WL==EW_WIN32
	// if a top modal, enable it only
	// else, enable all

	EW_Window *topmodal=GetTopMappedModalWindow();
	long exclusive= (topmodal)? (topmodal->GetFlags()&EW_WINDOW_MODAL_EXCLUSIVE_FLAG)!=0: FALSE;

//	EW_PRINT(EW_WINDOW,EW_LOG,"restack modals topmodal=0x%x",topmodal);

	winlist.ToHead();
	while((node = winlist.PostIncrement())!=NULL)
		{
		if(!topmodal || node==topmodal || (!exclusive&&node->IsModal()) )
			{
			if(!IsWindowEnabled(node->GetHWND()))
				EnableWindow(node->GetHWND(),TRUE);
			}
		else
			{
			if(IsWindowEnabled(node->GetHWND()))
				EnableWindow(node->GetHWND(),FALSE);
				}
		}
#endif

	if(reset)
		{
#if TRUE
		// only raise top modal
		node=GetTopMappedModalWindow();
		if(node)
			node->Raise(2);
#else
		// raise all modals
		modalstack.ToHead();
		while((node = modalstack.PostIncrement())!=NULL)
			{
			if(node->GetMapped())
				node->Raise(2);
			}
#endif
		}
	}



#if EW_WL==EW_XWIN
/******************************************************************************
void	EW_EventContext::QueryTreeRecursively(Window parent,
									Window **descendenttree,long *descendents)

	not used, too slow

******************************************************************************/
void EW_EventContext::QueryTreeRecursively(Window parent,Window **descendenttree,long *descendents)
	{
	*descendenttree=NULL;
	*descendents=0;

#if WDS_QUERY_TEST
	printf("for 0x%x\n",(unsigned int)parent);
#endif

	EW_Window *node=GetTopMappedModalWindow();
	if(!node)
		return;

	Display *display=node->GetDisplay();

	//Window thiswin=node->GetXWindow();
	Window rootwin,grandparent,*childwin;
	unsigned int children;

	if(!XQueryTree(display,parent,&rootwin,&grandparent,&childwin,&children))
		{
		EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::QueryTreeRecursively() XQueryTree() failed");
		return;
		}

	if(childwin==NULL || children==0)
		{
#if WDS_QUERY_TEST
		printf("no children\n");
#endif
		return;
		}

	Window *newtree=NULL,*subtree;
	long total=0,subdescendents=0;
	long desired,allocated=0;
	long child,sub;
	for(child=0;child<(long)(children);child++)
		{
		QueryTreeRecursively(childwin[child],&subtree,&subdescendents);

#if WDS_QUERY_TEST>1
		printf("child %d 0x%x total=%d subdescendents=%d\n",child,childwin[child],total,subdescendents);
#endif

		desired=(((total+subdescendents+1)%WDS_QUERY_GRAIN)+1)*WDS_QUERY_GRAIN;
		if(allocated<desired)
			{
			newtree=(Window *)osdRealloc(newtree,desired*sizeof(Window));
			allocated=desired;
			}

		newtree[total]=childwin[child];

		for(sub=0;sub<subdescendents;sub++)
			{
			newtree[total+1+sub]=subtree[sub];

#if WDS_QUERY_TEST>1
			printf("sub %d 0x%x\n",sub,subtree[sub]);
#endif
			}

		total+=(1+subdescendents);

		if(subtree)
			osdFree(subtree);
		}

	XFree(childwin);

#if WDS_QUERY_TEST
	printf("total %d 0x%x\n",(int)total, (int)childwin);
#endif

	*descendenttree=newtree;
	*descendents=total;
	}
#endif



/******************************************************************************
long	EW_EventContext::CopyTextToClipboard(char *data)

	returns non-zero if could not set up with window system

******************************************************************************/
long EW_EventContext::CopyTextToClipboard(char *data)
	{
	GetClipboardString()->NewBuffer(data);

#if EW_WL==EW_XWIN

	XSetSelectionOwner(GetPhantom()->GetDisplay(),XA_PRIMARY,GetPhantom()->GetXWindow(),CurrentTime);
	if(XGetSelectionOwner(GetPhantom()->GetDisplay(),XA_PRIMARY)!=GetPhantom()->GetXWindow())
		{
		EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextToClipboard() could not XSetSelectionOwner()");
		return TRUE;
		}

	return FALSE;

#elif EW_WL==EW_WIN32

	long result=TRUE;

	HWND hwnd=GetPhantom()->GetHWND();

	if(!OpenClipboard(hwnd))
		{
		EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextToClipboard() OpenClipboard() failed");
		result=FALSE;
		}
	else
		{
		EW_String tempstring;

		tempstring.CopyFromString(GetClipboardString());
		tempstring.ConvertToNativeFormat();

		EmptyClipboard();

		HGLOBAL hglobal=GlobalAlloc(GMEM_DDESHARE,(tempstring.GetLength()+1)*sizeof(TCHAR));
		if(hglobal==NULL)
			{
			EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextToClipboard() GlobalAlloc() failed");
			result=FALSE;
			}
		else
			{
			char *data=(char *)GlobalLock(hglobal);

			if(!data)
				{
				EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextToClipboard() GlobalLock() failed");
				result=FALSE;
				}
			else
				{
				tempstring.CopyToBuffer(data);

				GlobalUnlock(hglobal);

				SetClipboardData(CF_TEXT,hglobal);
				}
			}

		CloseClipboard();
		}

	return result;

#endif
	}



/******************************************************************************
void	EW_EventContext::RequestTextFromClipboard(EW_Window *requestor)

	loop will send a clipboard event when recieved

******************************************************************************/
void EW_EventContext::RequestTextFromClipboard(EW_Window *requestor)
	{
	clipboard_requestor=requestor;

#if EW_WL==EW_XWIN

	Atom property=XInternAtom(GetPhantom()->GetDisplay(),"STRING",False);

	XConvertSelection(GetPhantom()->GetDisplay(),XA_PRIMARY,XA_STRING,property,GetPhantom()->GetXWindow(),CurrentTime);

#elif EW_WL==EW_WIN32

	if(!IsClipboardFormatAvailable(CF_TEXT))
		{
		EW_PRINT(EW_EVENT,EW_WARN,"EW_EventContext::CopyTextFromClipboard() wrong format");
		}
	else
		{
		HWND hwnd=GetPhantom()->GetHWND();

		if(!OpenClipboard(hwnd))
			{
			EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextFromClipboard() OpenClipboard() failed");
			}
		else
			{
			HGLOBAL hglobal=GetClipboardData(CF_TEXT);
			if(hglobal==NULL)
				{
				EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextFromClipboard() GetClipboardData() failed");
				}
			else
				{
				char *data=(char *)GlobalLock(hglobal);

				if(!data)
					{
					EW_PRINT(EW_EVENT,EW_ERROR,"EW_EventContext::CopyTextFromClipboard() GlobalLock() failed");
					}
				else
					{
					GetClipboardString()->NewBuffer(data);
					GetClipboardString()->ConvertFromNativeFormat();

					GlobalUnlock(hglobal);
					}
				}

			CloseClipboard();
			}
		}

#endif
	}



/******************************************************************************
char	*EW_EventContext::CopyTextFromClipboard(void)

	returned buffer should be osdFree()'d by caller

******************************************************************************/
char *EW_EventContext::CopyTextFromClipboard(void)
	{
	char *buffer=(char *)osdMalloc(GetClipboardString()->Length()+1);
	GetClipboardString()->CopyToBuffer(buffer);

	return buffer;
	}
