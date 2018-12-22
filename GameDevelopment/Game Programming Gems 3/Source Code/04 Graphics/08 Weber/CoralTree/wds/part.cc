/******************************************************************************

Coral Tree wds/part.cc
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

#if FALSE
	#undef WDS_COLOR_WIRE
	#define WDS_COLOR_WIRE TRUE
#endif

#define WDS_PARTITION_FILL_DEBUG		(WDS_COLOR_WIRE==TRUE)
#define WDS_PARTITION_RESIZE_DEBUG		FALSE
#define WDS_PARTITION_DRAG_DEBUG		FALSE
#define WDS_PARTITION_REGISTER_DEBUG	FALSE
#define WDS_PARTITION_EVENT_DEBUG		FALSE
#define WDS_PARTITION_PREFS_ARE_FIXED	TRUE	// TRUE= widgets with pref set are considered fixed

#define WDS_PARTITION_PREF_HACK			FALSE	// TRUE= temporary hack limiting pref setting
#define WDS_PARTITION_MAX_ON_WINDOW		FALSE	// size limiting on max size

#define WDS_PARTITION_WIRE				FALSE
#define WDS_DUMP						FALSE	// TRUE = print verbose window/widget-hierarchy info when parent is a window

#define WDS_PARTITION_REPRESS			TRUE	// activate repression stage
#define WDS_PARTITION_DIRTY_POSTCHECK	TRUE	// check child dirtiness only after all resizes are done

#define WDS_PARTITION_RESIZE_MODE		1		// 0 = resize once
												// 1 = extra resizes if CheckSize() says changed	(preferred)
												// 2 = 3 resizes every time

												// if only allow a single recursive resize
												//		(more efficient, restricted flexibility)
												// may have problems with direct children whose size depends their decendents
												// or have a variable fixed primary dimension.
												// 3 resizes every time can be very slow considering the underlying hierarchy


#define WDS_PARTITION_FILL1		( WDS_PARTITION_FILL_DEBUG? EW_RED:		InterpretElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND) )
#define WDS_PARTITION_FILL2		( WDS_PARTITION_FILL_DEBUG? EW_YELLOW:	InterpretElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND) )
#define WDS_PARTITION_FILL3		( WDS_PARTITION_FILL_DEBUG? EW_PURPLE:	InterpretElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND) )
#define WDS_PARTITION_FILL4		( WDS_PARTITION_FILL_DEBUG? EW_BLUE:	InterpretElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND) )


/******************************************************************************
		WDS_Partition::WDS_Partition()

******************************************************************************/
WDS_Partition::WDS_Partition()
	{
	registered=FALSE;
	direction=WDS_HORIZONTAL;
	reverse=FALSE;
	drag=NULL;
	exclusive=FALSE;

	SetName("Partition");

	SetClipping(EW_CLIPPING_CHILDREN);
	SetInversion(TRUE);
	SetFlags(WDS_PARTITION_FLAGS_DEFAULT);

	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS);
	};


/******************************************************************************
void	WDS_Partition::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_Partition::Draw(EW_Redraw *redraw)
	{
//	printf("WDS_Partition::Draw() (%s)\n",GetNameString()->Buffer());

	EW_Window *root=GetRootWindow();
	long truex,truey,sx,sy;
	long limited=( (GetFlags()&WDS_PARTITION_FLAGS_LIMIT)!=0 );

	GetTrueGeometry(&truex,&truey,&sx,&sy);

#if WDS_COLOR_FILL
	root->RectFill(truex,truey,truex+sx-1,truey+sy-1,EW_MAGENTA);
#endif

#if WDS_COLOR_WIRE
	root->RectWire(truex+2,truey+2,truex+sx-3,truey+sy-3,EW_YELLOW);
#endif

	EW_Widget *node;					// shouldn't this be a WDS_Widget ?

	//long index= (direction==WDS_VERTICAL);
	long knob= InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PARTITION_KNOB_OFFSET);
	long attach= ( (GetFlags() & WDS_PARTITION_FLAGS_ATTACH_BIT) != FALSE );
	long width[3];
	long children=GetWidgetList()->GetNumberNodes();
	long m;
	//long n;
	long x,y,w,h,nextval= -1;
	long corner;
	long fix[2];		// number of widgets fixed to an edge
	//long pref[2],min[2],max[2];


	GetSeparatorWidth(width);

	// determine immobile partitions
	GetFixed(&fix[0],&fix[1]);

	// draw the partition bars

	if(reverse)
		GetWidgetList()->ToTail();
	else
		GetWidgetList()->ToHead();

	m=0;
	while( (node=PostIncDecrement(!reverse)) != NULL  || (!limited && nextval>0) )
		{
//		long mobile= !( m<=fix[0] || m>=children-fix[1] );
		long mobile= !(m<fix[0] || (attach && (m>(children-fix[1]-1) || children-fix[0]-fix[1]<2)) );

		long draw_as= (mobile && knob<0);

		SetWidgetState(draw_as);

		if(direction==WDS_HORIZONTAL)
			{
			/* horizontal partition */

			if(nextval>0)
				{
				GetRootWindow()->RectFill(nextval,truey,nextval+width[2]-1,truey+sy-1,
																			GetRootWindow()->GetColor(EW_COLOR_BACKGROUND));

				BevelBox(TRUE,TRUE,NULL,nextval+(width[2]-width[draw_as])/2,truey,width[draw_as],sy);

				if(mobile && knob>=0)
					{
					SetWidgetState(mobile);
					BevelBox(TRUE,TRUE,NULL,nextval+(width[2]-width[mobile])/2,truey+knob,width[mobile],width[mobile]);
					}
				}

			if(node==NULL)
				nextval= -1;
			else
				{
				node->GetTrueGeometry(&x,&y,&w,&h);

				nextval=x+w;

				// clear area around dirty widget in case it doesn't fill partition
//				if( node->GetDirty() == EW_DIRTY_COMPLETE )
					{
//					if(reverse? !node->Prev(): !node->Next())
					if(!SkipNext(node,reverse))
						{
						corner= x+sx-1;
						root->RectFill(x+w,truey,corner,truey+sy-1,WDS_PARTITION_FILL1);
						}
					else
						corner= x+w-1;

					root->RectFill(x,truey,corner,y-1,WDS_PARTITION_FILL2);
					}
				}
			}
		else
			{
			/* vertical partition */

			if(nextval>0)
				{
				GetRootWindow()->RectFill(truex,nextval,truex+sx-1,nextval+width[2]-1,
																			GetRootWindow()->GetColor(EW_COLOR_BACKGROUND));

				BevelBox(TRUE,TRUE,NULL,truex,nextval+(width[2]-width[draw_as])/2,sx,width[draw_as]);

				if(mobile && knob>=0)
					{
					SetWidgetState(mobile);
					BevelBox(TRUE,TRUE,NULL,truex+sx-1-knob-width[mobile],nextval+(width[2]-width[mobile])/2,
																								width[mobile],width[mobile]);
					}
				}

			if(node==NULL)
				nextval= -1;
			else
				{
				node->GetTrueGeometry(&x,&y,&w,&h);

				nextval=y-width[2];

				// clear area around dirty widget in case it doesn't fill partition
//				if( node->GetDirty() == EW_DIRTY_COMPLETE )
					{
//					if(reverse? !node->Prev(): !node->Next())
					if(!SkipNext(node,reverse))
						root->RectFill(truex,truey,truex+sx-1,y-1,WDS_PARTITION_FILL3);

					root->RectFill(x+w,y,x+sx-1,y+h-1,WDS_PARTITION_FILL4);

#if WDS_PARTITION_FILL_DEBUG
					long n;
					for(n=w;n<sx;n+=5)
						root->RectFill(x+n,y,x+n,y+h-1,m);
#endif
					}
				}
			}

#if WDS_COLOR_WIRE && WDS_PARTITION_WIRE

		root->PushClipRegion();
//		root->IntersectClipRegion(x,y,EW_MIN(w,sx),EW_MAX(h,sy));

		root->RectWire(x+w/3,y,x+w*2/3,y+h-1,EW_RED);
		root->RectWire(x,y+h/3,x+w-1,y+h*2/3,EW_GREEN);

		root->PopClipRegion();
#endif

#if WDS_PARTITION_RESIZE_DEBUG
			printf("Partition Draw %d,%d %d,%d -> %d,%d %d,%d \"%s\"->\"%s\"\n",
					truex,truey,sx,sy,
					x,y,w,h,
					this->GetNameString()->Buffer(),node? node->GetNameString()->Buffer(): "null");
#endif
		m++;
		}

#if WDS_DUMP
	if(GetParentWidget()==NULL)
		root->Dump(TRUE);
#endif
	}



/******************************************************************************
void WDS_Partition::RegisterMouse(long on)

	exclusive grab of leftmouse movement
	non-exclusive watch for leftmouse release

	use deepest scope

Keywords:
******************************************************************************/
void WDS_Partition::RegisterMouse(long on)
	{
	long index= (direction==WDS_VERTICAL);

#if WDS_PARTITION_REGISTER_DEBUG
	printf("0x%x register partition on=%d (%s)\n",(long)this,on,GetName());
#endif

	if(registered==on)
		{
		if(registered)
			{
#if !EW_RELEASE_GRADE
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Partition::RegisterMouse(TRUE) already registered");
#endif
			}

#if WDS_PARTITION_EVENT_DEBUG
		else
			// occurs under normal conditions
			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Partition::RegisterMouse(FALSE) not already registered");
#endif

		return;
		}

	EW_Event event;

	event.SetSIS(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,EW_STATE_RELEASE);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_NULL,FALSE);

	event.SetSIS(EW_EVENT_MOUSEPOSITION,index? EW_ITEM_Y: EW_ITEM_X,EW_STATE_ANY);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_NULL,FALSE);

	registered=on;

#if FALSE
	if(!EW_RELEASE_GRADE)
		GetRootWindow()->GetEventContext()->SetCursorIndex(on? EW_CURSOR_FOURWAY: EW_CURSOR_NORMAL);
#endif
	}



/******************************************************************************
long	WDS_Partition::Operate(EW_Event *event)

******************************************************************************/
long WDS_Partition::Operate(EW_Event *event)
	{
	long index= (direction==WDS_VERTICAL);
	long relocate=FALSE;

	realmouse= -1;

	if(event->IsFocusChange())
		drag=NULL;

	if(event->IsMouse() && exclusive)
		{
		event->SetUsed( event->GetUsed() | (index? EW_USED_GRABY: EW_USED_GRABX) );
		relocate=TRUE;
		}

	long result=DoOperate(event,FALSE);

	if(relocate)
		{
		// tell children the event happened at last mouse position in grain dimension
		if(!event->GetWidgetFlags()&WDS_BYPASS_EVENT)
			{
			// change counter-grain mouse position to last
			long mouse[2];

			event->GetMousePosition(&mouse[0],&mouse[1]);
			realmouse=mouse[index];
			mouse[index]=last;

			event->SetMousePosition(mouse[0],mouse[1]);
			}
		}

	return result;
	}



/******************************************************************************
long	WDS_Partition::PostOperate(EW_Event *event)

******************************************************************************/
long WDS_Partition::PostOperate(EW_Event *event)
	{
	long index= (direction==WDS_VERTICAL);

	// restore original mouse position
	if(realmouse> -1)
		{
		long mouse[2];

		event->GetMousePosition(&mouse[0],&mouse[1]);
		mouse[index]=realmouse;
		event->SetMousePosition(mouse[0],mouse[1]);
		}

	return DoOperate(event,TRUE);
	}



/******************************************************************************
long	WDS_Partition::DoOperate(EW_Event *event,long post)

	called from Operate() as DoOperate(event,FALSE)
	called from PostOperate() as DoOperate(event,TRUE)

Keywords:
******************************************************************************/
long WDS_Partition::DoOperate(EW_Event *event,long post)
	{
	EW_Widget *node;
	EW_Resize resize;	// dummy resize
	EW_RectangleList lastgeo;	// last geometry of each child

	long width[3];
	long pos[2],size[2];	// widget's geometry
	long mouse[2];			// mouse position
	long min[2],max[2];		// min/max widget size
	long middle=FALSE;

	long index= (direction==WDS_VERTICAL);
	long limited=( (GetFlags()&WDS_PARTITION_FLAGS_LIMIT)!=0 );
	long grab_width=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PARTITION_GRAB_WIDTH);
	long grab_length=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PARTITION_GRAB_LENGTH);
	long grab_extra;

	// store last geometries for each child before any resizing
	PreSizeCheck(&lastgeo);

	event->GetMousePosition(&mouse[0],&mouse[1]);

	GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	if(index)
		grab_extra= (grab_length<0 || mouse[!index]-pos[!index]<grab_length)? grab_width: 0;
	else
		grab_extra= (grab_length<0 || mouse[!index]-(pos[!index]+size[!index])> -grab_length)? grab_width: 0;

	GetSeparatorWidth(width);

	// ignore all non-mouse events
	if( !(event->IsMouse() || event->IsFocusChange()) )
		return FALSE;

#if WDS_PARTITION_EVENT_DEBUG
	char message[256];
	sprintf(message,"WDS_Partition::DoOperate() event:  (%s)",GetNameString()->Buffer());
	EW_PRINT(EW_WIDGET,EW_LOG,message);

	event->Print((long)this);
	printf("post=%d index=%d\n",post,index);
#endif

	if(event->IsFocusChange())
		{
#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"focus change%s%s%s",
									ewColorCode(EW_CODE_YELLOW),drag? " -> DRAG LOST": "",ewColorCode(EW_CODE_NORMAL));
#endif

		drag=NULL;
		exclusive=FALSE;

		last=mouse[index];
		RegisterMouse(FALSE);

		return FALSE;
		}

	if(event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS)
		{
#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"focus ignored");
#endif

		return FALSE;
		}

	if(event->IsMouseRelease())
		{
#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"release%s%s%s",
									ewColorCode(EW_CODE_YELLOW),drag? " -> DRAG LOST": "",ewColorCode(EW_CODE_NORMAL));
#endif

		drag=NULL;
		exclusive=FALSE;
		RegisterMouse(FALSE);
		return FALSE;
		}

	if( event->IsUsed(index? EW_USED_DRAGY: EW_USED_DRAGX) || (!drag && event->IsUsed(index? EW_USED_Y: EW_USED_X)) )
		{
#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"event used%s%s%s",
									ewColorCode(EW_CODE_YELLOW),drag? " -> DRAG LOST": "",ewColorCode(EW_CODE_NORMAL));
#endif

#if FALSE
		drag=NULL;
		exclusive=FALSE;

		RegisterMouse(FALSE);
#endif

		last=mouse[index];

		return FALSE;
		}

	long is_bypass= ((event->GetWidgetFlags()&WDS_BYPASS_FLAGS)==WDS_BYPASS_EVENT);

//is_bypass=FALSE;

	if( (event->IsLeftMousePress() || (!drag && event->IsLeftMouseMotion()) ||
				(middle=(event->IsMiddleMousePress() || (!drag && event->IsMiddleMouseMotion()))) ) )
		{
		// determine immobile partitions
		long fix[2];
		GetFixed(&fix[0],&fix[1]);

		long nodenumber,nodes=GetWidgetList()->GetNumberNodes();
		long immobile;

		long attach= ( (GetFlags() & WDS_PARTITION_FLAGS_ATTACH_BIT) != FALSE );

#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"hit");
#endif

//		if(!drag)
			{
			/* check if left mouse press is on a partition bar */
			long nextval= -1;

			last_bar=FALSE;
			on_bar=middle;

			if(reverse)
				{
				GetWidgetList()->ToTail();
				nodenumber=nodes-1;
				}
			else
				{
				GetWidgetList()->ToHead();
				nodenumber=0;
				}

			while( (node=PostIncDecrement(!reverse)) != NULL  || (!limited && nextval>0) )
				{
				immobile= (nodenumber<fix[0] || (attach && (nodenumber>(nodes-fix[1]-1) || nodes-fix[0]-fix[1]<2)) );

				// bar drag
				if(!immobile && nextval>0 && mouse[index]>=nextval-grab_extra && mouse[index]<nextval+width[2]+grab_extra)
					{
					if(node)
						drag= (WDS_Widget *)SkipNext(node,!reverse);
					else
						{
						// one previous
						PostIncDecrement(reverse);
						drag=GetWidgetList()->Current();

						last_bar=TRUE;
						}

					on_bar=TRUE;
					break;
					}

				if(node==NULL)
					break;
					

				node->GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);

				long polarity= ( (index && GetInversion()) != GetReverse() );

//				long above= ( (polarity? node->Prev(): node->Next()) != NULL  || !limited);
//				long below= ( (polarity? node->Next(): node->Prev()) != NULL );
				long above= ( SkipNext(node,polarity) != NULL  || !limited);
				long below= ( SkipNext(node,!polarity) != NULL );

				// background drag
				if( !immobile && post && (!below || mouse[index]>=pos[index]) &&
																(!above || mouse[index]<pos[index]+size[index]-grab_extra) )
					{
//					drag= (WDS_Widget *)( reverse? node->Next(): node->Prev());
					drag= (WDS_Widget *)( SkipNext(node,!reverse) );

					if(!drag)
						{
						// off bar inside first partition

						// if limited, like dragging first partition bar
						// if unlimited, like dragging second region
						drag=node;
						on_bar=limited;
						}
					break;
					}

				if(index)
					nextval=pos[index]-width[2];
				else
					nextval=pos[index]+size[index];

				nodenumber+= reverse? -1: 1;
				}
			}

		if(drag)
			{
#if WDS_PARTITION_EVENT_DEBUG
			EW_PRINT(EW_WIDGET,EW_LOG,"new drag nodenumber %d between %d,%d of %d",nodenumber,fix[0],nodes-fix[1]-1,nodes);
#endif

			exclusive=event->IsLeftMousePress();
			last=mouse[index];
			event->SetUsed( event->GetUsed() | (index? EW_USED_GRABY: EW_USED_GRABX) );

#if WDS_PARTITION_DRAG_DEBUG
			printf("drag %d %s bar 0x%x on (%s)->(%s) (%s)\n",	index,on_bar? "ON ":"OFF",(long)this,GetName(),
																drag? drag->GetName(): "NULL",
																node? node->GetName(): "NULL");
#endif

			if(on_bar)
				RegisterMouse(TRUE);
			}
		}
	else if( (post || is_bypass) && drag && event->IsMouseMotion() )
		{
#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"continuing drag");
#endif

		/* dragging a partition bar */

		long diff=mouse[index]-last;

		if(index)
			diff= -diff;

#if WDS_PARTITION_DRAG_DEBUG
		printf("dragging %d %s bar 0x%x by %d on (%s)->(%s)\n",
				index,on_bar? "ON ":"OFF",(long)this,diff,GetName(),drag->GetName());
#endif

		if(diff||is_bypass)
			{
			long opos[2],osize[2];		// old geometry

			node= (WDS_Widget *)( SkipNext(drag,!reverse) );
			long leftmost= (node==NULL);

			/* widget(s) to the left/top */
			long backslide=diff;
			node=drag;
			while(node && backslide)
				{
				node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
				opos[0]=pos[0];
				opos[1]=pos[1];
				osize[0]=size[0];
				osize[1]=size[1];

				node->GetMinSize(&min[0],&min[1]);
				node->GetMaxSize(&max[0],&max[1]);

				size[index]+= backslide;
				if(size[index]<min[index])
					{
					backslide=size[index]-min[index];
					pos[index]+=backslide;
					size[index]-= backslide;
					}
				else if(max[index]>0 && size[index]>max[index])
					{
					backslide=size[index]-max[index];
					pos[index]+=backslide;
					size[index]-= backslide;
					}
				else
					backslide=0;

				// left side limit (press negative OR rip off edge)
				if(pos[index]<0 || (pos[index]>0 && leftmost) )
					{
					backslide-=pos[index];
					diff=backslide;
					pos[index]=0;
					}

				if( opos[0]!=pos[0] || opos[1]!=pos[1] || osize[0]!=size[0] || osize[1]!=size[1])
					{
					node->SetGeometry(pos[0],pos[1],size[0],size[1]);

#if !WDS_PARTITION_DIRTY_POSTCHECK
					node->SetDirty(EW_DIRTY_COMPLETE);
					SetDirty(EW_DIRTY_PARTIAL);
#endif

					((WDS_Widget *)node)->SuggestPrefSize(size[0],size[1]);
					node->CheckSize(&resize);

#if WDS_PARTITION_DRAG_DEBUG
        			printf("moved behind 0x%x %d,%d %d,%d -> %d,%d %d,%d (%s)\n",node,
							opos[0],opos[1],osize[0],osize[1],
							pos[0],pos[1],size[0],size[1],
							node->GetNameString()->Buffer());
#endif

					}

//				node= (WDS_Widget *)( reverse? node->Next(): node->Prev());
				node= (WDS_Widget *)( SkipNext(node,!reverse) );
				}

			/* widget to the right/bottom */
			long pass=0;
			node=drag;
//			while( (pass++)<2 && (node=(WDS_Widget *)(reverse? node->Prev(): node->Next())) )
			while( diff!=0 && (pass++)<2 && (node=(WDS_Widget *)SkipNext(node,reverse)) )
				{
				node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
				opos[0]=pos[0];
				opos[1]=pos[1];
				osize[0]=size[0];
				osize[1]=size[1];

				pos[index]+= diff;

#if WDS_PARTITION_DRAG_DEBUG
				printf("moved ahead 0x%x %d,%d %d,%d -> %d,%d %d,%d (%s)\n",node,
						opos[0],opos[1],osize[0],osize[1],
						pos[0],pos[1],size[0],size[1],
						node->GetNameString()->Buffer());
#endif

				node->SetGeometry(pos[0],pos[1],size[0],size[1]);

#if !WDS_PARTITION_DIRTY_POSTCHECK
				node->SetDirty(EW_DIRTY_COMPLETE);
				SetDirty(EW_DIRTY_PARTIAL);
#endif

				((WDS_Widget *)node)->SuggestPrefSize(size[0],size[1]);
				node->CheckSize(&resize);

				// if on bar, then pass out (Heh)
				if(on_bar)
					pass=2;

				// if size not limited, don't pass out
				if(!limited)
					pass=0;
				}

			last=mouse[index];

			// resize the widgets
#if WDS_PARTITION_MAX_ON_WINDOW
			long w,h;
			GetRootWindow()->GetSize(&w,&h);

			resize.Reset();
			resize.SetMaxSize(w,h);
#endif

			Resize(&resize);

#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"dragged");
#endif

			// mark partition as containing immediate dirty children
			SetDirty(EW_DIRTY_PARTIAL);
			event->SetUsed( event->GetUsed() | (index? EW_USED_DRAGY: EW_USED_DRAGX) );
			}
		}
	else if(post)
		{
#if WDS_PARTITION_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"no hit%s%s%s",
									ewColorCode(EW_CODE_YELLOW),drag? " -> DRAG LOST": "",ewColorCode(EW_CODE_NORMAL));
#endif

		drag=NULL;
		exclusive=FALSE;

		RegisterMouse(FALSE);
		}

	// compare each child versus previous geometry
	PostSizeCheck(&lastgeo);

	return FALSE;
	}



/******************************************************************************
void	WDS_Partition::GetFixed(long *headmost,long *tailmost)

******************************************************************************/
void WDS_Partition::GetFixed(long *headmost,long *tailmost)
	{
	long index= (direction==WDS_VERTICAL);
	EW_Widget *node;
	long pref[2],min[2],max[2];
	long fix[2];

	fix[0]=0;
	fix[1]=0;

	long m,n;
	for(n=0;n<2;n++)
		{
		m=1;

		if( n && !(GetFlags()&WDS_PARTITION_FLAGS_ATTACH_BIT) )
			break;

		if(n)
			GetWidgetList()->ToTail();
		else
			GetWidgetList()->ToHead();

		while( (node=PostIncDecrement(!n)) != NULL )
			{
			node->GetPrefSize(&pref[0],&pref[1]);
			node->GetMinSize(&min[0],&min[1]);
			node->GetMaxSize(&max[0],&max[1]);

#if WDS_PARTITION_PREFS_ARE_FIXED
			if( pref[index]>=0 || (min[index]>=0 && min[index]==max[index]) )
#else
			if( min[index]>=0 && min[index]==max[index] )
#endif
				fix[n]=m;
			else
				break;

			m++;
			}
		}

	*headmost=fix[0];
	*tailmost=fix[1];
	}



/******************************************************************************
long	WDS_Partition::Resize(EW_Resize *resize)

	This gets a little deep, mainly because of the support for fixed widgets.

Keywords:
******************************************************************************/
long WDS_Partition::Resize(EW_Resize *resize)
	{
#if WDS_PARTITION_RESIZE_DEBUG
    printf("WDS_Partition::Resize() (%s)\n",GetNameString()->Buffer());
#endif

	EW_Widget *node,*other;
	EW_Resize new_resize;
	EW_RectangleList lastgeo;	// last geometry of each child

	long width[3];
	long min[2],max[2];			// child widget's minimum/maximum
	long pref[2];
	long minmost[2];			// maximum of child widgets' minimums
	long maxmost[2];			// maximum of child widgets' maximums
	long pos[2],size[2];		// widget's geometry
	long opos[2],osize[2];		// old geometry (check for change)
	long apos[2],asize[2];		// adjacent widget's geometry
	long fullsize[2];			// partition widget size (parent)
	long m;
	long totalpref=0;			// total of min/pref sizes

	long index= (direction==WDS_VERTICAL);

	// store last geometries for each child before any resizing
	PreSizeCheck(&lastgeo);

	long resized=CheckSize(resize);

    long truex,truey,sx,sy;

	GetSeparatorWidth(width);

    GetTrueGeometry(&truex,&truey,&sx,&sy);

	// pass along any info
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	fullsize[0]=sx;
	fullsize[1]=sy;

	long limited=( (GetFlags()&WDS_PARTITION_FLAGS_LIMIT)!=0 );

	GetMinSize(&minmost[0],&minmost[1]);
	GetMaxSize(&maxmost[0],&maxmost[1]);

/*
	minmost[0]=0;
	minmost[1]=0;
	maxmost[index]= -1;
	maxmost[!index]=0;
*/
	minmost[index]=0;
	minmost[!index]= -1;
	maxmost[index]= -1;
	maxmost[!index]= -1;

	long first=TRUE;
	long carry=0;

#if WDS_PARTITION_RESIZE_DEBUG
	printf("Partition Push Out (%s)\n",GetNameString()->Buffer());
#endif

	/* push out */
	if(reverse)
		GetWidgetList()->ToTail();
	else
		GetWidgetList()->ToHead();

	while( (node=PostIncDecrement(!reverse)) != NULL )
		{
		node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

		opos[0]=pos[0];
		opos[1]=pos[1];
		osize[0]=size[0];
		osize[1]=size[1];

		node->GetMaxSize(&max[0],&max[1]);
		node->GetPrefSize(&pref[0],&pref[1]);
		node->GetMinSize(&min[0],&min[1]);
		if(minmost[!index]<min[!index])
			minmost[!index]=min[!index];
		if(max[!index]<0)
			maxmost[!index]= -1;
		else if(maxmost[!index]>=0 && maxmost[!index]<max[!index])
			maxmost[!index]=max[!index];
		minmost[index]+= min[index] + (!first)*width[2];

		if(first)
			pos[index]= 0;
		else
			pos[index]=carry;

		pos[!index]=0;

//		other=(WDS_Widget *)( reverse? node->Prev(): node->Next() );
		other=(WDS_Widget *)SkipNext(node,reverse);

		if(other)
			{
			other->GetGeometry(&apos[0],&apos[1],&asize[0],&asize[1]);
			size[index]=apos[index]-pos[index]-width[2];
			}
		else if(limited)
			size[index]=fullsize[index]-pos[index];
		else
			size[index]= -1;

		long prefgrain=0;

		if(prefgrain<min[index])
			prefgrain=min[index];
		if(prefgrain<pref[index])
			prefgrain=pref[index];

		totalpref+=prefgrain;

		size[!index]= index? sx: sy;
		if(size[!index]>pref[!index] && pref[!index]>0)
			size[!index]=pref[!index];

		for(m=0;m<2;m++)
			{
			// on first pass, all positions will be 0 and size[]'s will result in 0 or less -> so, use -1
			if(size[m]<=0)
				size[m]= -1;

			if(size[m]>max[m] && max[m]>0)
				size[m]=max[m];
			}

		node->SetOrigin(pos[0],pos[1]);

		if(direction==WDS_HORIZONTAL)
			{
			/* horizontal partition */

			new_resize.SetPrefSize(size[0],sy);

#if WDS_PARTITION_PREF_HACK
			new_resize.SetPrefSize(-1,sy);
#endif
			}
		else
			{
			/* vertical partition */

			new_resize.SetPrefSize(sx,size[1]);

#if WDS_PARTITION_PREF_HACK
			new_resize.SetPrefSize(sx,-1);
#endif
			}

#if WDS_PARTITION_RESIZE_DEBUG
		printf("Partition PreResize (%s)->(%s)\n",GetNameString()->Buffer(),node->GetNameString()->Buffer());
#endif

#if WDS_PARTITION_RESIZE_MODE == 0
		if( node->CheckSize(&new_resize) )
			resized=TRUE;
#else
		if( node->PreResize(&new_resize) )
			resized=TRUE;

//		node->DumpInfo(0);
#endif

		node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

		// if geometry changed
		if( pos[0]-opos[0] || pos[1]-opos[1] ||  size[0]-osize[0] ||  size[1]-osize[1] )
			{
#if WDS_PARTITION_RESIZE_DEBUG
			printf("Partition Press Out Geometry Change %d,%d %d,%d to %d,%d %d,%d \"%s\"->\"%s\"\n",
					opos[0],opos[1],osize[0],osize[1],
					pos[0],pos[1],size[0],size[1],
					this->GetNameString()->Buffer(),node->GetNameString()->Buffer());
#endif

#if !WDS_PARTITION_DIRTY_POSTCHECK
			node->SetDirty(EW_DIRTY_COMPLETE);
			SetDirty(EW_DIRTY_PARTIAL);
#endif
			}
#if WDS_PARTITION_RESIZE_DEBUG
		else
			printf("Partition Press Out No Change %d,%d %d,%d \"%s\"->\"%s\"\n",
					opos[0],opos[1],osize[0],osize[1],GetName(),node->GetName());
#endif

		carry=pos[index]+size[index]+width[2];

		first=FALSE;
		}

	// find minimum cross-grain dimension based on childrens' minimums

	// restrain to resize's max
	resize->GetMaxSize(&max[0],&max[1]);

	if(max[!index]>=0 && minmost[!index]>max[!index])
		minmost[!index]=max[!index];
	if(max[!index]>=0 && maxmost[!index]>max[!index])
		maxmost[!index]=max[!index];

	// if specifically bounded, don't override
	if(GetBounds(!index))
		{
		minmost[!index]= -1;
		maxmost[!index]= -1;
		}

	SetMinSize(minmost[0],minmost[1]);
	SetMaxSize(maxmost[0],maxmost[1]);


#if WDS_PARTITION_REPRESS


	// repress back
	long limit;

	if(reverse)
		GetWidgetList()->ToHead();
	else
		GetWidgetList()->ToTail();

	if(limited && totalpref>fullsize[index])
		{
#if WDS_PARTITION_RESIZE_DEBUG
		printf("Partition Repression avoided, not enough space (%s)\n",GetNameString()->Buffer());
#endif
		}
	else if(limited)
		{
#if WDS_PARTITION_RESIZE_DEBUG
		printf("Partition Repress (%s)\n",GetNameString()->Buffer());
#endif

		while( (node=PostIncDecrement(reverse)) != NULL )
			{
			node->GetPrefSize(&pref[0],&pref[1]);
			node->GetMinSize(&min[0],&min[1]);
			node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
			opos[0]=pos[0];
			opos[1]=pos[1];
			osize[0]=size[0];
			osize[1]=size[1];

			long attach= ( (GetFlags() & WDS_PARTITION_FLAGS_ATTACH_BIT) != FALSE );

//			other=(WDS_Widget *)( reverse? node->Prev(): node->Next() );
			other=(WDS_Widget *)SkipNext(node,reverse);
			if(other)
				{
				other->GetGeometry(&apos[0],&apos[1],&asize[0],&asize[1]);
				limit=apos[index]-width[2];
				}
			else
				{
				limit=fullsize[index];
				}

			// if over limit, reduce size
			if(size[index]>limit-pos[index] || ((attach||other) && size[index]<limit-pos[index]) )
				size[index]=limit-pos[index];

			if(pref[index]<min[index])
				pref[index]=min[index];

			// if too small, push back
			if(size[index]<pref[index])
				{
				size[index]=pref[index];
				pos[index]=limit-size[index];
				}

			// if geometry changed
			if( (!other && attach && pos[index]!=limit-size[index]) ||
						pos[0]-opos[0] || pos[1]-opos[1] || size[0]-osize[0] || size[1]-osize[1] )
				{
#if WDS_PARTITION_RESIZE_DEBUG
				printf("Partition Repress Geometry Change %d,%d %d,%d to %d,%d %d,%d \"%s\"->\"%s\"\n",
						opos[0],opos[1],osize[0],osize[1],
						pos[0],pos[1],size[0],size[1],
						this->GetNameString()->Buffer(),node->GetNameString()->Buffer());
#endif


				if(direction==WDS_HORIZONTAL)
					{
					/* horizontal partition */

					new_resize.SetPrefSize(size[0],sy);

#if WDS_PARTITION_PREF_HACK
//					new_resize.SetPrefSize(-1,sy);
#endif
					}
				else
					{
					/* vertical partition */

					new_resize.SetPrefSize(sx,size[1]);

#if WDS_PARTITION_PREF_HACK
//					new_resize.SetPrefSize(sx,-1);
#endif
					}

#if WDS_PARTITION_RESIZE_MODE == 0
				if( node->CheckSize(&new_resize) )
					resized=TRUE;
#elif WDS_PARTITION_RESIZE_MODE == 1
				if( node->CheckSize(&new_resize) )
					{
					node->PreResize(&new_resize);
					resized=TRUE;

#if WDS_PARTITION_RESIZE_DEBUG
					printf("Partition Resize 2 \"%s\"->\"%s\"\n",this->GetNameString()->Buffer(),node->GetNameString()->Buffer());
#endif
					}
#else
				if( node->PreResize(&new_resize) )
					resized=TRUE;
#endif

				// reset position (apos[] used as dummy ponters)
				node->GetGeometry(&apos[0],&apos[1],&size[0],&size[1]);
				node->SetGeometry(pos[0],pos[1],size[0],size[1]);

				// attach last widget to far end
				node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
				if(attach && pos[index]!=limit-size[index])
					{
#if WDS_PARTITION_RESIZE_DEBUG
					printf("Partition Reposition \"%s\"->\"%s\"\n",this->GetNameString()->Buffer(),node->GetNameString()->Buffer());
#endif

					pos[index]=limit-size[index];
					node->SetGeometry(pos[0],pos[1],size[0],size[1]);
					node->CalcTruePosition();

					// The above should only be a position change, so theoretically
					// CheckSize() and Resize() shouldn't ever return TRUE (TRUE = a size change).

#if WDS_PARTITION_RESIZE_MODE == 0
					if( node->CheckSize(&new_resize) )
						resized=TRUE;
#elif WDS_PARTITION_RESIZE_MODE == 1
					if( node->CheckSize(&new_resize) )
						{
						// should never happen

						node->PreResize(&new_resize);
						resized=TRUE;

#if WDS_PARTITION_RESIZE_DEBUG
						printf("Partition Resize 3 \"%s\"->\"%s\"\n",this->GetNameString()->Buffer(),node->GetNameString()->Buffer());
#endif
						}
#else
					if( node->PreResize(&new_resize) )
							resized=TRUE;
#endif
					}

#if WDS_PARTITION_RESIZE_MODE == 1
				node->CalcTruePosition(TRUE);
#endif

#if WDS_PARTITION_RESIZE_DEBUG
				printf("Partition Resize dirty node: \"%s\"->\"%s\"\n",this->GetName(),node->GetName());
#endif

#if !WDS_PARTITION_DIRTY_POSTCHECK
				node->SetDirty(EW_DIRTY_COMPLETE);
				SetDirty(EW_DIRTY_PARTIAL);
#endif
				}
			else
				{
#if WDS_PARTITION_RESIZE_DEBUG
				printf("Partition No Change %d,%d %d,%d \"%s\"->\"%s\"\n",
						pos[0],pos[1],size[0],size[1],this->GetName(),node->GetName());
#endif
//				break;
				}
			}

		// if repressed back too far (or not far enough), push forward so that first widget is aligned
		long first_node=TRUE;
		long correction;

		GetWidgetList()->ToHead();
		while( (node=PostIncDecrement(TRUE)) != NULL )
			{
			node->GetOrigin(&pos[0],&pos[1]);

			if(first_node)
				{
				correction= -pos[index];

				// break for efficiency only
				if(correction==0)
					break;

#if WDS_PARTITION_RESIZE_DEBUG
				printf("Partition %sCounter-Repression%s: \"%s\"->\"%s\"\n",
															ewColorCode(6),this->GetName(),node->GetName(),ewColorCode(0));
#endif

#if !WDS_PARTITION_DIRTY_POSTCHECK
				SetDirty(EW_DIRTY_COMPLETE);
#endif

				first_node=FALSE;
				}

			pos[index]+=correction;

			node->SetOrigin(pos[0],pos[1]);
			}
		}

#endif

	long all_fixed=TRUE;

	GetWidgetList()->ToHead();

	while( (node=PostIncDecrement(TRUE)) != NULL )
		{
		node->GetMinSize(&min[0],&min[1]);
		node->GetMaxSize(&max[0],&max[1]);

		if( min[index]<=0 || min[index]!=max[index] )
			{
			all_fixed=FALSE;
			break;
			}

		m++;
		}

	GetMinSize(&min[0],&min[1]);
	GetMaxSize(&max[0],&max[1]);

	max[index]= -1;

	// if size not limited or all children fixed along grain, size the partitiion to fit
	if(!limited || all_fixed)
		{
#if WDS_PARTITION_RESIZE_DEBUG
		printf("Partition Expand (%s)\n",GetNameString()->Buffer());
#endif

		if(reverse)
			node=GetWidgetList()->ToHead();
		else
			node=GetWidgetList()->ToTail();

		// skip invisibles
		node=PostIncDecrement(reverse);

		if(node)
			{
			node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

			osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

			if(!limited)
				{
				new_resize.GetPrefSize(&pref[0],&pref[1]);

				long slack=size[index];
				slack=EW_MIN(slack,200);
				slack=EW_MAX(slack,30);
slack=0;

				// 2*size to leave extra space for expanding
				pref[index]=pos[index]+size[index]+width[2]+slack;

				new_resize.SetPrefSize(pref[0],pref[1]);
				}

			if(all_fixed)
				{
				max[index]=min[index];

				SetMaxSize(max[0],max[1]);
				}

			if(CheckSize(&new_resize))
				resized=TRUE;

			GetSize(&sx,&sy);

			// if vertical and inverted, when don't fill vertically, slide to top
			if(index && GetInversion())
				{
				resize->GetPrefSize(&pref[0],&pref[1]);
				GetOrigin(&pos[0],&pos[1]);

				pos[index]=pref[index]-(index? sy: sx);

				if(pos[index]>=0)
					SetOrigin(pos[0],pos[1]);
				}
			}
		}

	SetMaxSize(max[0],max[1]);

#if WDS_PARTITION_RESIZE_MODE == 0
	// waited till end to do the recursive resizes

	widgetlist->ToHead();
	while( (node=(WDS_Widget *)(widgetlist->PostIncrement())) != NULL )
		{
		node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

		if(direction==WDS_HORIZONTAL)
			new_resize.SetPrefSize(size[0],sy);
		else
			new_resize.SetPrefSize(sx,size[1]);

		if( node->PreResize(&new_resize) )
			resized=TRUE;
		}
#endif

	// make sure positions are current (recursive)
	CalcTruePosition(TRUE);

	// compare each child versus previous geometry
	PostSizeCheck(&lastgeo);

#if WDS_PARTITION_RESIZE_DEBUG
		printf("Partition Done (%s)\n",GetNameString()->Buffer());
#endif

//	return resized;

	if(resized)
		{
//		printf("resized (%s)\n",GetNameString()->Buffer());

#if WDS_PARTITION_RESIZE_DEBUG
		printf("Partition resized=%d (%s)\n",resized,GetName());
#endif

#if !WDS_PARTITION_DIRTY_POSTCHECK
		SetDirty(EW_DIRTY_COMPLETE);
#endif
		}

	return FALSE;
	}



/******************************************************************************
void	WDS_Partition::PreSizeCheck(EW_RectangleList *rectlist)

******************************************************************************/
void WDS_Partition::PreSizeCheck(EW_RectangleList *rectlist)
	{
	EW_Widget *node;
	long pos[2],size[2];		// widget's geometry
	//long opos[2],osize[2];		// old geometry (check for change)

	if(reverse)
		GetWidgetList()->ToTail();
	else
		GetWidgetList()->ToHead();

	while( (node=PostIncDecrement(!reverse)) != NULL )
		{
		node->CalcTruePosition(FALSE);
		node->GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);
		rectlist->AddRectangle(pos[0],pos[1],size[0],size[1]);
		}
	}



/******************************************************************************
void	WDS_Partition::PostSizeCheck(EW_RectangleList *rectlist)

******************************************************************************/
void WDS_Partition::PostSizeCheck(EW_RectangleList *rectlist)
	{
	EW_Widget *node;
	long pos[2],size[2];		// widget's geometry
	long opos[2],osize[2];		// old geometry (check for change)

	EW_Rectangle *rect;
	long changed;

	rectlist->ToHead();

	if(reverse)
		GetWidgetList()->ToTail();
	else
		GetWidgetList()->ToHead();

	while( (node=PostIncDecrement(!reverse)) != NULL )
		{
		if( (rect=rectlist->PostIncrement()) == NULL)
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Partition::Resize() geometry comparator out of entries (%s)",GetName());
		else
			{
			node->GetTrueGeometry(&opos[0],&opos[1],&osize[0],&osize[1]);
			rect->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

			changed= ( pos[0]!=opos[0] || pos[1]!=opos[1] || size[0]!=osize[0] || size[1]!=osize[1] );

			if(changed)
				{
#if FALSE
				EW_PRINT(EW_WIDGET,EW_WARN,"%d,%d %d,%d -> %d,%d %d,%d (%s)  (%s) %s%s%s",
					opos[0],opos[1],osize[0],osize[1],pos[0],pos[1],size[0],size[1],
					GetName(),node->GetName(),ewColorCode(EW_CODE_RED),changed? " -> CHANGED": "",ewColorCode(EW_CODE_NORMAL));
#endif

#if WDS_PARTITION_DIRTY_POSTCHECK
				node->SetDirty(EW_DIRTY_COMPLETE);
				SetDirty(EW_DIRTY_PARTIAL);
#endif
				}
			}
		}
	}



// for reference, pointers to member function (gcc 2.6.3 couldn't handle it)
#if FALSE

	// EW_Widget for step casting
	EW_Widget *(EW_WidgetList::*step)(void);

	if(reverse)
		{
		widgetlist->ToTail();
		step= (EW_Widget *(EW_WidgetList::*)()) &(EW_WidgetList::PostDecrement);
		}
	else
		{
		widgetlist->ToHead();
		step= (EW_Widget *(EW_WidgetList::*)()) &(EW_WidgetList::PostIncrement);
		}

	while( (node=(widgetlist->*step)()) != NULL )
		{
		}

#endif
