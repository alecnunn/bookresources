/******************************************************************************

Coral Tree wds/bevel.cc
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



#include "wds.h"


#define WDS_CLEAR_BEVEL			FALSE					/* TRUE = always clear inside every bevel box */
#define WDS_BEVEL_DEBUG			FALSE					/* TRUE = print verbosity */



/******************************************************************************
void	wdsBevelBox(EW_Window *root,WDS_Widget *widget,
							long x,long y,long dx,long dy,
							long bevel_depth,long state,long force_clear,
							long stx,long sty,long stsx,long stsy,
							WDS_String *bevelstring)

	see bevel.h for state values

	cursor is cursor position (<0 for none)

	widget can be NULL, root cannot

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void wdsBevelBox(EW_Window *root,WDS_Widget *widget,long x,long y,long dx,long dy,long bevel_depth,long state,long force_clear,
								long stx,long sty,long stsx,long stsy,WDS_String *bevelstring)
	{
	long depth=bevel_depth;
	long ridge=0;
	long x2,y2;
	long m;

	long pointdir=0;

	if(state&WDS_BEVELBOX_POINT)
		pointdir= 1 + 1*((state&WDS_BEVELBOX_POINT_VERTICAL)==0) + 2*((state&WDS_BEVELBOX_POINT_BACK)!=0);

	if(root==NULL)
		{
		EW_PRINT(EW_WIDGET,EW_ERROR,"wdsBevelBox() root=NULL");
		return;
		}

	long index_widget=		root->GetColor(EW_COLOR_WIDGET);
	long index_back=		root->GetColor(EW_COLOR_BACKGROUND);
	long index_lit=			root->GetColor(EW_COLOR_LIT);
	long index_halflit=		root->GetColor(EW_COLOR_HALF_LIT);
	long index_shadow=		root->GetColor(EW_COLOR_SHADOW);
	long index_halfshadow=	root->GetColor(EW_COLOR_HALF_SHADOW);
	long index_dither=		widget? widget->InterpretElement(widget->GetWidgetState(),WDS_ELEMENT_COLOR_WIDGET_DITHER): 0;

	long draw_top=		!(state&WDS_BEVELBOX_OMIT_TOP);
	long draw_right=	!(state&WDS_BEVELBOX_OMIT_RIGHT);
	long draw_left=		!(state&WDS_BEVELBOX_OMIT_LEFT);
	long draw_bottom=	!(state&WDS_BEVELBOX_OMIT_BOTTOM);

	long round=			(state&WDS_BEVELBOX_ROUND);
	long angle45=		(state&WDS_BEVELBOX_ANGLE_45);
	long elliptical=	(state&WDS_BEVELBOX_ELLIPTICAL);

	long mark=			(state&WDS_BEVELBOX_MARK_MASK);
	long checkmark=		(mark==WDS_BEVELBOX_CHECKMARK);
	long plusmark=		(mark==WDS_BEVELBOX_PLUSMARK);

	long label_left=	(state&WDS_BEVELBOX_LABEL_LEFT);

	if(label_left)
		state^=WDS_BEVELBOX_LABEL_LEFT;

#if WDS_BEVEL_DEBUG
	printf("wdsBevelBox(0x%x,%d,%d,%d,%d,%d,%d,...)\n",root,x,y,dx,dy,state,force_clear);
#endif

	if(bevel_depth && depth<1)
		depth=1;

	x2=x+dx-1;
	y2=y+dy-1;

	// in order, each of two lips goes:
	long over_outer[2],over_inner[2],under_inner[2],under_outer[2];

	over_outer[0]=		(state&WDS_BEVELBOX_IN)? index_shadow:		index_lit;
	under_outer[0]=		(state&WDS_BEVELBOX_IN)? index_lit:			index_shadow;

	if(state&WDS_BEVELBOX_SMOOTH)
		{
		over_inner[0]=	(state&WDS_BEVELBOX_IN)? index_halfshadow:	index_halflit;
		under_inner[0]=	(state&WDS_BEVELBOX_IN)? index_halflit:		index_halfshadow;
		}
	else
		{
		over_inner[0]=over_outer[0];
		under_inner[0]=under_outer[0];
		}

	if(state&WDS_BEVELBOX_SEAM)
		{
		over_outer[1]=over_outer[0];
		over_inner[1]=over_inner[0];
		under_outer[1]=under_outer[0];
		under_inner[1]=under_inner[0];
		}
	else
		{
		over_outer[1]=under_inner[0];
		over_inner[1]=under_inner[0];
		under_outer[1]=under_outer[0];
		under_inner[1]=under_outer[0];

		under_outer[0]=over_inner[0];
		under_inner[0]=over_inner[0];

		over_inner[0]=over_outer[0];
		}

	// if borderless with depth, need to draw border in background color (clear edge)
	if(!state && depth && force_clear)
		{
		state=WDS_BEVELBOX_OUT;
		for(m=0;m<2;m++)
			{
			over_outer[m]=index_back;
			over_inner[m]=index_back;
			under_outer[m]=index_back;
			under_inner[m]=index_back;
			}
		}

	if(state)
		{
		if( (state&WDS_BEVELBOX_RIDGE) && bevel_depth )
			{
			ridge=bevel_depth/3;

			if(ridge<1)
				ridge=1;
			}
		else
			ridge=0;

		depth=bevel_depth-ridge;
		if(bevel_depth && depth<1)
			depth=1;

		long depth2=depth/2;
		if(bevel_depth && depth2<1)
			depth2=1;

		if(mark)
			{
			long ix,iy;

			if(checkmark)
				{
				for(m=0;m<dx;m++)
					{
					ix=x+m;
					iy=y+abs(m-dx/3);
					root->Line(ix,iy,ix,iy+2,index_widget);
					}
				}
			else
				{
				ix=x+dx/2;
				iy=y+dy/2;

				root->Line(x,iy,x+dx-1,iy,index_widget);

				if(plusmark)
					root->Line(ix,y,ix,y+dy-1,index_widget);
				}

			// never clear after a checkmark
			force_clear=FALSE;
			}
		else if(elliptical)
			{
			long cx=x+dx/2;
			long cy=y+dy/2;
			long rx=dx/2;
			long ry=dy/2;

			m=0;
			root->ArcFill(cx,cy,rx-m,ry-m,45.0,225.0,over_outer[0]);

			m=depth2/2;
			root->ArcFill(cx,cy,rx-m,ry-m,45.0,225.0,over_inner[0]);

			m=depth2;
			root->ArcFill(cx,cy,rx-m,ry-m,45.0,225.0,index_widget);

			m=depth2+ridge;
			root->ArcFill(cx,cy,rx-m,ry-m,45.0,225.0,under_inner[0]);

			m=depth2*3/2+ridge;
			root->ArcFill(cx,cy,rx-m,ry-m,45.0,225.0,under_outer[0]);

			m=0;
			root->ArcFill(cx,cy,rx-m,ry-m,225.0,405.0,under_outer[1]);

			m=depth2/2;
			root->ArcFill(cx,cy,rx-m,ry-m,225.0,405.0,under_inner[1]);

			m=depth2;
			root->ArcFill(cx,cy,rx-m,ry-m,225.0,405.0,index_widget);

			m=depth2+ridge;
			root->ArcFill(cx,cy,rx-m,ry-m,225.0,405.0,over_inner[1]);

			m=depth2*3/2+ridge;
			root->ArcFill(cx,cy,rx-m,ry-m,225.0,405.0,over_outer[1]);

			m=depth+ridge;
			root->ArcFill(cx,cy,rx-m,ry-m,0.0,360.0,index_widget);

			// already cleared (always clear with ellipses)
			force_clear=FALSE;
			}
		else if(pointdir)
			{
			long nx,ny,ndx,ndy,nx2,ny2;

			if(angle45)
				{
				if(pointdir==1 || pointdir==3)
					{
					// vertical
					ndx=dx/2*2+1;		// make odd (round down)
					ndy=(ndx+1)/2;		// force 45 degree angles

					ndy=EW_MIN(ndy,dy);
					ndx=ndy*2-1;
					}
				else
					{
					ndy=dy/2*2+1;
					ndx=(ndy+1)/2;

					ndx=EW_MIN(ndx,dx);
					ndy=ndx*2-1;
					}
				}
			else
				{
				ndx=dx;
				ndy=dy;
				}

			nx=x+(dx-ndx)/2;
			ny=y+(dy-ndy)/2;
			nx2=nx+ndx-1;
			ny2=ny+ndy-1;

			long a[2],b[2],c[2];
			long reverse[3];

			// note mirroring before color reversal
			switch(pointdir)
				{
				case 1:		// up
					reverse[0]=TRUE;
					reverse[1]=FALSE;
					reverse[2]=TRUE;
					a[0]=nx;
					a[1]=ny;
					b[0]=nx2;
					b[1]=ny;
					c[0]=nx+ndx/2;
					c[1]=ny2;
					break;
				case 2:		// right
					reverse[0]=FALSE;
					reverse[1]=TRUE;
					reverse[2]=FALSE;
					a[0]=nx;
					a[1]=ny2;
					b[0]=nx;
					b[1]=ny;
					c[0]=nx2;
					c[1]=ny+ndy/2;
					break;
				case 3:		// down
					reverse[0]=FALSE;
					reverse[1]=FALSE;
					reverse[2]=TRUE;
					a[0]=nx;
					a[1]=ny2;
					b[0]=nx2;
					b[1]=ny2;
					c[0]=nx+ndx/2;
					c[1]=ny;
					break;
				case 4:		// left
					reverse[0]=TRUE;
					reverse[1]=TRUE;
					reverse[2]=FALSE;
					a[0]=nx2;
					a[1]=ny2;
					b[0]=nx2;
					b[1]=ny;
					c[0]=nx;
					c[1]=ny+ndy/2;
					break;
				default:
					EW_PRINT(EW_WIDGET,EW_ERROR,"pointdir has bad value");
					reverse[0]=FALSE;
					reverse[1]=FALSE;
					reverse[2]=FALSE;
					break;
				};

			if(force_clear)
				root->TriangleFill(a,b,c,index_widget);
			force_clear=FALSE;

#define		WDS_BEVEL_INVERT(ix,x,sx)	(2*(x)+(sx)-1-(ix))

// pointdir 1 to 4 is: up, right, down, left
#define		WDS_BEVEL_POINT_LINE(ix1,iy1,m1,ix2,iy2,m2,color)													\
				{																								\
				switch(pointdir)																				\
					{																							\
					case 1:																						\
						root->Line(ix1,iy1+m1,ix2,iy2+m2,color);												\
						break;																					\
					case 2:																						\
						root->Line(ix1+m1,iy1,ix2+m2,iy2,color);												\
						break;																					\
					case 3:																						\
						root->Line(ix1,WDS_BEVEL_INVERT(iy1+m1,y,dy),ix2,WDS_BEVEL_INVERT(iy2+m2,y,dy),color);	\
						break;																					\
					case 4:																						\
						root->Line(WDS_BEVEL_INVERT(ix1+m1,x,dx),iy1,WDS_BEVEL_INVERT(ix2+m2,x,dx),iy2,color);	\
						break;																					\
					default:																					\
						EW_PRINT(EW_WIDGET,EW_ERROR,"pointdir has bad value");									\
						break;																					\
					};																							\
				};

			long vertical=(pointdir&1)!=0;

			long xy1=nx;
			long yx1=ny;
			long xy2=vertical? nx2:	nx;
			long yx2=vertical? ny:	ny2;

			long slack,add=0,mult=1;
			long ridge2=ridge;
			long ddepth=depth;
			long depth32;

			long side;
			for(side=0;side<3;side++)
				{
				switch(side)
					{
					// 0 = stern (as initialized above)

					case 1:		// starboard
						ddepth=depth*2;
						ridge2=ridge2*2;
						depth2=depth2*2;

//						slack=depth+ridge2-1;
						slack=depth+ridge2+1;

						xy2=vertical? nx+ndx/2-1:	nx2;
						yx2=vertical? ny2:			ny+ndy/2-1;
						add=slack;
						mult= -1;

						if(vertical)
							xy2+=(ndx%2);
						else
							yx2+=(ndy%2);
						break;

					case 2:		// port
						if(vertical && !(ndx%2))
							xy2++;
						else if(!vertical && !(ndy%2))
							yx2++;
						xy1=vertical? nx2:	nx;
						yx1=vertical? ny:	ny2;
						break;
					}

				depth32=depth2*3/2;
				if(depth32>ddepth)
					depth32=ddepth;

				if( (side==0 && draw_bottom) || (side==1 && draw_right) || (side==2 && draw_left) )
					{
					// outer
					for(m=0;m<depth2/2;m++)
						WDS_BEVEL_POINT_LINE(xy1,yx1,add+mult*m,xy2,yx2,mult*m,reverse[side]? under_outer[1]: over_outer[0]);
					for(;m<depth2;m++)
						WDS_BEVEL_POINT_LINE(xy1,yx1,add+mult*m,xy2,yx2,mult*m,reverse[side]? under_inner[1]: over_inner[0]);

					// ridge
					for(m=depth2;m<depth2+ridge2;m++)
						WDS_BEVEL_POINT_LINE(xy1,yx1,add+mult*m,xy2,yx2,mult*m,index_widget);

					// inner
					for(m=depth2+ridge2;m<depth32+ridge2;m++)
						WDS_BEVEL_POINT_LINE(xy1,yx1,add+mult*m,xy2,yx2,mult*m,reverse[side]? over_inner[1]: under_inner[0]);
					for(;m<ddepth+ridge2;m++)
						WDS_BEVEL_POINT_LINE(xy1,yx1,add+mult*m,xy2,yx2,mult*m,reverse[side]? over_outer[1]: under_outer[0]);
					}
				}
			}
		else
			{
/*
#define	WDS_BEVEL_LINELEFT		x+m,y+m+(draw_bottom),x+m,y2-m
#define	WDS_BEVEL_LINETOP		x+m,y2-m,x2-m-(draw_right),y2-m
#define	WDS_BEVEL_LINEBOTTOM	x+m,y+m,x2-m,y+m
#define	WDS_BEVEL_LINERIGHT		x2-m,y+m,x2-m,y2-m
*/
#define	WDS_BEVEL_LINELEFT		x+m,y+(draw_bottom? m+1: depth2),x+m,y2-(draw_top? m: depth2)
#define	WDS_BEVEL_LINETOP		x+(draw_left? m: depth2),y2-m,x2-(draw_right? m+1: depth2),y2-m
#define	WDS_BEVEL_LINEBOTTOM	x+(draw_left? m: depth2),y+m,x2-(draw_right? m: depth2),y+m
#define	WDS_BEVEL_LINERIGHT		x2-m,y+(draw_bottom? m: depth2),x2-m,y2-(draw_top? m: depth2)

			/* outer upper left */
			for(m=0;m<depth2/2;m++)
				{
				if(draw_left)
					root->Line(WDS_BEVEL_LINELEFT,over_outer[0]);
				if(draw_top)
					root->Line(WDS_BEVEL_LINETOP,over_outer[0]);
				}
			for(;m<depth2;m++)
				{
				if(draw_left)
					root->Line(WDS_BEVEL_LINELEFT,over_inner[0]);
				if(draw_top)
					root->Line(WDS_BEVEL_LINETOP,over_inner[0]);
				}

			/* ridge upper left */
			for(m=depth2;m<depth2+ridge;m++)
				{
				if(draw_left)
					root->Line(WDS_BEVEL_LINELEFT,index_widget);
				if(draw_top)
					root->Line(WDS_BEVEL_LINETOP,index_widget);
				}

			/* inner upper left */
			for(m=depth2+ridge;m<depth2*3/2+ridge;m++)
				{
				if(draw_left)
					root->Line(WDS_BEVEL_LINELEFT,under_inner[0]);
				if(draw_top)
					root->Line(WDS_BEVEL_LINETOP,under_inner[0]);
				}
			for(;m<depth+ridge;m++)
				{
				if(draw_left)
					root->Line(WDS_BEVEL_LINELEFT,under_outer[0]);
				if(draw_top)
					root->Line(WDS_BEVEL_LINETOP,under_outer[0]);
				}

			/* outer lower right */
			for(m=0;m<depth2/2;m++)
				{
				if(draw_bottom)
					root->Line(WDS_BEVEL_LINEBOTTOM,under_outer[1]);
				if(draw_right)
					root->Line(WDS_BEVEL_LINERIGHT,under_outer[1]);
				}
			for(;m<depth2;m++)
				{
				if(draw_bottom)
					root->Line(WDS_BEVEL_LINEBOTTOM,under_inner[1]);
				if(draw_right)
					root->Line(WDS_BEVEL_LINERIGHT,under_inner[1]);
				}

			/* ridge lower right */
			for(m=depth2;m<depth2+ridge;m++)
				{
				if(draw_bottom)
					root->Line(WDS_BEVEL_LINEBOTTOM,index_widget);
				if(draw_right)
					root->Line(WDS_BEVEL_LINERIGHT,index_widget);
				}

			/* inner lower right */
			for(m=depth2+ridge;m<depth2*3/2+ridge;m++)
				{
				if(draw_bottom)
					root->Line(WDS_BEVEL_LINEBOTTOM,over_inner[1]);
				if(draw_right)
					root->Line(WDS_BEVEL_LINERIGHT,over_inner[1]);
				}
			for(;m<depth+ridge;m++)
				{
				if(draw_bottom)
					root->Line(WDS_BEVEL_LINEBOTTOM,over_outer[1]);
				if(draw_right)
					root->Line(WDS_BEVEL_LINERIGHT,over_outer[1]);
				}

			// corner rounding (diagonal clipping)

			if(draw_top && draw_left && round)
				for(m=0;m<depth;m++)
					{
					root->Line(x,y2-m,x+depth-m-1,y2-m,index_back);
					root->Point(x+depth-m,y2-m,over_outer[0]);
					}

			if(draw_top && draw_right && round)
				for(m=0;m<depth;m++)
					{
					root->Line(x2-depth+m+1,y2-m,x2,y2-m,index_back);
					root->Point(x2-depth+m+1,y2-m-1,under_outer[1]);
					}

			if(draw_bottom && draw_left && round)
				for(m=0;m<depth;m++)
					{
					root->Line(x,y+m,x+depth-m-1,y+m,index_back);
					root->Point(x+depth-m,y+m,under_outer[1]);
					}

			if(draw_bottom && draw_right && round)
				for(m=0;m<depth;m++)
					{
					root->Line(x2-depth+m+1,y+m,x2,y+m,index_back);
					root->Point(x2-depth+m,y+m,under_outer[1]);
					}
			}

		depth=bevel_depth;
		if(bevel_depth && depth<1)
			depth=1;
		}

	long inset=depth+ridge;

	if(WDS_CLEAR_BEVEL || force_clear)
		{
		if(index_dither>0)
			{
			root->LineOut(x+inset,y+inset,x2-x-2*inset+1,y2-y-2*inset+1,0,2,index_widget);
			root->LineOut(x+inset,y+inset,x2-x-2*inset+1,y2-y-2*inset+1,1,2,index_dither);
			}
		else
			root->RectFill(x+inset,y+inset,x2-inset,y2-inset,index_widget);
		}

	long string_op=WDS_STRING_DRAW;

	if( !(label_left) )
		string_op|=WDS_STRING_CENTER;

//	printf("%d,%d %d,%d (%s)\n",stx,sty,stsx,stsy,bevelstring? bevelstring->Buffer(): "NULL");

	/* write text */
	if( bevelstring!=NULL && bevelstring->Buffer()!=NULL )
		{

/*
		long x3,y3,x4,y4;
		x3=x+inset+subinsetx+labelx;
		y3=y+inset+subinsety+labely;
		x4=dx-2*(inset+subinsetx)-labelx;
		y4=dy-2*(inset+subinsety)-labely;
*/

//		root->RectFill(x3,y3,x3+x4-1,y3+y4-1,EW_GREEN);

		bevelstring->Operate(root,stx,sty,stsx,stsy,0,0,string_op);
		}
	}
