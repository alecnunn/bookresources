/******************************************************************************

Coral Tree wds/popup.h
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


#define WDS_MAX_POPUP_TITLE_NAME_LEN	256

#define WDS_POPUP_LOC_MOUSE				1
#define WDS_POPUP_LOC_TOP				2
#define WDS_POPUP_LOC_BOTTOM			3
#define WDS_POPUP_LOC_LEFT				4
#define WDS_POPUP_LOC_RIGHT				5
#define WDS_POPUP_LOC_MASK				0x0000FFFF
#define WDS_POPUP_LOC_FROM_REGION		0x00010000
#define WDS_POPUP_LOC_DEFAULT			WDS_POPUP_LOC_BOTTOM

// Configuration Flags
#define	WDS_POPUP_FLAGS_NONE			0
#define	WDS_POPUP_GRAB_POINTER			1
#define WDS_POPUP_SIBLING_HANDOFF		2
#define	WDS_POPUP_AUTOPOP				4
#define WDS_POPUP_NONSTICK				8
#define WDS_POPUP_PERSISTANT			16
#define WDS_POPUP_BUTTONLESS_POP		32


class WDS_Popup;



/******************************************************************************
class	WDS_PopupBase : public WDS_Widget

Keywords:
******************************************************************************/
class WDS_PopupBase : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_PopupBase);

	private:

		WDS_Popup	*mother;

	public:
					WDS_PopupBase(void);

		long		Operate(EW_Event *event);
		void		Draw(EW_Redraw *redraw);
		void		SetMother(WDS_Popup *a_widget){mother = a_widget;}
	};



/******************************************************************************
class	WDS_Popup : public WDS_Widget

Keywords:
******************************************************************************/
class WDS_Popup : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Popup);

/* - data members -----------------------------------------------------------*/

	private:

		EW_Window					*popup_window;
		EW_EventContext				*popup_context;
		WDS_PopupBase				popup_basewidget;
		long						added;
static	WDS_Popup					*grabbed;
		GPL_DoubleList<WDS_Popup>	*popup_register;
		long						popped;
		EW_Event					bypass_mask;
		long						pop_item,unpop_item,tear_item,untear_item;
		long						pop_block;
		long						popup_location;
		WDS_Scope					*popup_scope;
		char						title[WDS_MAX_POPUP_TITLE_NAME_LEN];
		long 						armed;
		long						xoffset, yoffset;
		unsigned long				window_flags;
		long 						torn;	
		long 						flags;	
		long						timer;

/* - main infrastructure ----------------------------------------------------*/

	private:

		void			WDS_PopupConstructor();

		long 			PropagateEvent(EW_Event *event);

		void			RegisterEvent(long set);
		void			RegisterPop(WDS_Popup* popup_reg);

//		long			SiblingHandoff(EW_Event *event);

		void			PopOrTear(EW_Event *event,long tearonly,long transient);
		void			OpenPopupWindow(long wflags);

		void			SetTimerID(long set)	{ timer=set; };
		long			GetTimerID(void)		{ return timer; };

		void			SetGrabbed(WDS_Popup *set);
		WDS_Popup		*GetGrabbed(void)		{ return grabbed; };

	protected:

		WDS_Scope		*GetPopupScope(void)	{ return popup_scope; };

/* - user interface ---------------------------------------------------------*/

	public:
						WDS_Popup(void);
						WDS_Popup(char *a_title);
virtual					~WDS_Popup(void);

		void 			Draw(EW_Redraw *redraw);
		long			Operate(EW_Event *event);
		long			PostOperate(EW_Event *event);
		long			Resize(EW_Resize *resize);

		void			SetWindowFlags(long set)	{ window_flags = set; };
		long			GetWindowFlags(void)		{ return flags; };

		long			GetFlags(void)				{ return flags; };
		void			SetFlags(long set)			{ flags = set; };
		void			SetFlagsIncluding(long set)	{ flags |= set; };
		void			SetFlagsExcluding(long set)	{ flags ^= (flags&set); };

		EW_Event		*GetBypassMask(void)		{ return &bypass_mask; };
		void			CopyToBypassMask(EW_Event *mask)
							{
							bypass_mask.CopyFrom(mask);
							};
		void			CopyFromBypassMask(EW_Event *mask)
							{
							mask->CopyFrom(&bypass_mask);
							};

		void			SetPopBlock(long set)		{ pop_block=set; };
		long			GetPopBlock(void)			{ return pop_block; };

		void			SetPopItem(long set)		{ pop_item=set; };
		long			GetPopItem(void)			{ return pop_item; };

		void			SetUnPopItem(long set)		{ unpop_item=set; };
		long			GetUnPopItem(void)			{ return unpop_item; };

		void			SetTearItem(long set)		{ tear_item=set; };
		long			GetTearItem(void)			{ return tear_item; };

		void			SetUnTearItem(long set)		{ untear_item=set; };
		long			GetUnTearItem(void)			{ return untear_item; };

		void			SetOffset(long x, long y)	{ xoffset=x;yoffset=y;};
		void			GetOffset(long *x, long *y)	{ *x=xoffset;*y=yoffset;};

		void			SetLocation(long set)		{ popup_location=set; };
		long			GetLocation(void)			{ return popup_location; };

virtual	void			Pop(EW_Event *event);
virtual	void			Drop(EW_Event *event);

		void			PendPop(EW_Event *event,long delay);
		void			StopPendPop(void);

		void			TearOff(long transient);
		void			UnTearOff(void);

		long 			GetPopped(void)				{ return popped; };
		long 			GetTorn(void)				{ return torn; };

		WDS_PopupBase	*GetBaseWidget(void)		{ return &popup_basewidget;}
		EW_Window		*GetBaseWindow(void)		{ return popup_window; }

		long			IsUnderPopup(void);
		};
