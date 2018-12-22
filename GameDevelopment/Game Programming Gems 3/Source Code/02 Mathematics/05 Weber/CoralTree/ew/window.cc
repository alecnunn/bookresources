/******************************************************************************

Coral Tree ew/window.cc
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


#define	EW_WINDOW_DEBUG			FALSE				// TRUE = activate verbosity
#define	EW_WINSET_DEBUG			FALSE				// TRUE = activate verbosity
#define EW_PROCESS_DEBUG		FALSE				// TRUE = event processing verbosity
#define EW_RESIZE_DEBUG			FALSE				// TRUE = AffectGeometry()  verbosity
#define EW_RAISE_DEBUG			FALSE
#define EW_MODAL_DEBUG			TRUE
#define	EW_GEIGER_COUNTER		FALSE				// TRUE = flash/click on forced redraws
#define	EW_FEEDBACK				FALSE				// TRUE = activate feedback mode
#define	EW_BELLS				FALSE				// TRUE = debug sounds
#define	EW_EVENT_SUBSTRUCTURE	FALSE				// TRUE = use SubstructureNotifyMask vs. StructureNotifyMask
#define EW_OPEN_RED				FALSE				// TRUE = clear red during open
#define EW_ALWAYS_REFRESH		FALSE				// TRUE = redraw everything every time
#define EW_MAX_ON_WINSIZE		FALSE				// TRUE = window resizes child with max size of its size
#define EW_KILL_DEBUG			(!EW_RELEASE_GRADE)

#define EW_WINDOW_MINX			10
#define EW_WINDOW_MINY			10

#define EW_REFRESH_ALL			TRUE				// TRUE = allow all windows a chance to refresh after any event

#define	EW_WIN32_SHOW_NORMAL	SW_SHOWNOACTIVATE	// mapped:	SW_SHOWNORMAL or SW_SHOWNOACTIVATE
#define	EW_WIN32_SHOW_NOMAP		SW_HIDE				// unmapped

#if EW_WL==EW_XWIN

#if EW_EVENT_SUBSTRUCTURE
	#define	WDS_X_STRUCTURE		SubstructureNotifyMask
#else
	#define	WDS_X_STRUCTURE		StructureNotifyMask
#endif

#if EW_EVENT_BUTTONLESS_MOTION
	#define	WDS_X_MOTIONMASK	PointerMotionMask
#else
	#define	WDS_X_MOTIONMASK	ButtonMotionMask
#endif

#define WDS_X_EVENTMASK		ExposureMask		| \
							EnterWindowMask		| \
							LeaveWindowMask		| \
							KeyPressMask		| \
							KeyReleaseMask		| \
							ButtonPressMask		| \
							ButtonReleaseMask	| \
							WDS_X_MOTIONMASK	| \
							WDS_X_STRUCTURE

	Display			*EW_Window::display = NULL;
	int				EW_Window::screen = 0;
	Atom			EW_Window::atom[2];

	Colormap		EW_Window::shared_colormap = 0;
	Colormap		EW_Window::shared_colormap_double = 0;
	Colormap		EW_Window::shared_colormap_rgb = 0;
	Colormap		EW_Window::shared_colormap_rgb_double = 0;
	long			EW_Window::colormap_allocated = FALSE;


	#if EW_GL==EW_X_GFX
		long			EW_Window::xdbuf_allocated = FALSE;
		long			EW_Window::xdbuf_colormapped = FALSE;
		long			EW_Window::xdbuf_colormapped_colors = 0;
		unsigned long	EW_Window::xdbuf_planelist[EW_XDBUF_MAX_PLANES];
		unsigned long	EW_Window::xdbuf_pixellist[EW_XDBUF_MAX_PIXELS];
		long			EW_Window::xdbuf_currentbufferindex=0;
		XColor			EW_Window::xdbuf_mapping[2][EW_XDBUF_MAX_COLORS];

	#endif

	#define EW_X_DEPTH_SINGLE	8	// 8, maybe 4
	#define EW_X_DEPTH_DOUBLE	8	// 8, maybe 12

#endif

long		EW_Window::dx = -1;
long 		EW_Window::dy = -1;
EW_Window	*EW_Window::setwindow = NULL;
long		EW_Window::defaultRGBMode=FALSE;

#if EW_WL==EW_WIN32
extern COLORREF EwWin32Colortable[EW_MAXCOLORS];

long	EW_Window::first_map = TRUE;
#endif



/******************************************************************************
		EW_Window::EW_Window(void)

Keywords:
******************************************************************************/
EW_Window::EW_Window(void)
	{
	strcpy(titlebuffer,"no title");
	SetFlags(EW_WINDOW_FLAG_NULL);
	event_context = NULL;
//	widgetlist = new EW_WidgetList();
	double_buffer = FALSE;
	SetNumberColors(16);
	SetIgnoreNextExposure(FALSE);
	SetTemporaryFontHeight(1);
	opened=FALSE;
	mapped=FALSE;
	modal=EW_MODAL_NOT;
	delete_me=FALSE;
	underline=FALSE;
	font_adjustx=0;
	font_adjusty=0;
	rgbMode=defaultRGBMode;

	index_pen=          EW_DEFAULT_COLOR_PEN;
	index_widget=       EW_DEFAULT_COLOR_WIDGET;
	index_back=         EW_DEFAULT_COLOR_BACKGROUND;
	index_lit=          EW_DEFAULT_COLOR_LIT;
	index_halflit=      EW_DEFAULT_COLOR_HALF_LIT;
	index_shadow=       EW_DEFAULT_COLOR_SHADOW;
	index_halfshadow=   EW_DEFAULT_COLOR_HALF_SHADOW;

	kill_callback = NULL;
	ws_handler = NULL;

	clip_level=0;

	SetDoWork(FALSE);

	SetGeometry(EW_WINDOW_DEFAULT_IX,EW_WINDOW_DEFAULT_IY,EW_WINDOW_DEFAULT_SX,EW_WINDOW_DEFAULT_SY);

	// copy default font to current
//	current_font.CopyFrom(&default_font);

#if EW_WL==EW_XWIN
	win=0;
	graphics_context=0;

	#if EW_GL!=EW_OPENGL

		xdbuf_context[0]=0;
		xdbuf_context[1]=0;

	#endif
#endif

#if EW_GL==EW_OPENGL
	lastfontbase=0;
#endif

#if EW_GL==EW_IRISGL

	// run process in foreground
	foreground();

#elif EW_WL==EW_WIN32

	hwnd=NULL;
	hdc=NULL;
	dwstyle=0;
	windrag=FALSE;
	was_iconic=0;

	last_mx= -1;
	last_my= -1;

	idle_mx= -1;
	idle_my= -1;

	force_idlemouse=TRUE;
	font_set=FALSE;

	#if EW_GL==EW_OPENGL

		hglrc=NULL;

	#elif EW_GL==EW_GDI

	#endif

#endif
	}



/******************************************************************************
		EW_Window::~EW_Window(void)

******************************************************************************/
EW_Window::~EW_Window(void)
	{
	if(opened) 
		{
		Close();
		}

	EW_Widget *node;

	// remove all non-newed pointers that will otherwise be deleted elsewhere, presumably
	GetWidgetList()->ToHead();
	while((node = GetWidgetList()->PostIncrement())!=NULL)
		{
		if(!node->GetNewed())
			{
#if EW_NUDE_WARN
			EW_PRINT(EW_APP,EW_WARN,
					"EW_Window::~EW_Window() 0x%x (%s%s%s) ejecting non-newed widget 0x%x (%s%s%s) to prevent auto-deletion",
					this,ewColorCode(3),GetTitle(),ewColorCode(0),node,ewColorCode(3),node->GetName(),ewColorCode(0));
#endif

			RemoveWidget(node);
			}
		}

	if(event_context)
		{
		event_context->RemoveWindow(this);
		}

//	if(widgetlist) delete widgetlist;

#if EW_WL==EW_XWIN
//	printf("Free (%s) graphics_context=0x%x DefaultGC()=0x%x\n",GetTitle(),graphics_context,DefaultGC(display,screen));

	if(graphics_context && graphics_context!=DefaultGC(display,screen))
		XFreeGC(display,graphics_context);

	#if EW_GL!=EW_OPENGL

		if(xdbuf_context[0] && xdbuf_context[0]!=DefaultGC(display,screen))
			XFreeGC(display,xdbuf_context[0]);
		if(xdbuf_context[1] && xdbuf_context[1]!=DefaultGC(display,screen))
			XFreeGC(display,xdbuf_context[1]);

	#endif
#endif
	}



/******************************************************************************
void	EW_Window::SetOrigin(long x,long y)

******************************************************************************/
void EW_Window::SetOrigin(long x,long y)
	{
	long ix,iy;
	GetOrigin(&ix,&iy);

#if EW_RESIZE_DEBUG
	printf("SetOrigin(%d,%d)\n",x,y);
#endif

	if(x!=ix || y!=iy)
		{
		EW_Node::SetOrigin(x,y);
		AffectGeometry();
		}
	}



/******************************************************************************
void	EW_Window::SetSize(long w,long h)

Keywords:
******************************************************************************/
void EW_Window::SetSize(long w,long h)
	{
	long sx,sy;
	GetSize(&sx,&sy);

#if EW_RESIZE_DEBUG
	printf("SetSize(%d,%d) from %d,%d\n",w,h,sx,sy);
#endif

	if(w<EW_WINDOW_MINX)
		{
#if EW_RESIZE_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::SetSize() new width %d < min %d (%s)",w,EW_WINDOW_MINX,titlebuffer);
#endif
		w=EW_WINDOW_MINX;
		}

	if(h<EW_WINDOW_MINY)
		{
#if EW_RESIZE_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::SetSize() new width %d < min %d (%s)",h,EW_WINDOW_MINY,titlebuffer);
#endif
		h=EW_WINDOW_MINY;
		}

	if(w!=sx || h!=sy)
		{
		EW_Node::SetSize(w,h);
		AffectGeometry();
		}
	}



/******************************************************************************
void	EW_Window::AffectGeometry(void)

******************************************************************************/
void EW_Window::AffectGeometry(void)
	{
//	if(!GetOpened() || !GetMapped())
	if(!GetOpened())
		{
#if EW_RESIZE_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"AffectGeometry() window not opened");
#endif
		return;
		}

	long x,y,w,h;
	GetGeometry(&x,&y,&w,&h);

#if EW_RESIZE_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"AffectGeometry() %d,%d %d,%d",x,y,w,h);
#endif

#if EW_WL==EW_XWIN

	XSizeHints *hints=XAllocSizeHints();
	if(hints==NULL)
		{
		EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::AffectGeometry() XAllocSizeHints() failed");
		return;
		}

	hints->flags=PMinSize|PMaxSize;

	hints->min_width=0;
	hints->min_height=0;
	hints->max_width=dx;
	hints->max_height=dy;

	XSetWMNormalHints(GetDisplay(),GetXWindow(),hints);

	// Sun, at least, will often not work correctly without the Flushes
	Flush(TRUE);
	XMoveResizeWindow(display,win,x,EW_INVERT_Y(y+h,dy),w,h);
	Flush(TRUE);

	if(IsFixedSize())
		{
		hints->min_width=w;
		hints->min_height=h;
		hints->max_width=w;
		hints->max_height=h;
		}

	XSetWMNormalHints(GetDisplay(),GetXWindow(),hints);

	UpdateSize();
	GetGeometry(&x,&y,&w,&h);

#if EW_RESIZE_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"        to %d,%d %d,%d",x,y,w,h);
#endif

	XFree(hints);

#elif EW_WL==EW_WIN32

	if(IsIconic(hwnd))
		return;

//	Win32_CheckStyle();

	// calc bordered window to hold given "client area"
	RECT rect;
	rect.left=x;
	rect.top=dy-y-h;
	rect.right=x+w-1;
	rect.bottom=rect.top+h-1;

	// is this neccessary?
	if(GetMapped())
		ShowWindow(hwnd,EW_WIN32_SHOW_NORMAL);

	if( !AdjustWindowRect(&rect,dwstyle,FALSE) )
		EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::AffectGeometry() AdjustWindowRect() failed");

	if( !MoveWindow(hwnd,rect.left,rect.top,rect.right-rect.left+1,rect.bottom-rect.top+1,TRUE) )
		EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::AffectGeometry() MoveWindow() failed");

	// blocking test
/*
	EW_EventContext *context=GetEventContext();
	if(GetModal() && context && context->GetTopMappedModalWindow()==this)
		{
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::AffectGeometry() TOPMOST");
		SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
		}
*/

/*
	long tix,tiy,tsx,tsy;
	long ix,iy,sx,sy;

	MoveWindow(hwnd,x,dy-y-h,w,h,FALSE);
	UpdateSize();

	GetGeometry(&ix,&iy,&sx,&sy);

	tix = ix - x;
	tiy = iy - y;
	tsx = w - sx;
	tsy = h - sy;

	MoveWindow(hwnd,x-tix,dy-y-h+tiy-tsy,w+tsx,h+tsy,TRUE);
*/
#endif
	}



/******************************************************************************
long	EW_Window::IsFixedSize(void)

******************************************************************************/
long EW_Window::IsFixedSize(void)
	{
	return ( (GetFlags()&EW_WINDOW_FIXED_SIZE) || GetNodeSizing()==EW_NODE_SIZING_FIT_CHILD );
	}



#if EW_WL==EW_WIN32
/******************************************************************************
void	EW_Window::Win32_CheckStyle(void)

******************************************************************************/
void EW_Window::Win32_CheckStyle(void)
	{
	DWORD newstyle,newexstyle;

	Win32_CalcStyle(&newstyle,&newexstyle);

	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Win32_CheckStyle() 0x%x/0x%x 0x%x/0x%x",newstyle,dwstyle,newexstyle,dwexstyle);

	if(dwstyle!=newstyle)
		{
		dwstyle=newstyle;
//		SetWindowLong(hwnd,GWL_STYLE,dwstyle);
		}

	// set extended style too!
	}



/******************************************************************************
void	EW_Window::Win32_CalcStyle(DWORD *dwstyle,DWORD *dwexstyle)

******************************************************************************/
void EW_Window::Win32_CalcStyle(DWORD *dwstyle,DWORD *dwexstyle)
	{
	DWORD style=0,exstyle=0;

	if(GetFlags() & EW_WINDOW_BORDERLESS)
		{
		style = WS_POPUP;
		exstyle = WS_EX_TOOLWINDOW;		// prevent appearance on taskbar
		}
	else
		{
		style=WS_OVERLAPPED|WS_CAPTION;

		if(IsFixedSize())
			style|=WS_BORDER;
		else
			{
			style|=WS_THICKFRAME;

			if(!IsModal())
				style|=WS_MAXIMIZEBOX;
			}

		if(!IsModal())
			style|=WS_SYSMENU|WS_MINIMIZEBOX;


		if(IsModal())
			{
//			exstyle|=WS_EX_DLGMODALFRAME;

// style=WS_OVERLAPPED|WS_CAPTION;
			}
		}

/*
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Win32_CalcStyle() modal=0x%x fixed=0x%x flags=0x%x style=0x%x exstyle=0x%x",
															GetModal(),IsFixedSize(),GetFlags(),style,exstyle);
*/

	// Note: WS_OVERLAPPEDWINDOW = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX

	// WS_CAPTION -> title bar
	// WS_SYSMENU -> upper left menu
	// WS_MINIMIZEBOX -> iconize
	// WS_MAXIMIZEBOX -> enlarge to fill screen
	// WS_THICKFRAME -> sizing border
	// WS_BORDER -> non-sizable alternative to WS_THICKFRAME

	// WS_OVERLAPPED=0x0
	// WS_CAPTION = WS_BORDER | WS_DLGFRAME

	style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	*dwstyle=style;
	*dwexstyle=exstyle;
	}
#endif



/******************************************************************************
void	EW_Window::SetDoubleBuffer(long set)

	set double-buffering to TRUE/FALSE

Keywords:
******************************************************************************/
void EW_Window::SetDoubleBuffer(long set)
	{
	if(set && EW_WL==EW_WIN32 && EW_GL!=EW_OPENGL)
		{
		EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::SetDoubleBuffer() cannot activate double-buffering under Win32/GDI");
		return;
		}

	double_buffer=set;
	}



/******************************************************************************
void	EW_Window::SetNumberColors(long set)

	limit number of colors to specific values

Keywords:
******************************************************************************/
void EW_Window::SetNumberColors(long set)
	{
	if(set<8)
		number_colors=4;
	else if(set<16)
		number_colors=8;
	else
		number_colors=16;
	}



/******************************************************************************
void	EW_Window::SetOriginBy(long mode,EW_Window *relation)

	mode:
		EW_POSITION_RELATIVE_TO		using window's origin
		EW_POSITION_OVER			auto center-x 2/3-center-y

	if relation==NULL, relative to screen

	if mode==EW_POSITION_RELATIVE_TO and relation==NULL, then no-op

******************************************************************************/
void EW_Window::SetOriginBy(long mode,EW_Window *relation)
	{
	if(mode==EW_POSITION_RELATIVE_TO && relation==NULL)
		return;

	long rx=0,ry=0,rsx,rsy;
	long x,y,sx,sy;
	long dx,dy;

	GetScreenSize(&dx,&dy);

	if(relation)
		relation->GetGeometry(&rx,&ry,&rsx,&rsy);
	else
		{
		rsx=dx;
		rsy=dy;
		}

	GetGeometry(&x,&y,&sx,&sy);

//	printf("mode=%d relation=0x%x %d,%d %d,%d on %d,%d %d,%d\n",mode,relation,x,y,sx,sy,rx,ry,rsx,rsy);

	if(mode==EW_POSITION_OVER)
		{
		x=rx+rsx/2-sx/2;
		y=ry+rsy*2/3-sy/2;
		}
	else
		{
		x+=rx;
		y+=ry;
		}

//	printf("result=%d,%d\n",x,y);

	// limit to edges of screen
	if(x>dx-sx)
		x=dx-sx;
	if(x<0)
		x=0;

	if(y<0)
		y=0;
	if(y>dy-sy)
		y=dy-sy;

	SetOrigin(x,y);
	}



/******************************************************************************
void	EW_Window::Open(long mode,EW_Window *relation)

	makes mulitple attempts, degrading as neccesary

******************************************************************************/
void EW_Window::Open(long mode,EW_Window *relation)
	{
	if(!GetEventContext())
		EW_PRINT(EW_APP,EW_WARN,"EW_Window::Open() window (%s) not in an event context",titlebuffer);

	long original_colors=GetNumberColors();

	if( !(GetFlags() & EW_WINDOW_PHANTOM_FLAG) )
		SetOriginBy(mode,relation);

	if(DoOpen())
		{
		// failure
		Close();

		if(GetDoubleBuffer())
			{
			EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::Open() cannot open window -> retrying single-buffered");

			SetDoubleBuffer(FALSE);
			SetNumberColors(original_colors);

			if(DoOpen())
				EW_PRINT(EW_WINDOW,EW_FATAL,"EW_Window::Open() still cannot open window -> exiting");
			}
		else
			EW_PRINT(EW_WINDOW,EW_FATAL,"EW_Window::Open() cannot open window -> exiting");
		}
#if EW_OPEN_RED
	else if(GetMapped())
		{
		long bgcolor=GetColor(EW_COLOR_BACKGROUND);

		SetColor(EW_COLOR_BACKGROUND,EW_DARKRED);
		Clear();
		Flush(TRUE);
		osdMilliSleep(300);

		SetColor(EW_COLOR_BACKGROUND,bgcolor);
		}
#endif

	if(GetOpened() && GetEventContext())
		GetEventContext()->UpdateWindowCursors();
	}



/******************************************************************************
long	EW_Window::DoOpen(void)

	protected

Keywords:
******************************************************************************/
long EW_Window::DoOpen(void)
{
#if EW_WINDOW_DEBUG
	char message[1024];

	EW_PRINT(EW_WINDOW,EW_LOG," EW_Window::DoOpen() (%s) mapped=%d opened=%d",titlebuffer,mapped,opened);
#endif

	if(!mapped)
	{
		if(!opened)
		{
		// use default font if current not valid
		if( !(GetFlags() & EW_WINDOW_PHANTOM_FLAG) && !current_font.GetValid())
			current_font.CopyFrom(&default_font);

		if( !(GetFlags() & EW_WINDOW_PHANTOM_FLAG) )
			Resize();

		long ix,iy,sx,sy;
		GetGeometry(&ix,&iy,&sx,&sy);

#if EW_WINDOW_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG," EW_Window::DoOpen() (%s) %d,%d %d,%d",titlebuffer,ix,iy,sx,sy);
//		Dump(FALSE);
#endif

		// Data needed

			#if EW_WL==EW_XWIN

				XSetWindowAttributes cwa;
				XVisualInfo *visuallist=NULL;
				XVisualInfo *visualinfo;
				Window parent;

				#if EW_GL==EW_OPENGL

					int attributes[32];

				#elif EW_GL==EW_IRISGL

					GLXconfig params[50];
					GLXconfig* next;
					GLXconfig* retconfig;
					XVisualInfo templ;
					XColor white;
					XWindowAttributes   pwa;
					int i, nret;

				#endif // EW_IRISGL

			#endif // EW_XWIN


			long m;

		#if EW_WL==EW_MEX

			prefposition(ix,ix+sx,iy,ix+sy);
			wid = winopen(titlebuffer);

			/* release fixed-size constraint imposed by prefposition */
			winconstraints();

			if(double_buffer)
				doublebuffer();

			gconfig();

		#endif // EW_MEX


		/* -Open window--------------------------------------------------------------*/
		#if EW_WL==EW_XWIN
			if(display == NULL)
				{
				if((display = XOpenDisplay(NULL)) == NULL)
					EW_PRINT(EW_WINDOW,EW_FATAL,"cannot connect to X server");

				screen=DefaultScreen(display);
				dx = DisplayWidth(display, screen);
				dy = DisplayHeight(display, screen);

//				delete_atom = XInternAtom(display,"WM_SAVE_YOURSELF",FALSE);
				atom[0]=XInternAtom(display,"WM_DELETE_WINDOW",TRUE);
				atom[1]=XInternAtom(display,"_WM_QUIT_APP",TRUE);

				if(atom[0]==None)
					EW_PRINT(EW_WINDOW,EW_WARN,"WM_DELETE_WINDOW not supported");
/*
				if(atom[1]==None)
					EW_PRINT(EW_WINDOW,EW_WARN,"_WM_QUIT_APP not supported");
*/
				}

			// to be replaced later, potentially
			colormap=DefaultColormap(display,screen);

			if(!graphics_context)
				graphics_context=DefaultGC(display,screen);

			parent=RootWindow(display,screen);
			if(!parent)
				EW_PRINT(EW_WINDOW,EW_FATAL,"RootWindow(display=0x%x,screen=0x%x)=NULL",display,screen);

		#if EW_GL==EW_OPENGL

			m=0;

			if(GetRGBMode())	/* if RGB mode */
				{
				attributes[m++]=GLX_RGBA;
				attributes[m++]=GLX_RED_SIZE;
				attributes[m++]=1;
				attributes[m++]=GLX_GREEN_SIZE;
				attributes[m++]=1;
				attributes[m++]=GLX_BLUE_SIZE;
				attributes[m++]=1;
				attributes[m++]=GLX_DEPTH_SIZE;
				attributes[m++]=1;
				}

			if(double_buffer)
				attributes[m++]=GLX_DOUBLEBUFFER;

			// for clipping but seems to stop Mesa from matching
//			attributes[m++]=GLX_STENCIL_SIZE;
//			attributes[m++]=1;

			attributes[m]=None;		/* None defined by GLX? */

#if EW_WINDOW_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG,"display=0x%x screen=0x%x parent=0x%x RGBmode=%d attributes[%d]",
										display,screen,parent,GetRGBMode(),m);
#endif

			/* get a visual matching attributes[] */
			if( !(visualinfo=glXChooseVisual(display,screen,attributes)) )
				{
				EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::Open(): "
							"compatible visual not available through GLX.\n"
							"\tTrying arbitrary visual.");

				int nitems;
				XVisualInfo vinfo;
				vinfo.depth=DefaultDepth(display,screen);
				vinfo.visual=DefaultVisual(display,screen);
				visualinfo=XGetVisualInfo(display,VisualNoMask,&vinfo,&nitems);
#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG,"visualinfo=0x%x",visualinfo);
#endif
				}

			/* create a GLX context (no sharing, attempt direct rendering) */
			if(!(glx_context=glXCreateContext(display,visualinfo,NULL,GL_TRUE)))
				EW_PRINT(EW_WINDOW,EW_FATAL,"EW_Window::Open(): could not create GLX context");

#if EW_WINDOW_DEBUG
			sprintf(message,"  visual=0x%x visualid=0x%x screen=0x%x\n    depth=%d colormap_size=%d bits_per_rgb=%d",
					(long)visualinfo->visual,
					visualinfo->visualid,
					visualinfo->screen,
					visualinfo->depth,
					visualinfo->colormap_size,
					visualinfo->bits_per_rgb
					);
			EW_PRINT(EW_WINDOW,EW_LOG,message);
#endif

		#elif EW_GL==EW_IRISGL

			next = params;

			if(GetRGBMode())
				ewGLXSetEntry(next++, GLX_NORMAL, GLX_RGB, TRUE);
			else
				ewGLXSetEntry(next++, GLX_NORMAL, GLX_RGB, FALSE);

			if(double_buffer)
				ewGLXSetEntry(next++, GLX_NORMAL, GLX_DOUBLE, TRUE);
			else
				ewGLXSetEntry(next++, GLX_NORMAL, GLX_DOUBLE, FALSE);

			ewGLXSetEntry(next, 0, 0, 0); /* The input to GLXgetconfig is null terminated */
			XGetWindowAttributes(display, parent, &pwa);
			retconfig = GLXgetconfig(display, XScreenNumberOfScreen(pwa.screen), params);
			if (retconfig == 0)
				{
				EW_PRINT(EW_WINDOW,EW_ERROR,"cannot support needed window type");
				}
			XSetErrorHandler(0);
			for (next = retconfig; next->buffer; next++)
				{
				unsigned long buffer = next->buffer;
				unsigned long mode = next->mode;
				unsigned long value = next->arg;
				switch (mode)
					{
					case GLX_COLORMAP:
						if (buffer == GLX_NORMAL)
							{
							colormap=value;
							}
						break;
					case GLX_VISUAL:
						if (buffer == GLX_NORMAL)
							{
							templ.visualid = value;
							templ.screen = DefaultScreen(display);
							visualinfo = XGetVisualInfo(display, VisualScreenMask|VisualIDMask, &templ, &nret);
							}
						break;
					}
				}

		#else // Not OpenGL or IrisGL (Pure X)

			XVisualInfo vinfo;

			// find visual
			XVisualInfo vinfo_template;
			int visuals;
			long vinfo_mask;

			// vinfo_template.class;
			long *point=(long *)&vinfo_template;
			for(m=0;m<10;m++)
				point[m]=PseudoColor;
//			*(&vinfo_template.depth+sizeof(unsigned int))=PseudoColor;

			vinfo_template.screen=screen;
			vinfo_mask=VisualScreenMask | VisualDepthMask | VisualClassMask;

			long attempt=0;
			while(attempt<2)
				{
//				vinfo_template.depth= double_buffer? EW_X_DEPTH_DOUBLE: EW_X_DEPTH_SINGLE;
				vinfo_template.depth= attempt? 8: 12;

//				vinfo_template.colormap_size=1024;

//				vinfo_mask=VisualScreenMask | VisualDepthMask | VisualColormapSizeMask;

				visuallist=XGetVisualInfo(display,vinfo_mask,&vinfo_template,&visuals);


#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," EW_Window::Open() attempt %d: %d matching visuals",attempt,visuals);
#endif

				// just take the first
				visualinfo=visuallist;

				// if default is on list, use it
				for(m=0;m<visuals;m++)
					{
#if EW_WINDOW_DEBUG
	sprintf(message,"  visual=0x%x visualid=0x%x screen=0x%x\n    depth=%d colormap_size=%d bits_per_rgb=%d",
					(long)visualinfo->visual,
					visualinfo->visualid,
					visualinfo->screen,
					visualinfo->depth,
					visualinfo->colormap_size,
					visualinfo->bits_per_rgb
					);
				EW_PRINT(EW_WINDOW,EW_LOG,message);
#endif

					if(visuallist[m].visual==DefaultVisual(display,screen))
						{
						visualinfo=NULL;

#if EW_WINDOW_DEBUG
						EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Open(): default visual acceptable");
#else
						// discontinue since no printing
						break;
#endif
						}
					}

				if(visualinfo)
					break;

				attempt++;
				}

			if(visualinfo==NULL)
				{
				if(!visuallist)
					EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::Open(): no visual match, trying mismatched default");

				// default visual

				visualinfo=&vinfo;
				visualinfo->depth=DefaultDepth(display,screen);
//				visualinfo->depth=vinfo_template.depth;
				visualinfo->visual=DefaultVisual(display,screen);
				}

#if EW_WINDOW_DEBUG
			sprintf(message," EW_Window::Open() using visualid 0x%x",visualinfo->visualid);
			EW_PRINT(EW_WINDOW,EW_LOG,message);
#endif


		#endif //EW_GL

			unsigned long value_mask=0;
//			if(!GetRGBMode())
				{
				// if double-buffered or not default visual, make new colormap instead of using default
//				if( EW_GL==EW_OPENGL || double_buffer || visualinfo->visual!=DefaultVisual(display,screen) )

				// if OpenGL or not default visual, make new colormap instead of using default
				if( EW_GL==EW_OPENGL || visualinfo->visual!=DefaultVisual(display,screen) )
					{
#if EW_WINDOW_DEBUG
					if( EW_GL!=EW_OPENGL && !double_buffer )
						EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Open(): setting colormap for non-default visual");
#endif

					Colormap *sharemap=NULL;
					if(GetRGBMode())
						sharemap= (EW_GL==EW_OPENGL && double_buffer)?
											&shared_colormap_rgb_double: &shared_colormap_rgb;
					else
						sharemap= (EW_GL==EW_OPENGL && double_buffer)?
											&shared_colormap_double: &shared_colormap;

					long create=!(*sharemap);
					if(create)
						(*sharemap)=XCreateColormap(display,parent,visualinfo->visual,AllocNone);

#if EW_WINDOW_DEBUG
					EW_PRINT(EW_WINDOW,EW_LOG,
							"EW_Window::DoOpen(): %s shared colormap 0x%x rgb=%d doublebuffer=%d",
							create? "created": "reused",(*sharemap),GetRGBMode(),GetDoubleBuffer());
#endif

					colormap=(*sharemap);
					}

				cwa.border_pixel = 0;
				cwa.colormap = colormap;

				value_mask = CWColormap|CWBorderPixel;
				}

#if TRUE
			if(GetFlags() & EW_WINDOW_BORDERLESS)
			{
				cwa.override_redirect = TRUE;
				value_mask |= CWOverrideRedirect;
			}
#endif

#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window: XCreateWindow(0x%x,0x%x, %d,%d,%d,%d, %d,%d,%d 0x%x 0x%x,0x%x",
						display,parent,ix,dy-iy-sy,sx,sy,
						0,visualinfo->depth,InputOutput,
						visualinfo->visual,
						value_mask,&cwa);

XSync(display,FALSE);
#endif

			win=XCreateWindow(display,parent,ix,dy-iy-sy,sx,sy,0,visualinfo->depth,InputOutput,visualinfo->visual,
																										value_mask,&cwa);

#if FALSE
XSync(display,TRUE);
printf("Create 1\n");
#endif

			osdMemcpy(&visual_information,visualinfo,sizeof(XVisualInfo));

			if(visuallist)
				XFree(visuallist);

			if(win==0)
				EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::Open(): window didn't open");
			else
				{
				opened=TRUE;
				PropagateTitle();

#if EW_WINDOW_DEBUG
				sprintf(message,"EW_Window::Open(): made window 0x%x",win);
				EW_PRINT(EW_WINDOW,EW_LOG,message);

XSync(display,FALSE);
#endif
				}

			if(GetFlags() & EW_WINDOW_TRANSIENT)
			{
				XSetTransientForHint(display,win,parent);
			}

		#if EW_GL==EW_IRISGL

			for (next = retconfig; next->buffer; next++)
				{
				if ((next->buffer == GLX_NORMAL) && (next->mode == GLX_WINDOW))
					{
					next->arg = win;
					break;
					}
				}

			i = GLXlink(display, retconfig);
			if (i < 0)
				{
				EW_PRINT(EW_WINDOW,EW_ERROR,"GLXlink failed");
				}

		#elif EW_GL==EW_X_GFX

			if(double_buffer)
				{
#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," Xdbuf_InitColorBuffer()");
				XSync(display,FALSE);
#endif
				if( Xdbuf_InitColorBuffer() )
					return TRUE;

#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," Xdbuf_SwapBuffers()");
				XSync(display,FALSE);
#endif

#if FALSE
				Xdbuf_SwapBuffers();
#endif

#if EW_WINDOW_DEBUG
				XSync(display,FALSE);
#endif
				}

		#endif


#if EW_WINDOW_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG," XSelectInput()");
#endif

			XSelectInput(display,win,WDS_X_EVENTMASK);

#if EW_WINDOW_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG," XStringListToTextProperty()");
#endif

#if FALSE
			//if(GetFlags() & EW_WINDOW_BORDERLESS)
			//{
				Cursor a_cursor;
				a_cursor = XCreateFontCursor(display, XC_left_ptr);
				XDefineCursor(display, win, a_cursor);
			//}
#endif

#if EW_WINDOW_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG," XSetWMProtocols()");
#endif

			if( !XSetWMProtocols(display,win,atom,2) )
				{
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not XSetWMProtocols()");
				}

#if EW_WINDOW_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG," Map()");
#endif

			if( !(GetFlags() & EW_WINDOW_NOMAP) )
				Map();

		#endif //EW_XWIN


		#if EW_WL==EW_WIN32
			if(dx == -1)
			{
				dx = GetSystemMetrics(SM_CXSCREEN);
				dy = GetSystemMetrics(SM_CYSCREEN);
			}

			Win32_CalcStyle(&dwstyle,&dwexstyle);

			HWND parent_hwnd=NULL;

			// if has parent, won't show up on taskbar on bottom	(redundant with WS_EX_TOOLWINDOW)
			if( (GetFlags()&EW_WINDOW_BORDERLESS) && !(GetFlags()&EW_WINDOW_PHANTOM_FLAG) )
				parent_hwnd=GetEventContext()->GetPhantom()->GetHWND();

			/* CREATE THE WINDOW */
			hwnd = CreateWindowEx(
				dwexstyle,		// extended style
				"EW_WClass",	// registered class name
				titlebuffer,	// window name
				dwstyle,		// style
				ix,dy-iy-sy,	// position
				sx,sy,			// size
				parent_hwnd,	// parent
				(HMENU) NULL,	// menu
				EW_HINSTANCE,	// app instance handle
				(LPVOID) NULL);	// pointer to window creation data

			if(!hwnd)
			{
				DWORD errnumber;
				errnumber = GetLastError();
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not open window");
			}


#if EW_WINDOW_DEBUG
			sprintf(message,"(%s) hwnd=%d (0x%x)",titlebuffer,hwnd,hwnd);
			EW_PRINT(EW_WINDOW,EW_LOG,message);
#endif

			// See EW_Window::AffectGeometry(void) to do this better

			long tix, tiy, tsx, tsy;
			long x,y,w,h;

			x = ix;
			y = iy;
			w = sx;
			h = sy;

			MoveWindow(hwnd,x,dy-y-h,w,h,FALSE);
			UpdateSize();

			tix = ix - x;
			tiy = iy - y;
			tsx = w - sx;
			tsy = h - sy;

			MoveWindow(hwnd,x-tix,dy-y-h+tiy-tsy,w+tsx,h+tsy,TRUE);

/*			SET THE EXTRA DATA TO HOLD A POINTER TO THE WINDOW OBJECT
 *			The reason for this convoluted double set to error check is
 *			because if SetWindowLong fails it returns 0, otherwise it returns
 *			the previous value.  However, Win32 intializes the space to zero,
 *			therefore the first time we check we can't tell if its an error or not
 */

			void *This; This = this;

			SetWindowLong(hwnd,0,*(long *)&This);
			if(!SetWindowLong(hwnd,0,*(long *)&This))
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not set window information");

			/* GET THE DEVICE CONTEXT */
			hdc = GetDC(hwnd);
			if(!hdc)
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not get device context");

			/* SET THE PIXEL FORMAT */

			DWORD db_mask;
			if(double_buffer)
				db_mask = PFD_DOUBLEBUFFER;
			else
				db_mask = 0;

			DWORD gl_mask;
			#if EW_GL==EW_OPENGL
				gl_mask = PFD_SUPPORT_OPENGL;
			#elif EW_GL==EW_GDI
				gl_mask = PFD_SUPPORT_GDI;
			#else
				#error gl_mask flag not set
			#endif

			DWORD rgbmode=PFD_TYPE_COLORINDEX;
			DWORD depth=8;
			if(GetRGBMode())
			{
				rgbmode=PFD_TYPE_RGBA;
				depth=24;
			}

			#if EW_GL==EW_OPENGL

				PIXELFORMATDESCRIPTOR pfd =
				{
					sizeof(PIXELFORMATDESCRIPTOR),
					1,
					PFD_DRAW_TO_WINDOW |
						gl_mask |
						db_mask,
					rgbmode,
					depth,
					0,0,0,0,0,0,
					0,
					0,
					0,
					0,0,0,0,
					16,
					0,
					0,
					PFD_MAIN_PLANE,
					0,
					0,0,0
				};

				int iPixelFormat = ChoosePixelFormat(hdc, &pfd);

				if(!iPixelFormat)
					EW_PRINT(EW_WINDOW,EW_ERROR,"could not choose pixel format");

				if(!SetPixelFormat(hdc, iPixelFormat, &pfd))
					EW_PRINT(EW_WINDOW,EW_ERROR,"could not set pixel format");

				if(!DescribePixelFormat(hdc,iPixelFormat,sizeof(PIXELFORMATDESCRIPTOR),&pfd))
					EW_PRINT(EW_WINDOW,EW_ERROR,"could not get pixel format");

				if(!(pfd.dwFlags & gl_mask))
					EW_PRINT(EW_WINDOW,EW_ERROR,"graphics system not supported");
			#endif

			#if EW_GL==EW_OPENGL
				if(!(hglrc = wglCreateContext(hdc)))
					EW_PRINT(EW_WINDOW,EW_ERROR,"could not create rendering context");
			#endif

			opened=TRUE;
			PropagateTitle();

		#endif

#if EW_WINDOW_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG," UpdateSize()");
#endif

//			UpdateSize();

			
#if EW_WINDOW_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG," Resize()");
#endif
			Resize();

			if( !(GetFlags() & EW_WINDOW_PHANTOM_FLAG) )
				{
#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," InitGraphics()");
#endif
				InitGraphics();
				}
			else
				{
#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," InitDefaultFont()");
#endif
				InitDefaultFont();
				}

		#if EW_WL==EW_WIN32
			#if EW_GL==EW_GDI
				int j;
				for(j = 0;j < EW_MAXCOLORS;j++)
				{
					hpen[j] = CreatePen(PS_SOLID,0,EwWin32Colortable[j]);
					if(!hpen[j])
						EW_PRINT(EW_WINDOW,EW_ERROR,"could not create pen");
				}

				for(j = 0;j < EW_MAXCOLORS;j++)
				{
					hbrush[j] = CreateSolidBrush(EwWin32Colortable[j]);
					if(!hbrush[j])
						EW_PRINT(EW_WINDOW,EW_ERROR,"could not create brush");
				}

				hbrush_invisible = (struct HBRUSH__ *)GetStockObject(HOLLOW_BRUSH);

				if(!SetMapMode(hdc,MM_TEXT))
					EW_PRINT(EW_WINDOW,EW_ERROR,"could not set map mode");
			#endif

#if EW_WINDOW_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG," ShowWindow()");
#endif

			if(! (GetFlags()&EW_WINDOW_NOMAP) )
				{
#if FALSE
				mapped = TRUE;

				// NOTICE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// Right here Microsoft will interrupt the current event with new Focus-Change and Exposure events.
				// We make sure opened and mapped are TRUE here so that the exposure can operate as desired.
				ShowWindow(hwnd,EW_NCMDSHOW);
				UpdateWindow(hwnd);

				// window-system-specified show-mode only potentially used for first window opened
				EW_NCMDSHOW=EW_WIN32_SHOW_NORMAL;
#else
				Map();
#endif
				}

/*
			if(GetFlags() & EW_WINDOW_NOMAP)
				{
#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," UnMap()");
#endif

				UnMap();
				}
*/

#if EW_WINDOW_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG," InitiateWork()");
#endif

			InitiateWork();
		#endif
		}
		else
			{
			/* already opened */

			if( !(GetFlags() & EW_WINDOW_NOMAP) )
				{
#if EW_WINDOW_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG," Map() already opened");
#endif

				Map();
				}
			}
	}

#if FALSE
Flush(TRUE);
printf("Create 2\n");
			ConfirmSize();
Flush(TRUE);
printf("Create 3\n");
			UpdateSize();
Flush(TRUE);
printf("Create 4\n");
#endif

#if FALSE
	if(GetMapped())
		Refresh();
#endif

	if(FALSE && double_buffer)
		{
#if EW_GL==EW_X_GFX
		BufferSwap();
#endif

/*
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Open(): dbuf pre  Refresh");
		Refresh();
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Open(): dbuf post Refresh");
*/
		}

#if EW_WINDOW_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG," UpdateSize()");
#endif

	UpdateSize();
	AffectGeometry();

#if EW_WINDOW_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Open(): done");
#endif

	return FALSE;
}



/******************************************************************************
void	EW_Window::Close()

Keywords:
******************************************************************************/
void EW_Window::Close()
	{
#if EW_WINDOW_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Close() 0x%x",(long)this);
#endif

	if(opened)
		{
//		Flush(TRUE);

		if(mapped)
			UnMap();

//		Flush(FALSE);

#if EW_WL==EW_XWIN
		XDestroyWindow(display,win);
#elif EW_WL==EW_WIN32
	#if EW_GL==EW_OPENGL
		wglMakeCurrent(NULL,NULL);
	#elif EW_GL==EW_GDI

		int j;
		BOOL n;

		for(j = 0;j < EW_MAXCOLORS;j++)
			{
			n = DeleteObject(hpen[j]);
			if(!n)
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not delete pen");

			n = DeleteObject(hbrush[j]);
			if(!n)
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not create brush");
			}
	#endif

		ReleaseDC(hwnd,hdc);

	#if EW_GL==EW_OPENGL
		wglDeleteContext(hglrc);
	#endif

		DestroyWindow(hwnd);
		font_set = FALSE;

#elif EW_WL==EW_MEX

		#error no MEX destroy window

#endif

		opened = FALSE;
		}

//	Flush(TRUE);

#if EW_WINDOW_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Close(): done");
#endif

	}



/******************************************************************************
long	EW_Window::Map(long mode,EW_Window *relation)

******************************************************************************/
long EW_Window::Map(long mode,EW_Window *relation)
	{
	if(!GetOpened())
		{
		Open(mode,relation);
/*
		EW_PRINT(EW_APP,EW_ERROR,"EW_Window::Map() cannot map unopened window");
		return FALSE;
*/
		}

/*
	EW_EventContext *context=GetEventContext();
	if(context)
		{
		long smx,smy;
		context->GetScreenMouse(&smx,&smy);
		EW_PRINT(EW_APP,EW_WARN,"EW_Window::Map() screen mouse %d,%d",smx,smy);
		}
*/

	if(!mapped)
		{
		SetOriginBy(mode,relation);

		mapped = TRUE;

		Resize();
		ConstrainSize(TRUE,TRUE,dx,dy);
		AffectGeometry();

		#if EW_WL==EW_XWIN
#if TRUE
			XSetWindowAttributes cwa;
			unsigned long value_mask=0;

			if(GetFlags() & EW_WINDOW_BORDERLESS)
				cwa.override_redirect = TRUE;
			else
				cwa.override_redirect = FALSE;

			if(GetFlags()&EW_WINDOW_TRANSIENT)
				XSetTransientForHint(display,win,win);

			value_mask |= CWOverrideRedirect;
			XChangeWindowAttributes(display,win,value_mask,&cwa);
#endif
			XMapWindow(display,win);

			// seems necessary for a usable graphics_context

//	printf("A gc=0x%x colormap_allocated=%d\n",graphics_context,colormap_allocated);
			InitGraphics();
//	printf("B gc=0x%x colormap_allocated=%d\n",graphics_context,colormap_allocated);

		#elif EW_WL==EW_WIN32

			long force=FALSE;
			if(GetEventContext())
				{
				long smx,smy,ix,iy,sx,sy;

				GetGeometry(&ix,&iy,&sx,&sy);
				GetEventContext()->GetScreenMouse(&smx,&smy);

				// if window is opened under mouse, activate it
				if(EW_WITHIN(smx,smy,ix,iy,sx,sy) || first_map)
					force=TRUE;
				}

			ShowWindow(hwnd,EW_WIN32_SHOW_NORMAL);
			UpdateWindow(hwnd);

			Raise(force);

			first_map=FALSE;

		#endif

		if(GetEventContext() && GetFlags()&EW_WINDOW_MODAL)
			GetEventContext()->AppendWindowToModalStack(this,(GetFlags()&EW_WINDOW_MODAL_EXCLUSIVE_FLAG)!=0);


		WinSet(2);
		SetDirty(EW_DIRTY_COMPLETE);

		return TRUE;
		}

	return FALSE;
	}



/******************************************************************************
long	EW_Window::UnMap(void)

******************************************************************************/
long EW_Window::UnMap(void)
	{
	EW_EventContext *context=GetEventContext();
	if(context)
		context->RemoveWindowFromModalStack(this);

	if(!GetOpened())
		{
		EW_PRINT(EW_APP,EW_ERROR,"EW_Window::UnMap() cannot unmap unopened window");
		return FALSE;
		}

	if(mapped)
		{
#if FALSE
		// clear background (menus popping down)
		// improvement in single-buffered, odd in double
		if(GetOpened() && !GetDoubleBuffer())
			Clear();
#endif

		#if EW_WL==EW_XWIN
			XUnmapWindow(display,win);
		#elif EW_WL==EW_WIN32
			ShowWindow(hwnd,EW_WIN32_SHOW_NOMAP);
		#endif

		mapped = FALSE;
		return TRUE;
		}
	return FALSE;
	}



/******************************************************************************
long	EW_Window::GrabPointer()

	returns TRUE if successful

Keywords:
******************************************************************************/
long EW_Window::GrabPointer()
{
	long result=FALSE;

#if EW_WL==EW_XWIN
	result=(XGrabPointer(display,
			win,
#if FALSE
			// use new mask
			False,
//			ButtonMotionMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask|ButtonReleaseMask,
//			WDS_X_EVENTMASK,

			PointerMotionMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask|ButtonReleaseMask,
//			ExposureMask|KeyPressMask|KeyReleaseMask|WDS_X_MOTIONMASK|StructureNotifyMask,

#else
			// use original mask
			True,
			ButtonPressMask|ButtonReleaseMask,
#endif
			GrabModeAsync,
			GrabModeAsync,
			None,
			None,
			CurrentTime) == GrabSuccess);

#elif EW_WL==EW_WIN32

	SetCapture(hwnd); 
	if(hwnd != GetCapture())
		{
		EW_PRINT(EW_WINDOW,EW_ERROR,"Grab Failed %d (%s)",hwnd,GetTitle());
		}
	else 
		{
#if EW_GRAB_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"Grab Pointer %d (%s)",hwnd,GetTitle());
#endif

		result=TRUE;
		}

	if(windrag)
		windrag=FALSE;

#endif

	if(result && GetEventContext())
		GetEventContext()->SetEventRedirection(this);

	return result;
}



/******************************************************************************
long	EW_Window::ReleasePointer()

Keywords:
******************************************************************************/
long EW_Window::ReleasePointer()
	{
	if(GetEventContext())
		GetEventContext()->SetEventRedirection(NULL);

#if EW_WL==EW_XWIN
	XUngrabPointer( display, CurrentTime );
	return TRUE;

#elif EW_WL==EW_WIN32

#if EW_GRAB_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"Release Pointer %d (%s) has:%d",hwnd,GetTitle(),GetCapture());
#endif

	if(ReleaseCapture())
		return TRUE;

	if(windrag)
		windrag=FALSE;

	return FALSE;

#else
	return FALSE;

#endif
	}



/******************************************************************************
void	EW_Window::Refresh(void)

	EW_Window::Refresh(EW_Redraw *redraw) called with default EW_Redraw

	most likely used for an application-initiated refresh

Keywords:
******************************************************************************/
void EW_Window::Refresh(void)
	{
	EW_Redraw redraw;

	/* set things user shouldn't have to mess with */
	redraw.SetMousePosition(-1,-1);
	redraw.SetForce(TRUE);

	EW_PRINT(EW_WINDOW,EW_LOG,"Refresh(void) on (%s)",GetTitle());

	/* pass off to real Refresh() */
	Refresh(&redraw);
	}



/******************************************************************************
void	EW_Window::Refresh(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void EW_Window::Refresh(EW_Redraw *redraw)
	{
	if(!GetMapped())
		return;

	EW_Widget *next;

//	EW_PRINT(EW_WINDOW,EW_LOG,"Refresh(0x%x) on (%s)",(long)redraw,GetTitle());
	WinSet();

#if EW_GL==EW_OPENGL && EW_FEEDBACK

	GLfloat token,feedbuffer[1024];
	glFeedbackBuffer(1024,GL_2D,feedbuffer);
	glRenderMode(GL_FEEDBACK);

#endif

#if EW_BELLS
	RingBell();
#endif


	// if clipped to full window, not really a limited redraw
	if(redraw->GetLimited())
		{
		long sx,sy;
		long cx,cy,csx,csy;

		GetSize(&sx,&sy);
		GetClipBounds(&cx,&cy,&csx,&csy);

		if(cx==0 && cy==0 && csx==sx && csy==sy) 
			{
//			EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Refresh() limited redraw is not really limited\n");
			redraw->SetLimited(FALSE);
			}
		}

	if(!redraw->GetLimited() && redraw->GetForce())
		{
		dirty[0]=EW_DIRTY_COMPLETE;
		dirty[1]=EW_DIRTY_COMPLETE;
		}

	if(redraw->GetLimited() || dirty[0]==EW_DIRTY_COMPLETE)
		{
/*
		EW_PRINT(EW_WINDOW,EW_LOG,"%sRefresh limited=%d force=%d dirty=%d (%s)%s",ewColorCode(5),
											redraw->GetLimited(),redraw->GetForce(),dirty0,GetTitle(),ewColorCode(0));
*/

#if EW_GEIGER_COUNTER
		long bgcolor=GetColor(EW_COLOR_BACKGROUND);

		SetColor(EW_COLOR_BACKGROUND,EW_GREEN);
		Clear();
		Flush(TRUE);
		osdMilliSleep(50);

		SetColor(EW_COLOR_BACKGROUND,bgcolor);
		RingBell();
//		osdMilliSleep(100);
#endif

/*
//	long indexb=index_back;
//	index_back=EW_GREEN;

		EW_PRINT(EW_WINDOW,EW_LOG,"Clear (%s)",GetTitle());
*/

		Clear();

//	index_back=indexb;

		redraw->SetForce(TRUE);
		}

#if FALSE
	EW_PRINT(EW_WINDOW,EW_LOG,"%sWindow Redrawing limited=%d force=%d dirty=%d (%s)%s",ewColorCode(5),
										redraw->GetLimited(),redraw->GetForce(),dirty0,GetTitle(),ewColorCode(0));
#endif

	long stackindex1=GetClipStack()->GetCurrentIndex();

	if(redraw->GetLimited() || GetDirty())
		{
		widgetlist->Rewind();
		while((next = widgetlist->Next())!=NULL)
			{
			next->Redraw(redraw);
			}
		}

	long stackindex2=GetClipStack()->GetCurrentIndex();

	if(stackindex1!=stackindex2)
		EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::Refresh() child Redraw() %sclip stack mismatch%s: level before=%d after=%d",
																		ewColorCode(1),ewColorCode(0),stackindex1,stackindex2);

	if(!redraw->GetLimited())
		{
		if(GetDoubleBuffer())
			{
			dirty[0]=dirty[1]&EW_DIRTY_MASK;
			dirty[1]=EW_DIRTY_NOT;
			}
		else
			dirty[0]=EW_DIRTY_NOT;
		}

	if(GetDoubleBuffer())
		Flush(FALSE);

#if EW_GL==EW_OPENGL && EW_FEEDBACK

	long size=glRenderMode(GL_RENDER);

	printf("FB: buffer size=%d\n",size);

	long count=size;
	while(count)
		{
		token=feedbuffer[size-count];
		count--;
		/* there are more cases that should be added (or will lock up) */
		if(token==GL_PASS_THROUGH_TOKEN)
			printf("FB: PASS THROUGH %.6G\n",feedbuffer[size-(count--)]);
		else if(token==GL_POINT_TOKEN)
			printf("FB: POINT %.6G %.6G\n",feedbuffer[size-(count--)],feedbuffer[size-(count--)]);
		else if(token==GL_LINE_TOKEN)
			printf("FB: LINE %.6G %.6G -> %.6G %.6G\n",	feedbuffer[size-(count--)],feedbuffer[size-(count--)],
														feedbuffer[size-(count--)],feedbuffer[size-(count--)]);
		else if(token==GL_LINE_RESET_TOKEN)
			printf("FB: LINE RESET %.6G %.6G -> %.6G %.6G\n",	feedbuffer[size-(count--)],feedbuffer[size-(count--)],
																feedbuffer[size-(count--)],feedbuffer[size-(count--)]);
		}
	
#endif

	// NOTE: Pure X swapped only once for all windows in EW_Window::ProcessEvent()
#if EW_GL!=EW_X_GFX
	if(GetDoubleBuffer())
		BufferSwap();
#endif
	}


/******************************************************************************
void EW_Window::Resize(void)

******************************************************************************/
long EW_Window::Resize(void)
	{
	EW_Resize resize;

#if EW_MAX_ON_WINSIZE
	long sx,sy;

	GetSize(&sx,&sy);
	resize.SetMaxSize(sx,sy);
#endif

	long result=widgetlist->Resize(&resize);

	ConstrainSize(TRUE,FALSE,dx,dy);

	return result;
	}



/******************************************************************************
void EW_Window::UseFullScreenOrthoViewport(void)

******************************************************************************/
void EW_Window::UseFullScreenOrthoViewport(void)
	{
#if EW_GL==EW_OPENGL
	long sx,sy;
	GetSize(&sx,&sy);

	glViewport(0,0,sx,sy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.375,(float)sx-0.375, -0.375,(float)sy-0.375, -1.0,1.0);
	glMatrixMode(GL_MODELVIEW);

/********** from SGI Manual *************
	void myReshape(GLsizei w, GLsizei h)
		{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho (0.0, w, 0.0, h, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		}
****************************************/

#if FALSE
	long x,y;

	for(y=0;y<sy;y+=20)
		for(x=0;x<sx;x+=20)
			{
			RectWire(x,y,x+6,y+6,EW_BLUE);
			RectFill(x+2,y+2,x+4,y+4,EW_GREEN);
			}

	RectWire(1,1,5,5,EW_RED);
	RectWire(sx-6,sy-6,sx-2,sy-2,EW_RED);
#endif

#endif
	}



/******************************************************************************
void	EW_Window::WinSet(long setfont=TRUE)

	setfont=
		0: don't try to set font
		1: try to set font (default)
		2: force WinSet even if already appears set

******************************************************************************/
void EW_Window::WinSet(long setfont)
	{
#if EW_WINSET_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::WinSet(%d) (%s)",setfont,GetTitle());
#endif

	if(GetSetWindow()==this && setfont!=2)
		{
#if EW_WINSET_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::WinSet() already set");
#endif
		return;
		}

	#if EW_GL==EW_IRISGL

		#if EW_WL==EW_MEX

			winset(wid);

		#elif EW_WL==EW_XWIN

			GLXwinset(display,win);

		#endif

	#elif EW_GL==EW_OPENGL

		// force re-init of font (you'd think this wasn't necessary)
		lastfontbase=0;

		#if EW_WL==EW_XWIN

			glXMakeCurrent(display,win,glx_context);

		#elif EW_WL==EW_WIN32

			if(!wglMakeCurrent(hdc,hglrc))
				EW_PRINT(EW_WINDOW,EW_ERROR,"could not WinSet");

		#endif

		UseFullScreenOrthoViewport();

	#endif

	if(setfont)
		SetFont(&current_font);

	ResetClipRegion(TRUE);

	SetSetWindow(this);
	}



/******************************************************************************
void	EW_Window::UpdateSize()

keywords: 
******************************************************************************/
void EW_Window::UpdateSize()
	{
	long ix,iy,sx,sy;

	GetGeometry(&ix,&iy,&sx,&sy);

#if EW_RESIZE_DEBUG
	long ox=ix,oy=iy,osx=sx,osy=sy;
#endif

#if EW_WL==EW_MEX

	winset(wid);
	reshapeviewport();
	getorigin(&ix,&iy);
	getsize(&sx,&sy);

	ortho2(-0.5,sx-0.5,-0.5,sy-0.5);
	viewport(0,sx-1,0,sy-1);


#elif EW_WL==EW_XWIN

	// note: does not attempt get changed size from fixed windows

	XWindowAttributes window_attributes;
	if(!XGetWindowAttributes(display,win,&window_attributes))
		{
		EW_PRINT(EW_WINDOW,EW_ERROR,"could not get window attributes");
		}
	else
		{
		if(IsFixedSize())
			{
#if EW_RESIZE_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG,"UpdateSize() fixed window ignoring size change");
#endif
			}
		else
			{
			sx = window_attributes.width;
			sy = window_attributes.height;
			}

		// position ignored
		ix = window_attributes.x;
		iy = EW_INVERT_Y(window_attributes.y + sy,dy);

#if EW_RESIZE_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"pos attr %d,%d",ix,iy);
#endif

		Window r, c;
		int rx, ry, wx, wy;
		unsigned int kb;

#if TRUE
//		EW_PRINT(EW_WINDOW,EW_LOG,"Flush and Query");
		
		Flush(TRUE);
		if(XQueryPointer(display, win, &r, &c, &rx, &ry, &wx, &wy, &kb))
			{
			ix = rx - wx;
			iy = ry - wy;
			iy = EW_INVERT_Y(iy+sy,dy);
			}
#endif
		}

#elif EW_WL==EW_WIN32

	RECT rect_struct;
	POINT point_struct;
	if(!GetClientRect(hwnd,&rect_struct))
	{
		EW_PRINT(EW_WINDOW,EW_ERROR,"could not get window attributes");
	}
	sx = rect_struct.right;
	sy = rect_struct.bottom;
	if(!GetWindowRect(hwnd,&rect_struct))
	{
		EW_PRINT(EW_WINDOW,EW_ERROR,"could not get window rectangle");
	}
	point_struct.x = rect_struct.left;
	point_struct.y = rect_struct.top;
	if(!ScreenToClient(hwnd,&point_struct))
	{
		EW_PRINT(EW_WINDOW,EW_ERROR,"could not convert client coords");
	}
	ix = rect_struct.left - point_struct.x;
	iy = rect_struct.top - point_struct.y;
	iy = dy - iy - sy;

#endif

	EW_RectangleList rectanglelist;
	rectanglelist.AddRectangle(0,0,sx,sy);
	SetAnyClipRegion(0,&rectanglelist);

	// set directly
	position[0]=ix;
	position[1]=iy;
	dimension[0]=sx;
	dimension[1]=sy;

#if EW_RESIZE_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::UpdateSize() 0x%x opened=%d mapped=%d %d,%d %d,%d -> %d,%d %d,%d (%s)",
													(long)this,GetOpened(),GetMapped(),ox,oy,osx,osy,ix,iy,sx,sy,GetTitle());
#endif
	}



#if EW_WL==EW_XWIN && EW_GL==EW_IRISGL

/******************************************************************************
static void ewGLXSetEntry(GLXconfig* ptr, int b, int m, int a)

Keywords:
******************************************************************************/
/*PROTOTYPE*/
static void ewGLXSetEntry(GLXconfig* ptr, int b, int m, int a)
	{
	ptr->buffer = b;
	ptr->mode = m;
	ptr->arg = a;
	}

#endif // EW_XWIN && EW_IRISGL



/******************************************************************************
void	EW_Window::RingBell(void)

	do something wierd to get users attention

Keywords:
******************************************************************************/
void EW_Window::RingBell(void)
	{
#if OSD_OS==OSD_IRIX && !EW_RELEASE_GRADE

	system("playaiff /usr/share/data/sounds/prosonus/instr/drm_cowbell.aiff & ");
//	system("playaiff /usr/share/data/sounds/prosonus/instr/castenet.aiff & ");

#elif EW_WL==EW_XWIN

	XBell(GetDisplay(),100);

#elif EW_WL==EW_WIN32

	MessageBeep(MB_OK);
	FlashWindow(hwnd,FALSE);

#endif
	}



/******************************************************************************
void EW_Window::Dump(long brief)

******************************************************************************/
void EW_Window::Dump(long brief)
	{
	char modalmode[32];
	EW_EventContext *context=GetEventContext();
	if(context)
		{
		if(context->GetTopMappedModalWindow()==this)
			strcpy(modalmode,"TOP");
		else
			strcpy(modalmode,"NOT TOP");
		}
	else
		strcpy(modalmode,"NO CONTEXT");

	long ix,iy,sx,sy;

	GetGeometry(&ix,&iy,&sx,&sy);

	EW_PRINT(EW_WINDOW,EW_LOG,
"Window 0x%x (%s) dump %d,%d %d,%d on %d,%d opened=%d mapped=%d modal=%d %s flags=0x%x colors=%d dbuffer=%d",
			(long)this,GetTitle(),ix,iy,sx,sy,dx,dy,GetOpened(),GetMapped(),GetModal(),modalmode,
			GetFlags(),GetNumberColors(),GetDoubleBuffer());

	EW_Widget *node=widgetlist->ToHead();
	if(node)
		node->DumpString();

	widgetlist->DumpInfo(0,TRUE,brief);
	}



/******************************************************************************
void	EW_Window::ProcessEvent(EW_Event *event,long primary)

This function internally does a WinSet(), therefore if this is used to
reroute an event WinSet() should be called afterwards to set the window
back to what it was before the call.

******************************************************************************/
void EW_Window::ProcessEvent(EW_Event *event,long primary)
	{
	EW_Window *swap_node=NULL;
	EW_Redraw redraw;
	long mx,my;

#if EW_PROCESS_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"%sProcessEvent() on (%s) primary=%d%s",
										ewColorCode(EW_CODE_YELLOW),GetTitle(),primary,ewColorCode(EW_CODE_NORMAL));
	event->Print((long)this);
#endif

	if(!GetMapped() && !event->IsWhenUnMapped())
		{
#if EW_PROCESS_DEBUG
		EW_PRINT(EW_EVENT,EW_LOG,"ProcessEvent() on (%s) ignoring event when unmapped",GetTitle());
		event->Print((long)this);
#endif
		return;
		}

#if TRUE
	if(!primary)
		{
		widgetlist->Descend(event,EW_DESCEND_IN_CHILD_WIDGET|EW_DESCEND_OUT_WINDOW);
		return;
		}
#endif

	event->SetTimeAutomatically();

	// in case the event loop added its own region (like a complex sub-region)
	if(GetClipStack()->GetCurrentIndex()>0)
		ResetClipRegion(TRUE);

	// noting repeated events like double-clicks
	if(event->Matches(GetLastEvent()))
		{
		long diff=event->GetTime() - GetLastEvent()->GetTime();

		// NOTE: should be adjustable
		if(diff<300)
			{
			event->SetRepetition(GetLastEvent()->GetRepetition()+1);
			event->SetLastChoice(GetLastEvent()->GetChoice());
/*
			printf("repeated %d times %d ms\n",event->GetRepetition(),diff);
			event->Print((long)this);
*/
			}
		}

//	WinSet();

	EW_Window *exposurenode=NULL;

	if(event->IsExposure())
		{
		exposurenode=(EW_Window *)event->GetState();

		if(exposurenode->GetIgnoreNextExposure())
			{
			exposurenode->SetIgnoreNextExposure(FALSE);

#if EW_PROCESS_DEBUG
			EW_PRINT(EW_EVENT,EW_LOG,"ProcessEvent() on (%s) %signoring one exposure%s as directed",
														GetTitle(),ewColorCode(EW_CODE_CYAN),ewColorCode(EW_CODE_NORMAL));
#endif
			return;
			}
		}

	/* operate recursively */

	// ignoring return value from Descend() and indirectly Resize(), Operate(), and PostOperate()
	widgetlist->Descend(event,EW_DESCEND_IN_CHILD_WIDGET|EW_DESCEND_OUT_WINDOW);

	/* redraw recursively */
	event->GetMousePosition(&mx,&my);
	redraw.SetMousePosition(mx,my);

	long frame=0;
	long frames=1+(GetDoubleBuffer() && event->IsExposure());

	while(frame<frames)
		{
//		EW_PRINT(EW_WINDOW,EW_LOG,"frame=%d/%d event=0x%x this=0x%x (%s)",frame,frames,event,this,this->GetTitle());

		// only primary's
//		if(EW_REFRESH_ALL && primary && !event->IsExposure())
		if(EW_REFRESH_ALL && primary)
			{
			// allow all windows a chance to refresh
			EW_EventContext *context=GetEventContext();
			if(!context)
				EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::ProcessEvent() no EW_EventContext");
			else
				{
				EW_WindowList *windowlist=context->GetWindowList();

				if(!windowlist)
					EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::ProcessEvent() no EW_WindowList");
				else
					{
					EW_Window *node;
					
					windowlist->ToHead();
					while( (node=windowlist->PostIncrement()) != NULL )
						{
						redraw.SetForce(EW_ALWAYS_REFRESH);

						if(event->IsExposure() && node==exposurenode)
							{
							redraw.SetForce(TRUE);
							redraw.SetLimited(frames-frame);

							node->PushClipRegion();
							EW_ClipRegion *exposure=node->GetClipStack()->GetCurrentRegion();
 							exposure->CopyFrom(event->GetClipRegion());

							ResetClipRegion(TRUE);
							}
						else
							redraw.SetLimited(FALSE);

						long previous_stacklevel=node->GetClipStack()->GetCurrentIndex();

#if FALSE
						EW_PRINT(EW_WINDOW,EW_LOG,
								"expo=%d node=0x%x/0x%x (%s) opened=%d mapped=%d previous_stacklevel=%d limited=%d",
								event->IsExposure(),node,event->GetState(),node->GetTitle(),
								node->GetOpened(),node->GetMapped(),previous_stacklevel,redraw.GetLimited());
#endif

						if(node->GetOpened() && node->GetMapped())
							{
//							node->ConfirmSize();
							node->Resize();

#if FALSE
							EW_PRINT(EW_WINDOW,EW_LOG,"(%s) dirty=%d force=%d limited=%d",
													node->GetTitle(),node->GetDirty(),redraw.GetForce(),redraw.GetLimited());
#endif

							// NOTE: limited gets cleared by Refresh when exposure is really entire window

							node->Refresh(&redraw);
							if(node->GetDoubleBuffer())
								swap_node=node;
							}

						long stacklevel=node->GetClipStack()->GetCurrentIndex();

#if FALSE
						EW_PRINT(EW_WINDOW,EW_LOG,"stacklevel=%d limited=%d",stacklevel,redraw.GetLimited());
#endif

//						if( stacklevel>0 && redraw.GetLimited()==1 )
						if( stacklevel>0 && previous_stacklevel>0 )
							{
							// BOGUS: previous_stacklevel is from <this>
							if(stacklevel!=previous_stacklevel)
								{
								EW_PRINT(EW_WINDOW,EW_ERROR,
								"EW_Window::ProcessEvent() (%s) clipping stack not popped as much as pushed (at %d from %d)",
									node->GetTitle(),stacklevel,previous_stacklevel);
								}
#if FALSE
							else
								EW_PRINT(EW_WINDOW,EW_LOG,
												"EW_Window::ProcessEvent() (%s) popping extra exposure clipping off stack",
												node->GetTitle());
#endif

							while(node->GetClipStack()->GetCurrentIndex()>0)
								node->PopClipRegion();
							}

						// confirm location in list
						windowlist->SetCurrent(node);
						windowlist->PostIncrement();
						}
					}
				}
			}
		// non-primary's don't refresh
		else if(primary)
			{
			if(EW_ALWAYS_REFRESH)
				redraw.SetForce(TRUE);

			Resize();
			Refresh(&redraw);

#if FALSE
			if(event->IsExposure())
				{
				long wcolor=GetColor(EW_COLOR_WIDGET);
				long wx,wy;
				GetSize(&wx,&wy);
				SetColor(EW_COLOR_WIDGET,EW_RED);
				GreyOut(0,0,wx-1,wy-1);
				SetColor(EW_COLOR_WIDGET,wcolor);
				}
#endif

			if(GetDoubleBuffer())
				swap_node=this;
			}

		// NOTE: limits buffer swapping to only one node when using pure X
		// not truly independent since using shared colormap
		//
		// NOTE: non-primary Processing can't swap buffers
		// so a double-buffered window run with non-primary processing
		// from a single-buffered window may not behave correctly
#if EW_GL==EW_X_GFX
		if(primary && swap_node)
			{
//			EW_PRINT(EW_EVENT,EW_LOG,"ProcessEvent() swap (%s)",GetTitle());
			swap_node->BufferSwap();
			}
#endif

		frame++;
		}

	if(primary)
		{
		// select events for repetition
		if(event->Is(EW_EVENT_MOUSEBUTTON|EW_EVENT_KEYBOARD,EW_ITEM_ALL,EW_STATE_PRESS))
			{
//			osdMemcpy(GetLastEvent(),event,sizeof(EW_Event));

			if(GetLastEvent()->GetChoice()!=event->GetChoice())
				event->SetRepetition(0);

			GetLastEvent()->CopyFrom(event);
			}
		}
	}



/******************************************************************************
void	EW_Window::RegisterKillCallback( void (*function)(EW_Window *,long) )

******************************************************************************/
void EW_Window::RegisterKillCallback( void (*function)(EW_Window *,long) )
	{
	kill_callback = function;
	}



/******************************************************************************
void	EW_Window::DoKillCallback(void)

******************************************************************************/
void EW_Window::DoKillCallback(long quit_app)
	{
#if EW_KILL_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"DoKillCallback(quit_app=%d) kill_callback=0x%x",quit_app,kill_callback);
#endif

	if(kill_callback)
		kill_callback(this,quit_app);
	else
		ewDefaultKillCallback(this,quit_app);
	}



/******************************************************************************
void	ewDefaultKillCallback(EW_Window *window,long quit_app)

******************************************************************************/
/*PROTOTYPE*/
void ewDefaultKillCallback(EW_Window *window,long quit_app)
	{
#if EW_KILL_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"DoKillCallback() quit_app=%d default to %s()",quit_app,quit_app?"EndLoop":"Close");
#endif

	if(!quit_app)
		{
		if(window->GetOpened())
			window->Close();

		// if no windows are currently opened, quit the application
		quit_app=TRUE;

		EW_Window *node;
		window->GetEventContext()->GetWindowList()->ToHead();
		while( (node=window->GetEventContext()->GetWindowList()->PostIncrement()) != NULL )
			{
			if(node->GetOpened() && node->GetMapped() && node!=window->GetEventContext()->GetPhantom())
				{
				EW_PRINT(EW_WINDOW,EW_LOG,"DoKillCallback() 0x%x (%s) still open and mapped",node,node->GetTitle());

				quit_app=FALSE;
				break;
				}
			}

#if EW_KILL_DEBUG
		if(quit_app)
			EW_PRINT(EW_WINDOW,EW_LOG,"DoKillCallback() all windows closed -> calling EndLoop()");
		else
			EW_PRINT(EW_WINDOW,EW_LOG,"DoKillCallback() windows remain open -> continuing");
#endif
		}

	if(quit_app)
		{
#if TRUE
		window->GetEventContext()->EndLoop();
#else
		// try three ways
		ewExit(0);
		EW_PRINT(EW_WINDOW,EW_FATAL,"DoKillCallback() default quit_app failed to ewExit()");
		exit(1);
#endif
		}
	}



/******************************************************************************
void	EW_Window::ConfirmSize(void)

******************************************************************************/
void EW_Window::ConfirmSize(void)
	{
	#if EW_WL==EW_XWIN

		if(GetOpened() && GetMapped())
			{
			// confirm current size
			Window rootwin=0;
			int x,y;
			unsigned int sx,sy,border,depth;
			long osx,osy;

			GetSize(&osx,&osy);

			if( XGetGeometry(display,win,&rootwin,&x,&y,&sx,&sy,&border,&depth) == False )
				EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::GetSize() could not get window geometry from XGetGeometry()");

#if EW_RESIZE_DEBUG
			EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::ConfirmSize() 0x%x %d,%d -> %d,%d %d,%d",(long)this,osx,osy,x,y,sx,sy);
#endif

			if(osx!=(long)sx || osy!=(long)sy)
				{
#if EW_RESIZE_DEBUG
				EW_PRINT(EW_WINDOW,EW_LOG,"Window 0x%x changed size %d,%d -> %d,%d",(long)this,osx,osy,sx,sy);
#endif

//				SetSize(sx,sy);
				UpdateSize();
				}
			}

	#endif
	}



/******************************************************************************
void	EW_Window::Activate(void)

	intended to explicitly pass on focus

	NOOP in X

******************************************************************************/
void EW_Window::Activate(void)
	{
#if EW_RAISE_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::%sActivate%s() (%s)",
									ewColorCode(EW_CODE_CYAN),ewColorCode(EW_CODE_NORMAL),this->GetTitle());
#endif

#if EW_WL==EW_WIN32
	SetForegroundWindow(hwnd);
#endif
	}



/******************************************************************************
void	EW_Window::Raise(long force)

	force=
		FALSE:	raise if WM allows
		TRUE:	force WIN32 raise by activating
		2:		force, but don't restack modals

******************************************************************************/
void EW_Window::Raise(long force)
	{
#if EW_RAISE_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::%sRaise%s(%d) mapped=%d (%s)",
									ewColorCode(EW_CODE_CYAN),ewColorCode(EW_CODE_NORMAL),force,mapped,this->GetTitle());
#endif

	if(!mapped)
		return;

#if EW_WL==EW_XWIN

	XRaiseWindow(display,win);

#elif EW_WL==EW_WIN32

//	BringWindowToTop(hwnd);

	if(force)
		Activate();

	SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);

#endif

	if(force<2)
		{
		EW_EventContext *context=GetEventContext();
		if(context)
			context->RestackModalWindows();
		}
	}



/******************************************************************************
void	EW_Window::Lower(long force)

******************************************************************************/
void EW_Window::Lower(long force)
	{
	if(!mapped)
		return;

#if EW_WL==EW_XWIN

	XLowerWindow(display,win);

#elif EW_WL==EW_WIN32

	SetWindowPos(hwnd,HWND_BOTTOM,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);

#endif
	}



/******************************************************************************
void	EW_Window::PropagateTitle(void)

******************************************************************************/
void EW_Window::PropagateTitle(void)
	{
	if(!opened && !mapped)
		return;

#if EW_WL==EW_XWIN
	XTextProperty windowName;
	char *titles[1];
	titles[0]=titlebuffer;
	if(XStringListToTextProperty(titles,1,&windowName) == 0)
		{
		EW_PRINT(EW_WINDOW,EW_ERROR,"PropagateTitle() could not allocate space for window title");
		}

#if EW_WINDOW_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"PropagateTitle() XSetWMName()");
#endif

	XSetWMName(display,win,&windowName);
	XFree((void *)windowName.value);

#elif EW_WL==EW_WIN32

	if(!SetWindowText(hwnd,titlebuffer))
		EW_PRINT(EW_WINDOW,EW_LOG,"PropagateTitle() SetWindowText() failed (%s)",titlebuffer);
#endif
	}



/******************************************************************************
void	EW_Window::SetDoWork(long set)

******************************************************************************/
void EW_Window::SetDoWork(long set)
	{
	do_work=set;

	if(set)
		InitiateWork();
	}



/******************************************************************************
void	EW_Window::InitiateWork(void)

******************************************************************************/
void EW_Window::InitiateWork(void)
	{
#if EW_WL==EW_WIN32

#if FALSE
	// post a Win32 message to start/continue work
	if(!PostMessage(hwnd,EW_WINDOW_WIN32_IDLE_MESSAGE,EW_WINDOW_WIN32_IDLE_VALUE,0))
		EW_PRINT(EW_EVENT,EW_ERROR,"Win32 could not post message");
#else
	// use Win32 timer id 1 as work function
//                  if( !KillTimer(hwnd,1) )
//                      EW_PRINT(EW_EVENT,EW_ERROR,"Win32 could not kill workfunction timer");

	long id;
	if( !(id=SetTimer(hwnd,1,EW_WORKFUNCTION_TIMER_INTERVAL,(TIMERPROC)NULL) ) )
		EW_PRINT(EW_EVENT,EW_ERROR,"Win32 could not start workfunction timer");
#if FALSE
	else
		{
		char message[128];
		sprintf(message,"NEW TIMER hwnd %d id=%d",hwnd,id);
		EW_PRINT(EW_EVENT,EW_LOG,message);
		}
#endif
#endif

#endif
	}



/******************************************************************************
void	EW_Window::Modalize(void)

******************************************************************************/
void EW_Window::Modalize(void)
	{
#if EW_WL==EW_WIN32

#if EW_MODAL_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Modalize()");
#endif

//	SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
//	Win32_CheckStyle();
#endif
	}



/******************************************************************************
void	EW_Window::DeModalize(void)

******************************************************************************/
void EW_Window::DeModalize(void)
	{
#if EW_WL==EW_WIN32

#if EW_MODAL_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::DeModalize()");
#endif

//	SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
//	Win32_CheckStyle();
#endif
	}



#if EW_WL==EW_WIN32
/******************************************************************************
HWND EW_Window::GetConsoleHWND(void)

	static

	hack from MS Knowledge base

******************************************************************************/
HWND EW_Window::GetConsoleHWND(void)
	{
    #define MY_BUFSIZE 1024 // buffer size for console window titles
    HWND hwndFound;         // this is what is returned to the caller
    char pszNewWindowTitle[MY_BUFSIZE]; // contains fabricated WindowTitle
    char pszOldWindowTitle[MY_BUFSIZE]; // contains original WindowTitle

    // fetch current window title
    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    // format a "unique" NewWindowTitle
    wsprintf(pszNewWindowTitle,"%d/%d",
                GetTickCount(),
                GetCurrentProcessId());

    // change current window title
    SetConsoleTitle(pszNewWindowTitle);

    // ensure window title has been updated
    Sleep(40);

    // look for NewWindowTitle
    hwndFound=FindWindow(NULL, pszNewWindowTitle);

    // restore original window title
    SetConsoleTitle(pszOldWindowTitle);

    return hwndFound;
	}



/******************************************************************************
void	EW_Window::ShowConsole(HWND console,long show)

	static

******************************************************************************/
void EW_Window::ShowConsole(HWND console,long show)
	{
	if(show)
		ShowWindow(console,SW_SHOWNORMAL);
	else
		ShowWindow(console,SW_SHOWMINIMIZED);
	}
#endif

void EW_WSHandler::WSHandle()
{
	/* NOOP */
}

