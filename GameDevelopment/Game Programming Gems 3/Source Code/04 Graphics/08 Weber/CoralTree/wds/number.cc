/******************************************************************************

Coral Tree wds/number.cc
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



#define WDS_STRING_BUFFER   128



/******************************************************************************
		WDS_NumberEntry::WDS_NumberEntry()

Keywords:
******************************************************************************/
WDS_NumberEntry::WDS_NumberEntry()
	{
	DB("WDS_NumberEntry::WDS_NumberEntry()");

/* 	label = NULL; */
	stepsize = (float)(1.0);
	number = NULL;

	SetName("NumberEntry");

//	GetLocalLook()->SetBorderType(WDS_BEVELBOX_BORDER,WDS_BEVELBOX_BORDER);
	}



/******************************************************************************
void	WDS_NumberEntry::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_NumberEntry::Draw(EW_Redraw *redraw)
	{
	DB("WDS_NumberEntry::Draw(EW_Redraw *redraw)");

	long truex,truey,sx,sy;

	GetTrueGeometry(&truex,&truey,&sx,&sy);

	BevelBox(FALSE,TRUE,&label,truex,truey,labelwidth,sy);
	}



/******************************************************************************
void	WDS_NumberEntry::SetLabel(char *a_string)

Keywords:
******************************************************************************/
void WDS_NumberEntry::SetLabel(char *a_string)
	{
	if(label.NewBuffer(a_string) == FALSE)
		EW_PRINT(EW_OS,EW_ERROR,"could not allocate more memory");
	}



/******************************************************************************
void WDS_NumberEntry::SetUp(long x,long y,long w,long h,
													long field_w,long button_w)

Keywords:
******************************************************************************/
void WDS_NumberEntry::SetUp(long x,long y,long w,long h,long field_w, long button_w)
	{
	char tmp[WDS_STRING_BUFFER];

	SetGeometry(x,y,w,h);

	AddWidget(&string_entry);
	AddWidget(&incr_button);
	AddWidget(&decr_button);

	string_entry.SetGeometry(	w - 2*button_w - field_w,
								0,
								field_w,
								h);

//	string_entry.RegisterCallback(wdsNumberEntry_GetStringCallback);

	sprintf(tmp,"%f",*number);
	string_entry.SetString(tmp);

	incr_button.SetGeometry(	w - 2*button_w,
								0,
								button_w,
								h);

	decr_button.SetGeometry(    w - button_w,
								0,
								button_w,
								h);

	long truex,truey,sx,sy;
	GetTrueGeometry(&truex,&truey,&sx,&sy);

	/* width of text label (note extra space) */
	labelwidth=sx-field_w-2*button_w-GetLocalLook()->GetElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH);
	}



/******************************************************************************
void	WDS_IncrButton::ChangeState(long newstate,EW_Event *event)

******************************************************************************/
void WDS_IncrButton::ChangeState(long newstate,EW_Event *event)
	{
	WDS_Widget::ChangeState(newstate,event);

	if(newstate)
		((WDS_NumberEntry *)GetParentWidget())->Increment();
	}



/******************************************************************************
void	WDS_DecrButton::ChangeState(long newstate,EW_Event *event)

******************************************************************************/
void WDS_DecrButton::ChangeState(long newstate,EW_Event *event)
	{
	WDS_Widget::ChangeState(newstate,event);

	if(newstate)
		((WDS_NumberEntry *)GetParentWidget())->Decrement();
	}



/******************************************************************************
void	wdsNumberEntry_GetStringCallback(WDS_Widget *string_widget,
																char *a_string)

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void wdsNumberEntry_GetStringCallback(WDS_Widget *string_widget, char *a_string)
	{
	char tmp[WDS_STRING_BUFFER];

	WDS_NumberEntry *tmp_num_entry;

	tmp_num_entry = (WDS_NumberEntry *)(string_widget->GetParentWidget());
	tmp_num_entry->SetNumberValue(atof(a_string));

	sprintf(tmp,"%f",*(tmp_num_entry->GetNumber()));
	((WDS_GetString *)string_widget)->SetString(tmp);
	}



/******************************************************************************
void	WDS_NumberEntry::Increment()

Keywords:
******************************************************************************/
void WDS_NumberEntry::Increment()
	{
	DB("WDS_NumberEntry::Increment()");

	char tmp[WDS_STRING_BUFFER];
	
	if(number)
		*number += stepsize;

	sprintf(tmp,"%f",*number);
	string_entry.SetString(tmp);

	SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void	WDS_NumberEntry::Decrement()

Keywords:
******************************************************************************/
void WDS_NumberEntry::Decrement()
	{
	DB("WDS_NumberEntry::Decrement()");

	char tmp[WDS_STRING_BUFFER];

	if(number)
		*number -= stepsize;

	sprintf(tmp,"%f",*number);
	string_entry.SetString(tmp);

	SetDirty(EW_DIRTY_COMPLETE);
	}



/*
 *						Some Display Functions 
 */

/******************************************************************************
void	wdsDfUpArrow(EW_Window *window,long ix,long iy,long sx,long sy,
																void *dummy)

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void wdsDfUpArrow(EW_Window *window,long ix,long iy,long sx,long sy,void *dummy)
	{
	long vert[3][2];

	vert[0][0]=(long)(ix+sx*0.2);
	vert[0][1]=(long)(iy+sy*0.2);

	vert[1][0]=(long)(ix+sx*0.8);
	vert[1][1]=(long)(iy+sy*0.2);

	vert[2][0]=(long)(ix+sx*0.5);
	vert[2][1]=(long)(iy+sy*0.8);

	window->TriangleFill(vert[0],vert[1],vert[2]);
	}



/******************************************************************************
void	wdsDfDownArrow(EW_Window *window,long ix,long iy,long sx,long sy,
																void *dummy)

Keywords:
******************************************************************************/
/*PROTOTYPE*/
void wdsDfDownArrow(EW_Window *window,long ix,long iy,long sx,long sy,void *dummy)
	{
	long vert[3][2];

	vert[0][0]=(long)(ix+sx*0.2);
	vert[0][1]=(long)(iy+sy*0.8);

	vert[1][0]=(long)(ix+sx*0.8);
	vert[1][1]=(long)(iy+sy*0.8);

	vert[2][0]=(long)(ix+sx*0.5);
	vert[2][1]=(long)(iy+sy*0.2);

	window->TriangleFill(vert[0],vert[1],vert[2]);
	}

