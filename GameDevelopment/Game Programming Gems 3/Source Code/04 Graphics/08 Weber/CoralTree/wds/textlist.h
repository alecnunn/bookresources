/******************************************************************************

Coral Tree wds/textlist.h
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


// WDS_TextList::SetFlags(?)
#define	WDS_TEXTLIST_NULL					0
#define	WDS_TEXTLIST_UNEDITABLE_GRAB_KEYBD	1

// WDS_TextList::SetEditable(?)
#define WDS_TEXTLIST_EDITABLE_NONE     0
#define WDS_TEXTLIST_EDITABLE_ENTRY    1
#define WDS_TEXTLIST_EDITABLE_HEADER   2


/******************************************************************************
class WDS_StringList : public GPL_DoubleList<WDS_String>

Keywords:
******************************************************************************/
class WDS_StringList : public GPL_DoubleList<WDS_String>
	{
	public:
		WDS_StringList(void)	{};
	};



/******************************************************************************
class	WDS_TextList_GetString: public WDS_GetString

	for independent look storage

Keywords:
******************************************************************************/
class WDS_TextList_GetString: public WDS_GetString
	{
	WDS_DECLARE_TYPE(WDS_TextList_GetString);

	public:
						WDS_TextList_GetString(void)
							{
							SetName("TextList_GetString");
							};

		long			Operate(EW_Event *event);
	};



/*****************************************************************************
class WDS_TextList : public WDS_Widget

	state changes:
		0: edit complete
		1: edit start
		2: uneditable select

Keywords:
*****************************************************************************/
class WDS_TextList : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_TextList);

	private:

		WDS_TextList_GetString	stringwidget;
		WDS_StringList			*stringlist;
		WDS_StringList			defaultlist;

		long			drawoffset;
		long			drawlength;

		long			selected_number;
		long			last_selected;	// for smart redraw
		long			shielded;
		float			offset;
		long			editable;
		long			flags;

		long			NumberOfLocation(long mousey);
		long			LocationOfNumber(long number);
		void			AdjustStringWidget(void);
		void			TransferString(long done);
		void			TakeKeyboard(long on);

	public:
						WDS_TextList();

		void			Draw(EW_Redraw *redraw);
		long			Resize(EW_Resize *resize);
		long			Operate(EW_Event *event);

		void			SetStringList(WDS_StringList *set)	{ stringlist=set; };
		WDS_StringList	*GetStringList(void)			{ return stringlist; };	

		long			GetSelectedNumber(void)		{ return selected_number; };
		void			SetSelectedNumber(long set);
		char			*GetSelectedString(void);

		void			GetAlteredGeometry(long *ax,long *ay,
														long *asx,long *asy);

		void			SetFlags(long set)		{ flags=set; };
		long			GetFlags(void)			{ return flags; };

		void			SetDrawOffset(long set)	{ drawoffset=set; };
		long			GetDrawOffset(void)		{ return drawoffset; };

		void			SetDrawLength(long set)	{ drawlength=set; };
		long			GetDrawLength(void)		{ return drawlength; };

		void			AlterRowHighlight(long row,long on);

		void			SetEditable(long set)	{ editable=set; };
		long			GetEditable(void)		{ return editable; };

		void			SetShielded(long set)	{ shielded=set; };
		long			GetShielded(void)		{ return shielded; };

		void			SetOffset(float set)	{ offset=set; };
		float			GetOffset(void)			{ return offset; };

		long			GetHeaderHeight(void);
		long			GetEntryHeight(void);
		long			GetHeight(long state);

		long			IsEditable(long index);

static	void			StringDeActivationCallback(WDS_Activation *activation);

virtual	void			SuggestPrefSize(long prefx,long prefy);

	};
