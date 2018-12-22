/******************************************************************************

Coral Tree ew/cursor.cc
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



#define	EW_CURSOR_DEBUG		FALSE


/*  See: /usr/include/X11/cursorfont.h
XC_left_ptr				normal?
XC_right_ptr			reverse
XC_X_cursor				big X
XC_bottom_left_corner	window adjusters
XC_bottom_right_corner
XC_bottom_side
XC_left_side
XC_right_side
XC_top_left_corner
XC_top_right_corner
XC_top_side
XC_center_ptr			like IDC_UPARROW?
XC_circle				simple circle, IDC_NO without the sash
XC_clock				lame
XC_crosshair			like IDC_CROSS?
XC_tcross				also like IDC_CROSS?
XC_dot					filled circle
XC_dotbox				like IDC_ICON?
XC_double_arrow			nice, like IDC_SIZENS?
XC_fleur				like IDC_SIZEALL?
XC_pencil				crude, but ok
XC_pirate				skull & crossbones
XC_question_arrow		good `?'
XC_sb_h_double_arrow	poor WE arrow
XC_sb_v_double_arrow	poor NS arrow
XC_watch				for IDC_WAIT?
*/

#if EW_WL==EW_XWIN

static long ewXCursorIndex[EW_CURSORS]=
				{
				XC_left_ptr,
				XC_watch,
				XC_question_arrow,
				XC_xterm,
				XC_crosshair,
				XC_sb_v_double_arrow,
				XC_sb_h_double_arrow,
				XC_fleur,
				XC_center_ptr,
				XC_pirate
				};

#elif EW_WL==EW_WIN32

static LPTSTR ewWin32CursorName[EW_CURSORS]=
				{
				IDC_ARROW,
				IDC_WAIT,
				IDC_HELP,		// #if(WINVER >= 0x0400)
				IDC_IBEAM,
				IDC_CROSS,
				IDC_SIZENS,
				IDC_SIZEWE,
				IDC_SIZEALL,
				IDC_UPARROW,
				IDC_NO
				};
#endif



/******************************************************************************
long	EW_EventContext::InitCursors(void)

	may return non-zero on failure

******************************************************************************/
long EW_EventContext::InitCursors(void)
	{
#if EW_CURSOR_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_EventContext::InitCursors()");
#endif

	long failed=FALSE;
	long m;

#if EW_WL==EW_XWIN
	xcursor[0]=None;
	for(m=1;m<EW_CURSORS;m++)
		xcursor[m]=XCreateFontCursor(GetPhantom()->GetDisplay(),ewXCursorIndex[m]);

#elif EW_WL==EW_WIN32

	for(m=0;m<EW_CURSORS;m++)
		{
		hcursor[m]=LoadCursor(NULL,ewWin32CursorName[m]);
		if(hcursor[m]==NULL)
			{
			EW_PRINT(EW_WINDOW,EW_WARN,"EW_EventContext::InitCursors() LoadCursor() failed=%d",failed);
			failed=TRUE;
			}
		}

#endif

#if EW_CURSOR_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_EventContext::InitCursors() done failed=%d",failed);
#endif

//	cursors_initialized=!failed;
	cursors_initialized=TRUE;

	return failed;
	}



/******************************************************************************
long	EW_EventContext::ActivateCursor(long index)

	may return non-zero on failure

******************************************************************************/
long EW_EventContext::ActivateCursor(long index)
	{
	if(!cursors_initialized)
		return TRUE;

#if EW_CURSOR_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_EventContext::SetCursor(%d)",index);
#endif

	long failed=FALSE;
	index%=EW_CURSORS;

#if EW_WL==EW_XWIN

	EW_Window *target;

	winlist.Rewind();
   	while((target=winlist.PostIncrement())!=NULL)
		if(target->GetOpened())
			XDefineCursor(target->GetDisplay(),target->GetXWindow(),xcursor[index]);

	GetPhantom()->Flush(TRUE);

#elif EW_WL==EW_WIN32

#if FALSE
	HCURSOR previous=SetCursor(hcursor[index]);
	if(previous==NULL)
		;
#else
	if( !SetClassLong(GetPhantom()->GetHWND(),GCL_HCURSOR,(long)hcursor[index]) )
		EW_PRINT(EW_WINDOW,EW_WARN,"EW_EventContext::SetCursor() SetClassLong() failed");
#endif

#endif

#if EW_CURSOR_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_EventContext::SetCursor() done failed=%d",failed);
#endif

	return failed;
	}
