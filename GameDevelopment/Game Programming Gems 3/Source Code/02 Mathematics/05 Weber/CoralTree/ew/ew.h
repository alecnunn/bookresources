/******************************************************************************

Coral Tree ew/ew.h
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



/* Event Window system */


#ifndef __EW_H__
#define __EW_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#if defined __unix || defined unix
#ifndef UNIX
	#define UNIX
#endif

	#include <unistd.h>
/* 	#include <time.h> */
	#include <sys/time.h>
#endif

#ifdef __sgi
	#define SGI_SPROC
#endif

#include <osd/osd.h>

#if !defined EW_WL
	#if OSD_OSTYPE==OSD_UNIX
	#define EW_WL	EW_XWIN
	#endif

	#if OSD_OSTYPE==OSD_WIN32
	#define EW_WL	EW_WIN32
	#endif
#endif

#if !defined EW_GL
	#if EW_WL==EW_XWIN
	#define EW_GL	EW_X_GFX
	#endif

	#if EW_WL==EW_WIN32
	#define EW_GL	EW_GDI
	#endif
#endif

/* EW_WL = which windowing/event library is being used */
#define EW_XWIN		1
#define EW_WIN32	2
#define EW_MEX		3

/* EW_GL = which graphics library is being used */
#define EW_X_GFX	1
#define EW_OPENGL	2
#define EW_IRISGL	3
#define EW_GDI		4

#ifndef TRUE
	#define FALSE	0
	#define TRUE	1
#endif

// if TRUE, act in a release grade manner
// may quiet some mild warnings
#ifndef EW_RELEASE_GRADE
	#define EW_RELEASE_GRADE	FALSE
#endif

#define EW_NUDE_WARN			(!EW_RELEASE_GRADE)

#ifndef EW_TRACK_NEWED
	#define EW_TRACK_NEWED		TRUE
#endif

/* don't use IrisGL Font Manager if not using IrisGL */
#if EW_GL!=EW_IRISGL
	#define FONTMANAGER FALSE
#endif

/* don't use IrisGL Font Manager unless FONTMANAGER defined TRUE */
#ifndef FONTMANAGER
	#define FONTMANAGER FALSE
#endif


#if EW_WL==EW_WIN32
	#include <windows.h>

	extern HINSTANCE EW_HINSTANCE;
	extern int EW_NCMDSHOW;
	extern LRESULT CALLBACK ewWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#endif


#if EW_WL==EW_XWIN || EW_GL==EW_X_GFX
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xatom.h>
	#include <X11/keysym.h>
	#include <X11/cursorfont.h>
#endif

#if EW_GL==EW_OPENGL
	#if EW_WL==EW_WIN32
		#include <gl/gl.h>
		#include <gl/glu.h>
	#else
		#include <GL/glx.h>
		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif
#endif

#if EW_GL==EW_IRISGL
	#include <gl.h>
	#include <device.h>

	#if EW_WL==EW_XWIN
		#include <gl/glws.h>
	#endif
#endif

#if FONTMANAGER
	#include<fmclient.h>
#endif

#if !(EW_WL==EW_XWIN && EW_GL==EW_IRISGL)
	#define GLXconfig void
#endif


class EW_EventContext;
class EW_Event;
class EW_ClipRegion;
class EW_WindowNode;
class EW_WindowList;
class EW_Node;
class EW_Widget;
class EW_WidgetList;
class EW_Window;
class EW_ColorMapping;

//#include <osd/osd.h>
#include <gpl/gpl.h>
#include <wba/wba.h>

#include <ew/ew_proto.h>

#include <ew/ew_error.h>
#include <ew/util.h>
#include <ew/region.h>
#include <ew/tablet.h>
#include <ew/redraw.h>
#include <ew/macro.h>
#include <ew/gfx.h>
#include <ew/estring.h>
#include <ew/text.h>
#include <ew/resize.h>
#include <ew/node.h>
#include <ew/widget.h>
#include <ew/cursor.h>
#include <ew/event.h>
#include <ew/window.h>
#include <ew/eventcxt.h>
#include <ew/wba_ew.h>



#endif /* __EW_H__ */
