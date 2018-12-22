
/******************************************************************************

Coral Tree wds/scope.h
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


// WDS_Bypass::flags
#define WDS_BYPASS_FLAG_NULL	0x00000000
#define WDS_BYPASS_FLAG_TAKE	0x00000001
#define WDS_BYPASS_FLAG_DESCEND	0x00000002 // call widget's Descend vs Operate


// EW_Event::widget_flags  (widget system defined flags)
// read through EW_Event::GetWidgetFlags()
//
// Events through a bypass will have WDS_BYPASS_EVENT set.
//
// If you bypass with the WDS_BYPASS_FLAG_TAKE flag,
// you can receive an event (your bypass) with the WDS_TAKE_LOST flag.
// This means that you just lost your exclusive rights.

#define WDS_BYPASS_EVENT		0x00000001
#define WDS_BYPASS_TAKE_LOST	0x00000002
#define WDS_BYPASS_IN_FOCUS		0x00000004
#define WDS_BYPASS_FLAGS		0x00000007	// all bypass flags



/******************************************************************************
class	WDS_Bypass

Keywords:
******************************************************************************/
class WDS_Bypass : public EW_Event
	{
	private:
		WDS_Widget		*destination;
		long			exclusion;
		long			flags;
		long			enabled;
		WDS_Validifier	*validifier;

	public:
					WDS_Bypass(void)
						{
						destination=NULL;
						validifier=NULL;
						SetExclusion(FALSE);
						SetEnabled(FALSE);
						SetFlags(0x0);
						};

		void		Invalidate(void)
						{
						if(validifier)
							{
							if(validifier->GetValid()>0)
								validifier->SetValid(validifier->GetValid()-1);
							else
								delete validifier;
							}
						};

		long		DestinationIsValid(void)
						{
						return (validifier && validifier->GetValid()>0);
						};

		void		SetDestination(WDS_Widget *dest)	{ destination=dest; };
		WDS_Widget	*GetDestination(void)				{ return destination; };

		void		SetExclusion(long set)		{ exclusion=set; };
		long		GetExclusion(void)			{ return exclusion; };

					// scope enables bypasses before using
		void		SetEnabled(long set)		{ enabled=set; };
		long		GetEnabled(void)			{ return enabled; };

		void		SetFlags(long set)			{ flags=set; };
		long		GetFlags(void)				{ return flags; };

		void			SetValidifier(WDS_Validifier *set)
							{ validifier=set; };
		WDS_Validifier	*GetValidifier(void)
							{ return validifier; };

		void		CopyDataFromEvent(EW_Event *event)
						{
						SetSource( event->GetSource() );
						SetItem( event->GetItem() );
						SetState( event->GetState() );
						SetUsed( event->GetUsed() );
						};
		void		CopyDataToEvent(EW_Event *event)
						{
						event->SetSource( GetSource() );
						event->SetItem( GetItem() );
						event->SetState( GetState() );
						event->SetUsed( GetUsed() );
						};

		void		CopyFrom(WDS_Bypass *other)
						{
						CopyDataFromEvent(other);
						SetDestination( other->GetDestination() );
						SetValidifier( other->GetValidifier() );
						SetExclusion( other->GetExclusion() );
						SetEnabled( other->GetEnabled() );
						SetFlags( other->GetFlags() );
						};
	};



/******************************************************************************
class	WDS_BypassList

	WDS_BypassList is for the use of WDS_Scope.
	Consistancy of the interface and functionality is not guaranteed.

Keywords:
******************************************************************************/
class WDS_BypassList : public GPL_DoubleList<WDS_Bypass>
	{
	private:

	public:

					WDS_BypassList(void)	{};
virtual				~WDS_BypassList(void);

		void		ConnectBypass(WDS_Bypass *bypass);
		void		RemoveBypass(WDS_Bypass *bypass);
		WDS_Bypass	*MatchEvent(EW_Event *event,long if_enabled);
		void		ResolveConflicts(WDS_Bypass *bypass,long mark);

		void		Print(char *name);
	};



/*****************************************************************************
class	WDS_Scope

*****************************************************************************/
class WDS_Scope : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Scope);

	private:

		WDS_BypassList	bypass_list;

		long			SendBypass(WDS_Bypass *match,WDS_Bypass *bypass);

	public:
						WDS_Scope(void);
virtual					~WDS_Scope(void);
		long			Descend(EW_Event *event);
		void			Draw(EW_Redraw *redraw);

		WDS_Validifier	*RegisterBypass(WDS_Bypass *bypass);
		void			DeRegisterBypass(WDS_Bypass *bypass);
		WDS_Bypass		*FindBypass(EW_Event *event);

		long			IsTaken(EW_Event *event);
		long			BypassEvent(EW_Event *event);

		void			ExcludeAncestors(WDS_Bypass *bypass);
		void			DumpList(void)
							{
							bypass_list.Print(GetName());
							};
	};
