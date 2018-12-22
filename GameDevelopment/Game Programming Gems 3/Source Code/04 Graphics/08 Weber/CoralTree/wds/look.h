/******************************************************************************

Coral Tree wds/look.h
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


#define WDS_LOOK_DEBUG				TRUE


// WDS_Looks::Initialize(???)
// no longer supported except WDS_LOOK_WIN32
#define WDS_LOOK_CORAL				0
#define WDS_LOOK_WIN32				1
#define WDS_LOOK_MOTIF				2
#define WDS_LOOK_ATHENA				3
#define WDS_LOOK_DEFAULT			WDS_LOOK_WIN32

		// default type (WDS_Widget represents this)
#define WDS_TYPE_DEFAULT			((WDS_Typecode)0xDEFA)

									// 0 more sense, but this is obvious
#define WDS_TYPE_UNDEFINED			((WDS_Typecode)0xADEF)

// SetElement(???,attribute,set)
#define WDS_STATE_0					0
#define WDS_STATE_1					1
#define WDS_STATE_2					2
#define WDS_STATE_3					3
#define WDS_STATE_4					4
#define WDS_STATE_5					5
#define WDS_STATES					6
#define WDS_STATE_ALL				0xFFFFFF

// SetElement(state,???,set)
#define WDS_ELEMENT_FIRST					(EW_COLOR_NUMBER)
#define WDS_ELEMENT_COLOR_WIDGET_DITHER		(WDS_ELEMENT_FIRST+0)
#define WDS_ELEMENT_BORDER					(WDS_ELEMENT_FIRST+1)
#define WDS_ELEMENT_PREF_X					(WDS_ELEMENT_FIRST+2)
#define WDS_ELEMENT_PREF_Y					(WDS_ELEMENT_FIRST+3)
#define WDS_ELEMENT_INDENT					(WDS_ELEMENT_FIRST+4)
#define WDS_ELEMENT_OUTDENT					(WDS_ELEMENT_FIRST+5)
#define WDS_ELEMENT_DEPTH					(WDS_ELEMENT_FIRST+6)
#define WDS_ELEMENT_INSET					(WDS_ELEMENT_FIRST+7)
#define WDS_ELEMENT_INSET_MODY				(WDS_ELEMENT_FIRST+8)
#define WDS_ELEMENT_SUBINSET				(WDS_ELEMENT_FIRST+9)
#define WDS_ELEMENT_SUBINSET_MODY			(WDS_ELEMENT_FIRST+10)
#define WDS_ELEMENT_SUBINDENT				(WDS_ELEMENT_FIRST+11)
#define WDS_ELEMENT_SUBOUTDENT				(WDS_ELEMENT_FIRST+12)
#define WDS_ELEMENT_LABEL_X					(WDS_ELEMENT_FIRST+13)
#define WDS_ELEMENT_LABEL_Y					(WDS_ELEMENT_FIRST+14)
#define WDS_ELEMENT_ANNOTATION_MODE			(WDS_ELEMENT_FIRST+15)
#define WDS_ELEMENT_ANNOTATION_X			(WDS_ELEMENT_FIRST+16)
#define WDS_ELEMENT_ANNOTATION_Y			(WDS_ELEMENT_FIRST+17)
#define WDS_ELEMENT_INHERITANCE				(WDS_ELEMENT_FIRST+18)
#define WDS_ELEMENT_CALLBACK_INIT			(WDS_ELEMENT_FIRST+19)
// psuedo-union (potentially overlapping enumerations for different widgets)
#define WDS_ELEMENT_UNION					(WDS_ELEMENT_FIRST+20) // =first of
#define WDS_ELEMENT_UNION_0					(WDS_ELEMENT_UNION+0)
#define WDS_ELEMENT_UNION_1					(WDS_ELEMENT_UNION+1)
#define WDS_ELEMENT_UNION_2					(WDS_ELEMENT_UNION+2)
#define WDS_ELEMENT_UNION_3					(WDS_ELEMENT_UNION+3)
#define WDS_ELEMENT_UNION_4					(WDS_ELEMENT_UNION+4)
#define WDS_ELEMENT_NUMBER					(WDS_ELEMENT_UNION+5) // >=highest+1
// psuedo-union mappings
// remember not to collide with inherited mapping
#define WDS_ELEMENT_BUTTON_SENSE_ANNOTATION	(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_HNODE_LINES				(WDS_ELEMENT_UNION_1) // button+
#define WDS_ELEMENT_GETSTR_FLAGS			(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_GETSTR_UNSELECT_FG		(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_GETSTR_SELECT_FG		(WDS_ELEMENT_UNION_2)
#define WDS_ELEMENT_GETSTR_SELECT_BG		(WDS_ELEMENT_UNION_3)
#define WDS_ELEMENT_GETSTR_HIGHLIGHT_BG		(WDS_ELEMENT_UNION_4)
#define WDS_ELEMENT_TEXTLIST_HIGHLIGHT_BG	(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_TEXTLIST_IDLE_ECHO		(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_FORM_SEPARATOR			(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_FORM_INDENT				(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_FORM_INDENT_AFTER		(WDS_ELEMENT_UNION_2)
#define WDS_ELEMENT_FORM_COLOR_SELECT		(WDS_ELEMENT_UNION_3)
#define WDS_ELEMENT_PARTITION_SEPARATOR		(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_PARTITION_KNOB_OFFSET	(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_PARTITION_GRAB_WIDTH	(WDS_ELEMENT_UNION_2)
#define WDS_ELEMENT_PARTITION_GRAB_LENGTH	(WDS_ELEMENT_UNION_3)
#define WDS_ELEMENT_SCROLLREGION_SCROLLABLE	(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_SCROLLBAR_WIDTH			(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_SCROLLBAR_CHILDREN		(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_POPUP_IDLE_HANDOFF		(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_POPUP_DELAY				(WDS_ELEMENT_UNION_1) // for menus
#define WDS_ELEMENT_DIVIDER_SLIDEFORM		(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_DIVIDER_OVERLAP			(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_PICKBUTTON_WIDTH		(WDS_ELEMENT_UNION_1) // button+
#define WDS_ELEMENT_PICK_AUTO_OFFSET		(WDS_ELEMENT_UNION_0)
#define WDS_ELEMENT_PICK_MAINTAIN_BUTTON	(WDS_ELEMENT_UNION_1)
#define WDS_ELEMENT_PICK_WIN_HEIGHT_MAX		(WDS_ELEMENT_UNION_2)
#define WDS_ELEMENT_MENUBUTTON_WIDTH		(WDS_ELEMENT_UNION_1) // button+

// SetElement(state,attribute,???): special values
#define WDS_ELEMENT_MASK			0x00FFFFFF
#define WDS_ELEMENT_INHERIT			0x01000000
#define WDS_ELEMENT_DEFAULT			0x02000000
#define WDS_ELEMENT_MAP				0x04000000
#define WDS_ELEMENT_SPECIAL			0x07000000
#define WDS_ELEMENT_COPY			0x08000000
#define WDS_ELEMENT_NEGATIVE		0x80000000
#define WDS_ELEMENT_IS_INHERIT(x)	( (x&WDS_ELEMENT_INHERIT)	&& !(x&WDS_ELEMENT_NEGATIVE) )
#define WDS_ELEMENT_IS_DEFAULT(x)	( (x&WDS_ELEMENT_DEFAULT)	&& !(x&WDS_ELEMENT_NEGATIVE) )
#define WDS_ELEMENT_IS_MAP(x)		( (x&WDS_ELEMENT_MAP)		&& !(x&WDS_ELEMENT_NEGATIVE) )
#define WDS_ELEMENT_IS_SPECIAL(x)	( (x&WDS_ELEMENT_SPECIAL)	&& !(x&WDS_ELEMENT_NEGATIVE) )
#define WDS_ELEMENT_IS_COPY(x)		( (x&WDS_ELEMENT_COPY)		&& !(x&WDS_ELEMENT_NEGATIVE) )

#define WDS_ELEMENT_DEFAULT_INIT	(WDS_ELEMENT_INHERIT|WDS_ELEMENT_DEFAULT)
#define WDS_ELEMENT_DEFAULT_VALUE	0


#define WDS_REGISTER_LOOK(typename,look) RegisterLook(typename::GetTypeCodeStatic(),typename::GetClassIdStatic(),#typename,look)





/******************************************************************************
class	WDS_Look_Flash

	single-state disposable cache

******************************************************************************/
class WDS_Look_Flash
	{
	private:

		long	element[WDS_ELEMENT_NUMBER];

	public:

				WDS_Look_Flash(void)
					{
					long m;
					for(m=0;m<WDS_ELEMENT_NUMBER;m++)
						element[m]=WDS_ELEMENT_DEFAULT_INIT;
					};

		void	SetElement(long index,long set)	{ element[index]=set; };
		long	GetElement(long index)			{ return element[index]; };
	};



/******************************************************************************
class	WDS_Look

Keywords:
******************************************************************************/
class WDS_Look
	{
	private:

		long	element[WDS_STATES][WDS_ELEMENT_NUMBER];

		// cache look pointers to inherited look's
		unsigned long	count[WDS_STATES];
		WDS_Look		*reference[WDS_STATES];

	public:

					WDS_Look(void)
						{
						Reset();
						};

		void		Reset(void)
						{
						long m;
						for(m=0;m<WDS_ELEMENT_NUMBER;m++)
							{
							SetElement(WDS_STATE_ALL,m,
													WDS_ELEMENT_DEFAULT_INIT);
							}

						SetElement(WDS_STATE_ALL,WDS_ELEMENT_INHERITANCE,
														WDS_TYPE_UNDEFINED);
						SetElement(WDS_STATE_ALL,WDS_ELEMENT_CALLBACK_INIT,
														WDS_TYPE_UNDEFINED);

						for(m=0;m<WDS_STATES;m++)
							{
							count[m]=0;
							reference[m]=NULL;
							}
						};

		void		Dump(WDS_Typecode typecode,char *type_name,
														WDS_Widget *widget);

		void			SetCount(long state,long set)
							{ count[state]=set; };
		unsigned long	GetCount(long state)
							{ return count[state]; };

		void		SetReference(long state,WDS_Look *set)
						{ reference[state]=set; };
		WDS_Look	*GetReference(long state)
						{ return reference[state]; };

		void		SetElement(long state,long attribute,long set);
		long		GetElement(long state,long attribute);
	};



/******************************************************************************
class WDS_LookEntry

Keywords:
******************************************************************************/
class WDS_LookEntry : public GPL_DoubleNode
	{
	private:

		WDS_Typecode	typecode;
		GPL_ClassId		class_id;
		WDS_String		type_name;
		WDS_Look		*look;

	public:

						WDS_LookEntry(void)
							{
							typecode= WDS_TYPE_UNDEFINED;
							look=NULL;
							};

		void			SetTypeCode(WDS_Typecode set)	{ typecode=set; };
		WDS_Typecode	GetTypeCode(void)				{ return typecode; };

		void			SetClassID(GPL_ClassId set)		{ class_id=set; };
		GPL_ClassId		GetClassID(void)				{ return class_id; };

		void			SetTypeName(char *set)	{ type_name.NewBuffer(set); };
		char			*GetTypeName(void)		{ return type_name.Buffer(); };

		void			SetLook(WDS_Look *set)			{ look=set; };
		WDS_Look		*GetLook(void)					{ return look; };
	};



/******************************************************************************
class	WDS_LookList


Keywords:
******************************************************************************/
class WDS_LookList : public GPL_IDoubleList<WDS_LookEntry>
	{
	public:
				WDS_LookList(void)	{};
virtual			~WDS_LookList(void)	{};
	};



/******************************************************************************
class	WDS_Looks

Keywords:
******************************************************************************/
class WDS_Looks
	{
	private:

		WDS_LookList	looklist;

		WDS_LookList	*GetLookList(void)	{ return &looklist; };

		WDS_LookEntry	*FindTypeName(char *name);

	public:

virtual				~WDS_Looks(void);

		void		RegisterLook(WDS_Typecode typecode,GPL_ClassId class_id,
											char *type_name,WDS_Look *look);
		WDS_Look	*LookupLook(WDS_Typecode typecode,GPL_ClassId class_id);

					// find element value following inheritance
		long		SearchForElement(WDS_Look *look,WDS_Typecode typecode,
										long value,long state,long element);

		long		AssignTypecodeToTypeName(WDS_Typecode typecode,
										GPL_ClassId class_id,char *type_name);

		WDS_Look	*GetAppropriateLook(WDS_Look *look,long state,
										WDS_Typecode typecode,long inherit);

		void		Initialize(long set,WDS_Looks *default_looks);
		long		LoadFromFile(char *name);
		long		ReadFromBuffer(long *buffer);

		void		SaveAsC(char *filename,char *variablename);

		void		ResetWindow(EW_Window *window);

static	void		DumpHeader(void);
		void		Dump(long code);
	};
