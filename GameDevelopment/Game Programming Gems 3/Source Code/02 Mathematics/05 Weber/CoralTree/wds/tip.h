/******************************************************************************

Coral Tree wds/tip.h
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
class	WDS_TipString

******************************************************************************/
class WDS_TipString : public WDS_GetString
	{
	WDS_DECLARE_TYPE(WDS_TipString);

	public:

				WDS_TipString(void);

virtual	long	Operate(EW_Event *event);
	};



/******************************************************************************
class	WDS_TipWindow

******************************************************************************/
class WDS_TipWindow : public EW_Window
	{
	private:

		WDS_TipString	tipstring;
		WDS_Widget		*source;
		WDS_Widget		*forbidden;
		long			timer;
		long			lastx,lasty;

	public:

					WDS_TipWindow(void);
virtual				~WDS_TipWindow(void);

		void		SetTipText(char *text)
						{ tipstring.SetString(text); };
		char		*GetTipText(void)
						{ return tipstring.GetString()->Buffer(); };

		void		SetSource(WDS_Widget *set,long mx,long my)
						{
						source=set;
						lastx=mx;
						lasty=my;
						};
		WDS_Widget	*GetSource(void)				{ return source; };

		void		SetForbidden(WDS_Widget *set)	{ forbidden=set; };
		WDS_Widget	*GetForbidden(void)				{ return forbidden; };

		void		SetTimerID(long set)			{ timer=set; };
		long		GetTimerID(void)				{ return timer; };

		void		CheckTip(WDS_Widget *widget,EW_Event *idleevent);
		void		Show(void);
		void		Deactivate(void);
	};
