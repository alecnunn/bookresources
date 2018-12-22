
/******************************************************************************

Coral Tree wds/divider.cc
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



#define WDS_DIVIDER_MISMATCH_DEBUG	FALSE
#define WDS_DIVIDER_TEST_CHILDREN	FALSE
#define WDS_DIVIDER_TEST_BUTTONS	4

#define WDS_DIVIDER_POPOUT_BUTTONS	(EW_ITEM_MIDDLE|EW_ITEM_RIGHT)	// don't include EW_ITEM_LEFT



/******************************************************************************
		WDS_DividerTab::WDS_DividerTab(void)

******************************************************************************/
WDS_DividerTab::WDS_DividerTab(void)
	{
	SetName("Div Tab");

	// no inherent actions (done manually)
	SetPopItem(EW_ITEM_NONE);
	SetUnPopItem(EW_ITEM_NONE);
	SetTearItem(EW_ITEM_NONE);
	SetUnTearItem(EW_ITEM_NONE);

	SetOffset(0,-32);
	SetFlags(WDS_POPUP_FLAGS_NONE);
	}



/******************************************************************************
		WDS_DividerTab::~WDS_DividerTab(void)

******************************************************************************/
WDS_DividerTab::~WDS_DividerTab(void)
	{
	}



/******************************************************************************
		WDS_Divider::WDS_Divider(void)

******************************************************************************/
WDS_Divider::WDS_Divider(void)
	{
	SetName("Divider");

	// tab array
	tab=NULL;
	tabs=0;

//	SetDirection(WDS_VERTICAL);
//	SetFlags(WDS_PARTITION_FLAGS_ATTACH);

	dividerform.SetName("Dv Form");
	dividerform.SetMode(WDS_FORM_ROW);

	SetPoppable(TRUE);
	selected_child=NULL;
	selected_tab=NULL;

	SetClipping(EW_CLIPPING_ALL);

#if WDS_DIVIDER_TEST_CHILDREN

	WDS_Form *testform=new WDS_Form[WDS_DIVIDER_TEST_BUTTONS];

	char message[256];
	long m;
	for(m=0;m<WDS_DIVIDER_TEST_BUTTONS;m++)
		{
		sprintf(message,"Dv tform %d",m);
		testform[m].SetName(message);
		sprintf(message,"Test %d",m);
		testform[m].SetLabel(message);

		testform[m].SetSizing(EW_HORIZONTAL,WDS_SIZING_MANUAL);
		testform[m].SetSizing(EW_VERTICAL,WDS_SIZING_MANUAL);
		testform[m].SetClear(TRUE);
		testform[m].GetLocalLook()->SetElement(WDS_STATE_ALL,EW_COLOR_WIDGET,EW_BLUE+m);
		AddWidget(&testform[m]);
		}

#endif
	}



/******************************************************************************
		WDS_Divider::~WDS_Divider(void)

******************************************************************************/
WDS_Divider::~WDS_Divider(void)
	{
/*
	DumpString();
	DumpInfo(0);
	GetWidgetList()->DumpInfo(1,TRUE,FALSE);
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Divider::~WDS_Divider() tab=0x%x tabs=%d",tab,tabs);
*/

	RemoveWidget(&dividerform);

	long m;
	for(m=0;m<tabs;m++)
		dividerform.RemoveWidget(tab[m]);

	SetNumberTabs(0);
	}



/******************************************************************************
void	WDS_Divider::SetNumberTabs(long set)

******************************************************************************/
void WDS_Divider::SetNumberTabs(long set)
	{
	long m;

	// delete previous
	if(tab)
		{
		for(m=0;m<tabs;m++)
			delete tab[m];

		osdFree(tab);
		tab=NULL;
		}

	// allocate new
	if(set>0)
		{
		tab=(WDS_DividerTab **)osdMalloc(set*sizeof(WDS_DividerTab *));

		for(m=0;m<set;m++)
			tab[m]=new WDS_DividerTab;
		}

	tabs=set;
	}



/******************************************************************************
void	WDS_Divider::ChangeState(long newstate,EW_Event *event)

******************************************************************************/
void WDS_Divider::ChangeState(long newstate,EW_Event *event)
	{
	WDS_Widget *selected=GetSelected();

#if FALSE
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Divider::ChangeState(%d,0x%x) (%s) to (%s)",
					newstate,event,GetNameString()->Buffer(),selected? selected->GetNameString()->Buffer(): "<NONE>");
#endif

	// overwrite activation's target
	activation[newstate].SetTargetWidget(selected);

	WDS_Widget::ChangeState(newstate,event);
	}



/******************************************************************************
void	WDS_Divider::SelectIndex(long index,EW_Event *event)

******************************************************************************/
void WDS_Divider::SelectIndex(long index,EW_Event *event)
	{
	GetWidgetList()->ToHead();

	GetWidgetList()->ToIndex(index+1);

	WDS_Widget *current=(WDS_Widget *)GetWidgetList()->GetCurrent();

	SetSelected(current);

	SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
long	WDS_Divider::PostOperate(EW_Event *event)

******************************************************************************/
long WDS_Divider::PostOperate(EW_Event *event)
	{
	if(event->IsLeftMousePress() || event->IsKeyTyped(EW_KEY_ESC) ||
							event->Is(EW_EVENT_MOUSEBUTTON,WDS_DIVIDER_POPOUT_BUTTONS,EW_STATE_PRESS))
		{
		long m;
		long popout=FALSE;

		for(m=0;m<tabs;m++)
			{
			if(event->InWidget(tab[m]))
				{
				if(GetPoppable())
					{
					if(IsPopped(m))
						DropIn(m);
					else if(!event->IsLeftMousePress() || event->GetRepetition())
						{
						PopOut(event,m);
						popout=TRUE;
						}
					}

				if(!popout && event->IsLeftMousePress())
					SelectIndex(m,event);

				break;
				}
			}

		event->SetUsed(EW_USED_ALL);
		}

	return FALSE;
	}



/******************************************************************************
void	WDS_Divider::PopOut(EW_Event *event,long index)

******************************************************************************/
void WDS_Divider::PopOut(EW_Event *event,long index)
	{
//	printf("WDS_Divider::PopOut()\n");

	WDS_Widget *node=(WDS_Widget *)GetWidgetList()->ToIndex(index+1);
	WDS_DividerPlaceHolder *holder=new WDS_DividerPlaceHolder;
	WDS_Divider *pop_divider=holder->GetDivider();

	holder->SetLabel(node->GetLabel());

	long sx,sy;
	GetSize(&sx,&sy);
	tab[index]->GetBaseWindow()->SetSize(sx,sy);

	// insert a place holder
	GetWidgetList()->SetCurrent(node);
	AddWidget(holder,EW_ADD_AFTER);
	holder->SetDisowned(node);

	// give child to pop-up window's new divider
	RemoveWidget(node);
	tab[index]->GetBaseWidget()->AddWidget(pop_divider);
	tab[index]->GetBaseWindow()->SetTitle(node->GetLabel());
	pop_divider->AddWidget(node);
	pop_divider->SetPoppable(FALSE);

//	tab[index]->Pop(event);
	tab[index]->TearOff(FALSE);
	}



/******************************************************************************
void	WDS_Divider::DropIn(long index)

******************************************************************************/
void WDS_Divider::DropIn(long index)
	{
//	printf("WDS_Divider::DropIn()\n");

	WDS_Widget *node=(WDS_Widget *)GetWidgetList()->ToIndex(index+1);

	if(!node || !node->WDS_WIDGET_IS(WDS_DividerPlaceHolder))
		{
		EW_PRINT(EW_APP,EW_ERROR,"WDS_Divider::DropIn(%d) indexed widget not popped out",index);
		return;
		}
	
	WDS_DividerPlaceHolder *holder=(WDS_DividerPlaceHolder *)node;

	WDS_Divider *pop_divider=holder->GetDivider();
	node=holder->GetDisowned();

	// reinstate disowned child after holder
	pop_divider->RemoveWidget(node);
	GetWidgetList()->SetCurrent(holder);
	AddWidget(node,EW_ADD_AFTER);

	// reverse the pop-out
	tab[index]->GetBaseWidget()->RemoveWidget(pop_divider);
	tab[index]->UnTearOff();

	// remove and discard place holder
	RemoveWidget(holder);
	delete holder;
	holder=NULL;
	}



/******************************************************************************
long	WDS_Divider::IsPopped(long index)

******************************************************************************/
long WDS_Divider::IsPopped(long index)
	{
	WDS_Widget *node=(WDS_Widget *)GetWidgetList()->ToIndex(index+1);

	return node->WDS_WIDGET_IS(WDS_DividerPlaceHolder);
	}



/******************************************************************************
void	WDS_Divider::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_Divider::Draw(EW_Redraw *redraw)
	{
/*
	printf("WDS_Divider::Draw() (%s) \n",GetNameString()->Buffer());
	GetParentWidget()->GetWidgetList()->DumpInfo(0,TRUE);
*/

	long pos[2],size[2];	// widget's geometry
	long fsize[2];			// form's geometry
	long cpos[2],csize[2];	// childs's geometry

	long overlap=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_DIVIDER_OVERLAP);

	GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	dividerform.GetSize(&fsize[0],&fsize[1]);

	BevelBox(FALSE,TRUE,NULL,pos[0],pos[1],size[0],size[1]-fsize[1]);

	if(selected_tab && overlap)
		{
		// clear space under active tab
		selected_tab->GetTrueGeometry(&cpos[0],&cpos[1],&csize[0],&csize[1]);

		GetRootWindow()->RectFill(cpos[0],cpos[1]-overlap,cpos[0]+csize[0]-1,cpos[1]-1,
												InterpretElement(WDS_STATE_ALL,EW_COLOR_WIDGET));
		}
	}



/******************************************************************************
void	WDS_Divider::PostDraw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_Divider::PostDraw(EW_Redraw *redraw)
	{
	// redraw selected tab because of potential overlap (such as under Win32 style)
	if(selected_tab)
		selected_tab->Draw(redraw);
	}



/******************************************************************************
void	WDS_DividerTab::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_DividerTab::Draw(EW_Redraw *redraw)
	{
	long pos[2],size[2];
	long psize[2];

	GetParentWidget()->GetSize(&psize[0],&psize[1]);
	GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

//	printf("tab %d,%d %d,%d in %d,%d (%s)\n",pos[0],pos[1],size[0],size[1],psize[0],psize[1],GetNameString()->Buffer());

	long not_bottom=FALSE;
	if(pos[1]+size[1]<psize[1])
		not_bottom=TRUE;

	long state=GetWidgetState();

	long local=GetLocalLook()->GetElement(state,WDS_ELEMENT_BORDER);

	long interpret=InterpretElement(state,WDS_ELEMENT_BORDER);

	long underline=FALSE;

	// if bottom omitted but not on bottom row, force drawing of bottom (and underline for clarity)
	if( not_bottom && (interpret&WDS_BEVELBOX_OMIT_BOTTOM) )
		{
		interpret^=WDS_BEVELBOX_OMIT_BOTTOM;
		GetLocalLook()->SetElement(state,WDS_ELEMENT_BORDER,interpret);

		if(state)
			underline=TRUE;
		}

	if(underline)
		GetLabelString()->SetStyle(GetLabelString()->GetStyle()|WDS_STRING_STYLE_BOLD);
	else
		GetLabelString()->SetStyle(EW_MASK_EXCLUDING(GetLabelString()->GetStyle(),WDS_STRING_STYLE_BOLD));

//	WDS_Popup::Draw(redraw);
	BevelBox(TRUE,GetLabelString());

	if(not_bottom)
		GetLocalLook()->SetElement(state,WDS_ELEMENT_BORDER,local);
	}



/******************************************************************************
long	WDS_Divider::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_Divider::Resize(EW_Resize *resize)
	{
//	printf("WDS_Divider::Resize() (%s)\n",GetNameString()->Buffer());

	EW_Resize new_resize;

	long pos[2],size[2];		// widget's geometry
	long cpos[2],csize[2];		// childs's geometry
	long pref[2],max[2];

	long slide=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_DIVIDER_SLIDEFORM);
	long inset=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_INSET);
	long edge=inset+InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH)+
												InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_SUBINSET);
	long form_displace=inset+slide;

	ConfirmSlaves();

    long resized=CheckSize(resize);

	// pass along any info
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

//>>
	GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

	new_resize.GetPrefSize(&pref[0],&pref[1]);
	new_resize.GetMaxSize(&max[0],&max[1]);
//	max[0]-= (form_displace)*2;
	max[0]= size[0]-(form_displace)*2;
	new_resize.SetPrefSize(max[0],pref[1]);
	new_resize.SetMaxSize(max[0],max[1]);

	dividerform.PreResize(&new_resize);

//	GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	dividerform.GetSize(&csize[0],&csize[1]);

	cpos[0]=form_displace;
	cpos[1]=size[1]-csize[1]-inset;

	dividerform.SetOrigin(cpos[0],cpos[1]);
	dividerform.CalcTruePosition(TRUE);

	max[0]=size[0]-2*edge;
	max[1]=size[1]-csize[1]-2*edge;

	new_resize.SetPrefSize(max[0],max[1]);
	new_resize.SetMaxSize(max[0],max[1]);

	if(GetSelected())
		{
		GetSelected()->PreResize(&new_resize);

		GetSelected()->SetOrigin(edge,edge);
		GetSelected()->CalcTruePosition(TRUE);
		}
	else
		{
//		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Divider::Resize() no child selected");
		}

	if(resized)
		SetDirty(EW_DIRTY_COMPLETE);

	return FALSE;
	}



/******************************************************************************
void	WDS_Divider::ConfirmSlaves(void)

	confirm that form is first child
	correct if neccessary

Keywords:
******************************************************************************/
void WDS_Divider::ConfirmSlaves(void)
	{
	WDS_Widget *node;
	long mismatch=FALSE;

	// check location of form

	node=(WDS_Widget *)GetWidgetList()->ToHead();
	if(node!=&dividerform)
		{
		mismatch=TRUE;

#if WDS_DIVIDER_MISMATCH_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Divider::ConfirmSlaves() mismatched form");
#endif
		}
   
//	any others ...
//	node=(WDS_Widget *)GetWidgetList()->PreIncrement();

	if(mismatch)
		{
		// remove (if exist) and force back on the beginning

		if(GetWidgetList()->SetCurrent(&dividerform))
			RemoveWidget(&dividerform);

#if WDS_DIVIDER_MISMATCH_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Divider::ConfirmSlaves() re-instating");
#endif

		AddWidget(&dividerform,EW_ADD_BEGINNING);
		}

	// check tabs on form

	mismatch=FALSE;
	long new_tabs=GetWidgetList()->GetNumberNodes()-1;

	long m;
	char message[256];

	if(tabs!=new_tabs)
		{
		// remove all old tabs

		dividerform.GetWidgetList()->ToHead();
		while( (node=(WDS_Widget *)dividerform.GetWidgetList()->GetCurrent()) != NULL )
			dividerform.RemoveWidget(node);

		// allocate and add new tabs (old tabs auto-deleted in SetNumberTabs())

		SetNumberTabs(new_tabs);

		GetWidgetList()->ToHead();
		GetWidgetList()->PostIncrement();

		for(m=0;m<new_tabs;m++)
			{
			if( !(node=(WDS_Widget *)GetWidgetList()->PostIncrement()) )
				break;

			sprintf(message,"Dv Tab %d",(int)m);
			tab[m]->SetName(message);
			tab[m]->SetLabel(node->GetLabel());

			tab[m]->SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);

#if WDS_DIVIDER_TEST_CHILDREN
			tab[m]->GetLocalLook()->SetElement(WDS_STATE_ALL,EW_COLOR_WIDGET,EW_BLUE+m);
#endif

			dividerform.AddWidget(tab[m]);
			}

		tabs=new_tabs;
		}

	// reset tab labels and set all but selected invisible
	// assumed faster to just set them than to check if they've been changed

	GetWidgetList()->ToHead();
	GetWidgetList()->PostIncrement();

	long selected_found=FALSE;
	long selected;

	selected_tab=NULL;

	for(m=0;m<tabs;m++)
		{
		if( !(node=(WDS_Widget *)GetWidgetList()->PostIncrement()) )
			break;

		tab[m]->SetLabel(node->GetLabel());

		selected= (node==GetSelected());

		if(selected)
			{
			selected_found=TRUE;
			tab[m]->ChangeStateIfDifferent(WDS_STATE_1,NULL);
			selected_tab=tab[m];
			}
		else
			tab[m]->ChangeStateIfDifferent(WDS_STATE_0,NULL);

		node->SetInvisible(!selected);
		node->SetDormant(!selected);
//		tab[m]->SetWidgetState(selected);
		}

	// if none of actual children selected, select first
	if(!selected_found)
		{
		ChangeState(WDS_STATE_0,NULL);

		GetWidgetList()->ToIndex(1);

		// but don't auto-select a placeholder
		m=0;
		while( (node=(WDS_Widget *)GetWidgetList()->PostIncrement()) != NULL &&
																			node->WDS_WIDGET_IS(WDS_DividerPlaceHolder))
			m++;

		if(m>tabs-1)
			m=0;

		SetSelected(node);
		SetDirty(EW_DIRTY_COMPLETE);

		if(node)
			{
			selected_tab=tab[m];
			tab[m]->ChangeStateIfDifferent(WDS_STATE_1,NULL);

			node->SetInvisible(FALSE);
			node->SetDormant(FALSE);
			}
		}
	}
