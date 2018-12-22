/******************************************************************************

Coral Tree wds/initlook.cc
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


#define WDS_MENU_COLOR				TRUE 	// colorize borderless menu widgets
#define WDS_ELEMENT_INIT_DUMP		FALSE
#define WDS_ELEMENT_CALLBACK_DEBUG	FALSE

#define WDS_DEFAULT_BEVEL_DEPTH	 4   // default beveling depth in pixels
										// multiple of 2 much prefered

#define WDS_DEFAULT_BEVEL_INSET	 0   // default beveling inset in pixels

// default beveling modes
#define WDS_DEFAULT_BEVEL_ACTIVE	WDS_BEVELBOX_IN
#define WDS_DEFAULT_BEVEL_INACTIVE  WDS_BEVELBOX_OUT



/******************************************************************************
void	WDS_Looks::Initialize(long set,WDS_Looks *default_looks)

	if default_looks is not NULL, it supplies a potential source for more
	initialization callbacks

	no longer supports multiple built-in looks
	Win32 mode retained and updated

Keywords:
******************************************************************************/
void WDS_Looks::Initialize(long set,WDS_Looks *default_looks)
	{
	WDS_Look look;

	if(set!=WDS_LOOK_WIN32)
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Looks::Initialize() only supports WDS_LOOK_WIN32, use WDS_Looks::LoadFromFile()");

//	looklist.Clear();

//	printf("WDS_Looks::Initialize(%d,0x%x)\n",set,default_looks);


	// general defaults
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_PEN,						EW_BLACK);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_LIGHTGREY);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_BACKGROUND,				EW_LIGHTGREY);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_LIT,					EW_LIGHTGREY);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_SHADOW,				EW_DARKGREY);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					EW_BLACK);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_DEFAULT_BEVEL_INACTIVE);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_DEFAULT_BEVEL_ACTIVE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					WDS_DEFAULT_BEVEL_DEPTH);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					WDS_DEFAULT_BEVEL_INSET);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					2);
	RegisterLook(WDS_TYPE_DEFAULT,WDS_Widget::GetClassIdStatic(),"WDS_Widget",&look);

	// WDS_Button
	// state 0: inactive
	// state 1: being pressed
	// state 2: locked
	// state 3,4,5: bevel figures for above
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BUTTON_SENSE_ANNOTATION,TRUE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				3);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_1,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_LABEL_X,				1);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_LABEL_Y,				-1);
	WDS_REGISTER_LOOK(WDS_Button,&look);

	// WDS_ToggleButton
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Button::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_ANNOTATION_MODE,		WDS_WIDGET_ANNOTATION_RIGHT|
																				WDS_WIDGET_ANNOTATION_SQUARE_WIDGET);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_ANNOTATION_X,			4);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_ANNOTATION_Y,			13);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_CHECKMARK);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_3,		EW_COLOR_WIDGET,					EW_BLACK);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_INSET,					3);
	WDS_REGISTER_LOOK(WDS_ToggleButton,&look);

	// WDS_RadioButton
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_ToggleButton::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_ANNOTATION_Y,			12);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH|
																				WDS_BEVELBOX_ELLIPTICAL);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH|
																				WDS_BEVELBOX_ELLIPTICAL);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_ELLIPTICAL);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_INSET,					4);
	WDS_REGISTER_LOOK(WDS_RadioButton,&look);

#define WDS_HIER_WIN32_LINE	EW_DARKGREY

	// WDS_HierarchyToggle
	// state 0/3: unopenable    1/4: closed 2/5: open
	// states 3-5 are bevel figures
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Button::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_PEN,						WDS_HIER_WIN32_LINE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						WDS_HIER_WIN32_LINE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					WDS_HIER_WIN32_LINE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PREF_X,					9);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PREF_Y,					9);
	look.SetElement(WDS_STATE_0,		EW_COLOR_WIDGET,					WDS_HIER_WIN32_LINE);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_INSET,					4);
	look.SetElement(WDS_STATE_3,		EW_COLOR_WIDGET,					WDS_HIER_WIN32_LINE);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_4,		EW_COLOR_WIDGET,					EW_BLACK);
	look.SetElement(WDS_STATE_4,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_PLUSMARK);
	look.SetElement(WDS_STATE_4,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_4,		WDS_ELEMENT_INSET,					2);
	look.SetElement(WDS_STATE_5,		EW_COLOR_WIDGET,					EW_BLACK);
	look.SetElement(WDS_STATE_5,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_MINUSMARK);
	look.SetElement(WDS_STATE_5,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_5,		WDS_ELEMENT_INSET,					2);
	WDS_REGISTER_LOOK(WDS_HierarchyToggle,&look);

	// WDS_HierarchyNode
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Button::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_PEN,						EW_BLACK);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_BACKGROUND,				EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_LABEL_Y,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INDENT,					15);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_HNODE_LINES,			TRUE);
	look.SetElement(WDS_STATE_1,		EW_COLOR_PEN,						EW_WHITE);
	look.SetElement(WDS_STATE_1,		EW_COLOR_WIDGET,					EW_BLUE);
	WDS_REGISTER_LOOK(WDS_HierarchyNode,&look);

	// WDS_HierarchyForm
	// state 0: unannotated     1: annotated
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Form::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_BACKGROUND,				EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,			2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_INDENT,			16);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_INDENT_AFTER,		1);
	WDS_REGISTER_LOOK(WDS_HierarchyForm,&look);

	// WDS_Partition
	// state 0: immobile
	// state 1: mobile
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_DEPTH,					1);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_PARTITION_SEPARATOR,	2);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_PARTITION_SEPARATOR,	7);
	look.SetElement(WDS_STATE_1,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PARTITION_KNOB_OFFSET,	-1);
	WDS_REGISTER_LOOK(WDS_Partition,&look);

#define WDS_TABLE_WIN32_BG	EW_LIGHTGREY

	// WDS_TablePartition, modification of WDS_Partition
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Partition::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_BACKGROUND,				WDS_TABLE_WIN32_BG);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_INHERIT|WDS_ELEMENT_MAP|
																									EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PARTITION_SEPARATOR,	0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PARTITION_KNOB_OFFSET,	-1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PARTITION_GRAB_WIDTH,	8);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PARTITION_GRAB_LENGTH,	16);
	WDS_REGISTER_LOOK(WDS_TablePartition,&look);

	// WDS_MFD, modification of WDS_Partition
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Partition::GetTypeCodeStatic());
	WDS_REGISTER_LOOK(WDS_MFD,&look);

	// WDS_TextList
	// state 0: Header Editable
	// state 1: Header Uneditable
	// state 2: Header Uneditable Selected
	// state 3: Entry Editable
	// state 4: Entry Uneditable
	// state 5: Entry Uneditable Selected
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_SMOOTH|WDS_BEVELBOX_OUT|WDS_BEVELBOX_LABEL_LEFT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_LABEL_X,				3);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET_MODY,			1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_TEXTLIST_HIGHLIGHT_BG,	EW_YELLOW);
	look.SetElement(WDS_STATE_5,		WDS_ELEMENT_TEXTLIST_HIGHLIGHT_BG,	EW_DARKGREY);
	look.SetElement(WDS_STATE_5,		EW_COLOR_PEN,						EW_WHITE);

	look.SetElement(WDS_STATE_ALL,		EW_COLOR_BACKGROUND,				WDS_TABLE_WIN32_BG);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_WHITE);
	look.SetElement(WDS_STATE_1,		EW_COLOR_WIDGET,					WDS_ELEMENT_INHERIT|WDS_ELEMENT_MAP|EW_COLOR_WIDGET);
	look.SetElement(WDS_STATE_2,		EW_COLOR_WIDGET,					EW_MEDGREY);
	look.SetElement(WDS_STATE_5,		EW_COLOR_WIDGET,					EW_DARKBLUE);

	look.SetElement(WDS_STATE_5,		EW_COLOR_LIT,						EW_DARKBLUE);

	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_LIT,					EW_WHITE);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_INHERIT|WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_2,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_INHERIT|WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_5,		EW_COLOR_HALF_LIT,					EW_DARKBLUE);

	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_SHADOW,				EW_WHITE);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_INHERIT|WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_2,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_INHERIT|WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_5,		EW_COLOR_HALF_SHADOW,				EW_DARKBLUE);

	look.SetElement(WDS_STATE_0,		EW_COLOR_SHADOW,					EW_DARKGREY);
	look.SetElement(WDS_STATE_3,		EW_COLOR_SHADOW,					EW_LIGHTGREY);
	look.SetElement(WDS_STATE_4,		EW_COLOR_SHADOW,					EW_WHITE);
	look.SetElement(WDS_STATE_5,		EW_COLOR_SHADOW,					EW_DARKBLUE);
	WDS_REGISTER_LOOK(WDS_TextList,&look);

	// WDS_GetString
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_GETSTR_FLAGS,			FALSE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_GETSTR_SELECT_FG,		EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_GETSTR_SELECT_BG,		EW_DARKBLUE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_GETSTR_HIGHLIGHT_BG,	EW_YELLOW);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				3);
	look.SetElement(WDS_STATE_3,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_WIDGET);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_DEPTH,					0);
	WDS_REGISTER_LOOK(WDS_GetString,&look);

	// WDS_TextList_GetString
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_GetString::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					1);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						EW_BLACK);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					EW_BLACK);
	WDS_REGISTER_LOOK(WDS_TextList_GetString,&look);

	// WDS_TipString
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_GetString::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				2);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_YELLOW);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_COLOR_WIDGET_DITHER,	EW_WHITE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						EW_BLACK);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					EW_BLACK);
	WDS_REGISTER_LOOK(WDS_TipString,&look);

	// WDS_Paragraph
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_GetString::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	WDS_REGISTER_LOOK(WDS_Paragraph,&look);

	// WDS_DocumentForm
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Form::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,			1);
	WDS_REGISTER_LOOK(WDS_DocumentForm,&look);

	// WDS_Document
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_ScrollRegion::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_OFF);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_VISIBLE | WDS_SCROLLABLE_PERSISTANT |																					WDS_SCROLLABLE_OPPOSE );
	WDS_REGISTER_LOOK(WDS_Document,&look);

	// WDS_ScrollBarButton (why the inactive lighting is reversed, I don't know)
	look.Reset();
	look.SetElement(WDS_STATE_0,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_0,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_1,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_1,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Button::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_POINT_45);
	look.SetElement(WDS_STATE_2,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_2,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_INSET,					4);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_POINT_45);
	look.SetElement(WDS_STATE_3,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_3,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_INSET,					4);
	WDS_REGISTER_LOOK(WDS_ScrollBarButton,&look);

	// WDS_ScrollBar
	// state 0: horizontal scrollbar
	// state 1: vertical scrollbar
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SCROLLBAR_CHILDREN,			WDS_SCROLLBAR_CHILDREN_SLAVES |
																				WDS_SCROLLBAR_CHILDREN_ALTERNATE |
																				WDS_SCROLLBAR_CHILDREN_INSIDE );
	// state 0: cylinder shaft
	// state 1: piston without fields
	// state 2: piston with fields	(no Win32 equivalent)
	look.SetElement(WDS_STATE_0,		EW_COLOR_WIDGET,					EW_LIGHTGREY);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_COLOR_WIDGET_DITHER,	EW_WHITE);
	look.SetElement(WDS_STATE_1,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_2,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_PEN,						EW_BLUE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SCROLLBAR_WIDTH,		15);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_0,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_0,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_0,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_0,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_1,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_1,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_RIDGED);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_DEPTH,					3);
	WDS_REGISTER_LOOK(WDS_ScrollBar,&look);

	// WDS_ScrollRegion
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	// scrollbar placement
	// state 0: horizontal scrollbar
	// state 1: vertical scrollbar
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_VISIBLE);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_VISIBLE |
																				WDS_SCROLLABLE_OPPOSE);
	// scrollable region
	// state 0: overall widget
	// state 1: subregion less the scrollbars (remember, inset includes state 0 depth)
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_0,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_INSET,					0);
	WDS_REGISTER_LOOK(WDS_ScrollRegion,&look);

	// WDS_Pick
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Popup::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH|
																							WDS_BEVELBOX_LABEL_LEFT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_SHADOW,				WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_LABEL_X,				3);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				3);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PICK_AUTO_OFFSET,		FALSE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PICK_MAINTAIN_BUTTON,	FALSE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PICK_WIN_HEIGHT_MAX,	128);
	WDS_REGISTER_LOOK(WDS_Pick,&look);

	// WDS_PickScroll
	// state 0: overall widget
	// state 1: subregion less the scrollbars (remember, inset includes state 0 depth)
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,		(long)WDS_ScrollRegion::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_IN);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					1);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_DEPTH,					0);
	WDS_REGISTER_LOOK(WDS_PickScroll,&look);

	// WDS_PickList
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_TextList::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS|WDS_BEVELBOX_LABEL_LEFT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_LABEL_X,				3);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_TEXTLIST_IDLE_ECHO,		TRUE);
	WDS_REGISTER_LOOK(WDS_PickList,&look);

	// WDS_PickButton
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,		(long)WDS_ScrollBarButton::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_POINT_DOWN|
																								WDS_BEVELBOX_POINT_45);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					2);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_INSET,					6);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_PICKBUTTON_WIDTH,		-1);
	WDS_REGISTER_LOOK(WDS_PickButton,&look);

	// WDS_PopupBase
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_LIT);
	WDS_REGISTER_LOOK(WDS_PopupBase,&look);

	// WDS_Popup
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_0,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_1,		EW_COLOR_WIDGET,					EW_DARKBLUE);
	WDS_REGISTER_LOOK(WDS_Popup,&look);

	// WDS_MenuButton
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_ScrollBarButton::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_MENUBUTTON_WIDTH,		8);
	look.SetElement(WDS_STATE_0,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_1,		EW_COLOR_WIDGET,					EW_DARKBLUE);
	look.SetElement(WDS_STATE_3,		EW_COLOR_WIDGET,					EW_WHITE);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_POINT_45);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_POINT_45);
	look.SetElement(WDS_STATE_2,		WDS_ELEMENT_INSET,					2);
	look.SetElement(WDS_STATE_3,		WDS_ELEMENT_INSET,					2);
	WDS_REGISTER_LOOK(WDS_MenuButton,&look);

	// WDS_MenuBar
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Form::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,			0);
	WDS_REGISTER_LOOK(WDS_MenuBar,&look);

	// WDS_MenuForm
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Form::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,			0);
	WDS_REGISTER_LOOK(WDS_MenuForm,&look);

	// WDS_MenuNode
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Popup::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS|WDS_BEVELBOX_LABEL_LEFT);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINDENT,				2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBOUTDENT,				2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_POPUP_IDLE_HANDOFF,		TRUE);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_POPUP_DELAY,			300);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					EW_DARKBLUE);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_PEN,						EW_WHITE);
	look.SetElement(WDS_STATE_0,		EW_COLOR_WIDGET,					WDS_ELEMENT_INHERIT|WDS_ELEMENT_DEFAULT);
	look.SetElement(WDS_STATE_0,		EW_COLOR_PEN,						WDS_ELEMENT_INHERIT|WDS_ELEMENT_DEFAULT);
	WDS_REGISTER_LOOK(WDS_MenuNode,&look);

	// WDS_Divider
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_SMOOTH);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					4);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				8);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DIVIDER_SLIDEFORM,		2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DIVIDER_OVERLAP,		2);
	WDS_REGISTER_LOOK(WDS_Divider,&look);

	// WDS_DividerTab
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Popup::GetTypeCodeStatic());
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_LIT,						WDS_ELEMENT_MAP|EW_COLOR_LIT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_HALF_LIT,					WDS_ELEMENT_MAP|EW_COLOR_HALF_LIT);
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,					WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_OUT|WDS_BEVELBOX_OMIT_BOTTOM|
																				WDS_BEVELBOX_SMOOTH|WDS_BEVELBOX_ROUND);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_INSET,					-2);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				1);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_SUBINSET,				3);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_LABEL_X,				0);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_LABEL_Y,				2);
	WDS_REGISTER_LOOK(WDS_DividerTab,&look);

	// WDS_DividerForm
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,			0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_Form::GetTypeCodeStatic());
	WDS_REGISTER_LOOK(WDS_DividerForm,&look);

	// WDS_Form
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		EW_COLOR_PEN,						EW_BLACK);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_BORDERLESS);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,					0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,					1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_SUBINSET,				0);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,			1);
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_COLOR_SELECT,		EW_RED);
	look.SetElement(WDS_STATE_1,		EW_COLOR_SHADOW,					WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_BORDER,					WDS_BEVELBOX_GROOVE);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_DEPTH,					2);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_INSET,					4);
	look.SetElement(WDS_STATE_1,		WDS_ELEMENT_SUBINSET,				8);
	WDS_REGISTER_LOOK(WDS_Form,&look);

	// WDS_PointerEntry
	look.Reset();
	look.SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,			(long)WDS_GetString::GetTypeCodeStatic());
	WDS_REGISTER_LOOK(WDS_PointerEntry,&look);


	// call initialization callbacks for all nodes on looklist

	WDS_LookEntry *node;
	WDS_Looks *a_looks;
	WDS_Look a_look;
	void (*callback)(WDS_Look *,long);

	long pass;
	for(pass=0; pass<1+(default_looks!=NULL && default_looks!=this); pass++)
		{
		a_looks= pass? default_looks: this;

		a_looks->GetLookList()->ToHead();
		while( (node=a_looks->GetLookList()->PostIncrement()) != NULL )
			{
			osdMemcpy(&a_look,node->GetLook(),sizeof(WDS_Look));

			callback=(void (*)(WDS_Look *,long))a_look.GetElement(WDS_STATE_ALL,WDS_ELEMENT_CALLBACK_INIT);

#if WDS_ELEMENT_CALLBACK_DEBUG
				printf("for look=0x%x typecode=0x%x (%s) ",
															node->GetLook(),node->GetTypeCode(),node->GetTypeName());
#endif
			if(callback && (WDS_Typecode)callback!=WDS_TYPE_UNDEFINED)
				{
#if WDS_ELEMENT_CALLBACK_DEBUG
				printf("calling callback 0x%x\n",(long)callback);
#endif

				(*callback)(&a_look,set);

#if WDS_ELEMENT_CALLBACK_DEBUG
				printf(" callback done\n");
#endif

				this->RegisterLook(node->GetTypeCode(),node->GetClassID(),node->GetTypeName(),&a_look);
				}
#if WDS_ELEMENT_CALLBACK_DEBUG
			else
				printf("no callback\n");
#endif


			// in case something reset the pointer
			a_looks->GetLookList()->SetCurrent(node);
			a_looks->GetLookList()->PostIncrement();
			}
		}

#if WDS_ELEMENT_CALLBACK_DEBUG
			printf("all callbacks done\n");
#endif

	// invalidate old look pointers
	WDS_Widget::IncrementLookCounter();

#if WDS_ELEMENT_INIT_DUMP
	Dump(0);
#endif
	}
