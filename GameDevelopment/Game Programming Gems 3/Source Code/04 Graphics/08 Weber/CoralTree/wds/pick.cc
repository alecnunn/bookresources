/******************************************************************************

Coral Tree wds/pick.cc
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



/******************************************************************************
		WDS_Pick::WDS_Pick(void)

******************************************************************************/
WDS_Pick::WDS_Pick(void)
	{
	SetName("Pick");

	SetBounds(EW_BOUNDS_IGNORE_PARENT);
	SetPopItem(EW_ITEM_LEFT);
	SetTearItem(EW_ITEM_RIGHT);
    SetFlags(WDS_POPUP_AUTOPOP|WDS_POPUP_GRAB_POINTER);
	SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
	SetLocation(WDS_POPUP_LOC_BOTTOM|WDS_POPUP_LOC_FROM_REGION);

	// do not bypass keyboard
	pick_bypass.SetSIS(
				EW_EVENT_MOUSEPOSITION|EW_EVENT_MOUSEBUTTON,
				EW_ITEM_ALL,
				EW_STATE_ANY);
	CopyToBypassMask(&pick_bypass);


	GetBaseWidget()->AddWidget(&scrollregion);
	scrollregion.AddWidget(&picklist);

	scrollregion.GetLocalLook()->SetElement(WDS_STATE_0,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_OFF);
	scrollregion.SetBounds(EW_BOUNDS_FILL_PARENT);

	picklist.SetEditable(FALSE);

    WDS_Activation activation;
    activation.SetTargetWidget(this);
    activation.SetCode(0);

	picklist.SetStateChangeCallback(WDS_STATE_ALL,&WDS_Pick::PickCallback,&activation);

	AddWidget(&pickbutton);
	pickbutton.SetDormant(TRUE);
	}



/******************************************************************************
void	WDS_Pick::Pop(EW_Event *event)

******************************************************************************/
void WDS_Pick::Pop(EW_Event *event)
	{
	WDS_Popup::Pop(event);

//	GetBaseWindow()->SetIgnoreNextExposure(TRUE);
	}



/******************************************************************************
void	WDS_Pick::PickCallback(WDS_Activation *activation)

******************************************************************************/
void WDS_Pick::PickCallback(WDS_Activation *activation)
	{
	WDS_Pick *thispick=(WDS_Pick *)activation->GetTargetWidget();

	long number=thispick->GetPickList()->GetSelectedNumber();
	long state=activation->GetState();

//	printf("PickCallback() code=%d state=%d number=%d\n",activation->GetCode(),activation->GetState(),number);

	if(state==WDS_STATE_2 || state==WDS_STATE_4)
		thispick->PickNumber(number);

/*
	long m;

	WDS_StringList *stringlist=thispick->GetPickList()->GetStringList();

	stringlist->ToHead();
	for(m=0;m<number;m++)
		stringlist->PostIncrement();

	WDS_String *string=stringlist->Current();

	char *buffer;

	if(string)
		buffer=string->Buffer();
	else
		buffer="ERROR";

//	printf("Pick (%s) \"%s\"\n",thispick->GetNameString()->Buffer(),buffer);

	thispick->SetLabel(buffer);
	thispick->Drop(NULL);
//	thispick->SetDirty(EW_DIRTY_COMPLETE);
*/
	}



/******************************************************************************
long	WDS_Pick::PickNumber(long index)

******************************************************************************/
long WDS_Pick::PickNumber(long index)
	{
	long result=FALSE;

	picked=index;

	WDS_StringList *stringlist=GetPickList()->GetStringList();

	stringlist->ToHead();
	long m;
	for(m=0;m<index;m++)
		stringlist->PostIncrement();

	WDS_String *string=stringlist->Current();

	char *buffer;

	if(string)
		buffer=string->Buffer();
	else
		{
		buffer="ERROR";
		result=TRUE;
		}

//	printf("Pick (%s) \"%s\"\n",thispick->GetNameString()->Buffer(),buffer);

	SetLabel(buffer);
	Drop(NULL);
//	SetDirty(EW_DIRTY_COMPLETE);

	return result;
	}



/******************************************************************************
long	WDS_Pick::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_Pick::Resize(EW_Resize *resize)
	{
	EW_Window *other_root=GetBaseWindow();

	long x,y,sx,sy;
	long bx,bsx;
	long rx,ry,rsx,rsy;
	long winx,winy;

	// need pick size for pickbutton size/position
	CheckSize(resize);

	GetTrueGeometry(&x,&y,&sx,&sy);
	GetRegion(TRUE,&rx,&ry,&rsx,&rsy);

/*
	printf("For Pick (%s)\n",GetNameString()->Buffer());
	DumpInfo(0);
	printf("region %d,%d %d,%d\n",rx,ry,rsx,rsy);
*/

	// button geometry
	bsx=pickbutton.InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PICKBUTTON_WIDTH);
	if(bsx<0)
		bsx=rsy;

//	bx=sx-bsx;
	bx=rx-x+rsx-bsx;

	pickbutton.SetGeometry(bx,ry-y,bsx,rsy);
	pickbutton.SetInvisible(bsx==0);

	long scrolloffset=GetScrollRegion()->GetOverheadHeight();
	long scrolloverhead=scrolloffset*2;
	long entries=0;
	long entrysize=picklist.GetEntryHeight();	// VALID????????
	long selected=picklist.GetSelectedNumber();
	long offset;

	if(picklist.GetStringList())
		entries=picklist.GetStringList()->GetNumberNodes();

	long max_height=EW_MAX(1,InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PICK_WIN_HEIGHT_MAX));
	long desired_height=scrolloverhead+entries*entrysize;

	// auto-offset
	long show_scrollbar= (desired_height>max_height);
	long do_offset=( !show_scrollbar && InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PICK_AUTO_OFFSET));

	if(do_offset)
		offset=scrolloffset+(selected+1)*entrysize+(rsy-entrysize)/2;
	else
		offset=0;

	SetOffset(0,offset);

	// scroll bar mode: off or WDS_ScrollRegion's default
	long scrollmode=WDS_SCROLLABLE_OFF;
	if(show_scrollbar)
		{
//		scrollmode=GetLooks()->LookupLook(WDS_ScrollRegion::GetTypeCodeStatic())->
//													GetElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);
		scrollmode=WDS_ScrollRegion::GetGlobalLookStatic()->GetElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE);

		picklist.SetBounds(EW_HORIZONTAL,EW_BOUNDS_NONE);
		}
//	else
//		picklist.SetBounds(EW_HORIZONTAL,EW_BOUNDS_FILL_PARENT);

	GetScrollRegion()->GetLocalLook()->SetElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,scrollmode);
	GetScrollRegion()->GetScrollBar(EW_VERTICAL)->SetStepSize(1.0/(float)entries);

	// copy pick width to other window
	other_root->GetSize(&winx,&winy);
	winx= (!scrollmode && InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_PICK_MAINTAIN_BUTTON) )? rsx-bsx: rsx;
	winy=EW_MIN(desired_height,max_height);
	other_root->SetSize(winx,winy);


	// let the inherited pop-up take care of the real resizing
	return WDS_Popup::Resize(resize);

/*
	printf("For Pick (%s)\n",GetNameString()->Buffer());
	DumpInfo(0);
	pickbutton.DumpInfo(1);
*/
	}
