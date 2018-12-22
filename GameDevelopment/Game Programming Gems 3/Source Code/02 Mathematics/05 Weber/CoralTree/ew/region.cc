/******************************************************************************

Coral Tree ew/region.cc
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



#define EW_RECT_DEBUG			FALSE
#define WDS_INTERSECT_DEBUG		FALSE



/******************************************************************************
		EW_Rectangle::EW_Rectangle(void)

******************************************************************************/
EW_Rectangle::EW_Rectangle(void)
	{
	position[0]=0;
	position[1]=0;
	dimension[0]=1;
	dimension[1]=1;
	}



/******************************************************************************
		EW_RectangleList::~EW_RectangleList(void)

******************************************************************************/
EW_RectangleList::~EW_RectangleList(void)
	{
	DeleteRectangles();
	}



/******************************************************************************
void	EW_RectangleList::AddRectangle(long x,long y,long w,long h)

******************************************************************************/
void EW_RectangleList::AddRectangle(long x,long y,long w,long h)
	{
	EW_Rectangle *newrect=new EW_Rectangle;

	newrect->SetGeometry(x,y,w,h);
	AppendNode(newrect);
	}



/******************************************************************************
void	EW_RectangleList::CopyFrom(EW_RectangleList *other)

******************************************************************************/
void EW_RectangleList::CopyFrom(EW_RectangleList *other)
	{
	DeleteRectangles();

	if(other)
		{
		EW_Rectangle *node;
		long x,y,sx,sy;

		other->ToHead();
		while( (node=other->PostIncrement()) != NULL )
			{
			node->GetGeometry(&x,&y,&sx,&sy);
			AddRectangle(x,y,sx,sy);
			}
		}
	}



/******************************************************************************
void	EW_RectangleList::Prune(void)

	delete null rectangles

******************************************************************************/
void EW_RectangleList::Prune(void)
	{
	EW_Rectangle *node;
	long sx,sy;

	ToHead();
	while( (node=PostIncrement()) != NULL )
		{
		node->GetSize(&sx,&sy);

		if(sx<1 || sy<1)
			{
			if( !RemoveNode(node) )
				{
				EW_PRINT(EW_EW,EW_ERROR,"EW_RectangleList::Prune() couldn't remove node");
				continue;
				}

#if EW_RECT_DEBUG
			EW_PRINT(EW_EW,EW_ERROR,"EW_RectangleList::Prune() removed node of size %d,%d",sx,sy);
#endif

			delete node;
			}
		}
	}



/******************************************************************************
void	EW_RectangleList::Draw(EW_Window *window,long colorindex)

******************************************************************************/
void EW_RectangleList::Draw(EW_Window *window,long colorindex)
	{
	Prune();

	EW_Rectangle *node;
	long x,y,sx,sy;

	ToHead();
	while( (node=PostIncrement()) != NULL )
		{
		node->GetGeometry(&x,&y,&sx,&sy);
		window->RectFill(x,y,x+sx-1,y+sy-1,colorindex);

//		printf("%d,%d %d,%d\n",x,y,sx,sy);
		}
	}



/******************************************************************************
void	EW_RectangleList::DeleteRectangles(void)

******************************************************************************/
void EW_RectangleList::DeleteRectangles(void)
	{
	EW_Rectangle *node;

	// disassemble list
	while( (node=ToHead()) != NULL )
		{
//		EW_PRINT(EW_EW,EW_LOG,"EW_RectangleList::DeleteRectangles() 0x%x delete 0x%x",(long)this,(long)node);

		if(node!=GetCurrent())
			EW_PRINT(EW_EW,EW_LOG,"EW_RectangleList::DeleteRectangles() different current 0x%x",(long)GetCurrent());

		if( !RemoveNode(node) )
			{
			EW_PRINT(EW_EW,EW_CRASH,"EW_RectangleList::DeleteRectangles() 0x%x couldn't remove node 0x%x current=0x%x",
																					(long)this,(long)node,(long)GetCurrent());
			return;
			}

		delete node;
		}
	}



/******************************************************************************
void	EW_RectangleList::GetBounds(long *x,long *y,long *w,long *h)

******************************************************************************/
void EW_RectangleList::GetBounds(long *x,long *y,long *w,long *h)
	{
	EW_Rectangle *rectangle;

	long rx,ry,rsx,rsy;
	long bx=0,by=0,bsx=0,bsy=0;
	long first=TRUE;

	ToHead();
	while( (rectangle=PostIncrement()) != NULL )
		{
		rectangle->GetGeometry(&rx,&ry,&rsx,&rsy);

		if(first || bx>rx)
			bx=rx;
		if(first || by>ry)
			by=ry;
		if(first || bsx<rsx+rx-bx)
			bsx=rsx+rx-bx;
		if(first || bsy<rsy+ry-by)
			bsy=rsy+ry-by;

		first=FALSE;
		}

	*x=bx;
	*y=by;
	*w=bsx;
	*h=bsy;
	}



/******************************************************************************
long	EW_RectangleList::Intersect(EW_Rectangle *other,
											EW_RectangleList *intersection)

	resultant intersection can be NULL

	will work if intersection==this

******************************************************************************/
long EW_RectangleList::Intersect(EW_Rectangle *other,EW_RectangleList *intersection)
	{
	long intersects=FALSE;

	EW_Rectangle		*rectangle;
	EW_RectangleList	*result,resultbuffer;

	if(intersection)
		result=intersection;
	else
		result=&resultbuffer;

	if(intersection!=this)
		result->DeleteRectangles();

	long cx,cy,cw,ch;
	long rx,ry,rsx,rsy;
	long nx,ny,nsx,nsy;

	long cx2,cy2;   // upper right location
	long nx2,ny2;

	other->GetGeometry(&cx,&cy,&cw,&ch);

#if WDS_INTERSECT_DEBUG
	long count=0;
#endif
	ToHead();
	while( (rectangle=PostIncrement()) != NULL )
		{
		rectangle->GetGeometry(&rx,&ry,&rsx,&rsy);

#if WDS_INTERSECT_DEBUG
		EW_PRINT(EW_GFX,EW_LOG,"EW_RectangleList::Intersects() %d: %d,%d %d,%d on %d,%d %d,%d",count++,cx,cy,cw,ch,rx,ry,rsx,rsy);
#endif

		cx2=cx+cw;
		cy2=cy+ch;

		nx2=rx+rsx;
		ny2=ry+rsy;

		nx=EW_MAX(rx,cx);
		ny=EW_MAX(ry,cy);
		nx2=EW_MIN(nx2,cx2);
		ny2=EW_MIN(ny2,cy2);

		nsx=EW_MAX(0,nx2-nx);
		nsy=EW_MAX(0,ny2-ny);

#if WDS_INTERSECT_DEBUG
		EW_PRINT(EW_GFX,EW_LOG,"    result %d,%d %d,%d",nx,ny,nsx,nsy);
#endif

		if(intersection==this)
			rectangle->SetGeometry(nx,ny,nsx,nsy);

		if(nsx>0 && nsy>0)
			{
			if(intersection!=this)
				result->AddRectangle(nx,ny,nsx,nsy);

			intersects=TRUE;
			}
		}

	if(intersection==this)
		Prune();

#if WDS_INTERSECT_DEBUG
		EW_PRINT(EW_GFX,EW_LOG,"    intersects=%d",intersects);
#endif

	return intersects;
	}
