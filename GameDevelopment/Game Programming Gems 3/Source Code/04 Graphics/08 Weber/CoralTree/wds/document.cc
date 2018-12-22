/******************************************************************************

Coral Tree wds/document.cc
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
		WDS_Paragraph::WDS_Paragraph(void)

******************************************************************************/
WDS_Paragraph::WDS_Paragraph(void)
	{
	SetName("Paragraph");

	SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
	SetBounds(EW_HORIZONTAL,EW_BOUNDS_FILL_PARENT);
	}



/******************************************************************************
long	WDS_Paragraph::Operate(EW_Event *event)

******************************************************************************/
long WDS_Paragraph::Operate(EW_Event *event)
	{
	long used=event->GetUsed();

	if(!used && event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_RETURN,EW_STATE_PRESS))
		SplitParagraph(event);
	else
		{
		long len=GetString()->GetLength();
		long pos=cursor;

		long result;
		result=WDS_GetString::Operate(event);

		// if editing and on real key press
		if( !used && GetWidgetState() && event->IsKeyTyped(EW_KEY_ALL) && !(event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS) )
			{
			if(len==GetString()->GetLength())
				switch(event->GetItem())
					{
					case EW_KEY_DELETE:
						MergeParagraph(FALSE,event);
						break;
					}

			if(pos==cursor)
				switch(event->GetItem())
					{
					case EW_KEY_BACKSPACE:
						MergeParagraph(TRUE,event);
						break;

					case EW_KEY_CURSOR_UP:
						SelectParagraph(TRUE,TRUE,event);
						break;

					case EW_KEY_CURSOR_LEFT:
						SelectParagraph(TRUE,FALSE,event);
						break;

					case EW_KEY_CURSOR_DOWN:
						SelectParagraph(FALSE,TRUE,event);
						break;

					case EW_KEY_CURSOR_RIGHT:
						SelectParagraph(FALSE,FALSE,event);
						break;
					}
			}
		}

	return FALSE;
	}



/******************************************************************************
void	WDS_Paragraph::SelectParagraph(long previous,long align,EW_Event *event)

******************************************************************************/
void WDS_Paragraph::SelectParagraph(long previous,long align,EW_Event *event)
	{
	WDS_Paragraph *para=FindParagraph(previous);

	if(para==NULL)
		return;

	long x,y,sx,sy;
	long mx,my;

	GetRegion(FALSE,&x,&y,&sx,&sy);
	mx=x+GetString()->GetLastCursorX();

//	printf("%d,%d %d,%d\n",x,y,sx,sy);

	para->GetRegion(FALSE,&x,&y,&sx,&sy);
	my=y+4;

//	printf("%d,%d %d,%d\n",x,y,sx,sy);

	long pos= (previous)? para->GetString()->GetLength(): 0;

	para->StartEdit(pos,event);
	para->SetDirty(EW_DIRTY_COMPLETE);

	if(align)
		{
		long justification=0;	// ???

		long ctr=para->GetString()->Operate(GetRootWindow(),x,y,sx,sy,mx,my,justification|WDS_STRING_HALF_SHIFT);

//		printf("%d,%d ctr=%d\n",mx,my,ctr);

		if(ctr >= 0)
			{
			para->selecting = TRUE;
			para->cursor=ctr;
			para->selection=ctr;
			para->range_mark=ctr;
			}
		}
	}



/******************************************************************************
void	WDS_Paragraph::SplitParagraph(EW_Event *event)

******************************************************************************/
void WDS_Paragraph::SplitParagraph(EW_Event *event)
	{
//	printf("split (%s) (%s)\n",GetString()->Buffer(),GetString()->Buffer());

	long pos=GetString()->GetCursorPosition();
//	printf("pos=%d/%d\n",pos,GetString()->GetLength());
	if(pos<0)
		return;

	WDS_Widget *parent=(WDS_Widget *)GetParentWidget();
//	printf("parent=0x%x\n",parent);
	if(parent==NULL)
		return;

	StopEdit(FALSE,event);

	char *string=GetString()->Buffer();

	WDS_Paragraph *para=new WDS_Paragraph;

	// copy data after cursor
	para->GetString()->NewBuffer(&string[pos]);

	// clip data after cursor
	GetString()->NewLength(pos);

	parent->GetWidgetList()->SetCurrent(this);
	parent->AddWidget(para,EW_ADD_AFTER);

	para->StartEdit(0,event);
	}



/******************************************************************************
void	WDS_Paragraph::MergeParagraph(long previous,EW_Event *event)

******************************************************************************/
void WDS_Paragraph::MergeParagraph(long previous,EW_Event *event)
	{
//	printf("merge previous=%d (%s) (%s)\n",previous,GetString()->Buffer(),GetString()->Buffer());

	WDS_Widget *parent=(WDS_Widget *)GetParentWidget();
//	printf("parent=0x%x\n",parent);
	if(parent==NULL)
		return;

	EW_WidgetList *siblings=parent->GetWidgetList();

	siblings->SetCurrent(this);

	WDS_Paragraph *para=NULL;

	para=FindParagraph(previous);

//	printf("para=0x%x\n",para);

	if(para==NULL)
		return;

	StopEdit(FALSE,event);

	long pos= (previous)? 0: GetString()->GetLength();

//	printf("insert (%s) in (%s) pos %d\n",para->GetString()->Buffer(),GetString()->Buffer());

	GetString()->InsertBuffer(pos,para->GetString()->Buffer());

//	printf("results in (%s)\n",GetString()->Buffer());

	parent->RemoveWidget(para);

	StartEdit(pos,event);
	}



/******************************************************************************
WDS_Paragraph	*WDS_Paragraph::FindParagraph(long previous)

	find nearest sibling paragraph, previous or after

******************************************************************************/
WDS_Paragraph *WDS_Paragraph::FindParagraph(long previous)
	{
	WDS_Widget *parent=(WDS_Widget *)GetParentWidget();
	if(parent==NULL)
		return NULL;

	EW_WidgetList *siblings=parent->GetWidgetList();

	siblings->SetCurrent(this);
	WDS_Widget *node=NULL;

	if(previous)
		while( (node=(WDS_Widget *)siblings->PreDecrement()) != NULL && !node->WDS_WIDGET_IS(WDS_Paragraph) );
	else
		while( (node=(WDS_Widget *)siblings->PreIncrement()) != NULL && !node->WDS_WIDGET_IS(WDS_Paragraph) );

	return (WDS_Paragraph *)node;
	}



/******************************************************************************
		WDS_DocumentForm::WDS_DocumentForm(void)

******************************************************************************/
WDS_DocumentForm::WDS_DocumentForm(void)
	{
	SetName("DocumentForm");
	}



/******************************************************************************
		WDS_Document::WDS_Document(void)

******************************************************************************/
WDS_Document::WDS_Document(void)
	{
	SetName("Document");
	SetCompound(TRUE);

	form.SetSizing(EW_HORIZONTAL,WDS_SIZING_MANUAL);
	form.SetMode(WDS_FORM_COLUMN);

	AddWidget(&form);
	form.AddWidget(new WDS_Paragraph);
	}



/******************************************************************************
void	WDS_Paragraph::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_Paragraph::Draw(EW_Redraw *redraw)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Paragraph::Draw() force=%d dirty=%d",redraw->GetForce(),GetDirty());

	WDS_GetString::Draw(redraw);
	}



/******************************************************************************
void	WDS_DocumentForm::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_DocumentForm::Draw(EW_Redraw *redraw)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_DocumentForm::Draw() force=%d dirty=%d",redraw->GetForce(),GetDirty());

	WDS_Form::Draw(redraw);
	}



/******************************************************************************
void	WDS_Document::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_Document::Draw(EW_Redraw *redraw)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Document::Draw() force=%d dirty=%d",redraw->GetForce(),GetDirty());

	WDS_ScrollRegion::Draw(redraw);
	}
