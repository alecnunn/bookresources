
/******************************************************************************

Coral Tree wds/relay.cc
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


#define	WDS_RELAY_DEBUG		FALSE



/******************************************************************************
		WDS_Relay::WDS_Relay(void)

******************************************************************************/
WDS_Relay::WDS_Relay(void)
    {
	SetName("Relay");
	registered=FALSE;

	// accept bypassed events
	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS);
	GetLocalEventMask()->SetWidgetFlags(WDS_BYPASS_FLAGS);

	SetPrefSize(0,0);
	}



/******************************************************************************
		WDS_Relay::~WDS_Relay(void)

******************************************************************************/
WDS_Relay::~WDS_Relay(void)
    {
//	printf("WDS_Relay::~WDS_Relay() registered=%d\n",registered);

/*
	if(registered)
		RegisterEventBypass(FALSE,&registered_eventmask,WDS_BYPASS_FLAG_NULL,FALSE);
*/
	}



/******************************************************************************
long	WDS_Relay::Operate(EW_Event *event);

******************************************************************************/
long WDS_Relay::Operate(EW_Event *event)
	{
#if WDS_RELAY_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Relay::Operate() of");
	event->Print((long)this);
#endif

	if( !(event->GetWidgetFlags()&WDS_BYPASS_EVENT) || event->IsIdleMouse())
		return FALSE;

	ChangeState(WDS_STATE_1,event);
	return FALSE;
	}



/******************************************************************************
long	WDS_Relay::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_Relay::Resize(EW_Resize *resize)
    {
	SetSize(0,0);
	return FALSE;
	}



/******************************************************************************
void	WDS_Relay::Register(long on)

******************************************************************************/
void WDS_Relay::Register(long on)
	{
	registered=on;
	RegisterEventBypass(on,&eventmask,WDS_BYPASS_FLAG_NULL,FALSE);

	if(on)
		registered_eventmask.CopyFrom(&eventmask);
	}



/******************************************************************************
		WDS_Relay_F1Help::WDS_Relay_F1Help(void)

******************************************************************************/
WDS_Relay_F1Help::WDS_Relay_F1Help(void)
	{
	SetName("Relay_F1Help");

	WDS_Activation act;

	SetStateChangeCallback(WDS_STATE_1,&WDS_Relay_F1Help::Callback,&act);

	EW_Event relaymask;
	relaymask.SetSIS(EW_EVENT_KEYBOARD,EW_KEY_F_KEYS,EW_STATE_PRESS);
	CopyEventMaskFrom(&relaymask);
	}



/******************************************************************************
void	WDS_Relay_F1Help::Callback(WDS_Activation *activation)

	static

******************************************************************************/
void WDS_Relay_F1Help::Callback(WDS_Activation *activation)
	{
	WDS_Widget *widget=(WDS_Widget *)activation->GetThisWidget();

	EW_Event *event=activation->GetEvent();

#if WDS_RELAY_DEBUG
	EW_PRINT(EW_APP,EW_LOG,"WDS_Relay_F1Help::Callback() got:");
	event->Print((long)widget);
#endif

//	if(event && event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_SHIFT|EW_KEY_F(1),EW_STATE_PRESS))
	if(event && event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_F(1),EW_STATE_PRESS))
		{
#if WDS_RELAY_DEBUG
#endif

#if !EW_RELEASE_GRADE
		EW_PRINT(EW_APP,EW_LOG,"WDS_Relay_F1Help::Callback() got F1, calling SpawnHelp()");
#endif

		widget->SpawnHelp();
		}

	return;
	}



/******************************************************************************
		WDS_DumpWidgetsRelay::WDS_DumpWidgetsRelay(void)

******************************************************************************/
WDS_DumpWidgetsRelay::WDS_DumpWidgetsRelay(void)
	{
	SetName("DumpWidgetsRelay");
	WDS_Activation act;

	SetStateChangeCallback(WDS_STATE_1,&WDS_DumpWidgetsRelay::Callback,&act);

	EW_Event relaymask;
	relaymask.SetSIS(EW_EVENT_KEYBOARD,EW_KEY_F_KEYS,EW_STATE_PRESS);
	CopyEventMaskFrom(&relaymask);

	dump=(EW_Widget *)GPL_BAD_ADDRESS;
	tablet.SetFlags(EW_TABLET_PRETTY);
	tablet.Initialize();
	item=EW_KEY_CONTROL|EW_KEY_F(7);
	}



/******************************************************************************
		WDS_DumpWidgetsRelay::~WDS_DumpWidgetsRelay(void)

******************************************************************************/
WDS_DumpWidgetsRelay::~WDS_DumpWidgetsRelay(void)
	{
	tablet.Finalize();
	}



/******************************************************************************
void	WDS_DumpWidgetsRelay::Callback(WDS_Activation *activation)

	static

******************************************************************************/
void WDS_DumpWidgetsRelay::Callback(WDS_Activation *activation)
	{
	WDS_DumpWidgetsRelay *widget=(WDS_DumpWidgetsRelay *)activation->GetThisWidget();

	EW_Event *event=activation->GetEvent();

#if WDS_RELAY_DEBUG
	EW_PRINT(EW_APP,EW_LOG,"WDS_DumpWidgetsRelay::Callback() got:");
	event->Print((long)widget);
#endif

	if(event && event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(7),EW_STATE_PRESS))
		{

#if !EW_RELEASE_GRADE
		EW_PRINT(EW_APP,EW_LOG,"WDS_DumpWidgetsRelay::Callback() got Ctrl+F7, dumping PostScript");
#endif

			widget->GetTablet()->Clear();
			EW_Redraw redraw;
			redraw.SetForce(TRUE);
			redraw.SetTablet(widget->GetTablet());
			if(widget->dump)
				{
				widget->dump->Redraw(&redraw);
				}
			else
				{
				EW_WidgetList *widgets=widget->GetRootWindow()->GetWidgetList();
				EW_Widget *w;
				while(w=widgets->PostIncrement())
					{
					w->Redraw(&redraw);
					}
				}
			widget->GetTablet()->Save("dump.ps");
		}
	}
