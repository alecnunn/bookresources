
/******************************************************************************

Coral Tree wds/relay.h
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
class	WDS_Relay : public WDS_Widget

******************************************************************************/
class WDS_Relay : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Relay);

	private:

		EW_Event	eventmask,registered_eventmask;
		long		registered;

	public:

				WDS_Relay(void);
virtual			~WDS_Relay(void);

		void	Draw(EW_Redraw *redraw)		{};
		long	Resize(EW_Resize *resize);
		long	Operate(EW_Event *event);

		void	CopyEventMaskFrom(EW_Event *other)
					{ eventmask.CopyFrom(other); };
		void	CopyEventMaskTo(EW_Event *other)
					{ other->CopyFrom(&eventmask); };

		void	Register(long on);
	};



/******************************************************************************
class	WDS_Relay_F1Help : public WDS_Relay

******************************************************************************/
class WDS_Relay_F1Help : public WDS_Relay
    {
	public:
				WDS_Relay_F1Help(void);

static	void	Callback(WDS_Activation *activation);
	};

/******************************************************************************
class	WDS_DumpWidgetsRelay : public WDS_Relay

******************************************************************************/
class WDS_DumpWidgetsRelay : public WDS_Relay
    {
	protected:
		EW_Widget			*dump;
		EW_PostScriptTablet	tablet;
		long				item;

	public:
				WDS_DumpWidgetsRelay(void);
	virtual		~WDS_DumpWidgetsRelay(void);

	void		SetDumpWidget(EW_Widget *set)	{ dump=set; };
	EW_Widget	*GetDumpWidget(void)			{ return dump; };

	EW_PostScriptTablet	*GetTablet(void)		{ return &tablet; };

	void		SetItem(long set)				{ item=set; };
	long		GetItem(void)					{ return item; };

static	void	Callback(WDS_Activation *activation);
	};
