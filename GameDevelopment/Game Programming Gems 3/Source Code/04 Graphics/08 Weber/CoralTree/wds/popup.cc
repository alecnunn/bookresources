/******************************************************************************

Coral Tree wds/popup.cc
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


#define WDS_POPUP_DEBUG_CHANGE		FALSE
#define WDS_POPUP_DEBUG				FALSE
#define WDS_POPUP_TIMER_DEBUG		FALSE

#define WDS_POPUP_OLDDEBUG			FALSE
#define WDS_POPUP_WIRE				FALSE
#define WDS_EXCLUSIVE_DEBUG			FALSE

#define WDS_POPUP_ALLOW_IDLE		TRUE	// TRUE = normal
#define WDS_POPUP_INCLUDE_BORDER	TRUE	// TRUE = pop when border hit, too
#define WDS_POPUP_NOGRAB			FALSE	// TRUE = prevent mouse grab (for capturing images)

#define	WDS_POPUP_BYPASS_SOURCE		(EW_EVENT_MOUSEPOSITION|EW_EVENT_MOUSEBUTTON|EW_EVENT_KEYBOARD)
#define	WDS_POPUP_BYPASS_ITEM		EW_ITEM_ALL
#define	WDS_POPUP_BYPASS_STATE		EW_STATE_ANY

#if FALSE
#define	WDS_POPUP_OPEN				Open
#define	WDS_POPUP_CLOSE				Close
#else
#define	WDS_POPUP_OPEN				Map
#define	WDS_POPUP_CLOSE				UnMap
#endif



WDS_Popup	*WDS_Popup::grabbed = NULL;



/******************************************************************************
		WDS_PopupBase::WDS_PopupBase(void)

Keywords:
******************************************************************************/
WDS_PopupBase::WDS_PopupBase(void)
	{
	SetName("Popup Base");

	SetBounds(EW_BOUNDS_FILL_PARENT);
	SetEventMask(GetFullEventMask());
	SetRetainLimits(TRUE);
	}



/******************************************************************************
void	WDS_PopupBase::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_PopupBase::Draw(EW_Redraw *redraw)
	{
	BevelBox(TRUE,NULL);

#if WDS_POPUP_WIRE
	long truex,truey,sx,sy;

	GetTrueGeometry(&truex,&truey,&sx,&sy);
	
	GetRootWindow()->RectWire(truex,truey,truex+sx-1, truey+sy-1, EW_CYAN);
#endif
	}



/******************************************************************************
long	WDS_PopupBase::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_PopupBase::Operate(EW_Event *event)
	{
#if WDS_POPUP_OLDDEBUG
	fprintf(stderr,"+");
	fprintf(stderr,"BASE OP SIS=%d,%d,%d | used=%d iskill=%d mother_torn=%d\n",
																event->GetSource(),event->GetItem(),event->GetState(),
																event->GetUsed(),event->IsCloseWindow(),mother->GetTorn());
#endif

	if(event->IsCloseWindow())
		{
		if(mother->GetTorn())
			{
#if WDS_POPUP_OLDDEBUG
			printf("WDS_PopupBase::Operate() mother->UnTearOff()\n");
#endif
			mother->UnTearOff();
			}
#if WDS_POPUP_OLDDEBUG
		else
			printf("WDS_PopupBase::Operate() mother not torn\n");
#endif
		}

	return FALSE;
	}



/******************************************************************************
		WDS_Popup::WDS_Popup()

Keywords:
******************************************************************************/
WDS_Popup::WDS_Popup()
	{
	window_flags = EW_WINDOW_BORDERLESS;
	popup_window = new EW_Window();
	strncpy(title,"WDS POPUP",WDS_MAX_POPUP_TITLE_NAME_LEN);
	WDS_PopupConstructor();
	}



/******************************************************************************
		WDS_Popup::WDS_Popup(char *a_title)

Keywords:
******************************************************************************/
WDS_Popup::WDS_Popup(char *a_title)
	{
	window_flags = 0;
	popup_window = new EW_Window();
	strncpy(title,a_title,WDS_MAX_POPUP_TITLE_NAME_LEN);
	WDS_PopupConstructor();
	}



/******************************************************************************
void	WDS_Popup::WDS_PopupConstructor()

Keywords:
******************************************************************************/
void WDS_Popup::WDS_PopupConstructor()
	{
#if WDS_POPUP_OLDDEBUG
	fprintf(stderr,"WDS_Popup::WDS_PopupConstructor()\n");
#endif

	SetName("Popup");
	SetLabel("Popup");
	SetAnnotatable(TRUE);
	SetPopBlock(FALSE);

	popup_scope = new WDS_Scope();
	popup_scope->SetBounds(EW_BOUNDS_FILL_PARENT);
	popup_scope->SetName("Popup Scope");

	popup_basewidget.SetMother(this);
	popup_scope->AddWidget(&popup_basewidget);

	popup_window->SetTitle(title);
	popup_window->AddWidget(popup_scope);

	popup_context=NULL;
	added = FALSE;
	popped = FALSE;
	popup_register = new GPL_DoubleList<WDS_Popup>;
	armed = FALSE;
	xoffset = 0;
	yoffset = 0;
	torn = FALSE;
	timer = -1;

	SetPopItem(EW_ITEM_RIGHT);
	SetUnPopItem(EW_ITEM_ALL);
	SetTearItem(EW_ITEM_ALL);
	SetUnTearItem(EW_ITEM_ALL);
	SetLocation(WDS_POPUP_LOC_DEFAULT);
	SetFlags(WDS_POPUP_AUTOPOP|WDS_POPUP_GRAB_POINTER|WDS_POPUP_SIBLING_HANDOFF);
	SetEventMask(GetFullEventMask());

	// default bypass mask
	bypass_mask.SetSIS(WDS_POPUP_BYPASS_SOURCE,WDS_POPUP_BYPASS_ITEM,WDS_POPUP_BYPASS_STATE);
	}



/******************************************************************************
		WDS_Popup::~WDS_Popup()

Keywords:
******************************************************************************/
WDS_Popup::~WDS_Popup()
	{
	if(popped)
		Drop(NULL);

	popup_scope->RemoveWidget(&popup_basewidget);

	if(added)
		popup_context->RemoveWindow(popup_window);

//	delete popup_scope;
	delete popup_window;
	delete popup_register;
	}



/******************************************************************************
void	WDS_Popup::SetGrabbed(WDS_Popup *set)

******************************************************************************/
void WDS_Popup::SetGrabbed(WDS_Popup *set)
	{
#if WDS_POPUP_DEBUG
	printf("WDS_Popup::SetGrabbed() 0x%x (%s) -> 0x%x (%s)\n",
		grabbed, grabbed? grabbed->GetNameString()->Buffer(): "null",
		set, set? set->GetNameString()->Buffer(): "null");
#endif

	grabbed=set;
	}



/******************************************************************************
long	WDS_Popup::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_Popup::Operate(EW_Event *event)
	{
#if WDS_POPUP_DEBUG
	if( event->IsIdleMouse() && event->Print((long)this) )
		{
		char message[256];
		sprintf(message,"        for Op In Popup (%s)",GetNameString()->Buffer());
		EW_PRINT(EW_EVENT,EW_LOG,message);
		}
#endif

	if(event->GetUsed())
		{
#if WDS_POPUP_DEBUG
		EW_PRINT(EW_EVENT,EW_LOG,"WDS_Popup::Operate() event is already used (%s)",GetNameString()->Buffer());
#endif
		return FALSE;
		}

/*
	long mx,my;
	event->GetMousePosition(&mx,&my);
	if( event->IsIdleMouse() )
		printf("IDLEMOUSE (%s) %d,%d in=%d popped=%d state=%d\n",
					GetNameString()->Buffer(),mx,my,event->InWidget(this),popped,GetWidgetState());
*/

    if( event->Is(EW_EVENT_WIDGET,WDS_ITEM_POPUP,EW_STATE_ANY) )
		{
		if(event->GetWidgetFlags()&WDS_BYPASS_TAKE_LOST)
			{
#if WDS_POPUP_DEBUG_CHANGE
			event->Print((long)this);
			EW_PRINT(EW_EVENT,EW_LOG,"    for take lost (%s)",GetLabel());
#endif

			Drop(event);
			return FALSE;
			}
		}

#if WDS_POPUP_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"grabbed=0x%x/0x%x popped=%d InWidget(this)=%d",
																GetGrabbed(),(long)this,popped,event->InWidget(this));
#endif

	if( event->IsTimer() && event->GetState()==GetTimerID())
		{
#if WDS_POPUP_TIMER_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Popup::Operate() timer %d received",GetTimerID());
#endif

		StopPendPop();

		if(GetWidgetState() && GetRootWindow()->GetMapped())
			{
#if WDS_POPUP_TIMER_DEBUG
			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Popup::Operate() delayed Pop()");
#endif
			Pop(event);
			}
		}

	long buttonless_pop= (GetFlags()&WDS_POPUP_BUTTONLESS_POP);
	long sibling_handoff= (GetFlags()&WDS_POPUP_SIBLING_HANDOFF);


//	if( !GetGrabbed() && !popped && event->InWidget(this) )
	if( !popped && event->InWidget(this) )
		{
		long idle_handoff=InterpretElement(GetWidgetState(),WDS_ELEMENT_POPUP_IDLE_HANDOFF);

		EW_Event exclusion;
		exclusion.SetSIS(EW_EVENT_WIDGET,WDS_ITEM_POPUP,EW_STATE_ANY);

#if TRUE
		if(sibling_handoff)
			{
			WDS_Scope *bypass_scope=(WDS_Scope *)FindWidgetType(WDS_Scope::GetTypeCodeStatic(),TRUE);
			if( bypass_scope->FindBypass(&exclusion) )
				buttonless_pop=TRUE;
			}
#else
		if(buttonless_pop)
		{
			WDS_Scope *bypass_scope=(WDS_Scope *)FindWidgetType(WDS_Scope::GetTypeCodeStatic(),TRUE);
			if( !(bypass_scope->FindBypass(&exclusion)) )
				buttonless_pop=FALSE;

		}
#endif

		long press_pop=event->Is(EW_EVENT_MOUSEBUTTON,GetPopItem(),EW_STATE_PRESS);
		long motion_pop= (event->IsMouseMotion() && (GetFlags()&WDS_POPUP_SIBLING_HANDOFF));

#if WDS_POPUP_DEBUG_CHANGE
		EW_PRINT(EW_EVENT,EW_LOG,"press_pop=%d motion_pop=%d idle_handoff=%d buttonless_pop=%d",
												press_pop,motion_pop,idle_handoff,buttonless_pop);
		event->Print(0);
#endif

		if(		( (press_pop || motion_pop) && (event->GetWidgetFlags()&WDS_BYPASS_EVENT) )
						||
				( idle_handoff && buttonless_pop && event->IsIdleMouse() && WDS_POPUP_ALLOW_IDLE)
				)
			{
			long delay=InterpretElement(GetWidgetState(),WDS_ELEMENT_POPUP_DELAY);

#if WDS_POPUP_DEBUG_CHANGE
			event->Print((long)this);
			DumpInfo(0);
			if(event->IsIdleMouse())
				EW_PRINT(EW_EVENT,EW_LOG,"    for Op buttonless idle on (%s) -> Pop() delay=%d",GetLabel(),delay);
			else
				EW_PRINT(EW_EVENT,EW_LOG,"    for Op press/motion on (%s) -> Pop()",GetLabel());
#endif

			if(event->IsIdleMouse() && delay>0)
				PendPop(event,delay);
			else
				Pop(event);

			event->SetUsed(EW_USED_ALL);
			}
		}

	return FALSE;
	}



/******************************************************************************
long	WDS_Popup::PostOperate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_Popup::PostOperate(EW_Event *event)
{
	long mx, my;
	long rx,ry,rw,rh;

#if WDS_POPUP_DEBUG
	if( event->IsMouse() && event->Print((long)this) )
		{
		char message[256];
		sprintf(message,"        for PostOp In Popup (%s)",GetNameString()->Buffer());
		EW_PRINT(EW_EVENT,EW_LOG,message);
		}
#endif

	if( WDS_POPUP_ALLOW_IDLE && GetPopBlock() && event->IsIdleMouse() && !event->InWidget(this) && GetWidgetState() )
		{
#if WDS_POPUP_DEBUG_CHANGE
		event->Print((long)this);
		EW_PRINT(EW_EVENT,EW_LOG,"    is off widget (%s)",GetNameString()->Buffer());
#endif

		Drop(event);
		}

/*
	if(event->IsMouse())
		event->Print((long)this);
*/

	if(!GetGrabbed()||(GetGrabbed() == this)||(GetGrabbed() && GetGrabbed()->GetRootWindow() != GetRootWindow()))
	{
		if(event->GetUsed())
		{
#if WDS_POPUP_OLDDEBUG
//			printf("event used 1\n");
#endif
			return FALSE;
		}
	}
	else
	{
#if WDS_POPUP_OLDDEBUG
		printf("event not grabbed grabbed=0x%x this=0x%x grabbed->root=0x%x this->root=0x%x\n",
					GetGrabbed(),(long)this,(long)GetGrabbed()->GetRootWindow(),(long)this->GetRootWindow());
#endif
		return FALSE;
	}

	// ignore mouse focus events, SHOULD WE ????????
	if( (event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS) )
		{
#if WDS_POPUP_OLDDEBUG
//		printf("focus ignored\n");
#endif
		return FALSE;
		}

#if WDS_POPUP_OLDDEBUG
	fprintf(stderr,".");
#endif

	if(!added)
		{
		popup_context=GetRootWindow()->GetEventContext();
		popup_context->AddWindow(popup_window);
		added = TRUE;
		}

	if(popped)
		{
		if(!GetPopBlock())
			{
			if(PropagateEvent(event))
				{
				// in popped up window

				if(event->Is(EW_EVENT_MOUSEBUTTON,GetTearItem(),EW_STATE_PRESS))
					{
#if WDS_POPUP_DEBUG_CHANGE
					EW_PRINT(EW_EVENT,EW_LOG,"press (%s) -> TearOff()",GetLabel());
#endif
					TearOff(TRUE);
					event->SetUsed(EW_USED_ALL);
					}

				if(event->IsLocational())
					return FALSE;
				}
			else
				{
				// outside popped up window

				if(event->Is(EW_EVENT_MOUSEBUTTON,GetUnPopItem(),EW_STATE_PRESS) && !GetTorn())
					{
#if WDS_POPUP_DEBUG_CHANGE
					EW_PRINT(EW_EVENT,EW_LOG,"press outside window (%s) -> Drop()",GetLabel());
#endif
					Drop(event);
					event->SetUsed(EW_USED_ALL);
					}

				if(event->Is(EW_EVENT_MOUSEPOSITION,EW_ITEM_ALL,EW_STATE_ANY) &&
																	(GetFlags()&WDS_POPUP_SIBLING_HANDOFF) && GetGrabbed())
					{
#if WDS_EXCLUSIVE_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"handoff from (%s)",GetLabel());
#endif

					if(GetParentWidget())
						{
/*
						if(SiblingHandoff(event))
							event->SetUsed(EW_USED_ALL);
*/
						}
					else
						EW_PRINT(EW_WIDGET,EW_WARN,"sibling handoff requires at least one ancestor scope widget");
					}
				}
			}
		}
	else if( event->Is(EW_EVENT_MOUSEPOSITION,EW_ITEM_ALL,EW_STATE_ANY) && (GetFlags() & WDS_POPUP_AUTOPOP) && GetGrabbed() )
		{
#if WDS_POPUP_DEBUG_CHANGE
		EW_PRINT(EW_EVENT,EW_LOG,"motion Pop() on (%s)",GetLabel());
#endif

		Pop(event);
		event->SetUsed(EW_USED_ALL);
		}
	else if(armed)
		{
#if WDS_POPUP_DEBUG_CHANGE
		EW_PRINT(EW_EVENT,EW_LOG,"armed Pop() on (%s)",GetLabel());
#endif

		armed = FALSE;
		Pop(event);
		event->SetUsed(EW_USED_ALL);
		}

	if(event->GetUsed())
	{
#if WDS_POPUP_OLDDEBUG
		printf("event used 2\n");
#endif
		return FALSE;
	}

	if(event->Is(EW_EVENT_MOUSEBUTTON,GetUnPopItem(),EW_STATE_RELEASE))
		{

#if WDS_POPUP_OLDDEBUG
		fprintf(stderr,"RELEASE EVENT\n");
#endif

		if(popped)
			{
			if( ((!InRegion(event,WDS_POPUP_INCLUDE_BORDER))&&(GetFlags()&WDS_POPUP_GRAB_POINTER)) ||
						(InRegion(event,WDS_POPUP_INCLUDE_BORDER) && GetPopBlock() ) ||
						(GetFlags() & WDS_POPUP_NONSTICK) )
				{
#if WDS_POPUP_DEBUG_CHANGE
				EW_PRINT(EW_EVENT,EW_LOG,
					"release: outside_region(%d)&&grab(%d) OR inside(%d)&&block(%d) OR non-stick(%d) (%s) -> Drop()",
					!InRegion(event,WDS_POPUP_INCLUDE_BORDER),GetFlags()&WDS_POPUP_GRAB_POINTER,
					InRegion(event,WDS_POPUP_INCLUDE_BORDER),GetPopBlock(),
					GetFlags() & WDS_POPUP_NONSTICK,
					GetLabel());
#endif
				Drop(event);
				event->SetUsed(EW_USED_ALL);
				}
			}
		}
	else if(event->Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_ALL,EW_STATE_PRESS))
		{
		if(popped)
			{
			if(InRegion(event,WDS_POPUP_INCLUDE_BORDER))
				{
				// check for Un-Tear
				if(torn && (event->GetItem() & GetUnTearItem()) )
					{
#if WDS_POPUP_DEBUG_CHANGE
					EW_PRINT(EW_EVENT,EW_LOG,"press popped inside_region torn (%s) -> UnTearOff() Pop()",GetLabel());
#endif
					UnTearOff();
					Pop(event);
					event->SetUsed(EW_USED_ALL);
					}
				}
			else if(GetFlags() & WDS_POPUP_GRAB_POINTER)
				{
				// check for Un-Pop

				if(GetFlags() & WDS_POPUP_SIBLING_HANDOFF)
					{
//					if(!SiblingHandoff(event))
					if(TRUE)
						{
#if WDS_POPUP_DEBUG_CHANGE
						EW_PRINT(EW_EVENT,EW_LOG,"press popped outside_region not-handed-off (%s) -> Drop()",GetLabel());
#endif
						Drop(event);
						event->SetUsed(EW_USED_ALL);
						}
					}
				else if(event->GetItem() & GetUnPopItem())
					{
#if WDS_POPUP_DEBUG_CHANGE
					EW_PRINT(EW_EVENT,EW_LOG,"press popped outside_region grabbed not-sibling (%s) -> Drop()",GetLabel());
#endif
					Drop(event);
					event->SetUsed(EW_USED_ALL);
					}
				}
			}
		else if( InRegion(event,WDS_POPUP_INCLUDE_BORDER) && (event->GetItem()&GetPopItem()) )
			{
#if WDS_POPUP_DEBUG_CHANGE
			EW_PRINT(EW_EVENT,EW_LOG,"press unpopped (%s) -> Pop()",GetLabel());
#endif
			// POP THE WINDOW
			Pop(event);
			event->SetUsed(EW_USED_ALL);
			}
		}

	if(popped && event->IsMouseMotion())
		event->SetUsed(EW_USED_ALL);

	return FALSE;
}



/******************************************************************************
void	WDS_Popup::PendPop(EW_Event *event,long delay)

******************************************************************************/
void WDS_Popup::PendPop(EW_Event *event,long delay)
	{
	if(GetWidgetState())
		return;

	StopPendPop();

//	printf("IsUnderPopup()=%d\n",IsUnderPopup());

	if(GetPopBlock() || !IsUnderPopup())
		{
		Pop(event);

#if WDS_POPUP_TIMER_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Popup::PendPop() (pop-block || not under popup) popped without delay");
#endif
		}
	else
		{
		ChangeState(WDS_STATE_1,event);
		RegisterEvent(TRUE);
		SetDirty(EW_DIRTY_COMPLETE);

		SetTimerID(GetRootWindow()->GetEventContext()->AppendTimerInterval(WDS_TIMER_ID_POPUP,delay,GetRootWindow()));

#if WDS_POPUP_TIMER_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Popup::PendPop() pending timer %d time %dms",GetTimerID(),delay);
#endif
		}
	}



/******************************************************************************
void	WDS_Popup::StopPendPop(void)

******************************************************************************/
void WDS_Popup::StopPendPop(void)
	{
#if WDS_POPUP_TIMER_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Popup::StopPend() timer=%d",GetTimerID());
#endif

	if(GetTimerID()>=0)
		{
#if WDS_POPUP_TIMER_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Popup::StopPend() existing timer %d removed",GetTimerID());
#endif

		GetRootWindow()->GetEventContext()->RemoveTimerInterval(GetTimerID());
		SetTimerID(-1);
		}
	}



/******************************************************************************
void	WDS_Popup::Pop(EW_Event *event)

Keywords:
******************************************************************************/
void WDS_Popup::Pop(EW_Event *event)
	{
#if WDS_POPUP_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"Pop (%s) popped=%d",GetNameString()->Buffer(),popped);
#endif

	StopPendPop();

	if(popped)
		return;

	popped = TRUE;

	if(GetPopBlock())
		{
		ChangeState(WDS_STATE_1,event);
		RegisterEvent(TRUE);
		}
	else
		{
		PopOrTear(event,FALSE,FALSE);
		}

	SetDirty(EW_DIRTY_COMPLETE);

#if WDS_POPUP_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"Pop (%s) complete",GetNameString()->Buffer());
#endif
	}



/******************************************************************************
void	WDS_Popup::PopOrTear(EW_Event *event,long tearonly,long transient)

******************************************************************************/
void WDS_Popup::PopOrTear(EW_Event *event,long tearonly,long transient)
	{
	EW_Window *root=GetRootWindow();

	long dx, dy;
	long mx, my;
	long rx, ry, rw, rh;
	long px, py, pw, ph;

	long truex,truey,sx,sy;

	if(popup_location&WDS_POPUP_LOC_FROM_REGION)
		GetRegion(TRUE,&truex,&truey,&sx,&sy);
	else
		GetTrueGeometry(&truex,&truey,&sx,&sy);
	
#if WDS_POPUP_OLDDEBUG
	fprintf(stderr,"POP\n");
#endif

	// if popup doesn't have valid font and root window does, use root
	if( !popup_window->GetFont()->GetValid())
		{
//		printf("PopOrTear() invalid font (%s)\n",GetNameString()->Buffer());

		if(GetRootWindow()->GetFont()->GetValid() )
			{
//			EW_PRINT(EW_WIDGET,EW_LOG,"pop/tear invalid font (%s)",GetNameString()->Buffer());

//			printf("  -> replacing from root (valid)\n");
			popup_window->SetFont(GetRootWindow()->GetFont());
			}
		}

	root->GetScreenSize(&dx,&dy);
	root->GetGeometry(&rx,&ry,&rw,&rh);

	// resize prior to get proper size
	popup_window->Resize();
	popup_window->GetGeometry(&px,&py,&pw,&ph);

	// SET LOCATION BASED ON popup_location
	switch(popup_location&WDS_POPUP_LOC_MASK)
	{
		case WDS_POPUP_LOC_MOUSE:
			if(event)
				event->GetMousePosition(&mx,&my);
			else
				{
				mx= -1;
				my= -1;
				}
			px = rx+mx+xoffset;
			py = ry+my-ph+yoffset;
			break;
		case WDS_POPUP_LOC_BOTTOM:
			px=rx+truex+xoffset;
			if(ry+truey-ph+yoffset >= 0)
			{
				py = ry+truey-ph+yoffset;
			}
			else
			{
				py = ry+truey+sy-yoffset;
			}
			break;
		case WDS_POPUP_LOC_RIGHT:
			py=ry+truey+sy-ph+yoffset;
			if(rx+truex+sx+pw+xoffset <= dx)
			{
				px = rx+truex+sx+xoffset;
			}
			else
			{
				px = rx+truex-pw-xoffset;
			}
			break;
		case WDS_POPUP_LOC_TOP:
			px=rx+truex+xoffset;
			if(ry+truey+sy+ph+yoffset <= dy)
			{
				py = ry+truey+sy+yoffset;
			}
			else
			{
				py = ry+truey-ph-yoffset;
			}
			break;
		case WDS_POPUP_LOC_LEFT:
			py=ry+truey+sy-ph+yoffset;
			if(rx+truex-pw+xoffset >= 0)
			{
				px = rx+truex-pw+xoffset;
			}
			else
			{
				px = rx+truex+sx-xoffset;
			}
			break;
		default:
			EW_PRINT(EW_WIDGET,EW_WARN,"invalid popup location");
			break;
	}

	/* ADJUST LOCATION FOR SCREEN LIMITS */
	/* GOES OFF RIGHT SIDE */
	if(px+pw > dx)
		px = dx-pw;

	/* GOES OFF LEFT SIDE */
	if(px < 0)
		px = 0;

	/* GOES OFF BOTTOM */
	if(py < 0)
		py = 0;

	/* GOES OFF TOP */
	if(py+ph>dy)
		py = dy-ph;

	popup_window->SetGeometry(px,py,pw,ph);

	ChangeState(WDS_STATE_1,event);
	SetDirty(EW_DIRTY_COMPLETE);

	if(tearonly)
		OpenPopupWindow(transient? EW_WINDOW_TRANSIENT: EW_WINDOW_FLAG_NULL);
	else
		{
		OpenPopupWindow(window_flags);

		// since Win32 uses re-entrant event handling (What the ...),
		// we have to NOT register event until the popup is open and re-entrant events are complete
		RegisterEvent(TRUE);

		if( !WDS_POPUP_NOGRAB && (GetFlags()&WDS_POPUP_GRAB_POINTER) )
			{
			if(!GetGrabbed())
				{
				SetGrabbed(this);
				root->GrabPointer();
				}

			GetGrabbed()->RegisterPop(this);
			}

//		PropagateEvent(event);
		}
	}



/******************************************************************************
void	WDS_Popup::OpenPopupWindow(long wflags)

	popup window's unseen title is copied from the popping widget's label

******************************************************************************/
void WDS_Popup::OpenPopupWindow(long wflags)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"open (%s)",GetNameString()->Buffer());

	popup_window->SetFlags(wflags);
	popup_window->SetDoubleBuffer(GetRootWindow()->GetDoubleBuffer());
	popup_window->SetRGBMode(GetRootWindow()->GetRGBMode());
	popup_window->SetNumberColors(GetRootWindow()->GetNumberColors());
	popup_window->SetTitle(GetLabel());

	popup_window->WDS_POPUP_OPEN(EW_POSITION_RELATIVE_TO,NULL);
//	popup_window->SetFont(GetRootWindow()->GetFont());

	EW_EventContext *context=popup_window->GetEventContext();
	EW_Window *root=GetRootWindow();
	if(context && root && root->GetModal())
		context->AppendWindowToModalStack(popup_window,FALSE);

	popup_window->Raise(FALSE);
	popup_window->SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void	WDS_Popup::Drop(EW_Event *event)

Keywords:
******************************************************************************/
void WDS_Popup::Drop(EW_Event *event)
	{
#if WDS_POPUP_DEBUG
	char message[256];
	sprintf(message,"Drop (%s) popped=%d",GetNameString()->Buffer(),popped);
	EW_PRINT(EW_WIDGET,EW_LOG,message);
#endif

	StopPendPop();

	if(GetPopBlock())
		{
		popped = FALSE;
		RegisterEvent(FALSE);
		}
	else if(popped)
		{
		if(torn||(GetFlags() & WDS_POPUP_PERSISTANT))
			{
			}
		else
			{
			popped = FALSE;

			// if not bumped by another pop-up, reassign explicit focus before closing
			if(!event || !(event->GetWidgetFlags()&WDS_BYPASS_TAKE_LOST) )
				GetRootWindow()->Activate();

			popup_window->WDS_POPUP_CLOSE();
			}

		if((GetFlags() & WDS_POPUP_GRAB_POINTER)&&(GetGrabbed() == this))
			{
			WDS_Popup *popup_reg;

			popup_register->Rewind();

			while((popup_reg = popup_register->Next())!=NULL)
				{
				if(popup_reg==this)
					{
#if WDS_POPUP_DEBUG_CHANGE
					EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Popup::Drop() popup registered with self 0x%x",(long)this);
#endif
					}
				else
					{
					popup_reg->Drop(event);
					popup_register->RemoveNode(popup_reg);
					}
				}

			SetGrabbed(NULL);
			GetRootWindow()->ReleasePointer();
			}

		RegisterEvent(FALSE);
		}

	ChangeState(WDS_STATE_0,event);
	SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void	WDS_Popup::UnTearOff(void)

Keywords:
******************************************************************************/
void WDS_Popup::UnTearOff(void)
	{
//	printf("UnTearOff torn=%d popped=%d flags=%d\n",torn,popped,GetFlags());

	if(!torn) return;

#if WDS_POPUP_OLDDEBUG
	fprintf(stderr,"UNTEAROFF\n");
#endif

	torn = FALSE;

//	if(popped)
//		{
		if(GetFlags() & WDS_POPUP_PERSISTANT)
			{
			}
		else
			{
			popped = FALSE;
			popup_window->WDS_POPUP_CLOSE();
			}
//		}

	ChangeState(WDS_STATE_0,NULL);
	}



/******************************************************************************
void	WDS_Popup::TearOff(long transient)

Keywords:
******************************************************************************/
void WDS_Popup::TearOff(long transient)
	{
#if WDS_POPUP_OLDDEBUG
	fprintf(stderr,"TEAROFF\n");
#endif

	torn = TRUE;

	if(popped)
		{
		RegisterEvent(FALSE);

		if((GetFlags() & WDS_POPUP_GRAB_POINTER)&&(GetGrabbed() == this))
			{
			WDS_Popup *popup_reg;
			popup_register->Rewind();
			while((popup_reg = popup_register->Next())!=NULL)
				{
				popup_reg->Drop(NULL);
				popup_register->RemoveNode(popup_reg);
				}
			SetGrabbed(NULL);
			GetRootWindow()->ReleasePointer();
			}
		else if(GetGrabbed())
			{
			GetGrabbed()->Drop(NULL);
			}

		popup_window->WDS_POPUP_CLOSE();
		}

	PopOrTear(NULL,TRUE,transient);
	}



/******************************************************************************
void	WDS_Popup::RegisterPop(WDS_Popup* popup_reg)

Keywords:
******************************************************************************/
void WDS_Popup::RegisterPop(WDS_Popup* popup_reg)
	{
	// if it isn't already listed
	if(!popup_register->SetCurrent(popup_reg))
		popup_register->AppendNode(popup_reg);
	}



/******************************************************************************
long	WDS_Popup::PropagateEvent(EW_Event *event)

	send event to popped-up window

	returns TRUE if event was processed by another window

Keywords:
******************************************************************************/
long WDS_Popup::PropagateEvent(EW_Event *event)
	{
	long mx, my, px, py, pw, ph, rx, ry, rh, rw;
	long omx,omy;

	long old_flags=event->GetWidgetFlags()&WDS_BYPASS_FLAGS;
	long result=FALSE;
	long propagated=FALSE;

//	printf("PropagateEvent on (%s)\n",GetNameString()->Buffer());

	// remove bypass flags so that handed window gets fresh event
	event->SetWidgetFlagsExcluding(WDS_BYPASS_FLAGS);

	event->GetMousePosition(&mx,&my);
	GetRootWindow()->GetGeometry(&rx,&ry,&rw,&rh);
	if((GetFlags() & WDS_POPUP_GRAB_POINTER)&&(GetGrabbed() == this))
		{
		WDS_Popup *popup_reg, *hand_to,*next;
		long hx,hy;
		popup_register->ToHead();
		while((popup_reg = popup_register->PostIncrement())!=NULL)
			{
			next=popup_register->GetCurrent();

			hand_to = NULL;
			popup_reg->popup_window->GetGeometry(&px,&py,&pw,&ph);

//			if( ((mx>px-rx)&&(mx<px-rx+pw)) && ((my>py-ry)&&(my<py-ry+ph)) )
//			if( EW_WITHIN(mx,my,px-rx,py-ry,pw,ph) )
			if( !event->IsLocational() || event->InBounds(px-rx,py-ry,pw,ph) )
				{
#if WDS_POPUP_DEBUG_CHANGE
				event->Print((long)this);
				EW_PRINT(EW_EVENT,EW_LOG,"   for (%s) root popup %d,%d to registered window of popup (%s) %d,%d %d,%d",
									GetNameString()->Buffer(),
									rx,ry,
									popup_reg->GetNameString()->Buffer(),
									px,py,pw,ph);
#endif

				hx = px;hy = py;
				hand_to = popup_reg;
				}

//			if(hand_to && hand_to!=this && hand_to->popup_window->GetOpened())
			if(hand_to && hand_to->popup_window->GetOpened())
				{
				event->GetMousePosition(&omx,&omy);
				event->SetMousePosition(rx+mx-hx,ry+my-hy);
				hand_to->popup_window->ProcessEvent(event,FALSE);
//				event->SetUsed(EW_USED_ALL);

				event->SetMousePosition(omx,omy);
//				GetRootWindow()->WinSet();

//				if(event->IsLocational())
					propagated=TRUE;
				}

/*
			popup_register->ToHead();
			while((popup_reg = popup_register->PostIncrement())!=NULL)
				EW_PRINT(EW_EVENT,EW_LOG,"   under (%s) is (%s)",GetNameString()->Buffer(),popup_reg->GetNameString()->Buffer());
*/

			// maintain consistancy in reading list
			popup_register->SetCurrent(next);
			}
		}

	// if locational AND at least one other window received the event, done
//	if(propagated)
	if(!event->IsLocational() || propagated)
		result=TRUE;
	else
		{
		popup_window->GetGeometry(&px,&py,&pw,&ph);

#if WDS_POPUP_OLDDEBUG
		fprintf(stderr,"M:%d %d    P:%d %d %d %d    R:%d %d  PY-RY %d   PY-RY+PH %d\n",mx,my,px,py,pw,ph,rx,ry,py-ry,py-ry+ph);
#endif

//		if( ((mx>px-rx)&&(mx<px-rx+pw)) && ((my>py-ry)&&(my<py-ry+ph)) )
//		if( EW_WITHIN(mx,my,px-rx,py-ry,pw,ph) )
		if( (!event->IsLocational() || event->InBounds(px-rx,py-ry,pw,ph)) && popup_window->GetOpened() )
			{
#if WDS_POPUP_DEBUG_CHANGE
			event->Print((long)this);
			EW_PRINT(EW_EVENT,EW_LOG,"(%s) to first window",GetNameString()->Buffer());
			EW_PRINT(EW_EVENT,EW_LOG,"   for (%s) root %d,%d to first window %d,%d %d,%d",
								GetNameString()->Buffer(),
								rx,ry,
								px,py,pw,ph);
#endif

			event->GetMousePosition(&omx,&omy);
			event->SetMousePosition(rx+mx-px,ry+my-py);
			popup_window->ProcessEvent(event,FALSE);
//			event->SetUsed(EW_USED_ALL);

			event->SetMousePosition(omx,omy);
//			GetRootWindow()->WinSet();
			result=TRUE;
			}
		}
		
	// restore bypass flags
	event->SetWidgetFlagsIncluding(old_flags);

	return result;
	}



#if FALSE
/******************************************************************************
long	WDS_Popup::SiblingHandoff(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_Popup::SiblingHandoff(EW_Event *event)
{
	return FALSE;

	EW_WidgetList *parents_widgetlist=GetParentWidget()->GetWidgetList();
	WDS_Widget *sibling;

#if WDS_EXCLUSIVE_DEBUG
	EW_PRINT(EW_EVENT,EW_LOG,"SiblingHandoff() from (%s)",GetLabel());
#endif

	parents_widgetlist->Rewind();

	while( (sibling = (WDS_Widget *)(parents_widgetlist->Next())) != NULL)
	{
		if((sibling != this)&&(sibling->InRegion(event,WDS_POPUP_INCLUDE_BORDER)))
		{
			// if sibling is same type (WDS_Popup or specific derived)
			if(sibling->GetTypeCode() == this->GetTypeCode())
			{
				if(event->GetMouseButtons() & ((WDS_Popup *)sibling)->GetPopItem())
				{	
#if WDS_EXCLUSIVE_DEBUG
					EW_PRINT(EW_EVENT,EW_LOG,"SiblingHandoff() (%s) bumped by (%s)",GetLabel(),sibling->GetLabel());
#endif
					Drop(event);
EW_PRINT(EW_EVENT,EW_LOG,"SiblingHandoff() arm %s",sibling->GetLabel());
					((WDS_Popup *)sibling)->armed=TRUE;
					GetRootWindow()->ProcessEvent(event);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
#endif



/******************************************************************************
void	WDS_Popup::RegisterEvent(long set)

Keywords:
******************************************************************************/
void WDS_Popup::RegisterEvent(long set)
	{
//	printf("WDS_Popup::RegisterEvent(%d) (%s%s%s)\n",set,ewColorCode(EW_CODE_RED),GetName(),ewColorCode(EW_CODE_NORMAL));

	EW_Event event;

	RegisterEventBypass(set,&bypass_mask,WDS_BYPASS_FLAG_TAKE|WDS_BYPASS_FLAG_DESCEND,FALSE);

	event.SetSIS(EW_EVENT_WIDGET,WDS_ITEM_POPUP,EW_STATE_ANY);
	RegisterEventBypass(set,&event,WDS_BYPASS_FLAG_TAKE|WDS_BYPASS_FLAG_DESCEND,FALSE);
	}



/******************************************************************************
void	WDS_Popup::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_Popup::Draw(EW_Redraw *redraw)
	{
//	printf("%sWDS_Popup::Draw() (%s) state=%d%s\n",ewColorCode(3),GetNameString()->Buffer(),GetWidgetState(),ewColorCode(0));

	BevelBox(TRUE,GetLabelString());

#if WDS_POPUP_WIRE
	long truex,truey,sx,sy;

	GetTrueGeometry(&truex,&truey,&sx,&sy);
	
	GetRootWindow()->RectWire(truex,truey,truex+sx-1, truey+sy-1, EW_MAGENTA);
#endif
	}



/******************************************************************************
long	WDS_Popup::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_Popup::Resize(EW_Resize *resize)
	{
	// propagate font settings to popup window
	if(popup_window->GetOpened())
		popup_window->SetFont(GetRootWindow()->GetFont());
	else
		popup_window->SetTemporaryFontHeight(GetRootWindow()->GetFontHeight());

	popup_window->SetEventContext(GetRootWindow()->GetEventContext());

	long result=WDS_Widget::Resize(resize);

	return result;
	}



/******************************************************************************
long	WDS_Popup::IsUnderPopup(void)

******************************************************************************/
long WDS_Popup::IsUnderPopup(void)
	{
	EW_Window *root=GetRootWindow();

	if(!root)
		return FALSE;

	WDS_Widget *scope=(WDS_Widget *)root->GetWidgetList()->ToHead();

	if( !scope || !(scope->WDS_WIDGET_IS(WDS_Scope)) )
		return FALSE;

	WDS_Widget *base=(WDS_Widget *)scope->GetWidgetList()->ToHead();

	if( !base || !(base->WDS_WIDGET_IS(WDS_PopupBase)) )
		return FALSE;

/*
	printf("root=0x%x scope=0x%x %d base=0x%x %d\n",
					root,scope,scope->WDS_WIDGET_IS(WDS_Scope),base,base->WDS_WIDGET_IS(WDS_PopupBase));
*/

	return TRUE;
	}
