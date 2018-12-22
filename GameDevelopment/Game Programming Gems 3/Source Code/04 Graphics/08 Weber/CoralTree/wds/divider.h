/******************************************************************************

Coral Tree wds/divider.h
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



/******************************************************************************
class	WDS_DividerForm : public WDS_Form

******************************************************************************/
class WDS_DividerForm : public WDS_Form
	{
	WDS_DECLARE_TYPE(WDS_DividerForm);

	public:
				WDS_DividerForm(void)
					{
					SetClipping(EW_CLIPPING_CHILDREN);
					};
	};



/******************************************************************************
class	WDS_DividerTab : public WDS_Popup

******************************************************************************/
class WDS_DividerTab : public WDS_Popup
	{
	WDS_DECLARE_TYPE(WDS_DividerTab);

	public:
				WDS_DividerTab(void);
virtual			~WDS_DividerTab(void);

virtual	void	Draw(EW_Redraw *redraw);
	};



class WDS_DividerPlaceHolder;

/******************************************************************************
class   WDS_Divider : public WDS_Partition

	state changes:
		0: none selected, defaulting
		1: new selected

******************************************************************************/
class WDS_Divider : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Divider);

	private:

		WDS_DividerTab		*selected_tab;
		WDS_Widget			*selected_child;

		WDS_DividerForm		dividerform;
		WDS_DividerTab		**tab;		// allocated array
		long				tabs;
		long				popable;

		void			ConfirmSlaves(void);
		void			SelectIndex(long index,EW_Event *event);

		long			IsPopped(long index);
		void			PopOut(EW_Event *event,long index);
		void			DropIn(long index);

		void			SetNumberTabs(long set);

	public:

						WDS_Divider(void);
virtual					~WDS_Divider(void);

virtual	long			PostOperate(EW_Event *event);
virtual	void			Draw(EW_Redraw *redraw);
virtual	void			PostDraw(EW_Redraw *redraw);
virtual	long			Resize(EW_Resize *resize);
virtual	void			ChangeState(long newstate,EW_Event *event);

		WDS_Widget		*GetSelected(void)			{ return selected_child; };
		void			SetSelected(WDS_Widget *set)
							{
							if(selected_child!=set)
								{
								selected_child=set;
								ChangeState(WDS_STATE_1,NULL);
								}
							};

		void			SetPoppable(long set)		{ popable=set; };
		long			GetPoppable(void)			{ return popable; };
	};



/******************************************************************************
class	WDS_DividerPlaceHolder : public WDS_Widget

	takes place of popped-out child
	holds storage pertaining to pop-out

******************************************************************************/
class WDS_DividerPlaceHolder : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_DividerPlaceHolder);

	private:
		WDS_Widget	*disowned;
		WDS_Divider	pop_divider;

	public:
					WDS_DividerPlaceHolder(void)
						{
						SetName("DV PlaceHolder");
						SetDormant(TRUE);
						SetInvisible(TRUE);
						SetDisowned(NULL);
						};

virtual				~WDS_DividerPlaceHolder(void)
						{
						};

		WDS_Divider	*GetDivider(void)				{ return &pop_divider; };

		void		SetDisowned(WDS_Widget *set)	{ disowned=set; };
		WDS_Widget	*GetDisowned(void)				{ return disowned; };
	};
