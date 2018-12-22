/******************************************************************************

Coral Tree wds/menu.cc
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



#define	WDS_MENU_DEBUG	FALSE



/******************************************************************************
        WDS_MenuBar::WDS_MenuBar(void)

******************************************************************************/
WDS_MenuBar::WDS_MenuBar(void)
	{
	SetName("Menu Bar");

	SetMode(WDS_FORM_ROW);
	SetClear(TRUE);
	}



/******************************************************************************
        WDS_MenuForm::WDS_MenuForm(void)

******************************************************************************/
WDS_MenuForm::WDS_MenuForm(void)
	{
	SetName("Menu Form");

	SetMode(WDS_FORM_COLUMN);
	SetUniformity(WDS_UNIFORM_X);
	}



/******************************************************************************
        WDS_MenuNode::WDS_MenuNode(void)

******************************************************************************/
WDS_MenuNode::WDS_MenuNode(void)
	{
	SetName("Menu Node");

	GetBaseWindow()->SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);

	// override fill-parent and size around child
	GetPopupScope()->SetName("Menu Scope");
	GetPopupScope()->SetBounds(EW_BOUNDS_NONE);
	GetPopupScope()->SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);

	GetBaseWidget()->SetName("Menu Base");
	GetBaseWidget()->SetBounds(EW_BOUNDS_NONE);
	GetBaseWidget()->SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
	GetBaseWidget()->AddWidget(&menuform);

//	SetFlagsIncluding(WDS_POPUP_BUTTONLESS_POP);

	SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOPREF);
	SetMenuFlags(WDS_MENU_FLAGS_AUTOPOSITION);
	SetPopItem(EW_ITEM_LEFT);
	SetTearItem(EW_ITEM_RIGHT);

	EW_Node::AddWidgetProtected(&menubutton,EW_ADD_DEFAULT);
	menubutton.SetDormant(TRUE);

	master=NULL;
	}



/******************************************************************************
long	WDS_MenuNode::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_MenuNode::Resize(EW_Resize *resize)
	{
	CheckSize(resize);

/*
long strx,stry;
GetLabelString()->GetStringSpace(GetRootWindow(),-1,&strx,&stry);
printf("%d,%d (%s) (%s)\n",strx,stry,GetLabelString(),GetName());
*/

	// only popable if there are children
	long children=menuform.GetWidgetList()->GetNumberNodes();
	SetPopBlock(!children);

	long x,y,sx,sy;
	long rx,ry,rsx,rsy;

	GetTrueGeometry(&x,&y,&sx,&sy);
	GetRegion(TRUE,&rx,&ry,&rsx,&rsy);

	long bx,bsx=menubutton.InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_MENUBUTTON_WIDTH);
    if(bsx<0)
        bsx=rsy;

//	bx=rx-x+rsx-bsx;
	bx=sx-bsx;

	long invisible=(bsx==0 || !children || master==NULL);

	menubutton.SetGeometry(bx,ry-y,bsx,rsy);
	menubutton.SetInvisible(invisible);
	menubutton.SetWidgetState(GetWidgetState());

	if(invisible)
		GetLocalLook()->SetElement(GetWidgetState(),WDS_ELEMENT_SUBOUTDENT,WDS_ELEMENT_DEFAULT_INIT);
	else
		GetLocalLook()->SetElement(GetWidgetState(),WDS_ELEMENT_SUBOUTDENT,bsx);

	WDS_Widget *parent=(WDS_Widget *)GetParentWidget();

	if(GetMenuFlags()&WDS_MENU_FLAGS_AUTOPOSITION)
		{
		if(parent && parent->WDS_WIDGET_IS(WDS_MenuForm))
			SetLocation(WDS_POPUP_LOC_RIGHT);
		else
			SetLocation(WDS_POPUP_LOC_BOTTOM);
		}

	long result=WDS_Popup::Resize(resize);

	return result;
	}



/******************************************************************************
void	WDS_MenuNode::Pop(EW_Event *event)

******************************************************************************/
void WDS_MenuNode::Pop(EW_Event *event)
	{
#if WDS_MENU_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_MenuNode::Pop() start (%s)",GetNameString()->Buffer());
#endif

	WDS_Popup::Pop(event);

#if WDS_MENU_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_MenuNode::Pop() done (%s) master menu=0x%x base=0x%x (%s)",
			GetNameString()->Buffer(),master,GetBaseWindow(),GetBaseWindow()->GetTitle());
#endif

#if WDS_MENU_DEBUG
	if(master)
		{

		EW_Window *master_window=master->GetBaseWindow();
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_MenuNode::Pop() (%s) dirty master 0x%x (%s)",
				GetNameString()->Buffer(),master_window,master_window->GetTitle());

//		master_window->SetDirty(EW_DIRTY_COMPLETE);
//		master_window->SetIgnoreNextExposure(TRUE);
		}
#endif

	// self
	GetBaseWindow()->SetDirty(EW_DIRTY_COMPLETE);
	GetBaseWindow()->SetIgnoreNextExposure(TRUE);
	}



/******************************************************************************
void	WDS_MenuNode::Drop(EW_Event *event)

******************************************************************************/
void WDS_MenuNode::Drop(EW_Event *event)
	{
	EW_WidgetList *widgetlist=menuform.GetWidgetList();
	WDS_Widget *node;

	widgetlist->ToHead();
	while( (node=(WDS_Widget *)widgetlist->PostIncrement()) != NULL)
		{
		if(node->WDS_WIDGET_IS(WDS_MenuNode))
			{
			WDS_MenuNode *mnode=(WDS_MenuNode *)node;

			if(mnode->GetPopped())
				mnode->Drop(event);
			}
		}

#if WDS_MENU_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_MenuNode::Drop() start (%s)",GetNameString()->Buffer());
#endif

	WDS_Popup::Drop(event);

#if WDS_MENU_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_MenuNode::Drop() end (%s)",GetNameString()->Buffer());
#endif
	}



/******************************************************************************
void    WDS_MenuNode::ChangeState(long newstate,EW_Event *event)

	filter to selection of menu items

******************************************************************************/
void WDS_MenuNode::ChangeState(long newstate,EW_Event *event)
	{
#if FALSE
	long oldstate=GetWidgetState();
#endif

	// silent change
	SetWidgetState(newstate);

	long children=menuform.GetWidgetList()->GetNumberNodes();

#if FALSE
	printf("WDS_MenuNode::ChangeState() %d -> %d (%s)\n",oldstate,newstate,GetNameString()->Buffer());
	if(event)
		event->Print((long)this);
#endif

	if( children==0 && (!event || event->Is(EW_EVENT_MOUSEBUTTON,GetPopItem(),EW_STATE_RELEASE)) )
		{
#if !EW_RELEASE_GRADE
		EW_PRINT(EW_WIDGET,EW_LOG,"pop mouse release (%s)",GetNameString()->Buffer());
#endif

		// vocal change
		WDS_Widget::ChangeState(newstate,event);

		// drop ancestor-most (and therefrom, its descendants)
		WDS_MenuNode *menunode=master,*next;

		while( menunode && (next=menunode->master)!=NULL )
			menunode=next;

		if(menunode)
			menunode->Drop(event);
		}
	}



/******************************************************************************
void	WDS_MenuNode::AddWidgetProtected(EW_Widget *widget,long placement)

	virtual 

******************************************************************************/
void WDS_MenuNode::AddWidgetProtected(EW_Widget *widget,long placement)
	{
	WDS_Widget *wwidget=(WDS_Widget *)widget;

	menuform.AddWidget(widget,placement);

	if(wwidget->WDS_WIDGET_IS(WDS_MenuNode))
		{
		WDS_MenuNode *node=(WDS_MenuNode *)wwidget;

		node->master=this;
		node->SetFlagsIncluding(WDS_POPUP_BUTTONLESS_POP);
		}
	}



/******************************************************************************
void	WDS_MenuNode::RemoveWidgetProtected(EW_Widget *widget)

	virtual

******************************************************************************/
void WDS_MenuNode::RemoveWidgetProtected(EW_Widget *widget)
	{
	WDS_Widget *wwidget=(WDS_Widget *)widget;

	menuform.RemoveWidget(widget);

	if(wwidget->WDS_WIDGET_IS(WDS_MenuNode))
		{
		WDS_MenuNode *node=(WDS_MenuNode *)wwidget;

		node->master=NULL;
		node->SetFlagsExcluding(WDS_POPUP_BUTTONLESS_POP);
		}
	}
