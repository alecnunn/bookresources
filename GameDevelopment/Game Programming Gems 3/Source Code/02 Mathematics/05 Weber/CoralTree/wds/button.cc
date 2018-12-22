/******************************************************************************

Coral Tree wds/button.cc
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


#define	WDS_BUTTON_DEBUG			FALSE

#define	WDS_BUTTON_READINESS		TRUE
#define	WDS_BUTTON_READINESS_DEBUG	FALSE


// cvs test


/*****************************************************************************
		WDS_Button::WDS_Button()

	initialize button upon construction

Keywords:
******************************************************************************/
WDS_Button::WDS_Button()
	{
	DB("WDS_Button::WDS_Button()");

	SetName("Button");

	displayfunction = NULL;
	displayarg = NULL;
//	callback = NULL;
	readied=FALSE;

	SetBounds(EW_BOUNDS_IGNORE_PARENT);
	SetAnnotatable(TRUE);
	SetFlags(WDS_BUTTON_FLAGS_DEFAULT);

	// accept bypassed events
	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS);
	GetLocalEventMask()->SetWidgetFlags(WDS_BYPASS_FLAGS);

#if FALSE
	SetGeometry(0,0,100,100);
	SetMinSize(-1,-1);
	SetMaxSize(100,100);
#endif
	}



/******************************************************************************
long	WDS_Button::Operate(EW_Event *event)

	react to event sent to button

Keywords:
******************************************************************************/
long WDS_Button::Operate(EW_Event *event)
	{
// #if WDS_BUTTON_DEBUG
#if FALSE
	if(event->InWidget(this) && event->IsIdleMouse())
		{
		EW_PRINT(EW_WIDGET,EW_LOG,"");
		EW_PRINT(EW_WIDGET,EW_LOG,"********************************");
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Button::GetEventMask()  >>>>");
		GetEventMask()->Print((long)this);

		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Button::Operate() event >>>>");
		event->Print((long)this);
		EW_PRINT(EW_WIDGET,EW_LOG,"");

//		printf("%d\n",event->Intersects(GetEventMask()));
		}
#endif

    long sense=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_BUTTON_SENSE_ANNOTATION);

	if( readied && event->IsIdleMouse() && !InRegion(event,TRUE) )
		{
#if WDS_BUTTON_READINESS_DEBUG
		printf("unready (%s) (%s)\n",GetNameString()->Buffer(),GetLabel());
#endif

		readied=FALSE;

#if WDS_BUTTON_READINESS
			SetDirty(EW_DIRTY_COMPLETE);
#endif
		}

//	CheckForTip(event);

	// ignore events outside of button unless focus-change or from a bypass
	if( !event->IsFocusChange() && ((sense && !event->InWidget(this)) || (!sense && !InRegion(event,TRUE))) &&
																			!(event->GetWidgetFlags()&WDS_BYPASS_EVENT) )
		return FALSE;

	if(event->GetUsed())
		return FALSE;

	long ready_effect= (InterpretElement(GetWidgetState()+4,WDS_ELEMENT_BORDER)!=WDS_BEVELBOX_BORDERLESS);

	if(event->IsIdleMouse())
		{
		if(ready_effect && !readied && InRegion(event,TRUE))
			{
#if WDS_BUTTON_READINESS_DEBUG
			printf("ready (%s) (%s) ready_effect=%d state=%d ready-border=%d\n",GetNameString()->Buffer(),GetLabel(),
									ready_effect,GetWidgetState(),InterpretElement(GetWidgetState()+4,WDS_ELEMENT_BORDER));
#endif

			readied=TRUE;

#if WDS_BUTTON_READINESS
			SetDirty(EW_DIRTY_COMPLETE);
#endif
			}

		return FALSE;
		}

	if(event->IsDragged())
		return 0;

	long toggleable= (GetFlags()&WDS_BUTTON_FLAGS_TOGGLE);

	if( (event->Is(EW_EVENT_WIDGET,WDS_ITEM_EXCLUDE_BUTTON,EW_STATE_ANY) && (event->GetWidgetFlags()&WDS_BYPASS_TAKE_LOST) ) ||
		(!toggleable && GetWidgetState() && ((event->IsLeftMouseRelease() && !(event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS) ) ||
																								event->IsFocusChange()) ) )
		{
		ChangeState(WDS_STATE_0,event);

		SetDirty(EW_DIRTY_COMPLETE);	// mark this widget as needing to be redrawn
		event->SetUsed(TRUE);			// mark this event as consumed
		RegisterRelease(FALSE);			// notification of left mouse release no longer needed
		}
	else if(event->IsLeftMousePress())
		{
#if FALSE
		if(event->GetRepetition())
			printf("WDS_Button Double-Click %d\n",event->GetRepetition());
#endif

		if(toggleable)
			{
			if(GetWidgetState() && !(GetFlags()&WDS_BUTTON_FLAGS_UNTAKE_ONLY) )
				ChangeState(WDS_STATE_0,event);
			else
				ChangeState(WDS_STATE_1,event);
			}
		else if(!GetWidgetState())
			{
			ChangeState(WDS_STATE_1,event);
			RegisterRelease(TRUE);			// notify scope that this widget will need to see ANY left mouse release
			}

		SetDirty(EW_DIRTY_COMPLETE);	// mark this widget as needing to be redrawn
		event->SetUsed(TRUE);			// mark this event as consumed
		}

	return 0;
	}



/******************************************************************************
void	WDS_Button::ChangeState(long newstate,EW_Event *event)

******************************************************************************/
void WDS_Button::ChangeState(long newstate,EW_Event *event)
	{
	if(GetFlags()&WDS_BUTTON_FLAGS_EXCLUSIVE)
		RegisterExclusivity(newstate!=WDS_STATE_0);

	WDS_Widget::ChangeState(newstate,event);
	}



/******************************************************************************
void	WDS_Button::RegisterExclusivity(long on)

	exclusive access on shallowest scope

******************************************************************************/
void WDS_Button::RegisterExclusivity(long on)
	{
	EW_Event event;

	event.SetSIS(EW_EVENT_WIDGET,WDS_ITEM_EXCLUDE_BUTTON,EW_STATE_ANY);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_TAKE,TRUE);
	}



/******************************************************************************
void WDS_Button::RegisterRelease(long on)

	register to watch for leftmouse release in deepest scope

	This makes sure that the button will become un-pressed even if mouse
	release occurs outside of the button.

	Unless the first child of the window is a scope,
	this functionality can be partially defeated since there may be areas
	in the window that the found scope cannot see.

Keywords:
******************************************************************************/
void WDS_Button::RegisterRelease(long on)
	{
#if WDS_BUTTON_DEBUG
	printf("button register on=%d\n",on);
#endif

	EW_Event event;

	event.SetSIS(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,EW_STATE_RELEASE);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_NULL,FALSE);
	}



/******************************************************************************
void	WDS_Button::DrawOnTablet(EW_Redraw *redraw)

	draw the button

Keywords:
******************************************************************************/
void WDS_Button::DrawOnTablet(EW_Redraw *redraw)
	{
	if(redraw->GetTablet())
		{
		// Buttons always draw.
//		if (redraw->GetTablet()->GetFlags()&EW_TABLET_RAW)
		if (TRUE)
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
			redraw->GetTablet()->Text(x+2,(y+sy)-(GetRootWindow()->GetFontHeight()+2),GetLabel());
#else
			long x,y,sx,sy;
			// If parent clipping...
			if (GetClipping()&EW_CLIPPING_ALL)
				{
				// Set the bounds of the current clipping region.
				GetRootWindow()->GetClipBounds(&x,&y,&sx,&sy);
				if(x!=0 || y!=0 || sx!=0 || sy!=0)
					redraw->GetTablet()->SetClipBounds(x,y,sx,sy);
				}

			// Get the geometry.
			GetTrueGeometry(&x,&y,&sx,&sy);

			// Draw a filled rectangle.
			redraw->GetTablet()->RectFill(x,y,sx,sy);

			// Add the name.
			// TODO: Replace the arbitrary positioning.
			redraw->GetTablet()->Text(x+2,(y+sy)-(GetRootWindow()->GetFontHeight()+2),
										GetLabel());

			// If child clipping...
			if (GetClipping()&EW_CLIPPING_CHILDREN)
				{
				// Set the bounds of the current clipping region.
				GetRootWindow()->GetClipBounds(&x,&y,&sx,&sy);
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
void	WDS_Button::Draw(EW_Redraw *redraw)

	draw the button

Keywords:
******************************************************************************/
void WDS_Button::Draw(EW_Redraw *redraw)
	{
	DB("WDS_Button::Draw()");

	long truex,truey,sx,sy;

	GetTrueGeometry(&truex,&truey,&sx,&sy);

	// store
	long save_state=GetWidgetState();

	long figure_state=save_state+2;
	long ready_state=save_state+4;

	// ready background
	if(readied)
		{
		SetWidgetState(ready_state);
		BevelBox(InterpretElement(ready_state,WDS_ELEMENT_BORDER)!=WDS_BEVELBOX_BORDERLESS,FALSE,NULL);
		}

	SetWidgetState(save_state);
	BevelBox(TRUE,TRUE,GetLabelString());

	// bevel figure
	SetWidgetState(figure_state);
	BevelBox(InterpretElement(figure_state,WDS_ELEMENT_BORDER)!=WDS_BEVELBOX_BORDERLESS,FALSE,NULL);

	// restore
	SetWidgetState(save_state);

/*
	if(readied)
		GetRootWindow()->RectWire(truex,truey,truex+sx-1,truey+sy-1,EW_GREEN);
*/

	if(displayfunction)
		{
		long depth=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH);

		displayfunction(GetRootWindow(),truex+depth,truey+depth,sx-2*depth,sy-2*depth,displayarg);
		}
	}



/******************************************************************************
long WDS_Button::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_Button::Resize(EW_Resize *resize)
	{
#if FALSE
	if(GetFlags())
		{
		long size[2];

		GetSize(&size[0],&size[1]);

		if( GetFlags() & WDS_BUTTON_FLAGS_AUTOSIZE_X )
			size[0]=GetRootWindow()->StringWidth(GetLabel())+8;

		if( GetFlags() & WDS_BUTTON_FLAGS_AUTOSIZE_Y )
			size[1]=GetRootWindow()->GetFontHeight()+6;

		SetSize(size[0],size[1]);
		SetPrefSize(size[0],size[1]);
		}
#endif

	if(GetFlags()&WDS_BUTTON_FLAGS_TOGGLE_AUTOSIZE)
		{
		long annx,anny;
		long cly=GetRootWindow()->GetFontHeight();
		long prefx,prefy;

		annx=InterpretElement(GetWidgetState(),WDS_ELEMENT_ANNOTATION_X);
		anny=InterpretElement(GetWidgetState(),WDS_ELEMENT_ANNOTATION_Y);

		prefy=EW_MAX(anny,cly);
		prefx=GetRootWindow()->StringWidth(GetAnnotation()->Buffer())+prefy+annx;

		SetPrefSize(prefx,prefy);
		}

	return WDS_Widget::Resize(resize);
	}



/******************************************************************************
long	WDS_RadioScope::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_RadioScope::Resize(EW_Resize *resize)
	{
	long m;

	// confirm child form
	if(GetWidgetList()->ToHead() != &radioform)
		{
		if(GetWidgetList()->SetCurrent(&radioform))
			RemoveWidget(&radioform);

		AddWidget(&radioform,EW_ADD_BEGINNING);
		}

	if(GetAutoSizing())
		{
		SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
		radioform.SetBounds(EW_BOUNDS_NONE);
		}
	else
		{
		SetNodeSizing(EW_NODE_SIZING_NULL);
		radioform.SetBounds(EW_BOUNDS_FILL_PARENT);
		}

	// FIX: shouldn't repeat every time -> inefficient
	if(stringlist)
		{
		WDS_Activation activation;
		activation.SetTargetWidget(this);

		// confirm mapping from strings to buttons
		if(SetNumberRadios(stringlist->GetNumberNodes()))
			{
			stringlist->ToHead();

			for(m=0;m<radios;m++)
				{
				radio[m]->SetAnnotation(stringlist->PostIncrement()->Buffer());

				activation.SetCode(m);
				radio[m]->SetStateChangeCallback(WDS_STATE_1,&WDS_RadioScope::ButtonCallback,&activation);
				}
			}

		EW_WidgetList *formchildren=radioform.GetWidgetList();

		// confirm radio buttons in form
		long bad_order=FALSE;
		formchildren->ToHead();

		for(m=0;m<radios;m++)
			{
			if(formchildren->PostIncrement() != radio[m])
				{
				bad_order=TRUE;
				break;
				}
			}

		if(bad_order)
			{
			// remove all
			for(m=0;m<radios;m++)
				if(formchildren->SetCurrent(radio[m]))
					radioform.RemoveWidget(radio[m]);

			formchildren->ToHead();

			// replace
			for(m=0;m<radios;m++)
				radioform.AddWidget(radio[m],m==0? EW_ADD_BEGINNING: EW_ADD_AFTER);
			}

		// make sure somebody is active
		long found=FALSE;
		for(m=0;m<radios;m++)
			if(radio[m]->GetWidgetState())
				{
				found=TRUE;
				break;
				}

		if(!found && radios>0)
			radio[0]->ChangeState(WDS_STATE_1,NULL);
		}

	return WDS_Scope::Resize(resize);
	}



/******************************************************************************
long	WDS_RadioScope::SetNumberRadios(long set)

******************************************************************************/
long WDS_RadioScope::SetNumberRadios(long set)
	{
	if(set==radios)
		return FALSE;

	long m;
	for(m=set;m<radios;m++)
		delete radio[m];

	if(set)
		radio=(WDS_RadioButton **)osdRealloc(radio,sizeof(WDS_RadioButton *)*set);
	else
		{
		osdFree(radio);
		radio=NULL;
		}

	for(m=radios;m<set;m++)
		radio[m]=new WDS_RadioButton;

	radios=set;

	return TRUE;
	};



/******************************************************************************
void	WDS_RadioScope::ButtonCallback(WDS_Activation *activation);

******************************************************************************/
void WDS_RadioScope::ButtonCallback(WDS_Activation *activation)
	{
	WDS_RadioScope *radioscope=(WDS_RadioScope *)activation->GetTargetWidget();
	WDS_RadioButton *button=(WDS_RadioButton *)activation->GetThisWidget();
	long code=activation->GetCode();
	long state=activation->GetState();
	EW_Event *event=activation->GetEvent();

//	printf("(%s)->(%s) %d\n",radioscope->GetLabel(),button->GetAnnotation()->Buffer(),code);

	if(state)
		{
		state=WDS_STATE_1;

		radioscope->activation[state].SetTargetWidget(button);
		radioscope->activation[state].SetCode(code);
		radioscope->ChangeState(state,event);
		}
	}
