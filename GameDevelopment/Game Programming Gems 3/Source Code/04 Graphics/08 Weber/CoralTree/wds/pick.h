/******************************************************************************

Coral Tree wds/pick.h
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
class	WDS_PickButton : public WDS_Button

******************************************************************************/
class WDS_PickButton : public WDS_Button
	{
	WDS_DECLARE_TYPE(WDS_PickButton);

	private:

	public:
				WDS_PickButton(void)
					{
					SetName("Pick Button");
					};
	};



/******************************************************************************
class	WDS_PickList : public WDS_Widget

******************************************************************************/
class WDS_PickList : public WDS_TextList
	{
	WDS_DECLARE_TYPE(WDS_PickList);

	private:

	public:
				WDS_PickList(void)
					{
					SetName("Pick List");
					};
	};



/******************************************************************************
class	WDS_PickScroll : public WDS_ScrollRegion

******************************************************************************/
class WDS_PickScroll : public WDS_ScrollRegion
	{
	WDS_DECLARE_TYPE(WDS_PickScroll);

	private:

	public:
				WDS_PickScroll(void)
					{
					SetName("Pick SR");
					};
	};



/******************************************************************************
class	WDS_Pick: public WDS_Popup

******************************************************************************/
class WDS_Pick: public WDS_Popup
	{
	WDS_DECLARE_TYPE(WDS_Pick);

	private:

		WDS_PickScroll	scrollregion;
		WDS_PickList	picklist;
		WDS_PickButton	pickbutton;
		EW_Event		pick_bypass;
		long			picked;

	public:
						WDS_Pick(void);

virtual	void			Pop(EW_Event *event);

		WDS_PickList	*GetPickList(void)		{ return &picklist; };
		WDS_PickScroll	*GetScrollRegion(void)	{ return &scrollregion; };

		long			Resize(EW_Resize *resize);

		long			PickNumber(long index);
		long			GetPickedIndex(void)	{ return picked; };

static	void			PickCallback(WDS_Activation *activation);
	};
