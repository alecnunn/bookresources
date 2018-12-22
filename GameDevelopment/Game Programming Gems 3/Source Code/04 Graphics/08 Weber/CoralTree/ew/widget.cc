
/******************************************************************************

Coral Tree ew/widget.cc
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


#define EW_CLIPPING				1		// recommend 1:  0=never  1=by widget  2=all widgets
#define EW_CLIP_CHECK			TRUE	// don't draw widgets (and their children) who would be completely clipped
#define EW_CLIP_BOX				FALSE	// TRUE= display box around clip region

#define EW_SQUASH_WIDGETS		FALSE	// resize widgets to fit inside parent (kinda dumb)

#define EW_WIDGET_DIRTY_PARENT	FALSE	// dirty parent if child sizes to expose
#define EW_USED_CHANGE_DEBUG	FALSE
#define EW_REDRAW_DEBUG			FALSE
#define EW_REDRAW_FLASH			FALSE
#define EW_SET_DIRTY_DEBUG		FALSE
#define EW_RESIZE_DEBUG			FALSE
#define EW_CHECKSIZE_DEBUG		FALSE
#define EW_DESCEND_DEBUG		FALSE	// print every widget in descension
#define EW_MASK_DEBUG			FALSE
#define EW_CLIP_CHECK_DEBUG		FALSE


long		EW_Widget::default_event_mask_set=FALSE;
EW_Event	EW_Widget::default_event_mask;
EW_Event	EW_Widget::full_event_mask;



/******************************************************************************
		EW_Widget::EW_Widget()

Keywords:
******************************************************************************/
EW_Widget::EW_Widget()
	{
	parentwidget=NULL;
	rootwindow=NULL;
//	widgetlist = new EW_WidgetList();

	SetGeometry(0,0,1,1);
	truex=0;
	truey=0;

	inversion=FALSE;

	SetGreyOut(FALSE);
	SetInvisible(FALSE);
	SetDormant(FALSE);
	SetCompound(FALSE);
	SetRetainLimits(FALSE);
	SetSmartClear(FALSE);

	SetMinSize(-1,-1);
	SetMaxSize(-1,-1);
	SetPrefSize(-1,-1);

	SetBounds(EW_BOUNDS_NONE);

	SetTranslation(0,0);
	SetTranslatable(TRUE);

	SetClipping(EW_CLIPPING_OFF);
	SetClipRegion(-1,-1,-1,-1);
	SetClippable(TRUE);

	local_event_mask=NULL;

	if(!default_event_mask_set)
		{
		// default (widget system may change)
		GetDefaultEventMask()->SetSIS(EW_EVENT_ANY,EW_ITEM_ALL,EW_STATE_ANY);
		GetDefaultEventMask()->SetWidgetFlags(EW_WIDGET_FLAGS_ALL);

		// all-pass filter (shouldn't filter out anything)
		GetFullEventMask()->SetSIS(EW_EVENT_ANY,EW_ITEM_ALL,EW_STATE_ANY);
		GetFullEventMask()->SetWidgetFlags(EW_WIDGET_FLAGS_ALL);
		}
	SetEventMask(GetDefaultEventMask());

	descend_mode = EW_DESCEND_IN_CHILD_WIDGET;
	}



/******************************************************************************
	EW_Widget::~EW_Widget()

******************************************************************************/
EW_Widget::~EW_Widget()
	{
	if(local_event_mask)
		delete local_event_mask;

	EW_Widget *node;

	// remove all non-newed pointers that will otherwise be deleted elsewhere, presumably
	GetWidgetList()->ToHead();
	while((node = GetWidgetList()->PostIncrement())!=NULL)
		{
		if(!node->GetNewed())
			{
#if EW_NUDE_WARN
        	EW_PRINT(EW_APP,EW_WARN,
					"~EW_Widget() 0x%x (%s%s%s) ejecting non-newed 0x%x (%s%s%s)",
					this,ewColorCode(3),GetName(),ewColorCode(0),node,ewColorCode(3),node->GetName(),ewColorCode(0));
#endif

			RemoveWidget(node);
			}
		}
	}



/******************************************************************************
EW_Event	*EW_Widget::GetLocalEventMask(void)

******************************************************************************/
EW_Event *EW_Widget::GetLocalEventMask(void)
	{
	if(!local_event_mask)
		{
		local_event_mask=new EW_Event;
		local_event_mask->SetSIS(EW_EVENT_COMMON,EW_ITEM_ALL,EW_STATE_ANY);
		local_event_mask->SetWidgetFlags(EW_WIDGET_FLAGS_ALL);
		}
	return local_event_mask;
	}



/******************************************************************************
void	EW_Widget::SetRootWindow(EW_Window *window)

Keywords:
******************************************************************************/
void EW_Widget::SetRootWindow(EW_Window *window)
{
	EW_Widget *next;

	if( rootwindow && window && rootwindow!=window)
		{
		EW_PRINT(EW_APP,EW_ERROR,"attempt to change root of non-free widget");
		return;
		}

	rootwindow = window;
	
	widgetlist->Rewind();
	while((next = widgetlist->Next())!=NULL)
		next->SetRootWindow(window);
}


/******************************************************************************
void	EW_Widget::SetParentWidget(EW_Widget *widget)

Keywords:
******************************************************************************/
void EW_Widget::SetParentWidget(EW_Widget *widget)
	{
	if( parentwidget && widget && parentwidget!=widget)
		{
		EW_PRINT(EW_APP,EW_ERROR,"attempt to change parent of non-free widget");
		return;
		}

	parentwidget = widget;

	if(parentwidget)
		SetRootWindow(parentwidget->rootwindow);
	} 



/******************************************************************************
void	EW_Widget::Redraw(EW_Redraw *redraw)

******************************************************************************/
void EW_Widget::Redraw(EW_Redraw *redraw)
	{
	EW_Widget *next;

	EW_Redraw new_redraw;

#if EW_REDRAW_DEBUG
	printf("Redraw 0x%x dirty=%d,%d force=%d limited=%d (%s)\n",
			(long)this,dirty[0],dirty[1],redraw->GetForce(),redraw->GetLimited(),GetName());
#endif

	if(!GetDirty() && !redraw->GetForce())
		return;

	long tx,ty,sx,sy;

	long drew_this=FALSE;

	GetTrueGeometry(&tx,&ty,&sx,&sy);

	if(!GetInvisible())
		{
#if EW_CLIPPING
		long cx,cy,cw,ch;

		if(EW_CLIPPING==2 || (GetClipping() && GetClipping()!=EW_CLIPPING_CHILDREN) )
			{
			cx= (clipx>=0)? clipx: truex;
			cy= (clipy>=0)? clipy: truey;
			cw= (clipw>=0)? clipw: sx;
			ch= (cliph>=0)? cliph: sy;

			rootwindow->PushClipRegion();
			rootwindow->IntersectClipRegion(cx,cy,cw,ch);
			}
#endif

		if((dirty[0]&EW_DIRTY_COMPLETE) || (dirty[0]&EW_DIRTY_PARTIAL) || redraw->GetForce())
			{
			long force_it=FALSE;

			if( (dirty[0]&EW_DIRTY_COMPLETE) && !redraw->GetForce() && !GetSmartClear())
				{
				// this is the base of a full redraw */

#if EW_REDRAW_FLASH
				long index=rootwindow->GetColor(EW_COLOR_BACKGROUND);

/*
				rootwindow->SetColor(EW_COLOR_BACKGROUND,EW_RED);
				ClearBackground();
*/

				rootwindow->RectFill(tx,ty,tx+sx-1,ty+sy-1,EW_RED);

				rootwindow->Flush(TRUE);
				osdMilliSleep(50);

				rootwindow->SetColor(EW_COLOR_BACKGROUND,index);
#endif

				ClearBackground();
				force_it=TRUE;
				}

#if EW_REDRAW_DEBUG
			printf("Draw 0x%x 0x%x,0x%x,%d %d,%d %d,%d \"%s\"\n",
								(long)this,dirty[0],dirty[1],redraw->GetForce(),truex,truey,sx,sy,GetNameString()->Buffer());
#endif

			if( !EW_CLIP_CHECK || !(IsClipped()==EW_CLIPPED_FULL) )
				{
//				Draw(redraw);
				DrawOnTablet(redraw);
				drew_this=TRUE;
				}
#if EW_CLIP_CHECK_DEBUG
			else
				printf("0x%x completely clipped (%s)\n",(long)this,GetNameString()->String());
#endif

			// !!!!!!!!!!!! need better check
			if(!redraw->GetLimited())
				{
				// store last unclipped
				lastgeom.SetGeometry(tx,ty,sx,sy);

				// store last clipped
				rootwindow->GetClipBounds(&cx,&cy,&cw,&ch);
				lastvisible.SetGeometry(cx,cy,cw,ch);
				}

			/* if not already forced, force all children to redraw */
			if(force_it)
				{
				/* don't want to alter data in original redraw structure */
				osdMemcpy(&new_redraw,redraw,sizeof(EW_Redraw));
				redraw=&new_redraw;

				redraw->SetForce(TRUE);
				}
			}

#if EW_CLIPPING
		if( !(EW_CLIPPING==2) && GetClipping()==EW_CLIPPING_CHILDREN )
			{
			cx= (clipx>=0)? clipx: truex;
			cy= (clipy>=0)? clipy: truey;
			cw= (clipw>=0)? clipw: sx;
			ch= (cliph>=0)? cliph: sy;

			rootwindow->PushClipRegion();
			rootwindow->IntersectClipRegion(cx,cy,cw,ch);
			}
#endif

#if EW_CLIPPING && EW_CLIP_BOX
		if(GetClipping())
			GetRootWindow()->RectWire(cx,cy,cx+cw-1,cy+ch-1,(GetClipping()==EW_CLIPPING_CHILDREN)? EW_PURPLE: EW_PINK);
#endif

		widgetlist->Rewind();
		while( (next = widgetlist->Next()) != NULL )
			{
			if(GetClipping() && !next->GetClippable())
				GetRootWindow()->RewindClipRegion();

			// notice that unclippable children of a fully clipped parent may still be viewable
			if( !EW_CLIP_CHECK || !next->GetClippable() || !(IsClipped()==EW_CLIPPED_FULL) )
				next->Redraw(redraw);

			if(GetClipping() && !next->GetClippable())
				GetRootWindow()->RestoreClipRegion();
			}

		if(drew_this)
			{
//			PostDraw(redraw);
			PostDrawOnTablet(redraw);

			if(GetGreyOut())
				{
//				printf("Grey Out (%s)\n",GetNameString()->Buffer());
				rootwindow->GreyOut(tx,ty,sx,sy);
				}
			}

#if EW_CLIPPING
		// pop-off child clipping
		if( !(EW_CLIPPING==2) && GetClipping()==EW_CLIPPING_CHILDREN )
			rootwindow->PopClipRegion();
#endif

#if EW_CLIPPING
		// pop-off everything clipping
		if(EW_CLIPPING==2 || (GetClipping() && GetClipping()!=EW_CLIPPING_CHILDREN) )
			rootwindow->PopClipRegion();
#endif

/*
#if EW_CLIPPING
		if(EW_CLIPPING==2 || GetClipping())
			rootwindow->PopClipRegion();
#endif
*/
		}

	if(!redraw->GetLimited())
		{
		// if double dirty from double-buffering, make it single dirty
		// else just mark it clean
		if(GetRootWindow() && GetRootWindow()->GetDoubleBuffer())
			{
			dirty[0]=dirty[1]&EW_DIRTY_MASK;
			dirty[1]=EW_DIRTY_NOT;
			}
		else
			dirty[0]=EW_DIRTY_NOT;
		}
	}



/******************************************************************************
long	EW_Widget::IsClipped(void)

	return value:
		EW_CLIPPED_NOT		= widget completely inside clipping region
not->	EW_CLIPPED_PARTIAL	= widget bounds intersect clipping region
		EW_CLIPPED_FULL		= widget completely outside clipping region

******************************************************************************/
long EW_Widget::IsClipped(void)
	{
	long sx,sy;
	GetSize(&sx,&sy);

#if TRUE
	EW_Rectangle rectangle;
	long result=EW_CLIPPED_FULL;

	rectangle.SetGeometry(truex,truey,sx,sy);
	if( GetRootWindow()->GetClipStack()->GetCurrentRegion()->Intersect(&rectangle,NULL) )
		result=EW_CLIPPED_NOT;

//	EW_PRINT(EW_WIDGET,EW_LOG,"EW_Widget::IsClipped() (%s) %s",GetNameString()->Buffer(),(result==EW_CLIPPED_FULL)? "YES": "NO");

	return result;

#else
	long cx,cy,cw,ch;


	GetRootWindow()->GetClipBounds(&cx,&cy,&cw,&ch);

	if(truex>cx+cw || truex+sx<cx || truey>cy+ch || truey+sy<cy)
		return EW_CLIPPED_FULL;
	else if(truex>=cx && truex+sx<=cx+cw && truey>=cy && truey+sy<=cy+ch)
		return EW_CLIPPED_NOT;
	else
		return EW_CLIPPED_PARTIAL;
#endif
	}



/******************************************************************************
void	EW_Widget::GetVisibility(long *x,long *y,long *sx,long *sy)

	valid during draw

******************************************************************************/
void EW_Widget::GetVisibility(long *x,long *y,long *sx,long *sy)
	{
	GetRootWindow()->GetClipBounds(x,y,sx,sy);
	}



/******************************************************************************
void	EW_Widget::GetLastVisibility(long *x,long *y,long *sx,long *sy)

******************************************************************************/
void EW_Widget::GetLastVisibility(long *x,long *y,long *sx,long *sy)
	{
	lastvisible.GetGeometry(x,y,sx,sy);
	}



/******************************************************************************
void	EW_Widget::GetLastTrueGeometry(long *x,long *y,long *sx,long *sy)

******************************************************************************/
void EW_Widget::GetLastTrueGeometry(long *x,long *y,long *sx,long *sy)
	{
	lastgeom.GetGeometry(x,y,sx,sy);
	}




/******************************************************************************
void	EW_Widget::SetDirty(long set)

	mark widget as    dirty=set:			like needs to be redrawn
	mark ancestors as dirty=EW_DIRTY_CHILD:	a descendant needs to be redrawn

	if any ancestor is already dirty, stop accension (already done)

Keywords:
******************************************************************************/
void EW_Widget::SetDirty(long set)
	{
	DB("EW_Widget::SetDirty(void)");

#if EW_SET_DIRTY_DEBUG
	char message[128];

	sprintf(message,"0x%x \"%s\" SetDirty(%d)",(long)this,GetNameString()->Buffer(),set);
	EW_PRINT(EW_WIDGET,EW_LOG,message);
#endif

	// 0 for single buffer, 1 for double buffer
	long current= (GetRootWindow() && GetRootWindow()->GetDoubleBuffer());
	long set_pending=FALSE;

	// don't overwrite if-pending request if previous value is not pending
	if(set&EW_DIRTY_IF_PENDING)
		{
		set_pending=TRUE;
		set^=EW_DIRTY_IF_PENDING;

		if( dirty[current] && !(dirty[current]&EW_DIRTY_PENDING) )
			return;
		}

	// if essentially EW_DIRTY_NOT with flags, just make it EW_DIRTY_NOT (0)
	if( !(set&EW_DIRTY_MASK) )
		set=EW_DIRTY_NOT;

	// don't demote (clears of pendings are ok)
/*
	if( ( (set&EW_DIRTY_MASK) && (set&EW_DIRTY_MASK)>(dirty[0]&EW_DIRTY_MASK) )
			|| (set==EW_DIRTY_NOT && (dirty[0]&EW_DIRTY_PENDING)) )
		EW_Node::SetDirty(0,set);
*/
	if( (set_pending && ( !current||(dirty[0]&EW_DIRTY_PENDING) ) )
							||	( (set&EW_DIRTY_MASK) && (set&EW_DIRTY_MASK)>(dirty[0]&EW_DIRTY_MASK) ) )
		EW_Node::SetDirty(0,set);

	EW_Node::SetDirty(1,set);

	EW_Node *parent=(EW_Node *)this;

#if FALSE
	if( (parent=GetParent()) != NULL)
		{
		long pdirty=parent->GetDirty();

		if(!(pdirty&EW_DIRTY_MASK) || (dbuf && !(pdirty&EW_DIRTY_CHILD)) )
			{
			if( !(pdirty&EW_DIRTY_MASK) )
				pdirty=EW_DIRTY_CHILD;

			parent->SetDirty(pdirty);
			}
		}
#else

	while( (parent=parent->GetParent()) != NULL  && !parent->GetDirty(0))
		parent->SetDirty(0,EW_DIRTY_CHILD);

	parent=(EW_Node *)this;

	if(GetRootWindow() && GetRootWindow()->GetDoubleBuffer())
		while( (parent=parent->GetParent()) != NULL  && !parent->GetDirty(1))
			parent->SetDirty(1,EW_DIRTY_CHILD);

#endif
	}



/******************************************************************************
void	EW_Widget::SetTrueOrigin(long x,long y)

******************************************************************************/
void EW_Widget::SetTrueOrigin(long x,long y)
	{
	long px,py,psx,psy;
	GetParentTrueGeometry(&px,&py,&psx,&psy);

	SetOrigin(x-px,y-py);
	}



/******************************************************************************
void	EW_Widget::SetTrueGeometry(long x,long y,long w,long h)

******************************************************************************/
void EW_Widget::SetTrueGeometry(long x,long y,long w,long h)
	{
	SetTrueOrigin(x,y);
	SetSize(w,h);
	}



/******************************************************************************
void	EW_Widget::GetTrueOrigin(long *x,long *y)

******************************************************************************/
void EW_Widget::GetTrueOrigin(long *x,long *y)
	{
	*x=truex;
	*y=truey;
	}



/******************************************************************************
void	EW_Widget::GetTrueGeometry(long *x,long *y,long *w,long *h)

******************************************************************************/
void EW_Widget::GetTrueGeometry(long *x,long *y,long *w,long *h)
	{
	long sx,sy;

	GetSize(&sx,&sy);

	*x=truex;
	*y=truey;
	*w=sx;
	*h=sy;
	}



/******************************************************************************
void	EW_Widget::GetParentTrueGeometry(long *x,long *y,long *w,long *h)

******************************************************************************/
void EW_Widget::GetParentTrueGeometry(long *x,long *y,long *w,long *h)
	{
	DB("EW_Widget::GetParentTrueGeometry(long *x,long *y,long *w,long *h)");

	if(parentwidget)
		parentwidget->GetTrueGeometry(x,y,w,h);
	else if(rootwindow)
		rootwindow->GetGeometry(x,y,w,h);
	else
		EW_PRINT(EW_APP,EW_ERROR,"attempt to access parent of free widget");
	}



/******************************************************************************
void	EW_Widget::ClearBackground(void)

******************************************************************************/
void EW_Widget::ClearBackground(void)
	{
	ClearBackground(GetRootWindow()->GetColor(EW_COLOR_BACKGROUND));
	}



/******************************************************************************
void	EW_Widget::ClearBackground(long colorindex)

******************************************************************************/
void EW_Widget::ClearBackground(long colorindex)
	{
	long sx,sy;

	GetSize(&sx,&sy);

	GetRootWindow()->RectFill(truex,truey,truex+sx-1,truey+sy-1,colorindex);
	}



/******************************************************************************
long	EW_Widget::CheckSize(EW_Resize *resize)

	makes sure size fits in bounds passed by EW_Resize

******************************************************************************/
long EW_Widget::CheckSize(EW_Resize *resize)
	{
	long nx,ny,mx,my,fx,fy;	// min/max/prefered size
	long ox,oy;				// original values
	long ix,iy,sx,sy;

	// store for debugging
	osdMemcpy(&last_resize,resize,sizeof(EW_Resize));

	GetGeometry(&ix,&iy,&sx,&sy);

	resize->GetMinSize(&nx,&ny);
	resize->GetMaxSize(&mx,&my);
	resize->GetPrefSize(&fx,&fy);

#if EW_CHECKSIZE_DEBUG

#define CHECKNAME "Divider"

	char message[200];

	if( !GetNameString()->Compare(CHECKNAME) )
		{
//		resize->Print((long)this);

		sprintf(message,"on widget 0x%x %d,%d [%d,%d %d,%d:%d,%d] on [%d,%d %d,%d:%d,%d] (%s)",
					(long)this,sx,sy,fx,fy,nx,ny,mx,my,prefx,prefy,minx,miny,maxx,maxy,GetNameString()->Buffer());
		EW_PRINT(EW_WIDGET,EW_LOG,message);

#define MAX_BOUND 2000

		if(nx>MAX_BOUND || ny>MAX_BOUND || mx>MAX_BOUND || my>MAX_BOUND || fx>MAX_BOUND || fy>MAX_BOUND ||
			minx>MAX_BOUND || miny>MAX_BOUND || maxx>MAX_BOUND || maxy>MAX_BOUND || prefx>MAX_BOUND || prefy>MAX_BOUND)
				{
				EW_PRINT(EW_WIDGET,EW_CRASH,"bounding error");
				}
		}
#endif

	// check widgets min size against fixed minimums
	long minx2,miny2;
	GetMinSize(&minx2,&miny2);

	// save originals
	ox=sx;
	oy=sy;

	long ignorex= (GetBounds(EW_HORIZONTAL)&EW_BOUNDS_IGNORE_PARENT);
	long ignorey= (GetBounds(EW_VERTICAL)&EW_BOUNDS_IGNORE_PARENT);

	// Set from prefered values
	// - widget's pref has priority unless bounding flags are set
	if(fx>=0 && !ignorex)
		sx=fx;
	if( prefx>=0 && (fx<0 || !(GetBounds(EW_HORIZONTAL)&EW_BOUNDS_USE_PARENT_MASK)) )
		sx=prefx;

	if(fy>=0 && !ignorey)
		sy=fy;
	if( prefy>=0 && (fy<0 || !(GetBounds(EW_VERTICAL)&EW_BOUNDS_USE_PARENT_MASK)) )
		sy=prefy;

	// check minimum limits
	if(minx2>=0)
		sx= EW_MAX(sx,minx2);
	if(nx>=0)
		sx= EW_MAX(sx,nx);

	if(miny2>=0)
		sy= EW_MAX(sy,miny2);
	if(ny>=0)
		sy= EW_MAX(sy,ny);

	// check maximum limits
	if(maxx>=0)
		sx= EW_MIN(sx,maxx);
	if(mx>=0 && !ignorex)
		sx= EW_MIN(sx,mx);

	if(maxy>=0)
		sy= EW_MIN(sy,maxy);
	if(my>=0 && !ignorey)
		sy= EW_MIN(sy,my);

	// clamp to parent
#if EW_SQUASH_WIDGETS
	long px,py,pw,ph;		// parent geometry

	GetParentTrueGeometry(&px,&py,&pw,&ph);
	if(ix<0)
		ix=0;
	if(iy<0)
		iy=0;

	if(sx>pw-ix)
		sx=pw-ix;
	if(sy>ph-iy)
		sy=ph-iy;

#endif

	// set directly
	position[0]=ix;
	position[1]=iy;
	dimension[0]=sx;
	dimension[1]=sy;

	// with inversion, size change can alter position
	CalcTruePosition();

	long resized=( sx!=ox || sy!=oy );

#if EW_CHECKSIZE_DEBUG
	if( !GetNameString()->Compare(CHECKNAME) )
		{
		sprintf(message,"results pos=%d,%d size=%d,%d from %d,%d resized=%d",ix,iy,sx,sy,ox,oy,resized);
		EW_PRINT(EW_WIDGET,EW_LOG,message);
		}
#endif

	// return TRUE if they really changed
	return resized;
	}



///////////////////////// DEFAULT WIDGET METHODS /////////////////////////



/******************************************************************************
long	EW_Widget::Descend(EW_Event *event)

******************************************************************************/
long EW_Widget::Descend(EW_Event *event)
	{
#if EW_DESCEND_DEBUG
//	if(event->IsExposure())
		EW_PRINT(EW_WIDGET,EW_LOG," Descend (%s)",GetName());
#endif

	long result = FALSE;

//	if(!GetDormant())
		{
#if EW_USED_CHANGE_DEBUG
		long used=event->GetUsed();
#endif

		if( event->GetSource() && PreOperate(event) )
			result = TRUE;

#if EW_USED_CHANGE_DEBUG
		if(used!=event->GetUsed())
			EW_PRINT(EW_WIDGET,EW_LOG,"used %d to %d (%s) 1",used,event->GetUsed(),GetName());
#endif

		event->SetChoice(this);

		if( !GetDormant() && event->GetSource() && widgetlist->Descend(event,descend_mode) )
			result = TRUE;

#if EW_USED_CHANGE_DEBUG
		if(used!=event->GetUsed())
			EW_PRINT(EW_WIDGET,EW_LOG,"used %d to %d (%s) 2",used,event->GetUsed(),GetName());
#endif

		if( event->GetSource() && PrePostOperate(event) )
			result = TRUE;

#if EW_USED_CHANGE_DEBUG
		if(used!=event->GetUsed())
			EW_PRINT(EW_WIDGET,EW_LOG,"used %d to %d (%s) 3",used,event->GetUsed(),GetName());
#endif
		}

	return result;
	}



/******************************************************************************
long	EW_Widget::PreOperate(EW_Event *event)

	virtual

******************************************************************************/
long EW_Widget::PreOperate(EW_Event *event)
	{
#if EW_MASK_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"EW_Widget::PreOperate() intersect=%d mask,event:",event->Intersects(GetEventMask()));
	GetEventMask()->Print((long)this);
	event->Print((long)this);
#endif

	if(!GetDormant() && event->Intersects(GetEventMask()))
		return Operate(event);

	return FALSE;
	}



/******************************************************************************
long	EW_Widget::PrePostOperate(EW_Event *event)

	virtual

******************************************************************************/
long EW_Widget::PrePostOperate(EW_Event *event)
	{
	if(!GetDormant() && event->Intersects(GetEventMask()))
		return PostOperate(event);

	return FALSE;
	}



/******************************************************************************
void EW_Widget::CalcTruePosition(long recursive)

******************************************************************************/
void EW_Widget::CalcTruePosition(long recursive)
	{
	long ix,iy,sx,sy;

	GetGeometry(&ix,&iy,&sx,&sy);

	if(parentwidget)
		{
		long invert=parentwidget->GetInversion();
		long tx=0,ty=0;

		long psx,psy;

		parentwidget->GetSize(&psx,&psy);

		if(GetTranslatable())
			parentwidget->GetTranslation(&tx,&ty);

		truex= parentwidget->truex + tx+ix;

		if(invert)
			truey= parentwidget->truey+psy - (ty+iy+sy);
		else
			truey= parentwidget->truey + ty+iy;
		}
	else if(rootwindow)
		{
		truex= ix;
		truey= iy;
		}
	else
		EW_PRINT(EW_APP,EW_ERROR,"CalcTruePosition() attempt on free widget");

	if(recursive)
		{
		EW_Widget *next;

		widgetlist->Rewind();
		while( (next=widgetlist->Next()) != NULL)
			next->CalcTruePosition(recursive);
		}
	}




/******************************************************************************
long 	EW_Widget::Resize(EW_Resize *resize)

	default virtual routine

	custom EW_Widget::Resize() probably won't use a custom widgetlist.Resize(),
		but just call the original EW_WidgetList::Resize() or child Resize()'s
		directly.

	note that this default functionality clears min,max,pref info for children

******************************************************************************/
long EW_Widget::Resize(EW_Resize *resize)
	{
#if EW_RESIZE_DEBUG

	resize->Print((long)this);

#endif

	long resized=CheckSize(resize);

	EW_Resize new_resize;

	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	if(!GetRetainLimits())
		{
		new_resize.SetMinSize(-1,-1);
		new_resize.SetMaxSize(-1,-1);
		new_resize.SetPrefSize(-1,-1);
		}

	return ( widgetlist->Resize(&new_resize) || resized );
	}



/******************************************************************************
long EW_WidgetList::Resize(EW_Resize *resize)

	default virtual routine

******************************************************************************/
long EW_WidgetList::Resize(EW_Resize *resize)
	{
	if( !resize->GetRecursive() )
		return FALSE;

	EW_Widget *next;

	ToHead();
	while( (next=PostIncrement()) != NULL)
		next->PreResize(resize);

	return FALSE;
	}



/******************************************************************************
long 	EW_Widget::PreResize(EW_Resize *resize)

	called from parent's EW_WidgetList::Resize()
	calls EW_Widget::Resize()

	accounts for EW_BOUNDS_FILL_PARENT or EW_BOUNDS_USE_MAX

	should be called instead of the child EW_Widget::Resize() from a
	parent EW_Widget::Resize() that doesn't use the
	default EW_WidgetList::Resize()

******************************************************************************/
long EW_Widget::PreResize(EW_Resize *resize)
	{
	//EW_Node *parent=GetParent();	// window or widget
	EW_Resize new_resize;

	long px,py,pw,ph;	// parent
	long ix,iy,sx,sy;	// child
	long mw,mh;			// resize's max width,height

	// get maximum bounds
	resize->GetMaxSize(&mw,&mh);

	// copy any info in resize
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	GetParentTrueGeometry(&px,&py,&pw,&ph);

	GetGeometry(&ix,&iy,&sx,&sy);

#if EW_SQUASH_WIDGETS
	new_resize.SetMaxSize(pw-ix,ph-iy);
#endif

	long origin[2];
	long pref[2];

	new_resize.GetPrefSize(&pref[0],&pref[1]);
	GetOrigin(&origin[0],&origin[1]);

	long m;
	for(m=0;m<2;m++)
		{
		if(GetBounds(m)&EW_BOUNDS_FILL_PARENT)
			{
			origin[m]=0;
			pref[m]= m? ph: pw;
			}
		else if(GetBounds(m)&EW_BOUNDS_USE_MAX)
			pref[m]= m? mh: mw;
		}

	new_resize.SetPrefSize(pref[0],pref[1]);
	SetOrigin(origin[0],origin[1]);

	// return value ignored
	Resize(&new_resize);

	// if size constrained, imply fixed size using identical min and max
	if(ConstrainSize(FALSE,FALSE,-1,-1))
		{
		long minx,miny;
		long maxx,maxy;

		GetSize(&sx,&sy);
		GetMinSize(&minx,&miny);
		GetMaxSize(&maxx,&maxy);

		if(GetNodeSizing()&EW_NODE_SIZING_FIT_CHILD_X)
			{
			minx=sx;
			maxx=sx;
			}
		if(GetNodeSizing()&EW_NODE_SIZING_FIT_CHILD_Y)
			{
			miny=sy;
			maxy=sy;
			}

		SetMinSize(minx,miny);
		SetMaxSize(maxx,maxy);
		}

	CalcTruePosition();

	// if underlying parent newly exposed, parent is dirty
#if EW_WIDGET_DIRTY_PARENT
	long nix,niy,nsx,nsy;
	GetGeometry(&nix,&niy,&nsx,&nsy);
	if(nix>ix || niy>iy || nix+nsx<ix+sx || niy+nsy<iy+sy)
		{
		EW_Widget *parent=GetParentWidget();

		EW_PRINT(EW_WIDGET,EW_LOG,"parent exposure under %d,%d %d,%d -> %d,%d %d,%d (%s)  (%s)",
									ix,iy,sx,sy,nix,niy,nsx,nsy,GetName(),parent->GetName());

		if(parent)
			parent->SetDirty(EW_DIRTY_COMPLETE);
		}
#endif

	return FALSE;
	}



/******************************************************************************
void		EW_Widget::DrawOnTablet(EW_Redraw *redraw)

******************************************************************************/
void EW_Widget::DrawOnTablet(EW_Redraw *redraw)
	{
	if(redraw->GetTablet())
		{
		// All widgets have default drawing for raw draws.
		if (redraw->GetTablet()->GetFlags()&EW_TABLET_RAW)
			{
#if TRUE
			long x,y,sx,sy;
			GetRootWindow()->GetClipBounds(&x,&y,&sx,&sy);
			redraw->GetTablet()->SetClipBounds(x,y,sx,sy);

			// Get the geometry.
			GetTrueGeometry(&x,&y,&sx,&sy);

			// Draw a filled rectangle.
			redraw->GetTablet()->RectFill(x,y,sx,sy);

			// Add the name.
			// TODO: Replace the arbitrary positioning.
			redraw->GetTablet()->Text(x+2,(y+sy)-(GetRootWindow()->GetFontHeight()+2),GetName());
#else
			long x,y,sx,sy;
			// If parent clipping...
			if (GetClipping()&EW_CLIPPING_ALL)
				{
				// Set the bounds of the current clipping region.
				rootwindow->GetClipBounds(&x,&y,&sx,&sy);
				if(x!=0 || y!=0 || sx!=0 || sy!=0)
					redraw->GetTablet()->SetClipBounds(x,y,sx,sy);
				}

			// Get the geometry.
			lastgeom.GetGeometry(&x,&y,&sx,&sy);

			// Draw a filled rectangle.
			redraw->GetTablet()->RectFill(x,y,sx,sy);

			// Add the name.
			// TODO: Replace the arbitrary positioning.
			redraw->GetTablet()->Text(x+2,(y+sy)-(rootwindow->GetFontHeight()+2),
										name.Buffer());

			// If child clipping...
			if (GetClipping()&EW_CLIPPING_CHILDREN)
				{
				// Set the bounds of the current clipping region.
				rootwindow->GetClipBounds(&x,&y,&sx,&sy);
				if(x!=0 || y!=0 || sx!=0 || sy!=0)
					redraw->GetTablet()->SetClipBounds(x,y,sx,sy);
				}
#endif
			}
		}
	else
		{
		Draw(redraw);
		}
	}



/******************************************************************************
void		EW_Widget::PostDrawOnTablet(EW_Redraw *redraw)

******************************************************************************/
void EW_Widget::PostDrawOnTablet(EW_Redraw *redraw)
	{
	if(redraw->GetTablet())
		{
		// Set the bounds of the current clipping region.
		long x,y,sx,sy;
		rootwindow->GetClipBounds(&x,&y,&sx,&sy);
//		if(x!=0 || y!=0 || sx!=0 || sy!=0)
			redraw->GetTablet()->SetClipBounds(x,y,sx,sy);
		}
	else
		{
		PostDraw(redraw);
		}
	}



/******************************************************************************
		EW_WidgetList::~EW_WidgetList(void)

******************************************************************************/
EW_WidgetList::~EW_WidgetList(void)
	{
	EW_Widget *node;

	// remove all non-newed pointers that will otherwise be deleted elsewhere, presumably
	ToHead();
	while((node = PostIncrement())!=NULL)
		{
		if(!node->GetNewed())
			{
        	EW_PRINT(EW_APP,EW_WARN,
								"EW_WidgetList::~EW_WidgetList() ejecting non-newed widget 0x%x to prevent auto-deletion",node);

			RemoveNode(node);

			node->SetRootWindow((EW_Window *)NULL);
			node->SetParentWidget((EW_Widget *)NULL);
			}
		}
	}



/******************************************************************************
long	EW_WidgetList::Descend(EW_Event *event,long mode)

Keywords:
******************************************************************************/
long EW_WidgetList::Descend(EW_Event *event,long mode)
	{
	EW_Widget *next,*parent=NULL;

	long mx,my;
	long do_resize = FALSE;
	long do_descend;
	long ix,iy,sx,sy;

	long cx=0,cy=0,csx=0,csy=0;
	long unclipped=TRUE;

	event->GetMousePosition(&mx,&my);
	
	/* for all children */
	if(ToHead())
		{
		parent=GetCurrent()->GetParentWidget();
		if(parent)
			parent->GetClipRegion(&cx,&cy,&csx,&csy);

		unclipped= (csx<1 || csy<1);
		}

	while((next = GetCurrent())!=NULL)
		{
		next->GetRootWindow()->GetGeometry(&ix,&iy,&sx,&sy);

		long was_dislocated=event->GetDislocated();
		long in_window=event->InBounds(0,0,sx,sy);

		do_descend=FALSE;

		if( in_window && (mode&EW_DESCEND_IN_WINDOW) )
			do_descend=TRUE;

		if( !in_window && (mode&EW_DESCEND_OUT_WINDOW) )
			do_descend=TRUE;


		if( !do_descend && event->InWidget(next) && (mode&EW_DESCEND_IN_CHILD_WIDGET) &&
								(unclipped || !next->GetClippable() || event->InBounds(cx,cy,csx,csy)) )
			do_descend=TRUE;

		if( !do_descend && !(event->GetSource() & EW_EVENT_LOCATIONAL) )
			{
			event->SetDislocated(TRUE);
			do_descend = TRUE;
			}

		/* if specific child encompasses the event's location */
		if(do_descend)
			{
			/* propagate event */
			if(next->Descend(event))
				do_resize = TRUE;
			}

		event->SetDislocated(was_dislocated);

		// in case a child scanned the linked list
		SetCurrent(next);
		PostIncrement();
		}
	return do_resize;
	}



/******************************************************************************
void	EW_WidgetList::DumpInfo(long level,long recursive,long brief)

Keywords:
******************************************************************************/
void EW_WidgetList::DumpInfo(long level,long recursive,long brief)
	{
	EW_Widget *next;

	Rewind();
	while( (next=Next()) != NULL)
		{
		next->DumpInfo(level);

		if( recursive && next->GetWidgetList())
			{
			if( brief && next->GetCompound() )
				EW_PRINT(EW_WIDGET,EW_LOG,"                           COMPOUND WIDGET: children restrained");
			else
				next->GetWidgetList()->DumpInfo(level+1,recursive,brief);
			}
		}
	}
