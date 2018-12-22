/******************************************************************************

Coral Tree wds/scroll.h
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


#define	WDS_SCROLLBAR_HORIZONTAL	WDS_HORIZONTAL
#define	WDS_SCROLLBAR_VERTICAL		WDS_VERTICAL

// for WDS_ELEMENT_SCROLLBAR_CHILDREN
// WDS_SCROLLBAR_CHILDREN_ALTERNATE overrides WDS_SCROLLBAR_CHILDREN_OPPOSE
#define	WDS_SCROLLBAR_CHILDREN_NONE			0
#define	WDS_SCROLLBAR_CHILDREN_SLAVES		1
#define	WDS_SCROLLBAR_CHILDREN_OPPOSE		2
#define	WDS_SCROLLBAR_CHILDREN_ALTERNATE	4
#define	WDS_SCROLLBAR_CHILDREN_INSIDE		8

// for WDS_ELEMENT_SCROLLREGION_SCROLLABLE
// if not scrollable (like WDS_SCROLLABLE_OFF),
// probably want to use WDS_SCROLLABLE_AUTOSCALE
#define	WDS_SCROLLABLE_OFF			0	// NULL
#define	WDS_SCROLLABLE_ON			1	// region can slide
#define	WDS_SCROLLABLE_VIEWABLE		2	// scroll bar is displayed
#define	WDS_SCROLLABLE_VISIBLE		( WDS_SCROLLABLE_VIEWABLE | WDS_SCROLLABLE_ON )
#define	WDS_SCROLLABLE_PERSISTANT	4	// always leave space for scrollbar
										// even when not needed
#define	WDS_SCROLLABLE_OPPOSE		8	// top/right instead of bottom/left
#define	WDS_SCROLLABLE_AUTOSCALE	16	// size dependent on children


// WDS_ScrollBar::SetTimerFlags()
#define WDS_SCROLL_TIMER_DECREMENT	0
#define WDS_SCROLL_TIMER_INCREMENT	1
#define WDS_SCROLL_TIMER_INITIAL	2



/******************************************************************************
class	WDS_ScrollField

Keywords:
******************************************************************************/
class WDS_ScrollField
	{
	private:

		float start;
		float size;

	public:

		float	GetFieldStart(void)			{ return start; };
		void	SetFieldStart(float set)	{ start=set; };

		float	GetFieldSize(void)			{ return size; };
		void	SetFieldSize(float set)		{ size=set; };
	};



/******************************************************************************
class	WDS_ScrollBarButton

	no extra functionality
	just used for inheritance purposes

Keywords:
******************************************************************************/
class WDS_ScrollBarButton : public WDS_Button
	{
	WDS_DECLARE_TYPE(WDS_ScrollBarButton);

	public:
				WDS_ScrollBarButton(void);
	};



/*****************************************************************************
class WDS_ScrollBar : public WDS_Widget

	A sliding bar in a shaft

*****************************************************************************/
class WDS_ScrollBar : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_ScrollBar);

	private:
		WDS_ScrollBarButton	button[2];		// slave children
											// 0=horizontal, 1=vertical

//		GPL_DoubleList<WDS_ScrollBar>	tielist;
		GPL_DoubleList<WDS_ScrollField>	fieldlist;

		long	cpos[2],csize[2];		// cylinder shaft sub-geometry
		long	crpos[2],crsize[2];		// cylinder shaft relative sub-geometry
		long	cipos[2],cisize[2];		// inset cyl. shaft sub-geometry
		long	cirpos[2],cirsize[2];	// inset cyl. shaft relative sub-geo.

		long	bpos[2],bsize[2];	// piston sub-geometry
		long	drag;				// TRUE when dragging
		long	last;				// last mouse position on drag
		long	tiechange;			// recurance counter to prevent
									//   loops/redundancy in TieValuesChanged()
		float	last_tie_value;		// value at last UpdateTie()

		long	timer_id;			// for repeating held buttons
		long	timerflags;
		long	direction;
		long	registered;

		float	offset;
		float	fraction;
		float	stepsize;
		float	last_offset;		// stored for limited redraw

		void	UpdateArrows(void)
					{
					long m,n;
					for(m=0;m<2;m++)
						for(n=0;n<WDS_STATES;n++)
							button[m].ModifyPointDirection(n,direction,!m);
					};

	public:
				WDS_ScrollBar(void);
virtual			~WDS_ScrollBar(void)
					{
					ClearFields();
					SetTieList(NULL);
					};

static	void	ActivationCallback(WDS_Activation *activation);

virtual	void	Draw(EW_Redraw *redraw);
virtual	long	Operate(EW_Event *event);
virtual	long	PostOperate(EW_Event *event);
virtual	long	Resize(EW_Resize *resize);

		long	GetDirection(void)		{ return direction; };
		void	SetDirection(long set)
					{
					direction=set;
					SetOffset((direction==WDS_SCROLLBAR_VERTICAL)? 1.0f: 0.0f);
					};

		float	GetFraction(void)		{ return fraction; };
		void	SetFraction(float set)
					{
					float old_offset=offset;
					float old_fraction=fraction;

					fraction=set;

					if(GetDirection()==WDS_SCROLLBAR_VERTICAL)
						offset-= (fraction-old_fraction);

					CheckScrollLimits();

					if(old_offset!=offset || old_fraction!=fraction)
						SetDirty(EW_DIRTY_COMPLETE);
					};

		float	GetOffset(void)			{ return offset; };
		void	SetOffset(float set)
					{
					float old_offset=offset;

					offset=set;
					CheckScrollLimits();
					if(old_offset!=offset)
						SetDirty(EW_DIRTY_COMPLETE);
					};

		float	GetStepSize(void)		{ return stepsize; };
		void	SetStepSize(float set)	{ stepsize=set; };

		long	GetTimerID(void)		{ return timer_id; };
		void	SetTimerID(long set)	{ timer_id=set; };

		long	GetTimerFlags(void)		{ return timerflags; };
		void	SetTimerFlags(long set)	{ timerflags=set; };

		void	StartTimer(long time);
		void	EndTimer(void);

		void	AffectFromButton(long increment,EW_Event *event);

		void	Increment(void)	{ SetOffset(GetOffset()+stepsize); };
		void	Decrement(void)	{ SetOffset(GetOffset()-stepsize); };

				// set offset, but avoid check until later
				// should be used carefully (by friend-like classes)
				//  which will do the CheckScrollLimits() appropriately
		void	SetOffsetPending(float set)	{ offset=set; };
		void	CheckScrollLimits(void);

				// replaces virtual WDS_Widget::TieValuesChanged()
		void	TieValuesChanged(WDS_TieValues *tievalues);

		void	UpdateTie(void);

		void	AddField(float start,float size)
					{
					WDS_ScrollField *field=new WDS_ScrollField;

					field->SetFieldStart(start);
					field->SetFieldSize(size);

					fieldlist.AppendNode(field);
					};

		void	ClearFields(void)
					{
					WDS_ScrollField *field;

					fieldlist.ToTail();
					while( (field=(WDS_ScrollField *)fieldlist.Current()) != NULL )
						{
						fieldlist.RemoveNode(field);
						delete field;
						};
					};

		void	RegisterMouse(long on);

	};



/*****************************************************************************
class WDS_ScrollRegion : public WDS_Widget

	A clipped scrollable region

	NOTE: could make scrollbar's slaves instead of foster children
		by rechecking hierarchy on resize and placing back if neccessary.
		Foster children are difficult to trace.

*****************************************************************************/
class WDS_ScrollRegion : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_ScrollRegion);

	private:
		WDS_ScrollBar scrollbar[2];			// foster children
											// 0=horizontal, 1=vertical

		long	scrolling[2];				// TRUE = scrollbar needed/visible

		long	msize[2];					// sub-map sub-geometry
		long	vsize[2];					// map-view sub-geometry
		long	region_offset[4];			// left,bottom right,top
		long	spos[2],ssize[2];			// scroll region sub-geometry
		long	lastgeo[4];					// geo when translation last set
		long	lasttrans[2];				// last translation

		long	extent[2];					// extent of children (x,y)
		long	last[2];					// last position on grab (x,y)
		long	grab;						// TRUE = background grabbed

		long	debug_color;

				// Resize for slave children (scrollbars)
		void	AuxResize(EW_Resize *resize);

		void	ConfirmScrollBars(void);

		void	SetGrab(long set);
		long	IsGrabbed(void)		{ return EW_INBOUNDS(1,grab,2); };

	public:
				WDS_ScrollRegion(void);
virtual			~WDS_ScrollRegion(void)
					{
					ClearScrollFields(EW_HORIZONTAL);
					ClearScrollFields(EW_VERTICAL);
					};

virtual	void	Draw(EW_Redraw *redraw);
virtual	void	PostDraw(EW_Redraw *redraw);
virtual	long	Operate(EW_Event *event);
virtual	long	PostOperate(EW_Event *event);
virtual	long	Resize(EW_Resize *resize);

		void	GetSubGeometry(long *x,long *y,long *w, long *h)
					{
					*x=spos[0];
					*y=spos[1];
					*w=ssize[0];
					*h=ssize[1];
					};

		WDS_ScrollBar	*GetScrollBar(long dir)		{ return &scrollbar[dir]; };

		void	AddScrollField(long direction,float start,float size)
					{
					scrollbar[direction].AddField(start,size);
					};

		void	ClearScrollFields(long direction)
					{
					scrollbar[direction].ClearFields();
					};

		long	GetOverheadHeight(void);
	};
