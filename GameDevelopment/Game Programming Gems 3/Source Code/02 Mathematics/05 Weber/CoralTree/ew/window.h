/******************************************************************************

Coral Tree ew/window.h
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


// mutually exclusive flags sent to constructor
#define EW_WINDOW_FLAG_NULL				0x00
#define EW_WINDOW_BORDERLESS			0x01
#define EW_WINDOW_FIXED_SIZE			0x02
#define EW_WINDOW_TRANSIENT				0x04
#define EW_WINDOW_NOMAP					0x08
#define EW_WINDOW_PHANTOM_FLAG			0x10
#define EW_WINDOW_PHANTOM				(EW_WINDOW_PHANTOM_FLAG|EW_WINDOW_NOMAP)
#define EW_WINDOW_MODAL					0x20
#define EW_WINDOW_MODAL_EXCLUSIVE_FLAG	0x40
#define EW_WINDOW_MODAL_EXCLUSIVE		(EW_WINDOW_MODAL| \
											EW_WINDOW_MODAL_EXCLUSIVE_FLAG)


#define EW_WINDOW_DEFAULT_IX	100
#define EW_WINDOW_DEFAULT_IY	100
#define EW_WINDOW_DEFAULT_SX	100
#define EW_WINDOW_DEFAULT_SY	100


// SetOriginBy(???,*window);
#define EW_POSITION_RELATIVE_TO	0
#define EW_POSITION_OVER		1


#if EW_WL==EW_WIN32

#define EW_WINDOW_WIN32_IDLE_MESSAGE	WM_TIMER		// Win32 message to propagate work function
#define EW_WINDOW_WIN32_IDLE_VALUE		(WPARAM)(-1)	// idle message value

#define EW_WORKFUNCTION_TIMER_INTERVAL	10

#endif

#define EW_XDBUF_MAX_PIXELS		16
#define EW_XDBUF_MAX_PLANES		4

#define EW_XDBUF_MAX_COLORS		(EW_XDBUF_MAX_PIXELS*(1<<EW_XDBUF_MAX_PLANES))

#define EW_MAX_TITLE_BUFFER		1024


#if EW_WL==EW_XWIN
	#define EW_DISPLAY_TYPE	Display *
#elif EW_WL==EW_WIN32
	#define EW_DISPLAY_TYPE	HWND
#else
	#define EW_DISPLAY_TYPE	long
#endif

#define	EW_XPM_FLAG_NONE		0
#define	EW_XPM_FLAG_NO_DITHER	1

// EW_Window::SetModal()
#define	EW_MODAL_NOT			0
#define	EW_MODAL_NONEXCLUSIVE	1
#define	EW_MODAL_EXCLUSIVE		2


/******************************************************************************
class EW_XColorBuffer

	without EW_GL==EW_X_GFX, defined but useless

Keywords:
******************************************************************************/
class EW_XColorBuffer
	{
	public:

#if EW_GL==EW_X_GFX

		XColor			*map;
		GC				context;

#endif
	};



/******************************************************************************
class	EW_ClipRegion

******************************************************************************/
class EW_ClipRegion : public EW_RectangleList
	{
	private:
		long		mirror;		// where a rewind copied from

	public:
					EW_ClipRegion(void)
						{
						mirror= -1;
						}

		void		SetMirror(long set)		{ mirror=set; };
		long		GetMirror(void)			{ return mirror; };
	};



/******************************************************************************
class	EW_ClipRegionStack

******************************************************************************/
class EW_ClipRegionStack : public GPL_DoubleList<EW_ClipRegion>
	{
	private:
		EW_ClipRegion		*current;
		long				index;

	public:
							EW_ClipRegionStack(void)
								{
								index= -1;
								current=NULL;
								SetCurrentIndex(0);
								};

virtual						~EW_ClipRegionStack(void)	{ ClearList(); };

		void				ClearList(void);

		long				GetCurrentIndex(void)		{ return index; };
		EW_ClipRegion		*SetCurrentIndex(long set);

		EW_ClipRegion		*GetCurrentRegion(void)		{ return current; };
	};


/******************************************************************************
class EW_TipHandler

******************************************************************************/
class EW_WSHandler
{
	public:
		EW_WSHandler()
		{	
		}
virtual	~EW_WSHandler(){}
virtual void WSHandle();
};


/******************************************************************************
class	EW_Window : public EW_Node


Keywords:
******************************************************************************/
class EW_Window : public EW_Node
{
	private:
		long	DoOpen(void);

	public:
	/*********************************************************************/
	/*                    WINDOW CONTROL INTERFACE                       */
	/* These functions are implemented in window.cc.                     */
	/*                                                                   */
	/*********************************************************************/
				EW_Window(void);
virtual			~EW_Window(void);

virtual	void	SetOrigin(long x,long y);
virtual	void	SetSize(long w,long h);
		void	AffectGeometry(void);

		void	Open(char *title)
					{
					SetTitle(title);
					Open();
					};
		void	Open(long mode=EW_POSITION_OVER,EW_Window *relation=NULL);
		void	Close();

		long	Map(long mode=EW_POSITION_RELATIVE_TO,EW_Window *relation=NULL);
		long	UnMap(void);

		void	SetOriginBy(long mode,EW_Window *relation);

		void	PropagateTitle(void);
		void	SetTitle(char *set)
					{
					strcpy(titlebuffer,set);
					PropagateTitle();
					};
		char	*GetTitle(void)		{ return titlebuffer; };

		void	SetFlags(long set)	{ flags=set; }
		long	GetFlags(void)		{ return flags; }

		long	IsFixedSize(void);

		void	DelayedDelete(void)			{ delete_me=TRUE; };
		void	SetDelayedDelete(long set)	{ delete_me=set; };
		long	GetDelayedDelete(void)		{ return delete_me; };

		/* refresh window ( use Refresh(void) for default full refresh ) */
		void	Refresh(void);
		void	Refresh(EW_Redraw *redraw);

		/* top of resize pass  */
		long	Resize(void);

		//	Set/Get number of colors in window
		void	SetNumberColors(long set);
		long	GetNumberColors(void)		{ return number_colors; };

		/* activate/deactivate double-buffering */
		void	SetDoubleBuffer(long set);
		long	GetDoubleBuffer(void)		{ return double_buffer; };

		/* activate/deactivate RGB color buffering (ignored unless OpenGL) */
		void	SetRGBMode(long set)		{ rgbMode=set; };
		long	GetRGBMode(void)			{ return rgbMode; };
static	void	SetDefaultRGBMode(long set)	{ defaultRGBMode=set; };
static	long	GetDefaultRGBMode(void)		{ return defaultRGBMode; };

		/* call WinSet() before drawing graphics to specific window */
		void	WinSet(long setfont=TRUE);
		void	UseFullScreenOrthoViewport(void);

		/* call UpdateSize() to update size location and size information */
		void	UpdateSize(void);

		void	ConfirmSize(void);

		/* dump info - WARNING: alters the linked list next-values */
		void	Dump(long brief);

		// for event context only
		long 	IsModal(void)	{ return (GetFlags()&EW_WINDOW_MODAL)!=0; }

		long 	GetModal(void)		{ return modal; }
		void 	SetModal(long set)	{ modal=set; }

		void	Modalize(void);
		void	DeModalize(void);

		void	Activate(void);
		void	Raise(long force=FALSE);
		void	Lower(long force=FALSE);

		/* is window mapped */
		long 	GetMapped(void)		{ return mapped; }
		void 	SetMapped(long set)	{ mapped=set; }		// for event loop only

		long	GetOpened(void)			{ return opened; }

		/* pointer capture functions */
		long 	GrabPointer(void);
		long 	ReleasePointer(void);

		/* event processing call */
		void 	ProcessEvent(EW_Event *event,long primary);

		/* register kill function */
		void	RegisterKillCallback( void (*function)(EW_Window *,long) );
		void	DoKillCallback(long quit_app);

		void	SetIgnoreNextExposure(long set)	{ ignore_exposure=set; };
		long	GetIgnoreNextExposure(void)		{ return ignore_exposure; };

		long	GetDoWork(void)		{ return do_work; };
		void	SetDoWork(long set);

		void	InitiateWork(void);

	public:
	/*********************************************************************/
	/*                     WINDOW DRAWING INTERFACE                      */
	/* These functions are implemented in gfx.cc.                        */
	/*                                                                   */
	/*********************************************************************/
		void SetColor(long type,long index);
		long GetColor(long type);

		void Clear(void);
		void Flush(long block);
		void BufferSwap(void);

		void GreyOut(long x,long y,long sx,long sy,long colorindex);
		void GreyOut(long x,long y,long sx,long sy)
				{ GreyOut(x,y,sx,sy,index_widget); };

		void LineOut(long x,long y,long sx,long sy,long offset,long inc,
															long colorindex);
		void LineOut(long x,long y,long sx,long sy,long offset,long inc)
				{ LineOut(x,y,sx,sy,offset,inc,index_widget); };

		void RectWire(long left,long bottom,long right,long top,
															long colorindex);
		void RectWire(long left,long bottom,long right,long top)
				{ RectWire(left,bottom,right,top,index_pen); };

		void RectFill(long left,long bottom,long right,long top,
															long colorindex);
		void RectFill(long left,long bottom,long right,long top)
				{ RectFill(left,bottom,right,top,index_pen); };

		void ArcWire(long cx,long cy,long rx,long ry,float start,float end,
															long colorindex)
				{ DrawArc(FALSE,cx,cy,rx,ry,start,end,colorindex); }
		void ArcWire(long cx,long cy,long rx,long ry,float start,float end)
				{ ArcWire(cx,cy,rx,ry,start,end,index_pen); };
		void ArcFill(long cx,long cy,long rx,long ry,float start,float end,
															long colorindex)
				{ DrawArc(TRUE,cx,cy,rx,ry,start,end,colorindex); }
		void ArcFill(long cx,long cy,long rx,long ry,float start,float end)
				{ ArcFill(cx,cy,rx,ry,start,end,index_pen); };

		void ArcWireFast(long cx,long cy,long rx,long ry,float start,float end,
															long colorindex);
		void ArcWireFast(long cx,long cy,long rx,long ry,float start,float end)
				{ ArcWireFast(cx,cy,rx,ry,start,end,index_pen); };

		void DrawArc(long fill,long cx,long cy,long rx,long ry,
										float start,float end,long colorindex);
		void PolygonalArc(long fill,long cx,long cy,long rx,long ry,
										float start,float end,long colorindex);
		void SymmetricArc(long fill,long cx,long cy,long rx,long ry,
										float start,float end,long colorindex);

		void PolyWire(long closed,long vert[][2],long vertices,long colorindex);
		void PolyWire(long closed,long vert[][2],long vertices)
				{ PolyWire(closed,vert,vertices,index_pen); };

		void PolyFill(long vert[][2],long vertices,long colorindex);
		void PolyFill(long vert[][2],long vertices)
				{ PolyFill(vert,vertices,index_pen); };

		void TriangleWire(long a[2],long b[2],long c[2],long colorindex);
		void TriangleWire(long a[2],long b[2],long c[2])
				{ TriangleWire(a,b,c,index_pen); };

		void TriangleFill(long a[2],long b[2],long c[2],long colorindex);
		void TriangleFill(long a[2],long b[2],long c[2])
				{ TriangleFill(a,b,c,index_pen); };

		void Line(long x1,long y1,long x2,long y2,long colorindex);
		void Line(long x1,long y1,long x2,long y2)
				{ Line(x1,y1,x2,y2,index_pen); };

		void Point(long x,long y,long colorindex);
		void Point(long x,long y)
				{ Point(x,y,index_pen); };

		void ImageDraw(long posx,long posy,long lenx,long leny,
								unsigned char *buffer,long transparent=FALSE);
		void ImageCopy(long srcx,long srcy,long destx,long desty,
														long sx,long sy);

		void RingBell(void);



		/*****************************************************************/
		// 			CLIPPING RECTANGE FUNCTIONS
		//					(clip.cc)
		//
		/*****************************************************************/

#if TRUE
	public:
		long ClipRegionIntersectsBounds(long x,long y,long sx,long sy);

		void GetClipBounds(long *x,long *y,long *sx,long *sy);
		void CopyClipRegionFrom(EW_RectangleList *other);
		void CopyClipRegionTo(EW_RectangleList *other);

		void IntersectClipRegion(long x,long y,long sx,long sy);
		// add later
//		void IntersectClipRegion(EW_RectangleList *other);

		void PushClipRegion(void);
		void PopClipRegion(void);

		void RewindClipRegion(void);
		void RestoreClipRegion(void);

		// for event loop to set up exposure
		EW_ClipRegionStack	*GetClipStack(void)	{ return &clipstack; };

	private:

		void SetAnyClipRegion(long level,EW_RectangleList *set);
		void ResetClipRegion(long set);

		EW_ClipRegion baseclip;
		long clip_level;
		EW_ClipRegionStack clipstack;
#else
	public:
		void GetClipRegion(long *x,long *y,long *sx,long *sy);
		void SetClipRegion(long x,long y,long sx,long sy);
		void IntersectClipRegion(long x,long y,long sx,long sy);

		void PushClipRegion(void);
		void PopClipRegion(void);

	private:

		void SetAnyClipRegion(long level,long x,long y,long sx,long sy);
		void ResetClipRegion(long set);
		void ResizeClipBuffer();

		long clip_me;
		long clip_level;
		long clip_buffer_level;
		long *clip_stack;
#endif


	/*********************************************************************/
	/*                        TEXT INTERFACE                             */
	/* These functions are implemented in text.cc                        */
	/*                                                                   */
	/*********************************************************************/
	protected:
		long	ProbeFontHeight(char *name);

	public:
		/*****************************************************************/
		/* FONT SPECIFICATION INTERFACE                                  */
		/* EW_Font is the 'handle' to a font in EW.  A Font is specified */
		/* with a EW_FontRequest structure.                              */
		/*****************************************************************/
		long	LoadFont(EW_Font *fontdata,EW_FontRequest *request);

		long	SetFont(EW_Font *fontdata);
		EW_Font	*GetFont(void)				{ return &current_font; };

		void	CopyCurrentFontFrom(EW_Font *fontdata);

		long	CopyDefaultFontTo(EW_Font *fontdata);
		long	CopyDefaultFontFrom(EW_Font *fontdata);

		void	SetUnderline(long setting);
		long	GetUnderline(void)			{ return underline; };

		/*****************************************************************/
		/* INFORMATION QUERIES                                           */
		/*****************************************************************/
		long CharWidth(char c);
		long StringWidth(const char *string);
		long FindFontAttribute(long type);
		long GetFontHeight(void);

		// set a fontheight so GetFontHeight() valid before window is opened
		// use carefully as it may reset a true value
		void SetTemporaryFontHeight(long set)
				{
				font_height=set;
				};

		/*****************************************************************/
		/* DRAWING TEXT                                                  */
		/*****************************************************************/
		void Text(long x,long y,char *string,long len,long shadow,
															long colorindex)
				{
				if(shadow)
					DrawTextWithShadow(x,y,string,len,colorindex);
				else
					DrawText(x,y,string,len,colorindex);
				}
		void Text(long x,long y,char *string,long len,long shadow)
				{
				Text(x,y,string,len,shadow,index_pen);
				};

	private:
		void DrawText(long x,long y,char *string,long len,long colorindex);
		void DrawTextWithShadow(long x,long y,char *string,long len,
															long colorindex);



	//////////////////////////////////////////////////////////////////////
	//                  INTERNAL MEMBER DATA
	// These data are private as to not be accessed by the user.
	//////////////////////////////////////////////////////////////////////

	private:

		long			delete_me;
		long			ignore_exposure;
		EW_Event		last_event;
		EW_EventContext	*event_context;
static	long			dx, dy;			// display (screen) dimensions
static	EW_Window		*setwindow;
		long			rgbMode;
static	long			defaultRGBMode;

		EW_Event		*GetLastEvent(void)				{ return &last_event; };

static	EW_Window		*GetSetWindow(void)				{ return setwindow; };

		#if EW_WL==EW_XWIN

static		Display		*display;
static		int			screen;
static		Atom		atom[2];
			GC			graphics_context;
			Window		win;
			Colormap	colormap;
			XVisualInfo	visual_information;

static		Colormap	shared_colormap;
static		Colormap	shared_colormap_double;
static		Colormap	shared_colormap_rgb;
static		Colormap	shared_colormap_rgb_double;

static		long		colormap_allocated;

			#if EW_GL==EW_OPENGL

				GLXContext	glx_context;

			#else

				// colormap double-buffering under X
static			long			xdbuf_allocated;
static			long			xdbuf_colormapped;
static			long			xdbuf_colormapped_colors;
static			unsigned long	xdbuf_planelist[EW_XDBUF_MAX_PLANES];
static			unsigned long	xdbuf_pixellist[EW_XDBUF_MAX_PIXELS];
static			XColor			xdbuf_mapping[2][EW_XDBUF_MAX_COLORS];
static			long			xdbuf_currentbufferindex;
				GC				xdbuf_context[2];

				long	Xdbuf_InitColorBuffer(void);
static			void	Xdbuf_CopyColor(XColor *def,XColor *color,
														unsigned long pixel);
				void	Xdbuf_SwapBuffers(void);

			#endif

		#elif EW_WL==EW_MEX 

			long			wid;

		#elif EW_WL==EW_WIN32

			HWND			hwnd;
			HDC				hdc;
			DWORD			dwstyle;
			DWORD			dwexstyle;
			long			last_mx;		// last known mouse location
			long			last_my;		
			long			idle_mx;		// last idle mouse location
			long			idle_my;		
			EW_Event		storeevent;
			long			item[1];
			long			state[1];
			long			force_idlemouse;
			long			windrag;		// dragging mouse with button down
			long			was_iconic;
			long			font_set;
static		long			first_map;
static		HPALETTE		hpal;
			NPLOGPALETTE	logpalette;
			void			Win32_CheckStyle(void);
			void			Win32_CalcStyle(DWORD *dwstyle,DWORD *dwexstyle);

			#if EW_GL==EW_OPENGL // under Win32

				HGLRC		hglrc;

			#elif EW_GL==EW_GDI // under Win32

				HPEN		hpen[EW_MAXCOLORS];
				HBRUSH		hbrush[EW_MAXCOLORS];
				HBRUSH		hbrush_invisible;

			#endif

		#else
			#error NO HOST EVENT WINDOW SYSTEM SPECIFIED
		#endif


	//////////////////////////////////////////////////////////////////////
	//                  INTERNAL MEMBER FUNCTIONS
	// These function members should not accessed by the user.
	// They are for internal EW use only.
	//////////////////////////////////////////////////////////////////////

	public:
static	void			SetSetWindow(EW_Window *set)	{ setwindow=set; };

		void			SetEventContext(EW_EventContext *set)	{ event_context=set; };
		EW_EventContext *GetEventContext(void)					{ return event_context; };

static	void			GetScreenSize(long *x,long *y)	{ *x=dx; *y=dy; };

		EW_DISPLAY_TYPE	GetDisplayIdentifier(void)
							{
#if EW_WL==EW_XWIN
							return GetDisplay();
#elif EW_WL==EW_WIN32
							return GetHWND();
#else
							return NULL;
#endif
							};

		#if EW_WL==EW_XWIN

static		void	SetDisplay(Display *set)	{ display=set; };
static		Display	*GetDisplay(void)			{ return display; };

static		void	SetScreen(int set)			{ screen=set; };
static		int		GetScreen(void)				{ return screen; };


			XVisualInfo	*GetVisualInfo(void)	{ return &visual_information; };

			GC		GetGC(void)
				 		{
			#if EW_GL==EW_OPENGL
						return graphics_context;
			#else
						if(!GetDoubleBuffer())
							return graphics_context;
						else
							return xdbuf_context[xdbuf_currentbufferindex];
			#endif
						};

static		Atom	GetDeleteAtom(void)			{ return atom[0]; };
static		Atom	GetKillAtom(void)			{ return atom[1]; };

			Window	GetXWindow(void)			{ return win; };

			#if EW_GL!=EW_OPENGL

				long	GetXDBufPixels(void)	{ return GetNumberColors(); };
				long	GetXDBufPlanes(void)
							{
							// log base 2 of colors, rounded up
							return (long)(log(GetNumberColors())/
											log(2.0)-0.01)+1;
							};
				long	GetXDBufColors(void)
							{
							return (GetXDBufPixels()*(1<<GetXDBufPlanes()));
							};

			#endif

		#elif EW_WL==EW_MEX 
		#elif EW_WL==EW_WIN32

			HWND		GetHWND(void)			{ return hwnd; };
			HDC			GetHDC(void)			{ return hdc; };
			EW_Event	*GetStoredEvent(void)	{ return &storeevent; };

			long		GetForceIdleMouse(void)		{ return force_idlemouse; };
			void		SetForceIdleMouse(long set)	{ force_idlemouse=set; };

			void		SetWasIconic(int set)		{ was_iconic=set; };
			int			GetWasIconic(void)			{ return was_iconic; };

			void		SetWinDrag(int set)			{ windrag=set; };
			int			GetWinDrag(void)			{ return windrag; };

			void		GetIdleMouse(long *mx,long *my)
							{ *mx=idle_mx; *my=idle_my; };
			void		SetIdleMouse(long mx,long my)
							{ idle_mx=mx; idle_my=my; };

			void		GetLastMouse(long *mx,long *my)
							{ *mx=last_mx; *my=last_my; };
			void		SetLastMouse(long mx,long my);

static		HWND		GetConsoleHWND(void);
static		void		ShowConsole(HWND console,long show);

		#else
			#error NO HOST EVENT WINDOW SYSTEM SPECIFIED
		#endif


	private:
	/*********************************************************************/
	/*                    PRIVATE DATA MEMBERS                           */
	/*                                                                   */
	/*********************************************************************/
		char			titlebuffer[EW_MAX_TITLE_BUFFER];
		long			double_buffer;
		long			number_colors;
		long			maxevents;

		EW_COLORTYPE	index_pen,index_widget,index_back;
		EW_COLORTYPE	index_lit,index_halflit,index_shadow,index_halfshadow;
static	unsigned long	index_remap[2][EW_MAXCOLORS];
static	long			colors_allocated;
		long			underline;
		long			opened;
		long 			mapped;
		long 			modal;
		unsigned long	flags;		
		long			do_work;	// send work events when nothing else pending

static	short			graphics_initialized;
static	EW_Font			default_font;
		EW_Font			current_font;
		long			font_height;
		long			font_adjustx,font_adjusty;

		void 			(*kill_callback)(EW_Window *,long);

#if EW_GL==EW_OPENGL
		GLint			lastfontbase;
#endif

	/*********************************************************************/
	/*                  WIDGET SET HANDLER HOOKS                         */
	/* This hook is for WDS level widgets that need it to be in windows  */
	/*                                                                   */
	/*********************************************************************/
	private:
		EW_WSHandler	*ws_handler;
	public:
		void			SetWSHandler(EW_WSHandler *wsh)
						{
							ws_handler = wsh;
						}
		EW_WSHandler	*GetWSHandler() { return ws_handler; }

	private:
	/*********************************************************************/
	/*                  WINDOW GRAPHICS INFRASTRUCTURE                   */
	/* These functions are implemented in gfx.cc                         */
	/*                                                                   */
	/*********************************************************************/
		void InitGraphics(void);
		void InitDefaultFont(void);
		void ActivateColor(EW_COLORTYPE index);

	private:
	/*********************************************************************/
	/*                      TEXT INFRASTRUCTURE                          */
	/* These functions are implemented in text.cc                        */
	/*                                                                   */
	/*********************************************************************/
		long MatchFont(EW_FontRequest *request,EW_FontMatch *match);
		long OpenFont(EW_Font *fontdata,EW_FontMatch *match);

};



/******************************************************************************
class	EW_WindowList

Keywords:
******************************************************************************/
class EW_WindowList : public GPL_IDoubleList<EW_Window>
	{
	public:
				EW_WindowList(void)		{};
virtual			~EW_WindowList(void)	{};
	};



/******************************************************************************
class	EW_WindowStack

Keywords:
******************************************************************************/
class EW_WindowStack : public GPL_DoubleList<EW_Window>
	{
	public:
				EW_WindowStack(void)	{};
virtual			~EW_WindowStack(void)	{};
	};
