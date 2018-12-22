/******************************************************************************

Coral Tree ew/widget.h
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


#define EW_MIN_WIDGET_SIZE			8


// EW_Widget::bounds flag
#define EW_BOUNDS_NONE				0x00
#define EW_BOUNDS_USE_MAX			0x01
#define EW_BOUNDS_FILL_PARENT		0x02
#define EW_BOUNDS_USE_PARENT_MASK	(EW_BOUNDS_USE_MAX|EW_BOUNDS_FILL_PARENT)
#define EW_BOUNDS_IGNORE_PARENT		0x04

// EW_Widget::SetClipping() flag
#define EW_CLIPPING_OFF				0
#define EW_CLIPPING_CHILDREN		1
#define EW_CLIPPING_ALL				2

// EW_Widget::IsClipped() return value
#define EW_CLIPPED_NOT				0
#define EW_CLIPPED_FULL				2

// EW_Widget::descend_mode bitwise settings
#define EW_DESCEND_NULL				0
#define EW_DESCEND_IN_CHILD_WIDGET	1
#define EW_DESCEND_OUT_WINDOW		2
#define EW_DESCEND_IN_WINDOW		4

#define EW_HORIZONTAL				0
#define EW_VERTICAL					1


class EW_Resize;



/******************************************************************************
class	EW_Widget : public EW_Node

Keywords:
******************************************************************************/
class EW_Widget : public EW_Node, public EW_SizeConstraints
	{
	private:

		long				truex,truey;	// lower left corner in true 
											// window coordinates

		EW_String			name;			// debug identifier
		long				bounds[2];		// simple bounding flags (0=horiz)
		long				clipping;		// clip widget drawing and all descendents
		long				clipx,clipy;	// clip bounds (-1 = edge)
		long				clipw,cliph;
		long				clippable;

		long				translatex,translatey;	// translation of this widget's children
		long				translatable;	// TRUE=this widget can be translated under it's parent
		long				descend_mode;	// how descend passes through events

		long				inversion;		// TRUE = children relative positions inverted in y 
		long				dormant;		// TRUE = block Operate's 
		long				greyout;		// TRUE = draw haze of area
		long				invisible;		// TRUE = block Draw  
		long				smart_clear;	// TRUE = no auto clear
		long				compound;		// hint that children needn't be dumped
		long				retain_limits;	// propagate resize's min/max/pref

		EW_Resize			last_resize;	// copy of last resize for debugging

		EW_Rectangle		lastgeom;		// real geom of last draw
		EW_Rectangle		lastvisible;	// clipped geom of last draw

static	long				default_event_mask_set;
static	EW_Event			default_event_mask;
static	EW_Event			full_event_mask;
		EW_Event			*local_event_mask;
		EW_Event			*event_mask;

	private:
	/*********************************************************************/
	/*                WIDGET HIERARCHY DATA MEMBERS                      */
	/*                                                                   */
	/*********************************************************************/
		EW_Widget			*parentwidget;		// parent widget
		EW_Window			*rootwindow;		// root window

	protected:

		void			SetCompound(long set)	{ compound=set; };

		void			SetSmartClear(long set)		{ smart_clear=set; };
		long			GetSmartClear(void)			{ return smart_clear; };

	public:
						EW_Widget();
virtual					~EW_Widget();

	//////////////////////////////////////////////////////////////////////
	//						Widget Hierarchy Organization
	//////////////////////////////////////////////////////////////////////
		void			SetRootWindow(EW_Window *window);
		EW_Window		*GetRootWindow(void)		{ return rootwindow; };

		void			SetParentWidget(EW_Widget *widget);
		EW_Widget		*GetParentWidget(void)		{ return parentwidget; };

		void			SetName(char *string)	{ name.NewBuffer(string); };
		char			*GetName(void)			{ return name.Buffer(); };
		EW_String		*GetNameString(void)	{ return &name; };

	//////////////////////////////////////////////////////////////////////
	//						Event-Related
	//////////////////////////////////////////////////////////////////////

		// avoid using GetFullEventMask(), can lead to future conflict
		// it is better to just activate known additional event types
		void		SetEventMask(EW_Event *set)	{ event_mask=set; };
		EW_Event	*GetEventMask(void)			{ return event_mask; };
		EW_Event	*GetDefaultEventMask(void)	{ return &default_event_mask; };
		EW_Event	*GetFullEventMask(void)		{ return &full_event_mask; };
		EW_Event	*GetLocalEventMask(void);

		void SetDormant(long set)		{ dormant=set; };
		long GetDormant(void)			{ return dormant; };

		void SetInversion(long set)		{ inversion=set; };
		long GetInversion(void)			{ return inversion; };

		void SetDescendMode(long set)	{ descend_mode = set; }
		long GetDescendMode(void)		{ return descend_mode; }

	//////////////////////////////////////////////////////////////////////
	//						Geometry and Drawing-Related
	//////////////////////////////////////////////////////////////////////

		void SetOrigin(long x,long y)
				{
				EW_Rectangle::SetOrigin(x,y);

				if(rootwindow || parentwidget)
					CalcTruePosition();
				}

		void SetSize(long w,long h)
				{
				EW_Rectangle::SetSize(w,h);

				if(rootwindow || parentwidget)
					CalcTruePosition();
				};

		void CalcTruePosition(long recursive=FALSE);

		void SetTrueOrigin(long x,long y);
		void GetTrueOrigin(long *x,long *y);
		void SetTrueGeometry(long x,long y,long w,long h);
		void GetTrueGeometry(long *x,long *y,long *w,long *h);

		void GetVisibility(long *x,long *y,long *sx,long *sy);
		void GetLastVisibility(long *x,long *y,long *sx,long *sy);
		void GetLastTrueGeometry(long *x,long *y,long *sx,long *sy);

		// replaces virtual EW_SizeConstraints::GetMinSize()
		void GetMinSize(long *x,long *y)
				{
				*x=EW_MAX(EW_MIN_WIDGET_SIZE,minx);
				*y=EW_MAX(EW_MIN_WIDGET_SIZE,miny);
				};

	//////////////////////////////////////////////////////////////////////
	//						Infrastructure
	//////////////////////////////////////////////////////////////////////

		void GetParentTrueGeometry(long *x,long *y,long *w,long *h);

		void SetGreyOut(long set)		{ greyout=set; };
		long GetGreyOut(void)			{ return greyout; };

		void SetInvisible(long set)		{ invisible=set; };
		long GetInvisible(void)			{ return invisible; };

		void SetClipping(long set)		{ clipping=set; };
		long GetClipping(void)			{ return clipping; };

		void SetRetainLimits(long set)	{ retain_limits=set; };
		long GetRetainLimits(void)		{ return retain_limits; };

		void SetClipRegion(long x,long y,long w,long h)
				{ clipx=x; clipy=y; clipw=w; cliph=h; };
		void GetClipRegion(long *x,long *y,long *w,long *h)
				{ *x=clipx; *y=clipy; *w=clipw; *h=cliph; };

		void SetClippable(long set)	{ clippable=set; };
		long GetClippable(void)		{ return clippable; };

		long IsClipped(void);

		void SetTranslation(long x,long y)
				{ translatex=x; translatey=y; };
		void GetTranslation(long *x,long *y)
				{ *x=translatex; *y=translatey; };

		void SetTranslatable(long set)	{ translatable=set; };
		long GetTranslatable(void)		{ return translatable; };

		void SetBounds(long set)
				{
				SetBounds(EW_HORIZONTAL,set);
				SetBounds(EW_VERTICAL,set);
				};
		void SetBounds(long dir,long set)	{ bounds[dir%2]=set; };
		long GetBounds(long dir)			{ return bounds[dir%2]; };

		void Redraw(EW_Redraw *redraw);

virtual	void ClearBackground(void);
		void ClearBackground(long colorindex);

virtual	long CheckSize(EW_Resize *resize);

		// replaces virtual EW_Node::SetDirty()
		void SetDirty(long set);

		// for debugging
		EW_Resize	*GetLastResize(void)		{ return &last_resize; };

	public:
	/*********************************************************************/
	/*                     WIDGET MAKER INTERFACE                        */
	/*                                                                   */
	/*********************************************************************/
		// By default Descend calls PreOperate, descends the hierarchy and
		// calls PrePostOperate.  Note that a widget providing its own
		// Descend() will not automatically call Operate or PostOperate.
		//
		// PreOperate and PrePostOperate potentially call
		// Operate and PostOperate using some checks.
		// replacement of PreOperate and PrePostOperate is intended for the
		// widget system, not the widgets themselves
		//
		// Operate is done before the operation of child widgets
		// PostOperate is done after the operation of child widgets
		//
		// previously returned TRUE|FALSE indicating whether a resize is needed 
		// return value now IGNORED and RESERVED
virtual long	Descend(EW_Event *event);
virtual	long	PreOperate(EW_Event *event);
virtual	long	PrePostOperate(EW_Event *event);
virtual long	Operate(EW_Event *event)		{ return FALSE; };
virtual long	PostOperate(EW_Event *event)	{ return FALSE; };

		// If Resize() is redone in a derived widget, CalcTruePosition()
		// should be called.  If a basic recursive descension is desired
		// then widgetlist->Resize() should be called from Resize().
		// A replaced widgetlist->Resize() should call the child's
		// PreResize(), not their Resize()
		//
		// previously returned TRUE|FALSE to indicate whether resizing was done
		// return value now IGNORED and RESERVED
		long	PreResize(EW_Resize *resize);
virtual long	Resize(EW_Resize *resize);

virtual void	DrawOnTablet(EW_Redraw *redraw);
virtual void	PostDrawOnTablet(EW_Redraw *redraw);
virtual void	Draw(EW_Redraw *redraw)			{ return; };
virtual void	PostDraw(EW_Redraw *redraw)		{ return; };

virtual	void	DumpString(void)		{};
virtual	void	DumpInfo(long level)	{};
		long	GetCompound(void)	{ return compound; };

	};



/******************************************************************************
class	EW_WidgetList : public GPL_IDoubleList<EW_Widget>

Keywords:
******************************************************************************/
class EW_WidgetList : public GPL_IDoubleList<EW_Widget>
	{
	public:
				EW_WidgetList(void)		{};
virtual			~EW_WidgetList(void);

		long	Descend(EW_Event *event,long mode);
		long	Resize(EW_Resize *resize);
		void	DumpInfo(long level,long recursive,long brief);
	};
