/******************************************************************************

Coral Tree wds/button.h
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

// WDS_Button::SetFlags(?)
#define	WDS_BUTTON_FLAGS_NONE				0
#define	WDS_BUTTON_FLAGS_TOGGLE				1	// locks and unlocks on press
#define	WDS_BUTTON_FLAGS_EXCLUSIVE			2	// registers with scope
#define	WDS_BUTTON_FLAGS_UNTAKE_ONLY		4	// only deselect on take lost
#define	WDS_BUTTON_FLAGS_TOGGLE_AUTOSIZE	8
#define	WDS_BUTTON_FLAGS_DEFAULT		WDS_BUTTON_FLAGS_NONE


/*****************************************************************************
class	WDS_Button : public WDS_Widget

	A plain push button.
	active == TRUE when pressed
	active == FALSE when not pressed

	void (*displayfunction)(long x,long y,long w,long h,void *displayarg)
		This function (if set) is called when the button is drawn.
*****************************************************************************/
class WDS_Button : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Button);

	private:

		long		flags;
		long		readied;

		void		(*displayfunction)(EW_Window*,long,long,long,long,void *);
		void		*displayarg;

		void		RegisterExclusivity(long on);
		void		RegisterRelease(long on);

	public:
					WDS_Button();
virtual				~WDS_Button(){}

		long		Operate(EW_Event *event);
virtual void		DrawOnTablet(EW_Redraw *redraw);
		void		Draw(EW_Redraw *redraw);
		long		Resize(EW_Resize *resize);

		long		GetFlags(void)				{ return flags; };
		void		SetFlags(long set)			{ flags = set; };
		void		SetFlagsIncluding(long set)	{ flags |= set; };
		void		SetFlagsExcluding(long set)	{ flags ^= (flags&set); };

virtual	void		ChangeState(long newstate,EW_Event *event);


		// Display Function not currently implemented
		void		SetDisplayFunction(void (*a_displayfunction)(EW_Window*,long,long,long,long,void *))
						{
						displayfunction = a_displayfunction;
						}
		void		SetDisplayArg(void *a_displayarg)
						{
						displayarg = a_displayarg;
						}
	};



/******************************************************************************
class WDS_ToggleButton : public WDS_Button

******************************************************************************/
class WDS_ToggleButton : public WDS_Button
	{
	WDS_DECLARE_TYPE(WDS_ToggleButton);

	public:
					WDS_ToggleButton()
						{
						SetName("ToggleButton");
						SetAnnotation("Toggle");

						SetFlags(WDS_BUTTON_FLAGS_TOGGLE|
									WDS_BUTTON_FLAGS_TOGGLE_AUTOSIZE);
						};
	};



/******************************************************************************
class WDS_RadioButton : public WDS_Button

******************************************************************************/
class WDS_RadioButton : public WDS_Button
	{
	WDS_DECLARE_TYPE(WDS_RadioButton);

	public:
					WDS_RadioButton(void)	{ Reset(); };

		void		Reset(void)
						{
						SetName("RadioButton");
						SetAnnotation("Radio");

						SetFlags(WDS_BUTTON_FLAGS_TOGGLE|
									WDS_BUTTON_FLAGS_TOGGLE_AUTOSIZE|
									WDS_BUTTON_FLAGS_EXCLUSIVE|
									WDS_BUTTON_FLAGS_UNTAKE_ONLY);
						};
	};



/******************************************************************************
class WDS_RadioScope : public WDS_Scope

******************************************************************************/
class WDS_RadioScope : public WDS_Scope
	{
	GPL_OBJECT_INTERFACE(WDS_RadioScope);

	private:

		WDS_StringList	*stringlist;
		WDS_Form		radioform;
		WDS_RadioButton	**radio;
		long			radios;
		long			autosize;

		long			SetNumberRadios(long set);

	public:
					WDS_RadioScope(void)
						{
						SetName("RadioScope");
						radioform.SetMode(WDS_FORM_COLUMN);
						radioform.GetLocalLook()->SetElement(WDS_STATE_ALL,
												WDS_ELEMENT_FORM_SEPARATOR,4);

						stringlist=NULL;
						radio=NULL;
						radios=0;
						SetAutoSizing(FALSE);
						};

virtual				~WDS_RadioScope(void)
						{
						SetNumberRadios(0);
						};

		WDS_Form		*GetForm(void)	{ return &radioform; };

		void			SetStringList(WDS_StringList *set)	{ stringlist=set; };
		WDS_StringList	*GetStringList(void)			{ return stringlist; };

		void			SetAutoSizing(long set)	{ autosize=set; };
		long			GetAutoSizing(void)		{ return autosize; };

virtual	long			Resize(EW_Resize *resize);

static	void			ButtonCallback(WDS_Activation *activation);
	};
