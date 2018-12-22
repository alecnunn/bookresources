/******************************************************************************

Coral Tree wds/tip.cc
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


#define WDS_TIP_DEBUG			FALSE
#define WDS_TIP_DONT_CLOSE		FALSE	/*** always leave last tip
														(for image capture) */
#define WDS_TIP_TIME			700		//* delay time, in milliseconds



/******************************************************************************
		WDS_TipWindow::WDS_TipWindow(void)

******************************************************************************/
WDS_TipWindow::WDS_TipWindow(void)
	{
	SetTitle("Tip Window");
	SetFlags(EW_WINDOW_BORDERLESS|EW_WINDOW_NOMAP);
	SetOrigin(100,100);
	SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);

	tipstring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);
	tipstring.GetString()->SetEditable(FALSE);

	AddWidget(&tipstring);

	SetTipText("Tip Text");
	SetSource(NULL,0,0);
	SetTimerID(-1);
	}



/******************************************************************************
		WDS_TipWindow::~WDS_TipWindow(void)

******************************************************************************/
WDS_TipWindow::~WDS_TipWindow(void)
	{
	// prevent future attempts at using
	WDS_Widget::NullifyTipWindow();
	}



/******************************************************************************
void	WDS_TipWindow::Deactivate(void)

******************************************************************************/
void WDS_TipWindow::Deactivate(void)
	{
	if(!GetEventContext())
		return;

	if(GetTimerID()>=0)
		{
		GetEventContext()->RemoveTimerInterval(GetTimerID());
		SetTimerID(-1);

#if WDS_TIP_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::Deactivate() disconnect pending source, clearing timer");
#endif
		}
	else if(!GetSource())
		{
		return;
		}
	else
		{
		SetTimerID(GetEventContext()->AppendTimerInterval(WDS_TIMER_ID_TIP,WDS_TIP_TIME,this));

#if WDS_TIP_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::Deactivate() disconnect source, resets to timer %d",GetTimerID());
#endif
		}

	SetSource(NULL,0,0);
	Show();
	}



/******************************************************************************
void	WDS_TipWindow::CheckTip(WDS_Widget *widget,EW_Event *idleevent)

******************************************************************************/
void WDS_TipWindow::CheckTip(WDS_Widget *widget,EW_Event *idleevent)
	{
	if(!GetOpened())
		Open(EW_POSITION_RELATIVE_TO,NULL);

#if FALSE
	if(widget->GetTipMode())
		{
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() 0x%x (%s)",widget,widget->GetName());
		idleevent->Print((long)this);
		}
#endif

#if !WDS_TIP_DONT_CLOSE
	if(GetSource()==widget && (!idleevent->InWidget(widget) || idleevent->GetDislocated()) )
		{
		Deactivate();
		return;
		}
#endif

	if(!idleevent->InWidget(widget))
		{
		if(widget==GetForbidden())
			{
#if WDS_TIP_DEBUG
			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() outside source 0x%x -> no longer forbidden",widget);
#endif
			SetForbidden(NULL);
			}

		return;
		}

	// don't accept dislocated events (idlemouse events that didn't actually occur within the visible bounds of the widget)
	if(idleevent->GetDislocated())
		return;

	long mx,my;
	idleevent->GetMousePosition(&mx,&my);

	if(widget==GetForbidden())
		{
#if WDS_TIP_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() source 0x%x forbidden",widget);
#endif
		return;
		}

	if(widget->GetTipMode())
		{
		long previous=FALSE;

		SetForbidden(NULL);

		if(GetTimerID()>=0)
			{
			GetEventContext()->RemoveTimerInterval(GetTimerID());
			SetTimerID(-1);
			previous=TRUE;
			}

		if(GetSource()==NULL)
			{
			SetSource(widget,mx,my);

			if(previous)
				{
				Show();
#if WDS_TIP_DEBUG
				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() xfer to source 0x%x clearing timer",widget);
#endif
				}
			else
				{
				SetTimerID(GetEventContext()->AppendTimerInterval(WDS_TIMER_ID_TIP,WDS_TIP_TIME,this));
#if WDS_TIP_DEBUG
				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() source 0x%x pending timer %d",widget,GetTimerID());
#endif
				}

			}
		else if(GetSource()==widget)
			{
			if(previous)
				{
				SetTimerID(GetEventContext()->AppendTimerInterval(WDS_TIMER_ID_TIP,WDS_TIP_TIME,this));

#if WDS_TIP_DEBUG
				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() motion on pending source 0x%x resets to timer %d",
																										widget,GetTimerID());
#endif
				}
			else
				{
#if WDS_TIP_DEBUG
//				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() motion on source 0x%x without timer -> no effect",widget);
#endif
				}
			}
		else if(previous)
			{
			SetTimerID(GetEventContext()->AppendTimerInterval(WDS_TIMER_ID_TIP,WDS_TIP_TIME,this));
			SetSource(widget,mx,my);

#if WDS_TIP_DEBUG
			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() new pending source 0x%x resets to timer %d",
																										widget,GetTimerID());
#endif
			}
		else
			{
			SetSource(widget,mx,my);
			Show();

#if WDS_TIP_DEBUG
			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::CheckTip() new source 0x%x without timer",widget);
#endif
			}
		}
	}



/******************************************************************************
void	WDS_TipWindow::Show(void)

******************************************************************************/
void WDS_TipWindow::Show(void)
	{
#if WDS_TIP_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipWindow::Show() source 0x%x",GetSource());
#endif

    if(source && source->GetRootWindow())
        {
		long displacment= -16;

		SetTipText(GetSource()->GetTipText());
		
		long tx,ty,tsx,tsy;
		Resize();
		GetSize(&tsx,&tsy);

		long wx,wy;

		long mx=lastx;
		long my=lasty;

		// Note: mouse is relative to widget's window, not this
		source->GetRootWindow()->GetOrigin(&wx,&wy);

		tx=wx+mx;
		ty=wy+my-tsy+displacment;
		SetOrigin(tx,ty);

//		printf("window %d,%d mouse %d,%d tip %d,%d\n",wx,wy,mx,my,tx,ty);

        if(!GetMapped())
            Map();

		Raise(FALSE);

		SetDirty(EW_DIRTY_COMPLETE);
        }
    else
        {
		SetSource(NULL,0,0);

        if(GetMapped())
            UnMap();
		}
	}



/******************************************************************************
		WDS_TipString::WDS_TipString(void)

******************************************************************************/
WDS_TipString::WDS_TipString(void)
	{
	SetName("TipString");

	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS|EW_EVENT_PERIODIC);
	}



/******************************************************************************
long	WDS_TipString::Operate(EW_Event *event)

******************************************************************************/
long WDS_TipString::Operate(EW_Event *event)
	{
#if FALSE
	printf("WDS_TipString::Operate()\n");
	event->Print((long)this);
#endif

	WDS_TipWindow *window=(WDS_TipWindow *)GetRootWindow();

	if(event->IsTimer())
		{
		if(event->GetState()==window->GetTimerID())
			{
			GetRootWindow()->GetEventContext()->RemoveTimerInterval(window->GetTimerID());
			window->SetTimerID(-1);
			
			if(window->GetSource())
				{
#if WDS_TIP_DEBUG
				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipString::Operate() timer %d triggered 0x%x",
																				event->GetState(),window->GetSource());
#endif

				window->Show();
				}
			else
				{
#if WDS_TIP_DEBUG
				EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipString::Operate() timer %d ran out -> reset",event->GetState());
#endif
				}
			}
		else
			{
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_TipString::Operate() stray timer %d vs %d",event->GetState(),window->GetTimerID());

			// TEMPORARY
			GetRootWindow()->GetEventContext()->RemoveTimerInterval(window->GetTimerID());
			window->SetTimerID(-1);
			}

		return FALSE;
		}

/*
	if(event->Is(EW_EVENT_FOCUS,EW_ITEM_ALL,EW_STATE_ANY))
		return FALSE;
*/

#if WDS_TIP_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TipString::Operate() non-timer event -> UnMap()");

	event->Print((long)this);
#endif

	window->UnMap();

	// NOTE: doesn't change source -> make WDS_Widget::CheckForTip() think this is still showing

	return FALSE;
	}
