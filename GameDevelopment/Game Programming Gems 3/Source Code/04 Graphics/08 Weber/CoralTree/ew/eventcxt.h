
/******************************************************************************

Coral Tree ew/eventcxt.h
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



/******************************************************************************
class	EW_TimerInterval

Keywords:
******************************************************************************/
class EW_TimerInterval
	{
	private:
		EW_Window	*target;	// window to which event will be sent
		long		identifier;	// not-neccessarily unique
		long		interval;	// in milliseconds
		long		last_time;	// time of last event sent

	public:
				EW_TimerInterval(void)
					{
					SetTargetWindow(NULL);
					SetIdentifier(1);
					SetInterval(1000);
					SetLastEventTime(osdTimer());
					};

		void		SetTargetWindow(EW_Window *set)	{ target=set; };
		EW_Window	*GetTargetWindow(void)			{ return target; };

		long	GetIdentifier(void)		{ return identifier; };
		long	SetIdentifier(long set)
					{
					identifier= (set<1)? 1: set;
					return identifier;
					};

		long	GetInterval(void)		{ return interval; };
		long	SetInterval(long set)
					{
					interval= (set<0)? 0: set;
					return interval;
					};

		void	SetLastEventTime(long set)	{ last_time=set; };
		long	GetLastEventTime(void)		{ return last_time; };
	};



/******************************************************************************
class	EW_TimerIntervalList


Keywords:
******************************************************************************/
class EW_TimerIntervalList : public GPL_DoubleList<EW_TimerInterval>
	{
	public:
				EW_TimerIntervalList(void)	{};
virtual			~EW_TimerIntervalList(void)	{};
	};



/******************************************************************************
class   EW_WorkFunction

******************************************************************************/
class EW_WorkFunction
	{
	public:
virtual	void    CallWorkFunction(void) {};
	};



/******************************************************************************
class	EW_EventContext

Keywords:
******************************************************************************/
class EW_EventContext
	{
	private:
		EW_EventQueue			eventqueue;
		EW_TimerIntervalList	timerlist;
		EW_FontHandleList		fonthandlelist;

		EW_WorkFunction	*workfunction;	// independent from work events
		EW_WindowStack	modalstack;		// non-intrusive stack of modal windows
		EW_WindowList	winlist;		// list of controlled windows
		EW_Window		phantom;		// hidden window for internal op's
		EW_Window		*focus_win;
		EW_Window		*last_focus_win;
		EW_Window		*redirection;
		EW_Window		*clipboard_requestor;
		EW_String		clipboard_string;
		long			screenx,screeny;
		long			cursorindex;
		long			cursors_initialized;
		long			exit_loop;			// exit event loop
		long			reentrant;
		long			maxevents;

#if EW_WL==EW_XWIN
		void			QueryTreeRecursively(Window parent,
									Window **descendenttree,long *descendents);

		Cursor			xcursor[EW_CURSORS];
#elif EW_WL==EW_WIN32
		HCURSOR			hcursor[EW_CURSORS];
#endif

		long			InitCursors(void);
		long			ActivateCursor(long index);

	public:
						EW_EventContext(void);
virtual					~EW_EventContext(void);

		EW_EventQueue	*GetEventQueue(void)			{ return &eventqueue; };

		void			AddWindow(EW_Window *window);
		void			RemoveWindow(EW_Window *window);
		void			BeginLoop(void);
		void			EndLoop(void);
		long			IsEndingLoop(void)				{ return exit_loop; };

		/// max # of events per BeginLoop() (<1 = unlimited)
		void			SetMaxEvents(long set)			{ maxevents=set; };

		long			AppendWindowToModalStack(EW_Window *window,
														long exclusive=TRUE);
		long			RemoveWindowFromModalStack(EW_Window *window);
		EW_Window		*GetTopMappedModalWindow(void);
		void			RestackModalWindows(void);

		long			CopyTextToClipboard(char *data);
		void			RequestTextFromClipboard(EW_Window *requestor);
		char			*CopyTextFromClipboard(void);

		void			SetClipboardRequestor(EW_Window *set)
							{ clipboard_requestor=set; };
		EW_Window		*GetClipboardRequestor(void)
							{ return clipboard_requestor; };

		EW_String		*GetClipboardString(void)
							{ return &clipboard_string; };

		void			DeleteDelayedWindows(void);

		void			SetReEntrant(long set)			{ reentrant=set; };
		long			GetReEntrant(void)				{ return reentrant; };

		long			GetCursorIndex(void)			{ return cursorindex; };
		long			SetCursorIndex(long index)
							{
							cursorindex=index;
							return UpdateWindowCursors();
							};
		long			UpdateWindowCursors(void)
							{
							return ActivateCursor(cursorindex);
							};

		void	SetScreenMouse(long x,long y)	{ screenx=x; screeny=y; };
		void	GetScreenMouse(long *x,long *y)	{ *x=screenx; *y=screeny; };

		// timer event interval in milliseconds, <=0 indicates none
		// (not guaranteed accurate or precise)
		// don't alter timerlist except through
		// EW_EventContext member functions
		EW_TimerIntervalList	*GetTimerIntervalList(void)
									{
									return &timerlist;
									};

		EW_WorkFunction	*GetWorkFunction(void)		{ return workfunction; };
		void			SetWorkFunction(EW_WorkFunction *set)
							{ workfunction=set; };
		void			CallWorkFunction(void)
							{
							if(workfunction)
								workfunction->CallWorkFunction();
							};

		EW_Window	*GetPhantom(void)	{ return &phantom; };

		long		AppendTimerInterval(long identifier,long interval,
															EW_Window *target);
		long		RemoveTimerInterval(long identifier);
		void		ClearTimerIntervals(EW_Window *target);

		EW_WindowList		*GetWindowList(void)		{ return &winlist; };
		EW_FontHandleList	*GetFontHandleList(void)
								{ return &fonthandlelist; };

		void		Dump(long code,long brief);

		void        SetEventRedirection(EW_Window *target)
						{ redirection=target; };
		EW_Window   *GetEventRedirection(void);
	};
