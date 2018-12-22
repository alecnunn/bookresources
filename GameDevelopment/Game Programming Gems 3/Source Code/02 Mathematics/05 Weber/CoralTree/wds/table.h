/******************************************************************************

Coral Tree wds/table.h
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


// WDS_Table::SetAllSelect(?)
#define WDS_TABLE_ALLSELECT_NONE	0
#define WDS_TABLE_ALLSELECT_ROW		1

// WDS_Table::SetEditable(?)
#define WDS_TABLE_EDITABLE_NONE		0
#define WDS_TABLE_EDITABLE_ENTRY	1
#define WDS_TABLE_EDITABLE_HEADER	2
#define WDS_TABLE_EDITABLE_ROWLABEL	4
#define WDS_TABLE_EDITABLE_ALL		7



/******************************************************************************
class	WDS_TablePartition : public WDS_TablePartition

	for independent look storage

******************************************************************************/
class WDS_TablePartition : public WDS_Partition
	{
	WDS_DECLARE_TYPE(WDS_TablePartition);

		long labels;	// number of children from front to offset
		long offset;	// amount to slide rowlabel lists down
		long length;	// max length of rowlabel lists

	public:
						WDS_TablePartition(void);

		void			SetLabels(long set)			{ labels=set; };
		long			GetLabels(void)				{ return labels; };

		void			SetLabelOffset(long set)	{ offset=set; };
		long			GetLabelOffset(void)		{ return offset; };

		void			SetLabelLength(long set)	{ length=set; };
		long			GetLabelLength(void)		{ return length; };

virtual	long			Resize(EW_Resize *resize);
virtual	void			Draw(EW_Redraw *redraw);
	};



/******************************************************************************
class	WDS_TableEntry : public GPL_DoubleNode

Keywords:
******************************************************************************/
class WDS_TableEntry : public GPL_DoubleNode
	{
	private:

		WDS_String		*rowlabel;
		WDS_StringList	*stringlist;

	public:

						WDS_TableEntry(void)
							{
							rowlabel=NULL;
							stringlist=NULL;
							};

		WDS_StringList	*GetStringList(void)
							{ return stringlist; };
		void			SetStringList(WDS_StringList *set)
							{ stringlist=set; };
	};



/******************************************************************************
class	WDS_TableEntryList : public GPL_IDoubleList<WDS_TableEntry>

Keywords:
******************************************************************************/
class WDS_TableEntryList : public GPL_IDoubleList<WDS_TableEntry>
	{
	public:

				WDS_TableEntryList(void)	{};

		void	Dump(long code);
	};



/*****************************************************************************
class	WDS_Table : public WDS_Widget

	echo state changes from WDS_TextList
	use GetLastEditIndices() to find out what changed

*****************************************************************************/
class WDS_Table : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Table);

	private:

		// slave children
		WDS_TablePartition	outer_partition,inner_partition;
		WDS_ScrollRegion	scroll_region;
		WDS_ScrollBar		scroll_bar;

		WDS_TextList		**textlist;			// columns (dynamic array)
		WDS_TableEntryList	*table_entry_list;	// rows

		long				allselect;
		long				allselecting;
		long				editable;
		long				textlists;
		long				rowlabels;
		long				headerlabels;
		long				lastrow,lastcolumn;

		void	AddSlaves(void);
		void	RemoveSlaves(void);

		void	FindLastEditIndices(void);
		void	SetLastColumn(WDS_TextList *lastlist);
		void	SetLastEditIndices(long row,long column)
					{
					lastrow=row;
					lastcolumn=column;
					};

	public:
				WDS_Table();
virtual			~WDS_Table();

		long	Resize(EW_Resize *resize);

		void	SetEditable(long set)		{ editable=set; };
		long	GetEditable(void)			{ return editable; };

		void	SetAllSelect(long set)		{ allselect=set; };
		long	GetAllSelect(void)			{ return allselect; };

		void	AlterRowHighlight(long row,long on);

		void	SetHeaderLabels(long set)	{ headerlabels=set; };
		long	GetHeaderLabels(void)		{ return headerlabels; };
		void	SetRowLabels(long set)		{ rowlabels=set; };
		long	GetRowLabels(void)			{ return rowlabels; };

		void	GetLastEditIndices(long *row,long *column)
					{
					*row=lastrow;
					*column=lastcolumn;
					};

		WDS_String	*GetString(long row,long column)
						{
						return table_entry_list->ToIndex(row)->
											GetStringList()->ToIndex(column);
						};

		void	DeleteColumns(void);
		void	RebuildColumns(void);

		void	InsertRowsBefore(long at,WDS_TableEntryList *newrows);
		void	MoveRows(long at,long rows,long to);
		void	DeleteRows(long at,long rows);
		void	SelectRow(long row);

static	void	TextListDeActivationCallback(WDS_Activation *activation);
static	void	ScrollDeActivationCallback(WDS_Activation *activation);

		WDS_TableEntryList *GetTableEntryList(void)
								{ return table_entry_list; };
		void				SetTableEntryList(WDS_TableEntryList *set)
								{ table_entry_list=set; };
	};
