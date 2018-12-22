/******************************************************************************

Coral Tree wds/table.cc
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
		WDS_TablePartition::WDS_TablePartition(void)

******************************************************************************/
WDS_TablePartition::WDS_TablePartition(void)
	{
	SetName("TablePartition");

	SetLabels(0);
	SetLabelOffset(0);
	SetLabelLength(-1);
	SetSmartClear(TRUE);
	};



/******************************************************************************
void	WDS_TablePartition::Draw(EW_Redraw *redraw)

	cleanup after WDS_Partition::Draw()
	for rowlabel offset and length

Keywords:
******************************************************************************/
void WDS_TablePartition::Draw(EW_Redraw *redraw)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TablePartition::Draw()");

	long colorindex=InterpretElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND);

	long pos[2],size[2];
	long npos[2],nsize[2];
	long ty1,ty2,by1,by2;

	GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);

	ty2=pos[1]+size[1]-1;
	ty1=ty2 - GetLabelOffset();

	by1=pos[1];
	by2=by1+size[1]-GetLabelOffset()-GetLabelLength()-1;

	long m=0;
	EW_Widget *node;

	if( (GetDirty()&EW_DIRTY_COMPLETE) || redraw->GetForce() )
		{
		// clear background
		GetRootWindow()->RectFill(pos[0],pos[1],pos[0]+size[0]-1,pos[1]+size[1]-1,colorindex);
		}

	WDS_Partition::Draw(redraw);
	}



/******************************************************************************
long	WDS_TablePartition::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_TablePartition::Resize(EW_Resize *resize)
	{
	EW_Widget *node=GetWidgetList()->ToHead();

	long opos[2],osize[2];
	long pos[2],size[2];

	if(node)
		node->GetGeometry(&opos[0],&opos[1],&osize[0],&osize[1]);

	long m=0;

	GetWidgetList()->ToHead();
	while( m<GetLabels() && (node=GetWidgetList()->PostIncrement()) != NULL)
		{
		if( ((WDS_Widget *)node)->WDS_WIDGET_IS(WDS_TextList) )
			{
			((WDS_TextList *)node)->SetDrawOffset(GetLabelOffset());
			((WDS_TextList *)node)->SetDrawLength(GetLabelLength());
			}

//		node->CalcTruePosition(TRUE);

		m++;
		};

	// remaining textlists other than row labels
	while( (node=GetWidgetList()->PostIncrement()) != NULL)
		{
		if( ((WDS_Widget *)node)->WDS_WIDGET_IS(WDS_TextList) )
			{
			((WDS_TextList *)node)->SetDrawOffset(0);
			((WDS_TextList *)node)->SetDrawLength(0);
			}
		}

	WDS_Partition::Resize(resize);

	return FALSE;
	}



/******************************************************************************
		WDS_Table::WDS_Table(void)

Keywords:
******************************************************************************/
WDS_Table::WDS_Table(void)
	{
	SetName("Table");

	WDS_Activation activation;
	activation.SetTargetWidget(this);

	activation.SetCode(0);
	scroll_bar.SetStateChangeCallback(WDS_STATE_0,&WDS_Table::ScrollDeActivationCallback,&activation);

	scroll_region.SetName("Table SR");
	scroll_bar.SetName("Table SB");

	table_entry_list=NULL;
	textlist=NULL;
	textlists=0;
	allselecting=FALSE;

	SetLastEditIndices(-1,-1);

	SetHeaderLabels(0);
	SetRowLabels(0);
	SetClipping(EW_CLIPPING_CHILDREN);
	SetAllSelect(WDS_TABLE_ALLSELECT_NONE);
	SetEditable(WDS_TABLE_EDITABLE_NONE);
	SetCompound(TRUE);
	SetRetainLimits(TRUE);

	AddSlaves();
	}
	


/******************************************************************************
		WDS_Table::~WDS_Table(void)

Keywords:
******************************************************************************/
WDS_Table::~WDS_Table(void)
	{
	DeleteColumns();
	RemoveSlaves();
	}



/******************************************************************************
void	WDS_Table::TextListDeActivationCallback(WDS_Activation *activation)

******************************************************************************/
void WDS_Table::TextListDeActivationCallback(WDS_Activation *activation)
	{
	WDS_TextList *thislist=(WDS_TextList *)activation->GetThisWidget();
	WDS_Table *table=(WDS_Table *)activation->GetTargetWidget();

	// prevent loops
	if(table->allselecting)
		return;

	long code=activation->GetCode();
	long newstate=activation->GetState();
	long row,column;
	long m;

//	printf("TextListDeActivationCallback() code=%d newstate=%d\n",code,newstate);

#if FALSE
	if(newstate)
		table->FindLastEditIndices();
#else
	if(newstate)
		table->SetLastColumn(thislist);
#endif

	table->GetLastEditIndices(&row,&column);

//	printf("row=%d column=%d\n",row,column);

	if(table->GetAllSelect()&WDS_TABLE_ALLSELECT_ROW)
		table->SelectRow(row);

	table->ChangeState(newstate,activation->GetEvent());

//	activation->GetEvent()->Print(0);
	}



/******************************************************************************
void	WDS_Table::SelectRow(long row)

	cooperates with WDS_Table::TextListDeActivationCallback() to prevent loops

******************************************************************************/
void WDS_Table::SelectRow(long row)
	{
	allselecting=TRUE;

	long m;
	for(m=0;m<textlists;m++)
		if(!textlist[m]->IsEditable(row))
			textlist[m]->SetSelectedNumber(row);
		else if(!textlist[m]->IsEditable(textlist[m]->GetSelectedNumber()))
			textlist[m]->SetSelectedNumber(-1);


	allselecting=FALSE;
	}



/******************************************************************************
void	WDS_Table::AlterRowHighlight(long row,long on)

******************************************************************************/
void WDS_Table::AlterRowHighlight(long row,long on)
	{
	WDS_String *string;

	long m;
	for(m=0;m<textlists;m++)
		if( (string=GetString(row,m)) != NULL )
			string->SetHighlight(on);
	}



/******************************************************************************
void	WDS_Table::ScrollDeActivationCallback(WDS_Activation *activation)

Keywords:
******************************************************************************/
void WDS_Table::ScrollDeActivationCallback(WDS_Activation *activation)
	{
	WDS_Table *table=(WDS_Table *)activation->GetTargetWidget();

	if(!table->CanSmartDraw())
		table->SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void	WDS_Table::SetLastColumn(WDS_TextList *lastlist)

******************************************************************************/
void WDS_Table::SetLastColumn(WDS_TextList *lastlist)
	{
	long number;
	long m;

	for(m=0;m<textlists;m++)
		{
		if(lastlist==textlist[m])
			{
			SetLastEditIndices(textlist[m]->GetSelectedNumber(),m);
			return;
			}
		}

	SetLastEditIndices(-1,-1);
	}



/******************************************************************************
void	WDS_Table::FindLastEditIndices(void)

******************************************************************************/
void WDS_Table::FindLastEditIndices(void)
	{
	long number;
	long m;

	for(m=0;m<textlists;m++)
		{
		if( (number=textlist[m]->GetSelectedNumber()) >= 0 )
			{
			SetLastEditIndices(number,m);
			return;
			}
		}

	SetLastEditIndices(-1,-1);
	}



/******************************************************************************
long	WDS_Table::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_Table::Resize(EW_Resize *resize)
	{
	EW_Resize new_resize;
	long pref[2];
	long m;

	FindLastEditIndices();

	// resize table itself
	CheckSize(resize);

	// scroll_region loosely resized now, for real again later
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));
	new_resize.GetPrefSize(&pref[0],&pref[1]);
	pref[0]= -1;
	new_resize.SetPrefSize(pref[0],pref[1]);

	// minimal resize of scroll region
	new_resize.SetRecursive(FALSE);
	scroll_region.PreResize(&new_resize);

	// reset slave children
//	RemoveSlaves();
//	AddSlaves();

	// for vert scroll bar, make space in lower right corner
	long width=scroll_region.GetScrollBar(WDS_HORIZONTAL)->InterpretElement(WDS_STATE_0,WDS_ELEMENT_SCROLLBAR_WIDTH);
	long scrollregiondepth=scroll_region.InterpretElement(WDS_STATE_0,WDS_ELEMENT_DEPTH);
	long pos[2],size[2];

	// if horizontal scrollbar not shown, don't account for width
	if(scroll_region.GetScrollBar(WDS_HORIZONTAL)->GetFraction()>=1.0)
		width=0;

	scroll_bar.GetPrefSize(&size[0],&size[1]);
	size[1]=pref[1]-width-scrollregiondepth;
	scroll_bar.SetPrefSize(size[0],size[1]);

	long npos[2];
	long spos[2],ssize[2];
	scroll_region.GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	scroll_region.GetSubGeometry(&spos[0],&spos[1],&ssize[0],&ssize[1]);

/*
	printf("sr %d,%d %d,%d -> %d,%d %d,%d (%s)\n",
			pos[0],pos[1],size[0],size[1],spos[0],spos[1],ssize[0],ssize[1],scroll_region.GetName());
*/

	float old_offset=0.0;
	float new_offset=0.0;

	if(GetTableEntryList() && textlists>1)
		{
		long entries=GetTableEntryList()->GetNumberNodes();
		long entry_height=textlist[0]->GetEntryHeight();

		float visible_lines=ssize[1]/(float)entry_height;

		scroll_bar.SetFraction((visible_lines-GetHeaderLabels())/(float)(entries-GetHeaderLabels()));
		scroll_bar.SetStepSize(1.0/(float)entries);

		old_offset=textlist[0]->GetOffset();
		new_offset=(1.0-scroll_bar.GetFraction()-scroll_bar.GetOffset())*(entries-GetHeaderLabels());

		for(m=0;m<textlists;m++)
			{
			textlist[m]->SetOffset(new_offset);
			textlist[m]->SetShielded(GetHeaderLabels());
			}
		}

	outer_partition.SetLabels(GetRowLabels());
	outer_partition.SetLabelOffset( (pos[1]+size[1]) - (spos[1]+ssize[1] ));
	outer_partition.SetLabelLength( ssize[1] );

	inner_partition.SetPrefSize(-1,ssize[1]);

	long result=EW_Widget::Resize(resize);

	if(CanSmartDraw() && new_offset!=old_offset)
		{
		long srcx,srcy,destx,desty,lenx,leny;

		long esizey=ssize[1]-GetHeaderLabels()*textlist[0]->GetHeaderHeight();

		long apos[2],asize[2];
		GetTrueGeometry(&apos[0],&apos[1],&asize[0],&asize[1]);

		long pass;
		for(pass=0;pass<2;pass++)
			{
			GetRootWindow()->PushClipRegion();
			EW_ClipRegion *clipregion=GetRootWindow()->GetClipStack()->GetCurrentRegion();

			clipregion->DeleteRectangles();

			if(pass==0)
				{
				// scrollable area
				long height=textlist[0]->GetEntryHeight();
				old_offset=(long)(old_offset*height);
				new_offset=(long)(new_offset*height);

				srcx=spos[0];
				srcy=(long)(spos[1]+EW_MAX(0,old_offset-new_offset));

				destx=spos[0];
				desty=(long)(spos[1]+EW_MAX(0,new_offset-old_offset));

				lenx=ssize[0];
				leny=(long)(esizey-abs((long)(new_offset-old_offset)));

				LimitDimensionsToScreen(&srcx,&srcy,&destx,&desty,&lenx,&leny);

				if(leny<1)
					{
					leny=0;
					srcy=spos[1];
					desty=srcy;
					}

//				GetRootWindow()->ImageCopy(srcx,srcy,destx,desty,lenx,leny);

				// do both pieces together, assumes between area is vertically consistant
				long ax,ay,asx,asy;
				scroll_bar.GetTrueGeometry(&ax,&ay,&asx,&asy);
				if(leny>0)
					GetRootWindow()->ImageCopy(apos[0],srcy,apos[0],desty,ax-1-apos[0],leny);

				clipregion->AddRectangle(srcx,spos[1],lenx,desty-spos[1]);						// bottom
				clipregion->AddRectangle(srcx,desty+leny,lenx,esizey+spos[1]-(desty+leny));		// top

				}
			else if(GetRowLabels()>0)
				{
				// unscrollable area (reuses some data from scrollable area)
				long ax,ay,asx,asy;
				textlist[EW_MIN(GetRowLabels(),textlists)-1]->GetTrueGeometry(&ax,&ay,&asx,&asy);

				srcx=apos[0];
				destx=apos[0];
				lenx=ax+asx-destx;

//				GetRootWindow()->ImageCopy(srcx,srcy,destx,desty,lenx,leny);

				clipregion->AddRectangle(srcx,spos[1],lenx,desty-spos[1]);						// bottom
				clipregion->AddRectangle(srcx,desty+leny,lenx,esizey+spos[1]-(desty+leny));		// top
				}

//			GetRootWindow()->IntersectClipRegion(apos[0],apos[1],asize[0],asize[1]);

			// clear and redraw

			clipregion->Prune();
//			clipregion->Draw(GetRootWindow(),InterpretElement(WDS_STATE_0,EW_COLOR_BACKGROUND));

			EW_Redraw redraw;

			redraw.SetMousePosition(-1,-1);
			redraw.SetForce(TRUE);
			redraw.SetLimited(TRUE);

			if(pass==0)
				for(m=GetRowLabels();m<textlists;m++)
					textlist[m]->Redraw(&redraw);
			else
				for(m=0;m<GetRowLabels();m++)
					textlist[m]->Redraw(&redraw);

			GetRootWindow()->PopClipRegion();
			}
		}

	return result;
	}



/******************************************************************************
void	WDS_Table::InsertRowsBefore(long at,WDS_TableEntryList *newrows)

	for at<0, add from end

	number of rows implied by length of newrows

******************************************************************************/
void WDS_Table::InsertRowsBefore(long at,WDS_TableEntryList *newrows)
	{
	WDS_TableEntry *entry;

	if(at<0)
		table_entry_list->ToTail();
	else
		{
		table_entry_list->ToIndex(at);

		// backup to insert after (so it's like inserting before following)
		table_entry_list->PostDecrement();
		}

	newrows->ToHead();
	while( (entry=newrows->PostIncrement()) != NULL)
		{
		newrows->RemoveNode(entry);
		table_entry_list->InsertAfter(entry);
		}

	// a more specific rebuild could be much faster
	RebuildColumns();
	}



/******************************************************************************
void	WDS_Table::MoveRows(long at,long rows,long to)

******************************************************************************/
void WDS_Table::MoveRows(long at,long rows,long to)
	{
	WDS_TableEntry *entry;

	// if move to within self, don't bother
	if(to>=at && to<at+rows)
		return;

	long moveback= (to<at);

	long row;
	for(row=0;row<rows;row++)
		{
		if(moveback)
			entry=table_entry_list->ToIndex(at+row);
		else
			entry=table_entry_list->ToIndex(at);

		if(!table_entry_list->RemoveNode(entry))
			{
			EW_PRINT(EW_WIDGET,EW_ERROR,"WDS_Table::MoveRows() can't remove old row");
			return;
			}
		
		if(moveback)
			table_entry_list->ToIndex(to+row);
		else
			table_entry_list->ToIndex(to-1);

		if(!table_entry_list->InsertBefore(entry))
			{
			EW_PRINT(EW_WIDGET,EW_ERROR,"WDS_Table::MoveRows() can't insert row (lost)");
			return;
			}
		}
	}



/******************************************************************************
void WDS_Table::DeleteRows(long at,long rows)

	for at<0, delete from end

******************************************************************************/
void WDS_Table::DeleteRows(long at,long rows)
	{
	WDS_TableEntry *entry;
	long m=0;

	if(at<0)
		table_entry_list->ToTail();
	else
		table_entry_list->ToIndex(at);

	m=0;
	while( m++<rows && (entry=table_entry_list->Current()) != NULL)
		table_entry_list->RemoveNode(entry);

	// a more specific rebuild could be much faster
	RebuildColumns();
	}



/******************************************************************************
void	WDS_Table::AddSlaves(void)

Keywords:
******************************************************************************/
void WDS_Table::AddSlaves(void)
	{
	WDS_Activation activation;
	activation.SetTargetWidget(this);

	long m;

	AddWidget(&outer_partition,EW_ADD_BEGINNING);

	char message[128];
	for(m=0;m<textlists;m++)
		{
		sprintf(message,"Table TL %d",m);
		textlist[m]->SetName(message);
		textlist[m]->SetFlags(WDS_TEXTLIST_UNEDITABLE_GRAB_KEYBD);

		activation.SetCode(m);
		textlist[m]->SetStateChangeCallback(WDS_STATE_ALL,&WDS_Table::TextListDeActivationCallback,&activation);
		}

	long listedit;

	listedit=	(GetEditable()&WDS_TABLE_EDITABLE_HEADER)? WDS_TEXTLIST_EDITABLE_HEADER: WDS_TEXTLIST_EDITABLE_NONE;
	listedit|=	(GetEditable()&WDS_TABLE_EDITABLE_ROWLABEL)? WDS_TEXTLIST_EDITABLE_ENTRY: 0;

	//	Row Labels
	for(m=0;m<textlists && m<GetRowLabels();m++)
		{
		textlist[m]->SetEditable(listedit);
		outer_partition.AddWidget(textlist[m],m? EW_ADD_AFTER: EW_ADD_BEGINNING);
		}

	outer_partition.AddWidget(&scroll_region,EW_ADD_AFTER);
	outer_partition.AddWidget(&scroll_bar,EW_ADD_AFTER);
	outer_partition.SetFlags(WDS_PARTITION_FLAGS_ATTACH);
	outer_partition.SetName("outer part");

	scroll_region.AddWidget(&inner_partition,EW_ADD_BEGINNING);
	scroll_region.SetLocalElement(WDS_STATE_0,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,
																			WDS_SCROLLABLE_VISIBLE);
//																			WDS_SCROLLABLE_VISIBLE|WDS_SCROLLABLE_PERSISTANT);
	scroll_region.SetLocalElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_OFF);

	inner_partition.SetName("inner part");
	inner_partition.SetFlags(WDS_PARTITION_FLAGS_NONE);

	listedit=	(GetEditable()&WDS_TABLE_EDITABLE_HEADER)? WDS_TEXTLIST_EDITABLE_HEADER: WDS_TEXTLIST_EDITABLE_NONE;
	listedit|=	(GetEditable()&WDS_TABLE_EDITABLE_ENTRY)? WDS_TEXTLIST_EDITABLE_ENTRY: 0;

	// non-Row-Label columns
	for(m=GetRowLabels();m<textlists;m++)
		{
		textlist[m]->SetEditable(listedit);
		inner_partition.AddWidget(textlist[m],m? EW_ADD_AFTER: EW_ADD_BEGINNING);
		}

	scroll_bar.SetDirection(WDS_VERTICAL);
//	scroll_bar.SetBounds(EW_VERTICAL,EW_BOUNDS_FILL_PARENT);
	scroll_bar.SetFraction(0.1);
	}



/******************************************************************************
void	WDS_Table::RemoveSlaves(void)

Keywords:
******************************************************************************/
void WDS_Table::RemoveSlaves(void)
	{
	long m;
	for(m=0;m<GetRowLabels() && m<textlists;m++)
		outer_partition.RemoveWidget(textlist[m]);
	for(m=GetRowLabels();m<textlists;m++)
		inner_partition.RemoveWidget(textlist[m]);

	scroll_region.RemoveWidget(&inner_partition);
	outer_partition.RemoveWidget(&scroll_bar);
	outer_partition.RemoveWidget(&scroll_region);
	RemoveWidget(&outer_partition);
	}



/******************************************************************************
void	WDS_Table::DeleteColumns(void)

Keywords:
******************************************************************************/
void WDS_Table::DeleteColumns(void)
	{
	WDS_StringList *stringlist;
	long m;

	// clean up all old WDS_TextList widgets
	for(m=0;m<textlists;m++)
		{
		stringlist=textlist[m]->GetStringList();

		textlist[m]->SetStringList(NULL);

		delete stringlist;
		}

	for(m=0;m<textlists;m++)
		delete textlist[m];

	if(textlist)
		{
		osdFree(textlist);
		textlist=NULL;
		}
	else
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Table::DeleteColumns() textlist already NULL");

	textlists=0;
	}



/******************************************************************************
void	WDS_Table::RebuildColumns(void)

Keywords:
******************************************************************************/
void WDS_Table::RebuildColumns(void)
	{
	WDS_TableEntry *table_entry;
	WDS_StringList *tablestringlist;	// horizontal
	WDS_StringList *textstringlist;		// vertical
	WDS_String *table_string;			// horizontal
	WDS_String *text_string;			// vertical

	long column;

	RemoveSlaves();
	DeleteColumns();

	// get new number of columns based on number of strings in first entry
	if(GetTableEntryList())
		{
		WDS_TableEntry *table_entry=GetTableEntryList()->ToHead();

		if(table_entry)
			{
			tablestringlist=table_entry->GetStringList();

			if(tablestringlist)
				textlists=tablestringlist->GetNumberNodes();
			}
		}

	if(textlists==0)
		return;

	textlist=(WDS_TextList **)osdMalloc(textlists*sizeof(WDS_TextList *));

	// create/assign string lists
	for(column=0;column<textlists;column++)
		{
		textlist[column]=new WDS_TextList;

		textstringlist=new WDS_StringList;

		textlist[column]->SetStringList(textstringlist);
		}

	long first_row=TRUE;

	// fill in string lists
	GetTableEntryList()->ToHead();
	while( (table_entry=GetTableEntryList()->PostIncrement()) != NULL)
		{
		tablestringlist=table_entry->GetStringList();

		tablestringlist->ToHead();
		for(column=0;column<textlists;column++)
			{
			// can be NULL
			table_string=tablestringlist->PostIncrement();

			textstringlist=textlist[column]->GetStringList();

			// non-intrusive nodes can exist on multiple lists
			text_string=table_string;

			textstringlist->AppendNode(text_string);

			if(first_row)
				{
				long pref[2];
				textlist[column]->GetPrefSize(&pref[0],&pref[1]);

				// autoset column widths to string widths of first line
				if(pref[0]<0)
					{
					if(GetRootWindow())
						pref[0]=GetRootWindow()->StringWidth(text_string->Buffer())+8;
					else
						pref[0]= 8 * text_string->GetLength();

					textlist[column]->SuggestPrefSize(pref[0],pref[1]);
					}
				}
			}

		first_row=FALSE;
		}

	AddSlaves();
	}



/******************************************************************************
void	WDS_TableEntryList::Dump(long code)

Keywords:
******************************************************************************/
void WDS_TableEntryList::Dump(long code)
	{
    WDS_TableEntry		*table_entry;
    WDS_StringList		*stringlist;
    WDS_String			*string;

	char message[4096];

	sprintf(message,"WDS_TableEntryList::Dump(0x%x)",code);
	EW_PRINT(EW_WIDGET,EW_LOG,message);

	ToHead();
	while( (table_entry=PostIncrement()) != NULL)
		{
		message[0]=0;

		stringlist=table_entry->GetStringList();

		stringlist->ToHead();
		while( (string=stringlist->PostIncrement()) != NULL)
			{
			strcat(message,"(");
			strcat(message,string->Buffer());
			strcat(message,") ");
			}

		EW_PRINT(EW_WIDGET,EW_LOG,message);
		}
	}
