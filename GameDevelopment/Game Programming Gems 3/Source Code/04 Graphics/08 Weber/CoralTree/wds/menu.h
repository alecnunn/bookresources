/******************************************************************************

Coral Tree wds/menu.h
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



// WDS_MenuNode::SetMenuFlags()
#define WDS_MENU_FLAGS_NONE			0
#define WDS_MENU_FLAGS_AUTOPOSITION	1



/******************************************************************************
class   WDS_MenuButton : public WDS_Button

******************************************************************************/
class WDS_MenuButton : public WDS_Button
	{
	WDS_DECLARE_TYPE(WDS_MenuButton);

	private:

	public:
				WDS_MenuButton(void)
					{
					SetName("Menu Button");
					};
	};



/******************************************************************************
class   WDS_MenuBar : public WDS_Form

    form configured for use as a menu bar

******************************************************************************/
class WDS_MenuBar : public WDS_Form
	{
	WDS_DECLARE_TYPE(WDS_MenuBar);

	public:

				WDS_MenuBar(void);
	};



/******************************************************************************
class   WDS_MenuForm : public WDS_Form

	column of menu entries

******************************************************************************/
class WDS_MenuForm : public WDS_Form
	{
	WDS_DECLARE_TYPE(WDS_MenuForm);

	public:

				WDS_MenuForm(void);
	};



/******************************************************************************
class	WDS_MenuNode : public WDS_Popup

******************************************************************************/
class WDS_MenuNode : public WDS_Popup
	{
	WDS_DECLARE_TYPE(WDS_MenuNode);

	private:

		WDS_MenuButton	menubutton;
		WDS_MenuForm	menuform;
		WDS_MenuNode	*master;
		long			menuflags;

	protected:

virtual	void			AddWidgetProtected(EW_Widget *widget,long placement);
virtual	void			RemoveWidgetProtected(EW_Widget *widget);

	public:

						WDS_MenuNode(void);

		WDS_MenuForm	*GetMenuForm(void)		{ return &menuform; };
		WDS_MenuButton	*GetMenuButton(void)	{ return &menubutton; };

		WDS_MenuNode	*GetMaster(void)		{ return master; }

		void			SetMenuFlags(long set)	{ menuflags=set; };
		long			GetMenuFlags(void)		{ return menuflags; };

virtual	long			Resize(EW_Resize *resize);
virtual	void			ChangeState(long newstate,EW_Event *event);
virtual	void			Pop(EW_Event *event);
virtual	void			Drop(EW_Event *event);
	};
