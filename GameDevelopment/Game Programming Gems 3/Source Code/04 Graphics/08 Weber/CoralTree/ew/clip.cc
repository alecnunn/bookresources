/******************************************************************************

Coral Tree ew/clip.cc
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

/*******************************************************************************

	Clip level 0 is the whole window and can not be altered except by
	EW_Window routines, and then only to set it to the current window frame.

	Apps should only push and pop in their own known relative context.
	The stack must be replaced to it's orignal state before control drops
	back down into the system.

	Not all widgets need to waste the overhead of clipping.  Generally, it is
	the enclosing and grouping widgets that should call
	SetClipping(EW_CLIPPING_CHILDREN) to limit the rendering of their children.

******************************************************************************/



#include "ew.h"



#define EW_CLIP_DEBUG					FALSE
#define EW_CLIP_DEBUG_CONTRAINTS		(clip_level<2)
#define EW_CLIP_PROTECT					TRUE	// check bounds on some arguments
#define	EW_CLIP_MENTION_PREVIOUS_NULL	FALSE	// log when there was no previous clip



/******************************************************************************
long	EW_Window::ClipRegionIntersectsBounds(long x,long y,long sx,long sy)

******************************************************************************/
long EW_Window::ClipRegionIntersectsBounds(long x,long y,long sx,long sy) 
	{
	EW_Rectangle box;

	box.SetGeometry(x,y,sx,sy);

	return GetClipStack()->GetCurrentRegion()->Intersect(&box,NULL);
	}



/******************************************************************************
void	EW_Window::GetClipBounds(long *cx,long *cy,long *cw,long *ch)

Keywords:
******************************************************************************/
void EW_Window::GetClipBounds(long *cx,long *cy,long *cw,long *ch)
	{
	EW_ClipRegion	*rectanglelist=clipstack.SetCurrentIndex(clip_level);

	if(rectanglelist)
		rectanglelist->GetBounds(cx,cy,cw,ch);
	else
		{
		*cx=0;
		*cy=0;
		*cw=0;
		*ch=0;
		}
	}



/******************************************************************************
void	EW_Window::CopyClipRegionTo(EW_RectangleList *other)

Keywords:
******************************************************************************/
void EW_Window::CopyClipRegionTo(EW_RectangleList *other)
	{
	if(clip_level<0)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::CopyClipRegionTo() cannot read region\n",clip_level);
		return;
		}

	EW_ClipRegion	*rectanglelist=clipstack.SetCurrentIndex(clip_level);
	EW_Rectangle	*rectangle;
	long			rx,ry,rsx,rsy;

	other->DeleteRectangles();

	rectanglelist->ToHead();
	while( (rectangle=rectanglelist->PostIncrement()) != NULL )
		{
		rectangle->GetGeometry(&rx,&ry,&rsx,&rsy);
		other->AddRectangle(rx,ry,rsx,rsy);
		}
	}



/******************************************************************************
void	EW_Window::CopyClipRegionFrom(EW_RectangleList *other)

Keywords:
******************************************************************************/
void EW_Window::CopyClipRegionFrom(EW_RectangleList *other)
	{
	if(clip_level<=0)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::CopyClipRegionFrom() cannot alter region %d\n",clip_level);
		return;
		}

	SetAnyClipRegion(clip_level,other);
	}



/******************************************************************************
void	EW_Window::SetAnyClipRegion(long level,EW_RectangleList *set)

	note that where level!=clip_level, changes do not propagate as if changed
		properly with pushes and pops

Keywords: private
******************************************************************************/
void EW_Window::SetAnyClipRegion(long level,EW_RectangleList *set)
	{
#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::SetAnyClipRegion() 0x%x level=%d/%d (%s)",
													(long)this,level,clip_level,GetTitle());
#endif

	if(level<0)
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::SetAnyClipRegion() cannot alter region %d\n",level);

	EW_ClipRegion	*rectanglelist=clipstack.SetCurrentIndex(level);

	// restore otherwise current level, NOOP if same
	clipstack.SetCurrentIndex(clip_level);

	EW_Rectangle		*rectangle;
	long				rx,ry,rsx,rsy;

	rectanglelist->DeleteRectangles();

	set->ToHead();
	while( (rectangle=set->PostIncrement()) != NULL )
		{
		rectangle->GetGeometry(&rx,&ry,&rsx,&rsy);

#if EW_CLIP_PROTECT
		long screenx,screeny;
#if EW_CLIP_DEBUG
		long ax=rx,ay=ry,aw=rsx,ah=rsy;
#endif

		GetScreenSize(&screenx,&screeny);

		rx=EW_MAX(rx,0);
		ry=EW_MAX(ry,0);
		rsx=EW_MAX(rsx,0);
		rsy=EW_MAX(rsy,0);

		rx=EW_MIN(rx,screenx);
		ry=EW_MIN(ry,screeny);
		rsx=EW_MIN(rsx,screenx-rx);
		rsy=EW_MIN(rsy,screeny-ry);

#if EW_CLIP_DEBUG
		if(ax!=rx || ay!=ry || aw!=rsx || ah!=rsy)
			{
			EW_PRINT(EW_GFX,EW_LOG,"EW_Window::SetAnyClipRegion(%d,...) %d,%d %d,%d constrained to %d,%d %d,%d",
																								level,ax,ay,aw,ah,rx,ry,rsx,rsy);
			}
#endif

#endif

		rectanglelist->AddRectangle(rx,ry,rsx,rsy);
		};

	if(level==clip_level)
		ResetClipRegion(clip_level>0);
	}



/******************************************************************************
void	EW_Window::IntersectClipRegion(long cx,long cy,long cw,long ch)

	sets the new current clip region to the intersection of
		the current and the passed

Keywords:
******************************************************************************/
void EW_Window::IntersectClipRegion(long cx,long cy,long cw,long ch)
	{
	EW_ClipRegion	*rectanglelist=clipstack.SetCurrentIndex(clip_level);

#if TRUE
	EW_Rectangle	rectangle;

	rectangle.SetGeometry(cx,cy,cw,ch);

#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::IntersectClipRegion(%d,%d %d,%d) on level %d",cx,cy,cw,ch,clip_level);
#endif

	// self-argument safe
	rectanglelist->Intersect(&rectangle,rectanglelist);

	ResetClipRegion(clip_level>0);

#else
	EW_Rectangle		*rectangle;

	long rx,ry,rsx,rsy;
	
	long cx2,cy2;	// upper right location
	long rx2,ry2;
	long m=0;

	rectanglelist->ToHead();
	while( (rectangle=rectanglelist->PostIncrement()) != NULL )
		{
		rectangle->GetGeometry(&rx,&ry,&rsx,&rsy);

#if FALSE && EW_CLIP_DEBUG
		if(EW_CLIP_DEBUG_CONTRAINTS)
			EW_PRINT(EW_GFX,EW_LOG,"EW_Window::IntersectClipRegion(%d,%d %d,%d) on %d: %d,%d %d,%d",cx,cy,cw,ch,m++,rx,ry,rsx,rsy);
#endif

		cx2=cx+cw;
		cy2=cy+ch;

		rx2=rx+rsx;
		ry2=ry+rsy;

		rx=EW_MAX(rx,cx);
		ry=EW_MAX(ry,cy);
		rx2=EW_MIN(rx2,cx2);
		ry2=EW_MIN(ry2,cy2);

		rsx=EW_MAX(0,rx2-rx);
		rsy=EW_MAX(0,ry2-ry);

		rectangle->SetGeometry(rx,ry,rsx,rsy);
		}

	rectanglelist->Prune();
#endif
	}



/******************************************************************************
void	EW_Window::RewindClipRegion(void)

Keywords:
******************************************************************************/
void EW_Window::RewindClipRegion(void)
	{
	long mirror=clipstack.GetCurrentRegion()->GetMirror();

#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::RewindClipRegion() 0x%x from clip_level=%d mirror=%d (%s)",
																		(long)this,clip_level,mirror,GetTitle());
#endif

	long copylevel=clip_level-1;

	if(mirror>=0)
		copylevel=mirror-1;
	
	if(copylevel<0)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::RewindClipRegion(): Attempt to rewind past beginning");
		}
	else
		{
		EW_ClipRegion *copyregion=clipstack.SetCurrentIndex(copylevel);
		clipstack.SetCurrentIndex(clip_level);

		PushClipRegion();
		CopyClipRegionFrom(copyregion);
		clipstack.GetCurrentRegion()->SetMirror(copylevel);
		ResetClipRegion(clip_level>0);
		}
	}



/******************************************************************************
void	EW_Window::RestoreClipRegion(void)

Keywords:
******************************************************************************/
void EW_Window::RestoreClipRegion(void)
	{
	long mirror=clipstack.GetCurrentRegion()->GetMirror();

#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::RestoreClipRegion() 0x%x from clip_level=%d mirror=%d (%s)",
																	(long)this,clip_level,mirror,GetTitle());
#endif

	
	if(mirror<0)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::RestoreClipRegion(): Attempt to restore non-rewound level");
		}
	else
		{
		clipstack.GetCurrentRegion()->SetMirror(-1);
		PopClipRegion();
		}
	}



/******************************************************************************
void	EW_Window::PushClipRegion(void)

Keywords:
******************************************************************************/
void EW_Window::PushClipRegion(void)
	{
#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::PushClipRegion() 0x%x from clip_level=%d (%s)",
													(long)this,clip_level,GetTitle());
#endif

	EW_ClipRegion *previous=clipstack.SetCurrentIndex(clip_level++);

	clipstack.SetCurrentIndex(clip_level);

	CopyClipRegionFrom(previous);
	clipstack.GetCurrentRegion()->SetMirror(-1);
	}



/******************************************************************************
void	EW_Window::PopClipRegion(void)

Keywords:
******************************************************************************/
void EW_Window::PopClipRegion(void)
	{
#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::PopClipRegion() 0x%x from clip_level=%d (%s)",
													(long)this,clip_level,GetTitle());
#endif

	if(clipstack.GetCurrentRegion()->GetMirror()>=0)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::PopClipRegion(): Attempt to pop a rewound level");
		}
	else if(clip_level<1)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::PopClipRegion(): Attempt to pop from empty stack");
		}
	else
		{
		clipstack.SetCurrentIndex(clip_level)->DeleteRectangles();
		clipstack.SetCurrentIndex(--clip_level);

		ResetClipRegion(clip_level>0);
		}
	}



/******************************************************************************
void	EW_Window::ResetClipRegion(long set)

	activate/deactivate gfx-dependent clipping rectangle

	set:
		0 = no mask
		1 = mask on
		2 = mask everything (prevent output)

Keywords: private
******************************************************************************/
void EW_Window::ResetClipRegion(long set)
	{
#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		printf("EW_Window::ResetClipRegion(%d)\n",set);
#endif

	if(!GetOpened())
		return;

	long cx,cy,cw,ch;
	long bx,by,bw,bh;
	long sx,sy;
	long m;

	GetSize(&sx,&sy);

	GetClipBounds(&bx,&by,&bw,&bh);

	if(bw<=0 || bh<=0)
		set=2;

	EW_ClipRegion	*rectanglelist=clipstack.SetCurrentIndex(clip_level);
	long			number=rectanglelist->GetNumberNodes();
	EW_Rectangle	*rectangle;

#if EW_CLIP_DEBUG
	if(EW_CLIP_DEBUG_CONTRAINTS)
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::ResetClipRegion(%d) level %d number %d"
								" bounds %d,%d %d,%d window %d,%d (%s)",
				set,clip_level,number,bx,by,bw,bh,sx,sy,GetTitle());
#endif

	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		if(set==2)
			{
			XRectangle xrectangle;
			// prevent all output
			XSetClipRectangles(display,GetGC(),0,0,&xrectangle,0,Unsorted);
			}
		else if(set)
			{
			XRectangle		*xrectangles=(XRectangle *)osdMalloc(number*sizeof(XRectangle));

			rectanglelist->ToHead();
			for(m=0;m<number && (rectangle=rectanglelist->PostIncrement())!=NULL;m++)
				{
				rectangle->GetGeometry(&cx,&cy,&cw,&ch);

#if EW_CLIP_DEBUG
				if(EW_CLIP_DEBUG_CONTRAINTS)
					{
					if(number>1)
						EW_PRINT(EW_GFX,EW_LOG,"EW_Window::ResetClipRegion(%d) level %d rect %d: %d,%d %d,%d",
																				set,clip_level,m,cx,cy,cw,ch);
					}
#endif

				// mask output
				xrectangles[m].x=cx;
				xrectangles[m].width=cw;

				xrectangles[m].y=EW_REMAP_Y(cy+ch-1,sy);
				xrectangles[m].height=ch;
				}

			XSetClipRectangles(display,GetGC(),0,0,xrectangles,number,Unsorted);

			osdFree(xrectangles);
			}
		else
			{
			// allow all output
			XSetClipMask(display,GetGC(),None);
			}

	#elif EW_GL==EW_OPENGL

		/* OpenGL */
		if(set==2)
			{
			glScissor(0,0,0,0);
			glEnable(GL_SCISSOR_TEST);
			glDisable(GL_STENCIL_TEST);
			}
		else if(set)
			{

			// if more than one rectangle, need stenciling
			if(number>1)
				{
				glDisable(GL_SCISSOR_TEST);
				glEnable(GL_STENCIL_TEST);
				glClearStencil(0x0);
				glClear(GL_STENCIL_BUFFER_BIT);

				glStencilFunc(GL_ALWAYS,0x1,0x1);
				glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);

				if(GetRGBMode())
					{
					glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
					glDepthMask(GL_FALSE);
					}
				else
					glIndexMask(0x0);

				rectanglelist->ToHead();
				for(m=0;m<number && (rectangle=rectanglelist->PostIncrement())!=NULL;m++)
					{
					rectangle->GetGeometry(&cx,&cy,&cw,&ch);

					// intentionally x+w, not x+w-1
					glRecti(cx,cy,cx+cw,cy+ch);

#if FALSE
					EW_PRINT(EW_GFX,EW_LOG,
							"EW_Window::ResetClipRegion(%d) level %d rect %d: %d,%d %d,%d in bounds %d,%d %d,%d",
																set,clip_level,m,cx,cy,cw,ch,bx,by,bw,bh);
#endif
					}

				glStencilFunc(GL_EQUAL,0x1,0x1);
				glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

				if(GetRGBMode())
					{
					glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
					glDepthMask(GL_TRUE);
					}
				else
					glIndexMask(0xFFFFFFFF);

//				RectFill(0,0,sx,sy,EW_RED);
				}
			else
				{
#if FALSE
				EW_PRINT(EW_GFX,EW_LOG,"EW_Window::ResetClipRegion(%d) level %d single rect %d,%d %d,%d",
															set,clip_level,bx,by,bw,bh);
#endif
				glScissor(bx,by,bw,bh);
				glEnable(GL_SCISSOR_TEST);
				glDisable(GL_STENCIL_TEST);
//				RectFill(0,0,sx,sy,EW_BLUE);
				}
			}
		else
			{
			glDisable(GL_SCISSOR_TEST);
			glDisable(GL_STENCIL_TEST);
			}

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		long result;

		// deselect any previous region
		result=SelectClipRgn(hdc,NULL);
		if(result==ERROR)
			{
#if	EW_CLIP_MENTION_PREVIOUS_NULL
			EW_PRINT(EW_GFX,EW_LOG,"EW_Window::ResetClipRegion() couldn't deselect previous region");
#endif
			}

		if(set==2)
			{
			// prevent all output
			HRGN rectregion=CreateRectRgn(0,0,0,0);

			result=SelectClipRgn(hdc,rectregion);
			if(result!=NULLREGION)
				EW_PRINT(EW_GFX,EW_WARN,"SelectClipRgn() could not block output");

			if(!DeleteObject(rectregion))
				EW_PRINT(EW_GFX,EW_WARN,"EW_Window::ResetClipRegion() couldn't delete sub clipping rect region");
			}
		else if(set && number>0)
			{
			HRGN resultant=0;
			long first=TRUE;

			rectanglelist->ToHead();
			for(m=0;m<number && (rectangle=rectanglelist->PostIncrement())!=NULL;m++)
				{
				rectangle->GetGeometry(&cx,&cy,&cw,&ch);

				long rx1=cx;
				long rx2=cx+cw;
				long ry1=EW_REMAP_Y(cy+ch-1,sy);
				long ry2=EW_REMAP_Y(cy,sy)+1;

#if EW_CLIP_DEBUG
				if(EW_CLIP_DEBUG_CONTRAINTS)
					EW_PRINT(EW_GFX,EW_LOG,"EW_Window::ResetClipRegion() CreateRectRgn(%d,%d,%d,%d) inside %d,%d",
																									rx1,ry1,rx2,ry2,sx,sy);
#endif

				HRGN rectregion=CreateRectRgn(rx1,ry1,rx2,ry2);
				if(rectregion==NULL)
					{
					EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ResetClipRegion() couldn't create clipping region");
					return;
					}
				else
					{
					if(first)
						{
						resultant=rectregion;
						first=FALSE;
						}
					else
						{
						if(CombineRgn(resultant,resultant,rectregion,RGN_OR)==ERROR)
							EW_PRINT(EW_GFX,EW_LOG,"EW_Window::ResetClipRegion() error combining clipping regions");

						if(!DeleteObject(rectregion))
							EW_PRINT(EW_GFX,EW_WARN,"EW_Window::ResetClipRegion() couldn't delete sub clipping rect region");
						}
					}
				}
				
			long regiontype=SelectClipRgn(hdc,resultant);

#if EW_CLIP_DEBUG
			if(EW_CLIP_DEBUG_CONTRAINTS)
				switch(regiontype)
					{
					case NULLREGION:
						EW_PRINT(EW_GFX,EW_LOG,"null clipping region (union of area is empty)");
						break;
					case SIMPLEREGION:
						EW_PRINT(EW_GFX,EW_LOG,"simple clipping region (single rectangle)");
						break;
					case COMPLEXREGION:
						EW_PRINT(EW_GFX,EW_LOG,"complex clipping region (multiple rectangles or contacts edge of window)");
						break;
					}
#endif
			if(regiontype==ERROR)
				{
				EW_PRINT(EW_GFX,EW_ERROR,
						"EW_Window::ResetClipRegion() error setting clipping region 0x%x regiontype=%d number=%d",
						resultant,regiontype,number);
				};

			// region is copied, so we can delete the original
			result=DeleteObject(resultant);
			if(!result)
				{
				EW_PRINT(EW_GFX,EW_WARN,
						"EW_Window::ResetClipRegion() couldn't delete old clipping region 0x%x regiontype=%d number=%d",
						resultant,regiontype,number);
				}
			}

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		if(set)
			scrmask(cx,cx+cw-1,cy,cy+ch-1);
		else
			scrmask(ix,iy,ix+sx-1,iy+sy-1);

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
EW_ClipRegion    *EW_ClipRegionStack::SetCurrentIndex(long set)

******************************************************************************/
EW_ClipRegion *EW_ClipRegionStack::SetCurrentIndex(long set)
	{
	// most common case: fastest way through
	if(set==index)
		return current;

	long m;

	// up
	if(set>index)
		{
		if(set>GetNumberNodes())
			EW_PRINT(EW_GFX,EW_WARN,
							"ClipRegionStack::SetCurrentIndex() creating more than one node to move to index %d of %d nodes"
							,set,GetNumberNodes());
		for(m=GetNumberNodes();m<set+1;m++)
			{
			EW_ClipRegion *list=new EW_ClipRegion;
			AppendNode(list);	// sets current already
			index=set;
			}

		// if didn't add nodes
		for(m=index;m<set;m++)
			PreIncrement();

		index=set;

		return (current=GetCurrent());
		}

	// down
	while(index>=0 && index>set)
		{
		index--;

#if EW_CLIP_DEBUG
		long clip_level=set;
		if(EW_CLIP_DEBUG_CONTRAINTS)
			EW_PRINT(EW_GFX,EW_WARN,"ClipRegionStack::SetCurrentIndex() decrement to %d toward %d of %d nodes",
																						index,set,GetNumberNodes());
#endif

		current=PreDecrement();
		}

	return current;
	}



/******************************************************************************
void	EW_ClipRegionStack::ClearList(void)

******************************************************************************/
void EW_ClipRegionStack::ClearList(void)
	{
	EW_ClipRegion *node;

	while( (node=ToHead()) != NULL )
		{
		if( !RemoveNode(node) )
			{
			EW_PRINT(EW_EW,EW_CRASH,"ClipRegionStack::ClearList() 0x%x couldn't remove node 0x%x",(long)this,(long)node);
			return;
			}

		delete node;
		}
	}
