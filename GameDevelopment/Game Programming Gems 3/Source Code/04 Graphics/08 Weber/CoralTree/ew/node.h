/******************************************************************************

Coral Tree ew/node.h
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



// EW_Widget::dirty values
#define EW_DIRTY_NOT				0	// not
#define EW_DIRTY_CHILD				1	// some descendent needs to be redrawn
#define EW_DIRTY_PARTIAL			2	// this widget should check children
#define EW_DIRTY_COMPLETE			4	// this widget needs to be redrawn
#define EW_DIRTY_MASK				7	// without OR-able flags
#define EW_DIRTY_PENDING			8	// OR-able
#define EW_DIRTY_IF_PENDING			16	// OR-able: on set, change only if
										// EW_DIRTY_PENDING is set

// EW_Node::AddWidget(widget,???)
#define EW_ADD_BEGINNING			0			// before all others
#define EW_ADD_BEFORE				1			// before current
#define EW_ADD_AFTER				2			// after current
#define EW_ADD_END					3			// after all others
#define EW_ADD_DEFAULT				EW_ADD_END	// default

// EW_Node::SetNodeSizing(???)
// x-only and y-only windows are not well supported under Win32 (don't use them)
#define EW_NODE_SIZING_NULL			0x0
#define EW_NODE_SIZING_FIT_CHILD_X	0x1
#define EW_NODE_SIZING_FIT_CHILD_Y	0x2
#define EW_NODE_SIZING_FIT_CHILD	0x3	// resize to fit children



/******************************************************************************
class	EW_Node : public GPL_DoubleNode

	items shared between windows and widgets

	NOTE the double casting through void to placate HP's compiler

Keywords:
******************************************************************************/
class EW_Node : public GPL_DoubleNode, public EW_Rectangle
	{
	protected:

#if	EW_TRACK_NEWED
static	EW_Node			*last_newed;
		long			newed;
#endif /* EW_TRACK_NEWED */
		EW_WidgetList	*widgetlist;

		long			dirty[2];
		long			nodesizing;

virtual	void			AddWidgetProtected(EW_Widget *widget,long placement);
virtual	void			RemoveWidgetProtected(EW_Widget *widget);

	public:

						EW_Node(void);
virtual					~EW_Node(void);

#if	EW_TRACK_NEWED
static	void			*operator new(size_t n);
static	void			operator delete(void *p);
#endif /* EW_TRACK_NEWED */

		EW_WidgetList	*GetWidgetList(void)	{ return widgetlist; };

		long			GetNewed(void)			{ return newed;};

		long			GetDirty(long index)			{ return dirty[index];};
		void			SetDirty(long index,long set);

		long			GetDirty(void)			{ return GetDirty(0); };
virtual	void			SetDirty(long set)
							{
							SetDirty(0,set);
							SetDirty(1,set);
							};

		//	these virtuals are set for EW_Window and replaced for EW_Widget
virtual void			SetRootWindow(EW_Window *window)	{ /* NOP*/; };
virtual EW_Window		*GetRootWindow(void)
							{ return (EW_Window *)((void *)this); };

virtual void			SetParentWidget(EW_Widget *widget)	{ /* NOP*/; };
virtual EW_Widget		*GetParentWidget(void)				{ return NULL; };

		long			IsWindow(void)
							{
							return (this==(EW_Node *)((void *)GetRootWindow()));
							};

		EW_Node			*GetParent(void);

		void			AddWidget(EW_Widget *widget)
							{
							AddWidget(widget,EW_ADD_DEFAULT);
							};
		void			AddWidget(EW_Widget *widget,long placement)
							{
							AddWidgetProtected(widget,placement);
							};
		void			RemoveWidget(EW_Widget *widget)
							{
							RemoveWidgetProtected(widget);
							};

/*
		void	GetGeometry(long *x,long *y,long *w,long *h)
					{
					GetOrigin(x,y);
					GetSize(w,h);
					};
virtual void	SetGeometry(long x, long y, long w, long h)
					{
					// origin last 'cause widgets add some functionality to it
//					EW_Node::SetSize(w,h);
					SetSize(w,h);
					SetOrigin(x,y);
					};

		void	GetOrigin(long *x,long *y)  { *x=position[0]; *y=position[1]; };
virtual	void	SetOrigin(long x,long y)
					{
					position[0] = x;
					position[1] = y;
					}

		void	GetSize(long *w,long *h) { *w=dimension[0]; *h=dimension[1]; };
virtual	void	SetSize(long w,long h)
					{
					dimension[0] = w;
					dimension[1] = h;
					};
*/

		long	GetNodeSizing(void)			{ return nodesizing; };
		void	SetNodeSizing(long set)		{ nodesizing=set; };

		void	GetChildrensExtents(long *mincx,long *mincy,
													long *maxcx,long *maxcy);
		long	ConstrainSize(long hold_top,long bound,long maxx,long maxy);
	};
