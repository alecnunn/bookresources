/******************************************************************************

Coral Tree wds/scroll.cc
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


#define WDS_SCROLL_DEBUG				FALSE
#define WDS_DIRTY_DEBUG					FALSE
#define WDS_SCROLL_REDRAW_DEBUG			FALSE	// toggling red background
#define WDS_SCROLL_EVENT_DEBUG			FALSE
#define WDS_SCROLLBAR_EVENT_DEBUG		FALSE
#define WDS_SCROLL_LIMIT_DEBUG			FALSE
#define WDS_SCROLL_TIE_DEBUG			FALSE
#define WDS_SCROLL_MISMATCH_DEBUG		FALSE
#define WDS_SCROLLBAR_ALLOW_CROSS_DRAG	FALSE  // allow other widgets to use the cross-grain movements
#define WDS_DIRECT_SCROLL				FALSE
#define WDS_SCROLL_SMART_SLIDE			FALSE

#define WDS_SCROLL_TIMER_ID				8		// only a request
#define WDS_SCROLL_TIME1				400		// milliseconds
#define WDS_SCROLL_TIME2				70		// milliseconds




/******************************************************************************
		WDS_ScrollBarButton::WDS_ScrollBarButton(void)

******************************************************************************/
WDS_ScrollBarButton::WDS_ScrollBarButton(void)
	{
	SetName("Scroll Bar Button");

	SetBounds(EW_BOUNDS_NONE);
	}



/******************************************************************************
		WDS_ScrollBar::WDS_ScrollBar()

Keywords:
******************************************************************************/
WDS_ScrollBar::WDS_ScrollBar()
	{
	long m;

	SetName("Scroll Bar");

	// pre-initialize for Purify
	offset= 0.0;
	fraction= 0.0;

	registered=FALSE;
	drag=FALSE;
	tiechange=0;
	last_tie_value=0.0;
	last=0;

	for(m=0;m<2;m++)
		{
		bpos[m]=0;
		bsize[m]=1;
		cpos[m]=0;
		csize[m]=1;
		cipos[m]=0;
		cisize[m]=1;
		crpos[m]=0;
		crsize[m]=0;
		cirpos[m]=0;
		cirsize[m]=0;
		}

	SetDirection(WDS_SCROLLBAR_HORIZONTAL);
	SetOffset(0.0);
	SetFraction(1.0);
	SetStepSize(0.04);
	SetBounds(EW_BOUNDS_IGNORE_PARENT);
	SetTimerID(-1);

	WDS_Activation activation;
	activation.SetTargetWidget(this);

	for(m=0;m<2;m++)
		{
		activation.SetCode(m);
		button[m].SetStateChangeCallback(WDS_STATE_ALL,&WDS_ScrollBar::ActivationCallback,&activation);
		}

	button[0].SetName("SB Dec");
	button[1].SetName("SB Inc");

	// accept bypassed events
	SetEventMask(GetLocalEventMask());
    GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_PERIODIC);
	GetLocalEventMask()->SetWidgetFlags(WDS_BYPASS_FLAGS);
	}



/******************************************************************************
void	WDS_ScrollBar::ActivationCallback(WDS_Activation *activation)

Keywords:
******************************************************************************/
void WDS_ScrollBar::ActivationCallback(WDS_Activation *activation)
	{
	WDS_ScrollBar *scrollbar=(WDS_ScrollBar *)activation->GetTargetWidget();
	EW_Event *event=activation->GetEvent();

	long code=activation->GetCode();
	long state=activation->GetState();

#if WDS_SCROLLBAR_EVENT_DEBUG
	printf("scrollbar=0x%x state=%d code=%d offset=%.6G fraction=%.6G stepsize=%.6G\n",(long)scrollbar,state,code,
			scrollbar->GetOffset(),scrollbar->GetFraction(),scrollbar->GetStepSize());
#endif

	if(state!=1)
		{
		scrollbar->EndTimer();
		return;
		}

	scrollbar->AffectFromButton(code,event);

	scrollbar->StartTimer(WDS_SCROLL_TIME1);
	scrollbar->SetTimerFlags(code|WDS_SCROLL_TIMER_INITIAL);

#if WDS_SCROLLBAR_EVENT_DEBUG
	printf("  offset=%.6G fraction=%.6G stepsize=%.6G\n",
			scrollbar->GetOffset(),scrollbar->GetFraction(),scrollbar->GetStepSize());
#endif

//	printf("interpret=%d\n",scrollbar->button[0].InterpretElement(-1,WDS_ELEMENT_BORDER));
	}



/******************************************************************************
void	WDS_ScrollBar::StartTimer(long time)

******************************************************************************/
void WDS_ScrollBar::StartTimer(long time)
	{
	EndTimer();
	SetTimerID(GetRootWindow()->GetEventContext()->AppendTimerInterval(WDS_SCROLL_TIMER_ID,time,GetRootWindow()));

#if WDS_SCROLLBAR_EVENT_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::StartTimer(%d) id %d",time,GetTimerID());
#endif
	}



/******************************************************************************
void	WDS_ScrollBar::EndTimer(void)

******************************************************************************/
void WDS_ScrollBar::EndTimer(void)
	{
#if WDS_SCROLLBAR_EVENT_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::EndTimer() id %d",GetTimerID());
#endif

	if(GetTimerID())
		{
		GetRootWindow()->GetEventContext()->RemoveTimerInterval(GetTimerID());
		SetTimerID(-1);
		}
	}



/******************************************************************************
void	WDS_ScrollBar::AffectFromButton(long increment,EW_Event *event)

******************************************************************************/
void WDS_ScrollBar::AffectFromButton(long increment,EW_Event *event)
	{
	ChangeState(WDS_STATE_1,event);

	if(increment)
		Increment();
	else
		Decrement();

	ChangeState(WDS_STATE_0,event);
	}



/******************************************************************************
void	WDS_ScrollBar::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_ScrollBar::Draw(EW_Redraw *redraw)
	{
#if WDS_SCROLL_DEBUG
	printf("WDS_ScrollBar::Draw()\n");
#endif

	long pos[2],size[2];
	GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);

	long fields=fieldlist.GetNumberNodes();
	long active_state= (fields)? WDS_STATE_2: WDS_STATE_1;
	long inset=InterpretElement(active_state,WDS_ELEMENT_INSET);
	long piston_min=1+2*(inset+InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH));
	long index= (direction==WDS_SCROLLBAR_VERTICAL);
	long m;

	for(m=0;m<2;m++)
		{
		cpos[m]=pos[m]+crpos[m];
		csize[m]=size[m]+crsize[m];
		cipos[m]=pos[m]+cirpos[m];
		cisize[m]=size[m]+cirsize[m];
		}

	// piston dimensions; second pass to check for minimum pixel size
	// NOTE: piston will be automatically inset later
	float mod_offset=offset,mod_fraction=fraction;
	bpos[!index]=cpos[!index];
	bsize[!index]=csize[!index];
	for(m=0;m<2;m++)
		{
		bpos[index]=(long)(cpos[index]+mod_offset*(csize[index]+0.999));
		bsize[index]=(long)(mod_fraction*(csize[index]+0.999));

		if(!m)
			{
			mod_fraction=EW_MAX(mod_fraction,piston_min/(float)csize[index]);
			mod_fraction=EW_MIN(mod_fraction,1.0);
			mod_offset=EW_MIN(mod_offset,1.0-mod_fraction);
			}
		}

/*
	printf("%d,%d %d,%d (%d) -> %d,%d %d,%d -> %d,%d %d,%d\n",	pos[0],pos[1],size[0],size[1], inset,
																cpos[0],cpos[1],csize[0],csize[1],
																bpos[0],bpos[1],bsize[0],bsize[1]);
*/

	// draw cylinder shaft
	long dpos[2],dsize[2];

	dpos[!index]=cpos[!index];
	dsize[!index]=csize[!index];

	if(InterpretElement(index? WDS_STATE_1: WDS_STATE_0,WDS_ELEMENT_SCROLLBAR_CHILDREN) & WDS_SCROLLBAR_CHILDREN_INSIDE)
		{
		dpos[index]=pos[index];
		dsize[index]=size[index];
		}
	else
		{
		dpos[index]=cpos[index];
		dsize[index]=csize[index];
		}

	long stored_state=GetWidgetState();

	long partial= CanSmartDraw() && ((GetDirty()&EW_DIRTY_PARTIAL) != 0);

//	printf("partial=%d childchange=%d force=%d limited=%d\n",partial,GetDirty(),redraw->GetForce(),redraw->GetLimited());

	long pass,passes= partial? 3: 1;
	for(pass=0;pass<passes;pass++)
		{
		if(partial)
			{
			long clippos[2],clipsize[2];

			clippos[!index]=dpos[!index];
			clipsize[!index]=dsize[!index];

			switch(pass)
				{
				case 0:
					clippos[index]= 	cipos[index]+inset;
					clipsize[index]=	bpos[index]-clippos[index]+inset;

					// if fields, draw background (including fields)
					if(fields)
						clipsize[index]+=bsize[index];
					break;
				case 1:
					clippos[index]=		bpos[index]+bsize[index]-inset;
					clipsize[index]=	cipos[index]+cisize[index]-clippos[index]-inset;
					break;
				}

			if(pass<2)
				{
/*
				GetRootWindow()->RectFill(clippos[0],clippos[1],clippos[0]+clipsize[0]-1,clippos[1]+clipsize[1]-1,EW_RED+pass);
				GetRootWindow()->RectWire(clippos[0],clippos[1],clippos[0]+clipsize[0]-1,clippos[1]+clipsize[1]-1,EW_BLUE+pass);
*/

				GetRootWindow()->PushClipRegion();
				GetRootWindow()->IntersectClipRegion(clippos[0],clippos[1],clipsize[0],clipsize[1]);
				}

/*
printf("%s%d: %d,%d %d,%d -> %d,%d %d,%d -> %d,%d %d,%d%s\n",ewColorCode(pass+1),pass,
pos[0],pos[1],size[0],size[1],
bpos[0],bpos[1],bsize[0],bsize[1],
clippos[0],clippos[1],clipsize[0],clipsize[1],
ewColorCode(EW_CODE_NORMAL));
*/
			}

#if FALSE
if(direction==WDS_SCROLLBAR_HORIZONTAL)
	{
	GetRootWindow()->Flush(FALSE);
	osdMilliSleep(500);
	}
#endif

		SetWidgetState(FALSE);

		if(!partial || pass<2)
			BevelBox(TRUE,TRUE,NULL,dpos[0],dpos[1],dsize[0],dsize[1]);

		WDS_ScrollField *field;
		long fpos[2],fsize[2];
		long depth=InterpretElement(WDS_STATE_0,WDS_ELEMENT_DEPTH);

		if(fields)
			{
			fpos[!index]=cipos[!index]+depth;
			fsize[!index]=cisize[!index]-2*depth;
			}

		fieldlist.ToHead();
		while( (field=(WDS_ScrollField *)fieldlist.PostIncrement()) != NULL )
			{
			fsize[index]=(long)(EW_MAX(1,field->GetFieldSize()*(cisize[index]-2*depth)));
			fpos[index]=(long)(cipos[index]+field->GetFieldStart()*cisize[index]+depth);

			GetRootWindow()->RectFill(fpos[0],fpos[1],fpos[0]+fsize[0]-1,fpos[1]+fsize[1]-1,
																		InterpretElement(WDS_STATE_ALL,EW_COLOR_PEN));
			};

		// draw piston
		SetWidgetState(active_state);

		if(!partial || pass==2)
			{
			if(fraction<1.0)
				BevelBox(!fields,TRUE,NULL,bpos[0],bpos[1],bsize[0],bsize[1]);
			}

		if(partial && pass<2)
			{
			GetRootWindow()->PopClipRegion();
			}
		}

	SetWidgetState(stored_state);
	}



/******************************************************************************
long WDS_ScrollBar::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_ScrollBar::Operate(EW_Event *event)
	{
	long used=event->GetUsed();
	long index= (direction==WDS_SCROLLBAR_VERTICAL);

#if FALSE
	// return if on slide channel edge
	if( !event->InBounds(cipos[0],cipos[1],cisize[0],cisize[1]) )
		return FALSE;
#endif

	long mouse[2];

	event->GetMousePosition(&mouse[0],&mouse[1]);

	if(event->IsTimer())
		{
#if WDS_SCROLLBAR_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::Operate() received timer:");
		event->Print((long)this);
#endif

        if(event->GetState()==GetTimerID())
			{
			if(GetTimerFlags()&WDS_SCROLL_TIMER_INITIAL)
				{
				SetTimerFlags(GetTimerFlags()&WDS_SCROLL_TIMER_INCREMENT);
				StartTimer(WDS_SCROLL_TIME2);
				}

			AffectFromButton(GetTimerFlags(),event);
			event->SetUsed(EW_USED_ALL);
			}

		return FALSE;
		}

	// ignore all non-mouse events
	if( !event->IsMouse() )
		return FALSE;

	// ignore mouse focus events
	if(event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS)
		return FALSE;

	// ignore outside shaft unless a bypass
	if( !event->InBounds(cpos[0],cpos[1],csize[0],csize[1]) && !(event->GetWidgetFlags()&WDS_BYPASS_EVENT) )
		return FALSE;

#if WDS_SCROLLBAR_EVENT_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::Operate() event:");
	event->Print((long)this);
#endif

	if( event->IsLeftMousePress() )
		{
		ChangeState(WDS_STATE_1,event);

		// check if outside scrollbar
		if( !event->InBounds(bpos[0],bpos[1],bsize[0],bsize[1]) )
			{
#if WDS_SCROLLBAR_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::Operate() mouse off bar");
#endif

			last_offset=offset;
			SetOffset( (mouse[index]-cipos[index])/(float)cisize[index] - fraction/2.0 );
			SetDirty(EW_DIRTY_PARTIAL);

#if WDS_DIRTY_DEBUG
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollBar::Operate() SetDirty(EW_DIRTY_PARTIAL) 1");
#endif

			// will re-deactivate for every change
			ChangeState(WDS_STATE_0,event);
			}

		drag=TRUE;

#if WDS_SCROLLBAR_EVENT_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::Operate() mouse grab");
#endif

#if WDS_SCROLLBAR_ALLOW_CROSS_DRAG
		used|= index? EW_USED_GRABY: EW_USED_GRABX;
#else
		used|= EW_USED_ALL;
#endif
		event->SetUsed(used);

		RegisterMouse(TRUE);
		}
	else if( drag && event->Is(EW_EVENT_MOUSEPOSITION,EW_ITEM_ALL,EW_STATE_ANY) )
		{
		// ignore redundant non-bypass mouse motion
		if(event->GetWidgetFlags()&WDS_BYPASS_EVENT)
			{
			last_offset=offset;
			offset= last_offset + (mouse[index]-last)/(float)csize[index];
			CheckScrollLimits();

			SetDirty(EW_DIRTY_PARTIAL);

#if WDS_DIRTY_DEBUG
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollBar::Operate() SetDirty(EW_DIRTY_PARTIAL) 2");
#endif

			if(offset != last_offset)
				{
#if WDS_SCROLLBAR_EVENT_DEBUG
				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::Operate() mouse drag");
#endif

//				used|= index? EW_USED_DRAGY: EW_USED_DRAGX;

				// will re-deactivate for every change
				ChangeState(WDS_STATE_0,event);
				}

#if WDS_SCROLLBAR_ALLOW_CROSS_DRAG
			used|= index? EW_USED_DRAGY: EW_USED_DRAGX;
#else
			used|= EW_USED_ALL;
#endif
			event->SetUsed(used);
			}
		}
	else
		{
		RegisterMouse(FALSE);
		drag=FALSE;
		}
	
	last=mouse[index];

/* 	printf("scrollbar offset=%.6G fraction=%.6G elements=%d\n",offset,fraction); */

	return FALSE;
	}



/******************************************************************************
long	WDS_ScrollBar::PostOperate(EW_Event *event)

******************************************************************************/
long WDS_ScrollBar::PostOperate(EW_Event *event)
	{
	long used=event->GetUsed();
	long index= (direction==WDS_SCROLLBAR_VERTICAL);

//	if(InRegion(event,TRUE))
	if(event->InWidget(this))
		{
#if WDS_SCROLLBAR_ALLOW_CROSS_DRAG
		used|= index? EW_USED_Y: EW_USED_X;
#else
		used|= EW_USED_ALL;
#endif

		event->SetUsed(used);
		}

	return FALSE;
	}



/******************************************************************************
void WDS_ScrollBar::RegisterMouse(long on)

    exclusive grab of leftmouse movement
    non-exclusive watch for leftmouse release

    use deepest scope

Keywords:
******************************************************************************/
void WDS_ScrollBar::RegisterMouse(long on)
    {
    long index= (direction==WDS_VERTICAL);

#if WDS_SCROLLBAR_EVENT_DEBUG
    printf("0x%x register scrollbar index=%d on=%d\n",(long)this,index,on);
#endif

    if(registered==on)
        {
        if(registered)
            EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollBar::RegisterMouse(TRUE) already registered");
#if WDS_SCROLLBAR_EVENT_DEBUG
        else
            // occurs under normal conditions
            EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollBar::RegisterMouse(FALSE) not already registered");
#endif

        return;
        }

    EW_Event event;

    event.SetSIS(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,EW_STATE_RELEASE);
    RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_NULL,FALSE);

    event.SetSIS(EW_EVENT_MOUSEPOSITION,index? EW_ITEM_Y: EW_ITEM_X,EW_STATE_ANY);
    RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_NULL,FALSE);

    registered=on;
    }



/******************************************************************************
void	WDS_ScrollBar::CheckScrollLimits(void)

Keywords:
******************************************************************************/
void WDS_ScrollBar::CheckScrollLimits(void)
	{
	if(fraction<0.0)
		fraction=0.0;

	if(fraction>1.0)
		fraction=1.0;

#if WDS_SCROLL_LIMIT_DEBUG
	float old_fraction=fraction;
	float old_offset=offset;
#endif

	if(offset>1.0-fraction)
		offset=1.0-fraction;

	if(offset<0.0)
		offset=0.0;

#if WDS_SCROLL_LIMIT_DEBUG
	if(fabs(old_fraction-fraction)>0.01 || fabs(old_offset-offset)>0.01)
		printf("(%s) fraction=%.6G -> %.6G  offset=%.6G -> %.6G\n",GetNameString()->Buffer(),
																				old_fraction,fraction,old_offset,offset);
#endif

	if(!tiechange)
		UpdateTie();
	}



/******************************************************************************
long	WDS_ScrollBar::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_ScrollBar::Resize(EW_Resize *resize)
	{
//	printf("SB Resize (%s)\n",GetNameString()->Buffer());

	WDS_Widget *node;

	long inset=InterpretElement(WDS_STATE_0,WDS_ELEMENT_INSET);
	long index= (direction==WDS_SCROLLBAR_VERTICAL);
	long child_placement= InterpretElement(index? WDS_STATE_1: WDS_STATE_0,WDS_ELEMENT_SCROLLBAR_CHILDREN);
	long children=GetWidgetList()->GetNumberNodes();
	long m,n;
	long pos[2],size[2];

	// use width from looks
	long width[2];
	long min[2],max[2];
	GetMinSize(&min[0],&min[1]);
	GetMaxSize(&max[0],&max[1]);
	GetPrefSize(&width[0],&width[1]);

	width[!index]=InterpretElement(index? WDS_STATE_1: WDS_STATE_0,WDS_ELEMENT_SCROLLBAR_WIDTH);
	min[!index]=width[!index];
	max[!index]=width[!index];

	SetPrefSize(width[0],width[1]);
	SetMinSize(min[0],min[1]);
	SetMaxSize(max[0],max[1]);

	long resized=CheckSize(resize);

	long has_node;
	long should_have= ((child_placement&WDS_SCROLLBAR_CHILDREN_SLAVES) != 0);
	for(m=0;m<2;m++)
		{
		has_node=GetWidgetList()->SetCurrent(&button[m]);

		if(!has_node && should_have)
			AddWidget(&button[m]);
		else if(has_node && !should_have)
			RemoveWidget(&button[m]);
		}

	UpdateArrows();

	EW_Resize new_resize;
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	
	long child_inset=InterpretElement(WDS_STATE_3,WDS_ELEMENT_INSET);;
	long limit= (GetDirection()? size[0]: size[1]) - 2*child_inset;

	new_resize.SetMinSize(limit,limit);
	new_resize.SetPrefSize(limit,limit);
	new_resize.SetMaxSize(limit,limit);

	if(GetWidgetList()->Resize(&new_resize))
		resized=TRUE;

	long apos[2],asize[2];

	// cylinder shaft sub-geometry
	for(m=0;m<2;m++)
		{
//		cpos[m]=pos[m];
//		csize[m]=size[m];
		crpos[m]=0;
		crsize[m]=0;
		}

	// position/size children (potentially including slave buttons)
	long margin[2];

	margin[0]=0;
	margin[1]=size[index];

	if( (child_placement&WDS_SCROLLBAR_CHILDREN_INSIDE) )
		{
		margin[0]+=child_inset;
		margin[1]-=child_inset;
		}

	if( (child_placement&WDS_SCROLLBAR_CHILDREN_OPPOSE) )
		margin[1]-=limit*children;
	else if( (child_placement&WDS_SCROLLBAR_CHILDREN_ALTERNATE) )
		margin[1]-=limit*(children/2);

	m=0;
	GetWidgetList()->ToHead();
	while( (node=(WDS_Widget *)GetWidgetList()->PostIncrement()) != NULL )
		{
		node->GetGeometry(&apos[0],&apos[1],&asize[0],&asize[1]);

		apos[!index]=child_inset;

#if FALSE
		if(child_placement&WDS_SCROLLBAR_CHILDREN_ALTERNATE)
			{
			apos[index]= (m%2)? size[index]-limit*(children/2-m/2): limit*m/2;

			if(!(m%2))
				crpos[index]+=limit;
			}
		else
#endif
		if( (child_placement&WDS_SCROLLBAR_CHILDREN_OPPOSE) || ((m%2) && (child_placement&WDS_SCROLLBAR_CHILDREN_ALTERNATE)) )
			{
//			apos[index]=size[index]-limit*(children-m)-inset;
			apos[index]=margin[1];

			margin[1]+=limit;
			}
		else
			{
//			apos[index]=inset+limit*m;
//			cpos[index]+=limit;

			apos[index]=margin[0];
			crpos[index]+=limit;

			margin[0]+=limit;
			}

		node->SetGeometry(apos[0],apos[1],asize[0],asize[1]);

		crsize[index]-=limit;
		m++;
		}

	// inset cylinder shaft sub-geometry
	for(m=0;m<2;m++)
		{
		cirpos[m]=crpos[m]+inset;
		cirsize[m]=crsize[m]-2*inset;
		}

	return resized;
	}



/******************************************************************************
void WDS_ScrollBar::TieValuesChanged(WDS_TieValues *tievalues)

	pseudo-callback called when a tie has changed

Keywords:
******************************************************************************/
void WDS_ScrollBar::TieValuesChanged(WDS_TieValues *tievalues)
	{
	WDS_TieValue *node;
	float value;

	// don't need to notify ourselves
	if(tiechange)
		{
#if WDS_SCROLL_TIE_DEBUG
		printf("0x%x TieValuesChanged() tiechange=%d\n",tiechange);
#endif

		return;
		}

#if WDS_SCROLL_TIE_DEBUG
	printf("0x%x TieValuesChanged() changing\n",(long)this);
#endif

	tiechange++;

	if( (node=tievalues->ToHead()) != NULL )
		{
		value=node->GetValue();

		last_tie_value=value;

		SetOffset(value*(1.0-fraction));
		}

	tiechange--;

#if WDS_SCROLL_TIE_DEBUG
	printf("0x%x TieValuesChanged() %.6G %.6G\n",(long)this,offset,fraction);
#endif
	}



/******************************************************************************
void	WDS_ScrollBar::UpdateTie(void)

	update tie when this widget changes it

Keywords:
******************************************************************************/
void WDS_ScrollBar::UpdateTie(void)
	{
	WDS_TieValues tievalues;
	WDS_TieValue tievalue;

	float value;

#if WDS_SCROLL_TIE_DEBUG
	printf("0x%x UpdateTie() %.6G %.6G 0x%x\n",(long)this,offset,fraction,GetTieList());
#endif

	if(!GetTieList())
		return;

	if(fraction<1.0)
		value=offset/(1.0-fraction);
	else
		{
		// if scrollbar not needed, don't contribute
		return;
		}

	tievalue.SetValue(value);

	tievalues.AppendNode(&tievalue);

	if(last_tie_value!=value)
		{
		last_tie_value=value;

		tiechange++;
		ChangeTieValues(&tievalues);
		tiechange--;

#if WDS_SCROLL_TIE_DEBUG
		printf("changed to %.6G/%.6G\n",value,last_tie_value);
#endif
		};

#if WDS_SCROLL_TIE_DEBUG
	printf("0x%x UpdateTie() done\n",(long)this);
#endif
	}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



/******************************************************************************
		WDS_ScrollRegion::WDS_ScrollRegion()

Keywords:
******************************************************************************/
WDS_ScrollRegion::WDS_ScrollRegion()
	{
	long m;
	for(m=0;m<2;m++)
		{
//		scrollbar[m].SetParentWidget(this);
		scrollbar[m].SetDirection(m? WDS_SCROLLBAR_VERTICAL: WDS_SCROLLBAR_HORIZONTAL);
		scrollbar[m].SetTranslatable(FALSE);
		scrollbar[m].SetName((char *)(m? "SR SB V": "SR SB H"));
		scrollbar[m].SetInvisible(TRUE);
		scrollbar[m].SetClippable(FALSE);

		// don't want division by zero before properly set up
		extent[m]=(long)(1.0);
		}

	for(m=0;m<4;m++)
		last[m]=0;

	ConfirmScrollBars();

	grab=FALSE;

	debug_color=0;

	SetName("Scroll Region");
//	SetBounds(EW_BOUNDS_FILL_PARENT);
	SetInversion(TRUE);
	SetClipping(EW_CLIPPING_CHILDREN);

#if WDS_SCROLL_SMART_SLIDE
	SetSmartClear(TRUE);
#endif

	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS);

/*
	SetScrollable(WDS_HORIZONTAL,WDS_SCROLLABLE_VISIBLE);
	SetScrollable(WDS_VERTICAL,WDS_SCROLLABLE_VISIBLE|WDS_SCROLLABLE_OPPOSE);
*/
	}



/******************************************************************************
void	WDS_ScrollRegion::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_ScrollRegion::Draw(EW_Redraw *redraw)
	{
	long truex,truey,sx,sy;
	long lastx,lasty,lastsx,lastsy;
	long visx,visy,vissx,vissy;
	long ovisx,ovisy,ovissx,ovissy;
	long m;

	GetTrueGeometry(&truex,&truey,&sx,&sy);
	GetLastTrueGeometry(&lastx,&lasty,&lastsx,&lastsy);
	GetVisibility(&visx,&visy,&vissx,&vissy);
	GetLastVisibility(&ovisx,&ovisy,&ovissx,&ovissy);

#if WDS_SCROLL_DEBUG
	printf("WDS_ScrollRegion::Draw()\n");
#endif

#if FALSE
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ScrollRegion::Draw() geo %d,%d %d,%d -> %d,%d %d,%d vis %d,%d %d,%d -> %d,%d %d,%d (%s)",
				lastx,lasty,lastsx,lastsy,
				truex,truey,sx,sy,
				ovisx,ovisy,ovissx,ovissy,
				visx,visy,vissx,vissy,
				GetName());
#endif

	// have to recalc scroll bars manually
	AuxResize(NULL);

#if WDS_SCROLL_REDRAW_DEBUG
	GetRootWindow()->RectFill(truex,truey,truex+sx,truey+sy,(debug_color++)%2? EW_RED: EW_BLUE);
#endif

	SetWidgetState(FALSE);
	BevelBox(WDS_SCROLL_REDRAW_DEBUG,TRUE,NULL);

	SetWidgetState(TRUE);
	BevelBox(WDS_SCROLL_REDRAW_DEBUG,TRUE,NULL,truex+region_offset[0],truey+region_offset[1],
											sx-region_offset[0]-region_offset[2],sy-region_offset[1]-region_offset[3]);

	long visible[2],hidden;

	// scrollbars are drawn directly
	for(m=0;m<2;m++)
		{
		visible[m]= ((scrolling[m]&WDS_SCROLLABLE_VISIBLE) == WDS_SCROLLABLE_VISIBLE);
		hidden= !(visible[m] && scrollbar[m].GetFraction()<1.0);

		scrollbar[m].SetInvisible(hidden);

		if(visible[m] && hidden && (GetDirty()&EW_DIRTY_COMPLETE))
			{
			long bg=InterpretElement(WDS_STATE_0,EW_COLOR_BACKGROUND);

			scrollbar[m].GetTrueGeometry(&truex,&truey,&sx,&sy);
			GetRootWindow()->RectFill(truex,truey,truex+sx-1,truey+sy-1,bg);
			}

/*
		if((scrolling[m]&WDS_SCROLLABLE_VISIBLE) == WDS_SCROLLABLE_VISIBLE && scrollbar[m].GetFraction()<1.0 )
			{
			scrollbar[m].SetInvisible(FALSE);

#if WDS_DIRTY_DEBUG
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::Draw() scrollbar[%d].SetDirty(EW_DIRTY_COMPLETE), Redraw()",m);
#endif

#if !WDS_SMART_REDRAW
			scrollbar[m].SetDirty(EW_DIRTY_COMPLETE);
			scrollbar[m].Redraw(redraw);

			// region will control the drawing
			scrollbar[m].SetInvisible(TRUE);
#endif
			}

#if !WDS_SMART_REDRAW
		// SetDirty() absolute to bypass redraw-degradation-prevention
		scrollbar[m].EW_Node::SetDirty(EW_DIRTY_NOT);
#endif

#if WDS_DIRTY_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::Draw() scrollbar[%d].SetDirty(EW_DIRTY_NOT)",m);
#endif
*/
		}

	// set clipping for all normal descendants
	SetClipRegion(spos[0],spos[1],ssize[0],ssize[1]);
	}



/******************************************************************************
void	WDS_ScrollRegion::PostDraw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_ScrollRegion::PostDraw(EW_Redraw *redraw)
	{
	// sliding echo box
	if(grab==2)
		{
		GetRootWindow()->RectWire(spos[0],spos[1],spos[0]+msize[0]-1,spos[1]+msize[1]-1,EW_CYAN);

		long vx=(long)(spos[0]+msize[0]*scrollbar[0].GetOffset());
		long vy=(long)(spos[1]+msize[1]*scrollbar[1].GetOffset());
		long vw=(long)(msize[0]*scrollbar[0].GetFraction());
		long vh=(long)(msize[1]*scrollbar[1].GetFraction());

		GetRootWindow()->RectWire(vx,vy,vx+vw-1,vy+vh-1,EW_CYAN);
		}
	}



/******************************************************************************
void	WDS_ScrollRegion::SetGrab(long set)

******************************************************************************/
void WDS_ScrollRegion::SetGrab(long set)
	{
	grab=set;
	}



/******************************************************************************
long	WDS_ScrollRegion::GetOverheadHeight(void)

******************************************************************************/
long WDS_ScrollRegion::GetOverheadHeight(void)
	{
	return InterpretElement(WDS_STATE_0,WDS_ELEMENT_INSET)+
			InterpretElement(WDS_STATE_0,WDS_ELEMENT_DEPTH)+
			InterpretElement(WDS_STATE_1,WDS_ELEMENT_INSET)+
			InterpretElement(WDS_STATE_1,WDS_ELEMENT_DEPTH);
	}



/******************************************************************************
long WDS_ScrollRegion::Operate(EW_Event *event)

******************************************************************************/
long WDS_ScrollRegion::Operate(EW_Event *event)
	{
	long do_resize=FALSE;

	// make unviewable scrollbars inoperable
	long m;
	for(m=0;m<2;m++)
		scrollbar[m].SetDormant( !(scrolling[m]&WDS_SCROLLABLE_VIEWABLE) );

	if(event->IsFocusChange())
		SetGrab(0);

#if WDS_DIRECT_SCROLL
	// ignore all non-mouse events
	if( !event->IsMouse() )
		return FALSE;

	long m;
	for(m=0;m<2;m++)
		if( (scrolling[m]&WDS_SCROLLABLE_VIEWABLE) && scrollbar[m].InRegion(event,TRUE) )
			{
			scrollbar[m].Descend(event);

			// nullify event for further use
//			event->SetSource(EW_EVENT_NONE);
			break;
			}

	if( scrollbar[0].GetDirty() || scrollbar[1].GetDirty() )
		{
#if !WDS_SMART_REDRAW
		SetDirty(EW_DIRTY_COMPLETE);
#endif

		do_resize=TRUE;

#if WDS_DIRTY_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::Operate() SetDirty(EW_DIRTY_COMPLETE)");
#endif
		}
#endif

/*
	if( (event->IsKey(EW_KEY_ALL) || event->IsMouseButton() ) && !event->InBounds(spos[0],spos[1],ssize[0],ssize[1]) )
		event->SetUsed(EW_USED_ALL);
*/

	return do_resize;
	}



/******************************************************************************
long WDS_ScrollRegion::PostOperate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_ScrollRegion::PostOperate(EW_Event *event)
	{
	long do_resize=FALSE;
	long mouse[2];
	long m;
	long scrollable[2];

	scrollable[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);
	scrollable[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);

	event->GetMousePosition(&mouse[0],&mouse[1]);

	// ignore all non-mouse events
	if( !event->IsMouse() )
		return FALSE;

	if( !IsGrabbed() && !event->InBounds(spos[0],spos[1],ssize[0],ssize[1]) )
		{
		return FALSE;
		}

	// fake grab on used left-mouse to kill tip
	if(event->IsLeftMousePress())
		{
		SetGrab(3);
		}

	if( !IsGrabbed() && event->GetUsed() )
		{
		return FALSE;
		}

#if WDS_SCROLL_EVENT_DEBUG
	printf("Event in ScrollRegion (%s) grab=%d\n",GetNameString()->Buffer(),grab);
	event->Print((long)this);
#endif

	// background grab and drag
	if( event->IsLeftMousePress() || (!IsGrabbed() && event->IsLeftMouseMotion()) )
		{
		SetGrab(1);

		event->SetUsed(EW_USED_ALL);
		}
	else if( event->IsMiddleMousePress() || (!IsGrabbed() && event->IsMiddleMouseMotion()) )
		{
		SetGrab(2);

		if( ssize[0]/(float)ssize[1] > extent[0]/(float)extent[1] )
			{
			msize[1]=ssize[1];
			msize[0]=(long)(msize[1] * extent[0]/(float)extent[1]);
			}
		else
			{
			msize[0]=ssize[0];
			msize[1]=(long)(msize[0] * extent[1]/(float)extent[0]);
			}

		event->SetUsed(EW_USED_ALL);

		SetDirty(EW_DIRTY_COMPLETE);

#if WDS_DIRTY_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::PostOperate() SetDirty(EW_DIRTY_COMPLETE) 1");
#endif
		}
	else if( IsGrabbed() && event->IsMouseMotion() )
		{
		long used=event->GetUsed();

		if(grab==2)
			{
			for(m=0;m<2;m++)
				if(scrollable[m]&WDS_SCROLLABLE_ON)
					{
					scrollbar[m].SetOffset( (mouse[m]-spos[m])/(float)msize[m] - scrollbar[m].GetFraction()/2.0 );

					used|= m? EW_USED_DRAGY: EW_USED_DRAGX;
					}
			}
		else
			{
			float new_offset,old_offset,offset;

			// notice here that offset may be over-set unchecked
			// the idea is that a ancestor may use the motion, but we'd like the scroll to check against the ancestor's
			//	new size, not the the current size here
			// the CheckScrollLimits() should occur before anyone else reads the scroll bar

			for(m=0;m<2;m++)
				if(scrollable[m]&WDS_SCROLLABLE_ON)
					{
					old_offset=scrollbar[m].GetOffset();
					new_offset= old_offset - (mouse[m]-last[m])/(float)extent[m];
					scrollbar[m].SetOffset(new_offset);
					offset=scrollbar[m].GetOffset();

					if(offset != old_offset)
						{
						used|= m? EW_USED_DRAGY: EW_USED_DRAGX;
						}

					scrollbar[m].SetOffsetPending(new_offset);
					}
			}

		if(grab==2 || !CanSmartDraw())
			SetDirty(EW_DIRTY_COMPLETE);

		event->SetUsed(used);
		do_resize=TRUE;

#if WDS_DIRTY_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::PostOperate() SetDirty(EW_DIRTY_COMPLETE) 2");
#endif
		}
	else
		{
		if(grab==2)
			{
			SetDirty(EW_DIRTY_COMPLETE);

#if WDS_DIRTY_DEBUG
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::PostOperate() SetDirty(EW_DIRTY_COMPLETE) 3");
#endif
			}

		SetGrab(0);
		}

	if(IsGrabbed())
		{
		last[0]=mouse[0];
		last[1]=mouse[1];
		}

	return do_resize;
	}



/******************************************************************************
void	WDS_ScrollRegion::ConfirmScrollBars(void)

	confirm that scrollbars are first two children
	correct if neccessary

Keywords:
******************************************************************************/
void WDS_ScrollRegion::ConfirmScrollBars(void)
	{
	WDS_Widget *node;
	long mismatch=FALSE;

	node=(WDS_Widget *)GetWidgetList()->ToHead();
	if(node!=&scrollbar[0])
		{
		mismatch=TRUE;

#if WDS_SCROLL_MISMATCH_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::ConfirmScrollBars() mismatched horizontal");
#endif
		}
	
	node=(WDS_Widget *)GetWidgetList()->PreIncrement();
	if(node!=&scrollbar[1])
		{
		mismatch=TRUE;

#if WDS_SCROLL_MISMATCH_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::ConfirmScrollBars() mismatched vertical");
#endif
		}

	if(mismatch)
		{
		// remove (if exist) and force back on the beginning

		if(GetWidgetList()->SetCurrent(&scrollbar[0]))
			RemoveWidget(&scrollbar[0]);
		if(GetWidgetList()->SetCurrent(&scrollbar[1]))
			RemoveWidget(&scrollbar[1]);

#if WDS_SCROLL_MISMATCH_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::ConfirmScrollBars() re-instating");
#endif

		AddWidget(&scrollbar[1],EW_ADD_BEGINNING);
		AddWidget(&scrollbar[0],EW_ADD_BEGINNING);
		}
	}



/******************************************************************************
long	WDS_ScrollRegion::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_ScrollRegion::Resize(EW_Resize *resize)
	{
	long childchange=FALSE;
	long width[2];
	long excess[2];	
	long inset[2];		// inset element
	long depth[2];		// depth element
	long pref[2];
	long indepth[2];
	long scrollable[2];

	long m;

	inset[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_INSET);
	inset[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_INSET);
	depth[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_DEPTH);
	depth[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_DEPTH);

	indepth[0]=inset[0]+depth[0];
	indepth[1]=inset[1]+depth[1];

	scrollable[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);
	scrollable[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);

	AdjustSizeByFlags();

	for(m=0;m<2;m++)
		{
		excess[m]=2*indepth[0]+2*indepth[1];

		width[m]=scrollbar[m].InterpretElement(m? WDS_STATE_1: WDS_STATE_0,WDS_ELEMENT_SCROLLBAR_WIDTH);

		if(scrollable[m]&WDS_SCROLLABLE_ON)
			excess[m]+=width[m];
		}

	// min size of scroll region itself
	long setmin[2];
	for(m=0;m<2;m++)
		setmin[m]= -1;
	for(m=0;m<2;m++)
		if( (scrollable[m]&WDS_SCROLLABLE_VIEWABLE) || (scrollable[m]&WDS_SCROLLABLE_PERSISTANT) )
			{
			setmin[m]=EW_MAX(setmin[m],3*width[m]);
			setmin[!m]=EW_MAX(setmin[!m],2*width[m]);
			}
	SetMinSize(setmin[0],setmin[1]);

	long resized=CheckSize(resize);

#if WDS_SCROLL_DEBUG
	printf("WDS_ScrollRegion::Resize()\n");
#endif

	ConfirmScrollBars();

	// resize foster children (is this one necessary?)
	AuxResize(resize);

	// pass along any info
	EW_Resize new_resize;
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	long x,y,size[2];
	GetGeometry(&x,&y,&size[0],&size[1]);

	for(m=0;m<2;m++)
		{
		if((scrollable[m]&WDS_SCROLLABLE_ON) || (scrollable[m]&WDS_SCROLLABLE_AUTOSCALE))
			pref[m]= -1;
		else
			pref[m]=size[m]-excess[!m];
		}
	new_resize.SetPrefSize(pref[0],pref[1]);

	new_resize.SetMaxSize(-1,-1);

//	WDS_Widget *widget;
//	GetWidgetList()->ToHead();
//	while((widget=(WDS_Widget *)GetWidgetList()->PostIncrement())!=NULL)
//		widget->SuggestPrefSize(pref[0],pref[1]);

	// resize children
	if(GetWidgetList()->Resize(&new_resize))
		resized=TRUE;

	////////////////////////////////////////

	WDS_Widget	*next;

	long transx,transy;		// translation
	long rmin[2],rmax[2];	// resize limits
	long min[2],max[2];		// child geometry limits
	long old;				// old value

	// find extents of children
	GetWidgetList()->ToHead();
	GetWidgetList()->PostIncrement();
	GetWidgetList()->PostIncrement();
	GetChildrensExtents(&min[0],&min[1],&max[0],&max[1]);

	for(m=0;m<2;m++)
		{
		old=extent[m];
		extent[m]=max[m]-min[m];

		if(old!=extent[m])
			childchange=TRUE;
		}

	GetMinSize(&rmin[0],&rmin[1]);
	GetMaxSize(&rmax[0],&rmax[1]);
	for(m=0;m<2;m++)
		if( (scrollable[m]&WDS_SCROLLABLE_AUTOSCALE) )
			{
			rmax[m]=extent[m]+region_offset[m]+region_offset[m+2]+2*indepth[1];
			rmin[m]=rmax[m];
			}
	SetMinSize(rmin[0],rmin[1]);
	SetMaxSize(rmax[0],rmax[1]);

/*
printf("0x%x %d,%d %d,%d  %d,%d %d,%d  %d,%d\n",(long)this,rmin[0],rmin[1],rmax[0],rmax[1],
														min[0],min[1],max[0],max[1],extent[0],extent[1]);
*/

	// resize scrollregion
	CheckSize(resize);

	scrollbar[0].CheckScrollLimits();
	scrollbar[1].CheckScrollLimits();

	// resize foster children (adjust scrollbars for new extents before calculating translation)
	AuxResize(resize);

	transx= (long)(region_offset[0]+indepth[1]-min[0]-scrollbar[0].GetOffset()*extent[0]);
	if( GetInversion() )
		transy= (long)(region_offset[3]+indepth[1]-min[1]-(1.0-scrollbar[1].GetFraction()-scrollbar[1].GetOffset())*extent[1]);
	else
		transy= (long)(region_offset[1]+indepth[1]-min[1]-scrollbar[1].GetOffset()*extent[1]);

/*
printf("min=%d,%d max=%d,%d extent=%d,%d translate=%d,%d ssize=%d,%d fraction=%.6G,%.6G offset=%.6G,%.6G\n",
		min[0],min[1],max[0],max[1],extent[0],extent[1],transx,transy,ssize[0],ssize[1],
		scrollbar[0].GetFraction(),scrollbar[1].GetFraction(),scrollbar[0].GetOffset(),scrollbar[1].GetOffset());
*/


	long oldtrans[2];
	GetTranslation(&oldtrans[0],&oldtrans[1]);

	long oldtrue[2];
	long newtrue[2];
	for(m=0;m<2;m++)
		{
		oldtrue[m]=lastgeo[m]+oldtrans[m];
		newtrue[m]=spos[m] + (m? transy: transx);
		}

//	long smart_redraw= (CanSmartDraw() && !(GetDirty()&EW_DIRTY_COMPLETE) && (transx!=oldtrans[0] || transy!=oldtrans[1]));
	long smart_redraw= (CanSmartDraw() && !childchange &&
#if WDS_SCROLL_SMART_SLIDE
								(newtrue[0]!=oldtrue[0] || newtrue[1]!=oldtrue[1]
								|| newtrue[0]+ssize[0]!=oldtrue[0]+lastgeo[2] || newtrue[1]+ssize[1]!=oldtrue[1]+lastgeo[3])
#else
								(transx!=oldtrans[0] || transy!=oldtrans[1])
#endif
								);

	if(smart_redraw)
		{
		long srcx,srcy,destx,desty,temp;

#if TRUE
		srcx=EW_MAX(0,oldtrans[0]-transx);
		srcy=EW_MAX(0,oldtrans[1]-transy);

		destx=EW_MAX(0,transx-oldtrans[0]);
		desty=EW_MAX(0,transy-oldtrans[1]);
#else
		srcx=EW_MAX(0,oldtrue[0]-newtrue[0]);
		srcy=EW_MAX(0,oldtrue[1]-newtrue[1]);

		destx=EW_MAX(0,newtrue[0]-oldtrue[0]);
		desty=EW_MAX(0,newtrue[1]-oldtrue[1]);
#endif

		// swap
		if(GetInversion())
			{
			temp=desty;
			desty=srcy;
			srcy=temp;

			srcy+=EW_MAX(0,lastgeo[3]-ssize[1]);
			desty+=EW_MAX(0,ssize[1]-lastgeo[3]);
			}

		srcx+=lastgeo[0];
		srcy+=lastgeo[1];

		destx+=spos[0];
		desty+=spos[1];

#if TRUE
		long lenx=EW_MIN(ssize[0],lastgeo[2])-abs(oldtrans[0]-transx);
		long leny=EW_MIN(ssize[1],lastgeo[3])-abs(oldtrans[1]-transy);
#else
		long lenx=EW_MIN(ssize[0],lastgeo[2])-abs(oldtrue[0]-newtrue[0]);
		long leny=EW_MIN(ssize[1],lastgeo[3])-abs(oldtrue[1]-newtrue[1]);
#endif

		LimitDimensionsToScreen(&srcx,&srcy,&destx,&desty,&lenx,&leny);

		// full replace (no copy)
		if(lenx<1 || leny<1)
			{
			lenx=0;
			leny=0;

			srcx=spos[0];
			srcy=spos[1];

			destx=srcx;
			desty=srcy;
			}

#if FALSE
		printf("ImageCopy(%d,%d, %d,%d, %d,%d) over %d,%d %d,%d from  %d,%d %d,%d (%s)\n",
				srcx,srcy,destx,desty,lenx,leny,
				spos[0],spos[1],ssize[0],ssize[1],
				lastgeo[0],lastgeo[1],lastgeo[2],lastgeo[3],
				GetName());
#endif
		GetRootWindow()->WinSet();

		if(lenx>0)
			GetRootWindow()->ImageCopy(srcx,srcy,destx,desty,lenx,leny);

		GetRootWindow()->PushClipRegion();
		EW_ClipRegion *clipregion=GetRootWindow()->GetClipStack()->GetCurrentRegion();

		clipregion->DeleteRectangles();

		clipregion->AddRectangle(spos[0],spos[1],ssize[0],desty-spos[1]);						// bottom
		clipregion->AddRectangle(spos[0],spos[1],destx-spos[0],ssize[1]);						// left
		clipregion->AddRectangle(spos[0],desty+leny,ssize[0],ssize[1]+spos[1]-(desty+leny));	// top
		clipregion->AddRectangle(destx+lenx,spos[1],spos[0]+ssize[0]-(destx+lenx),ssize[1]);	// right

		clipregion->Prune();

#if TRUE
		clipregion->Draw(GetRootWindow(),InterpretElement(WDS_STATE_0,EW_COLOR_BACKGROUND));
#else
		clipregion->Draw(GetRootWindow(),EW_GREEN);
#endif
		}

	SetTranslation(transx,transy);

	lastgeo[0]=spos[0];
	lastgeo[1]=spos[1];
	lastgeo[2]=ssize[0];
	lastgeo[3]=ssize[1];

	// adjust for translation

	WDS_Widget *node;

	GetWidgetList()->ToHead();
	while( (node=(WDS_Widget *)GetWidgetList()->PostIncrement()) != NULL )
		node->CalcTruePosition(TRUE);

	if(smart_redraw)
		{
		EW_Redraw redraw;

		redraw.SetMousePosition(-1,-1);
		redraw.SetForce(TRUE);
		redraw.SetLimited(TRUE);

		Redraw(&redraw);

		GetRootWindow()->PopClipRegion();
		}


//  Was this flag really needed?
#if WDS_SCROLL_SMART_SLIDE
	resized=FALSE;
	childchange=FALSE;
#endif

	
	if(resized || childchange)
		{
		if(!smart_redraw)
			{
#if WDS_DIRTY_DEBUG
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::Resize() resized=%d childchange=%d SetDirty(EW_DIRTY_COMPLETE)",
																		resized,childchange);
#endif

			SetDirty(EW_DIRTY_COMPLETE);
			}
		}

	return FALSE;
	}



/******************************************************************************
void	WDS_ScrollRegion::AuxResize(EW_Resize *resize)

	Resize the owned scrollbars

Keywords:
******************************************************************************/
void WDS_ScrollRegion::AuxResize(EW_Resize *resize)
	{
	char message[256];

	long width[2];
	long inverted=GetInversion();
	long m;
	long x,y,w,h;
	long inset[2];
	long depth[2];
	long indepth[2];
	long oppose[2];	// h,v placement: FALSE=left/bottom TRUE=right/top
	long scrollable[2];

	long truex,truey,sx,sy;

	depth[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_DEPTH);
	depth[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_DEPTH);
	inset[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_INSET);
	inset[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_INSET);
	indepth[0]=inset[0]+depth[0];
	indepth[1]=inset[1]+depth[1];

	scrollable[0]=InterpretElement(WDS_STATE_0,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);
	scrollable[1]=InterpretElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);

	oppose[0]= ( (scrollable[0]&WDS_SCROLLABLE_OPPOSE)	!= 0 );
	oppose[1]= ( (scrollable[1]&WDS_SCROLLABLE_OPPOSE)	!= 0 );

	EW_Resize new_resize;
	if(resize)
		osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	for(m=0;m<2;m++)
		width[m]=scrollbar[m].InterpretElement(m? WDS_STATE_1: WDS_STATE_0,WDS_ELEMENT_SCROLLBAR_WIDTH);

	GetTrueGeometry(&truex,&truey,&sx,&sy);

#if WDS_DIRECT_SCROLL
	scrollbar[0].SetRootWindow(GetRootWindow());
	scrollbar[1].SetRootWindow(GetRootWindow());
#endif

	// Scrollable Region

	long space[2];
	for(m=0;m<2;m++)
		space[m]=0;

	for(m=0;m<4;m++)
		region_offset[m]=indepth[0];

	// find out which scrollbars are neccesary
	scrolling[0]=FALSE;
	scrolling[1]=FALSE;

	long offsetted=FALSE;

	// horizontal, vertical, then check horizontal again
	for(m=0;m<3;m++)
		{
		long n=m%2;
		long size= (n? sy: sx) - 2*indepth[1] - region_offset[n]-region_offset[n+2] ;

		if( ( ((scrollable[n]&WDS_SCROLLABLE_ON) && extent[n]>size) && !scrolling[n] ) ||
					(scrollable[n]&WDS_SCROLLABLE_PERSISTANT) )
			{
			scrolling[n]=(scrollable[n]&WDS_SCROLLABLE_ON)? scrollable[n]: FALSE;
			
			if( (scrollable[n]&WDS_SCROLLABLE_VIEWABLE) || (scrollable[n]&WDS_SCROLLABLE_PERSISTANT) )
				{
				space[n]=width[n];

				if(!n)
					{
					if(!offsetted)
						{
						// horizontal (checked twice)
						if(oppose[0])
							region_offset[3]+=width[0];
						else
							region_offset[1]+=width[0];

						offsetted=TRUE;
						}
					}
				else
					{
					// vertical
					if(oppose[1])
						region_offset[2]+=width[1];
					else
						region_offset[0]+=width[1];
					}
				}
			}
		}

	spos[0]=truex+region_offset[0]+indepth[1];
	spos[1]=truey+region_offset[1]+indepth[1];
	ssize[0]=sx-region_offset[0]-region_offset[2]-2*indepth[1];
	ssize[1]=sy-region_offset[1]-region_offset[3]-2*indepth[1];

	// Horizontal Scroll Bar

	x=indepth[0] + (oppose[1]? 0: space[1]);
	w=sx-space[1]-2*indepth[0];
	y=(oppose[0]!=inverted)? (sy-indepth[0]-width[0]): indepth[0];
	h=width[0];

	new_resize.SetMinSize(w,h);
	new_resize.SetPrefSize(w,h);
	new_resize.SetMaxSize(w,h);

	scrollbar[0].SetGeometry(x,y,w,h);
	scrollbar[0].Resize(&new_resize);

	// Vertical Scroll Bar

	y=indepth[0] + (oppose[0]!=inverted? 0: space[0]);
	h=sy-space[0]-2*indepth[0];
	x=(oppose[1])? (sx-indepth[0]-width[1]): indepth[0];
	w=width[1];

	new_resize.SetMinSize(w,h);
	new_resize.SetPrefSize(w,h);
	new_resize.SetMaxSize(w,h);

	scrollbar[1].SetGeometry(x,y,w,h);
	scrollbar[1].Resize(&new_resize);

	scrollbar[0].CalcTruePosition();
	scrollbar[1].CalcTruePosition();

	for(m=0;m<2;m++)
		{
		if(extent[m]>0)
			scrollbar[m].SetFraction(	ssize[m] / (float)extent[m] );
		else if(GetWidgetList()->GetNumberNodes()==0)
			scrollbar[m].SetFraction(1.0);
		else if(GetWidgetList()->GetNumberNodes())
			{
			sprintf(message,"WDS_ScrollRegion::AuxResize() extent[%d] nonpositive (%d) 0x%x",m,extent[m],(long)this);
//			EW_PRINT(EW_WIDGET,EW_LOG,message);
			}
		}

	if(scrollbar[0].GetDirty() || scrollbar[1].GetDirty())
		{
		if(!CanSmartDraw())
			{
			SetDirty(EW_DIRTY_COMPLETE);

#if WDS_DIRTY_DEBUG
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_ScrollRegion::AuxResize() SetDirty(EW_DIRTY_COMPLETE) (%s)",GetName());
#endif
			}
		}
	}
