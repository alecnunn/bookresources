/******************************************************************************

Coral Tree wds/scope.cc
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



//#define	WDS_SCOPE_KEYS		(!EW_RELEASE_GRADE)
#define	WDS_SCOPE_KEYS			TRUE

#define	WDS_HIERDUMP			WDS_SCOPE_KEYS	// '@' key dumps widget hierarchy
#define	WDS_LOOKDUMP			WDS_SCOPE_KEYS	// '#' key dumps look list
#define	WDS_SCOPELIST_DEBUG		WDS_SCOPE_KEYS	// '^' key dumps bypass list

#define	WDS_BYPASS_DEBUG		FALSE
#define	WDS_VALIDIFIER_DEBUG	FALSE
#define WDS_SENDBYPASS_DEBUG	FALSE
#define	WDS_SCOPE_DEBUG			FALSE
#define	WDS_REGISTER_DEBUG		FALSE
#define WDS_MATCH_DEBUG			FALSE

#define WDS_BYPASS_PROPOGATE	TRUE	// affect original event with changes


#define WDS_SCOPE_MINSIZEX -1
#define WDS_SCOPE_MINSIZEY -1



/******************************************************************************
		WDS_Scope::WDS_Scope()

Keywords:
******************************************************************************/
WDS_Scope::WDS_Scope()
	{
	DB("WDS_Scope::WDS_Scope()");

	SetName("Scope");

	SetMinSize(WDS_SCOPE_MINSIZEX,WDS_SCOPE_MINSIZEY);

	SetClipping(EW_CLIPPING_CHILDREN);

	SetEventMask(GetFullEventMask());
	}



/******************************************************************************
void	WDS_Scope::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_Scope::Draw(EW_Redraw *redraw)
	{
	DB("WDS_Scope::Draw()");

#if WDS_COLOR_FILL
	GetRootWindow()->RectFill(truex,truey,truex+sx-1,truey+sy-1,EW_PURPLE);
#endif
#if WDS_COLOR_WIRE
	GetRootWindow()->RectWire(truex,truey,truex+sx-1,truey+sy-1,EW_GREEN);
#endif
	}



/******************************************************************************
long	WDS_Scope::Descend(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_Scope::Descend(EW_Event *event)
	{
#if WDS_SCOPE_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Scope::Descend() (%s)",GetName());

	event->Print((long)this);
	bypass_list.Print("Before");
#endif

#if WDS_HIERDUMP
	if(event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(2),EW_STATE_PRESS) && GetParentWidget()==NULL)
		GetRootWindow()->Dump(TRUE);
	if(event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(3),EW_STATE_PRESS) && GetParentWidget()==NULL)
		GetRootWindow()->Dump(FALSE);
	if(event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(4),EW_STATE_PRESS) && GetParentWidget()==NULL)
		GetRootWindow()->GetEventContext()->Dump((long)this,FALSE);
#endif

#if WDS_LOOKDUMP
	if(event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(5),EW_STATE_PRESS) && GetParentWidget()==NULL)
		GetLooks()->Dump((long)this);
#endif

#if WDS_SCOPELIST_DEBUG

	if(event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(6),EW_STATE_PRESS))
		{
//		event->Print((long)this);
		bypass_list.Print(GetNameString()->Buffer());
		}

#endif

	long result=BypassEvent(event);

#if WDS_SCOPE_DEBUG
	bypass_list.Print("After");
#endif

	if(event->IsOnly(EW_EVENT_KEYBOARD,EW_KEY_CONTROL|EW_KEY_F(1),EW_STATE_PRESS) && GetParentWidget()==NULL)
		{
		WDS_Widget *node,*choice=(WDS_Widget *)event->GetChoice();

		EW_PRINT(EW_WIDGET,EW_LOG,"Dump widget=0x%x (%s%s%s)",
							choice,ewColorCode(EW_CODE_YELLOW),choice? choice->GetName(): "NONE",ewColorCode(EW_CODE_NORMAL));

		if(choice)
			{
			// dump parent
			choice->DumpString();
			if(choice->GetParentWidget());
				choice->GetParentWidget()->DumpInfo(0);

			// dump choice itself
			choice->DumpInfo(1);

			// dump direct children only
			choice->GetWidgetList()->ToHead();
			while( (node=(WDS_Widget *)choice->GetWidgetList()->PostIncrement()) != NULL )
				node->DumpInfo(2);

			WDS_Looks::DumpHeader();
			EW_PRINT(EW_WIDGET,EW_LOG,"Local look:");
			choice->GetLocalLook()->Dump(choice->GetTypeCode(),choice->GetTypeName(),NULL);
			EW_PRINT(EW_WIDGET,EW_LOG,"Interpreted look:");
			choice->GetLocalLook()->Dump(choice->GetTypeCode(),choice->GetTypeName(),choice);
			}
		}

	return result;
	}



/******************************************************************************
WDS_Validifier	*WDS_Scope::RegisterBypass(WDS_Bypass *bypass)

	copies bypass so that caller can't mess with it

Keywords:
******************************************************************************/
WDS_Validifier *WDS_Scope::RegisterBypass(WDS_Bypass *bypass)
	{
#if WDS_REGISTER_DEBUG
	printf("\n");
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Scope::RegisterBypass()");
	bypass->Print((long)this);

	EW_PRINT(EW_WIDGET,EW_LOG,"From:");
	bypass_list.Print(GetNameString()->Buffer());
#endif

	WDS_Bypass *copy=new WDS_Bypass;
	WDS_Validifier *validifier=new WDS_Validifier;

//	printf("%s0x%x->0x%x new 0x%x%s\n",ewColorCode(1),this,&bypass_list,copy,ewColorCode(0));

	osdMemcpy(copy,bypass,sizeof(WDS_Bypass));

	copy->SetValidifier(validifier);
	validifier->SetValid(TRUE);

	// not enabled until scope enables it
	copy->SetEnabled(FALSE);

	bypass_list.ConnectBypass(copy);

#if WDS_REGISTER_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"To:");
	bypass_list.Print(GetNameString()->Buffer());
#endif

	return validifier;
	}



/******************************************************************************
void	WDS_Scope::DeRegisterBypass(WDS_Bypass *bypass)

	De-Register all entries from widget that intersect bypass's event data

	can use event <EW_EVENT_ANY,EW_ITEM_ALL,EW_STATE_ANY> to clear all events

Keywords:
******************************************************************************/
void WDS_Scope::DeRegisterBypass(WDS_Bypass *bypass)
	{
	WDS_Bypass *match;

	EW_Event event;

#if WDS_REGISTER_DEBUG
	printf("\n");
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Scope::DeRegisterBypass()");
	bypass->Print((long)this);

	EW_PRINT(EW_WIDGET,EW_LOG,"From:");
	bypass_list.Print(GetNameString()->Buffer());
#endif

	bypass->CopyDataToEvent(&event);

	bypass_list.ToHead();
	while( (match=bypass_list.MatchEvent(&event,FALSE)) != NULL )
		{
		if( match->GetDestination() == bypass->GetDestination() &&
					( match->GetSource() == bypass->GetSource() || bypass->GetSource()==EW_EVENT_ANY ) &&
					( match->GetItem() & bypass->GetItem() )	&&
					( match->GetState() & bypass->GetState() )	)
			bypass_list.RemoveBypass(match);
		}

#if WDS_REGISTER_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"To:");
	bypass_list.Print(GetNameString()->Buffer());
#endif
	}



/******************************************************************************
WDS_Bypass	*WDS_Scope::FindBypass(EW_Event *event)

	returns the bypass intersected by the given event (mask)

******************************************************************************/
WDS_Bypass *WDS_Scope::FindBypass(EW_Event *event)
	{
	bypass_list.ToHead();
	return bypass_list.MatchEvent(event,FALSE);
	}



/******************************************************************************
long	WDS_Scope::IsTaken(EW_Event *event)

******************************************************************************/
long WDS_Scope::IsTaken(EW_Event *event)
{
	WDS_Bypass *match;
	bypass_list.ToHead();
	while(( match=bypass_list.MatchEvent(event,TRUE)) != NULL )
		{
		if(match->GetFlags() & WDS_BYPASS_FLAG_TAKE) 
			{
				return TRUE;
			}
		}
		return FALSE;
}



/******************************************************************************
long	WDS_Scope::BypassEvent(EW_Event *event)

	watches used events because scopes can make them used
	for their own reference

	returns TRUE if resize may be needed

******************************************************************************/
long WDS_Scope::BypassEvent(EW_Event *event)
	{
	WDS_Bypass *match;
	long return_value=FALSE;
	long idlemouse= (event->IsIdleMouse());

	WDS_Bypass bypass_copy;

	long taken=FALSE;

//	osdMemcpy(&bypass_copy,event,sizeof(EW_Event));

	//bypass_copy.SetMousePosition(-1,-1);

#if WDS_BYPASS_DEBUG
	printf("\nBypassEvent() under scope idlemouse=%d (%s)\n",idlemouse,GetNameString()->Buffer());
	event->Print((long)this);
#endif

	// enable all bypasses on list
	bypass_list.ToHead();
	while( (match=bypass_list.PostIncrement()) != NULL )
		match->SetEnabled(TRUE);

	long pass;
	for(pass=0;pass<2;pass++)
		{
		bypass_list.ToHead();
		while( (idlemouse && (match=(WDS_Bypass *)bypass_list.PostIncrement()) != NULL) ||
																		(match=bypass_list.MatchEvent(event,TRUE)) != NULL )
			{
			osdMemcpy(&bypass_copy,event,sizeof(EW_Event));
//			bypass_copy.CopyDataFromEvent(event);

			// add WDS_BYPASS_EVENT flag
//			bypass_copy.SetWidgetFlags( bypass_copy.GetWidgetFlags() | WDS_BYPASS_EVENT);

#if WDS_BYPASS_DEBUG
			printf("pre (%s)",match->DestinationIsValid()? match->GetDestination()->GetNameString()->Buffer(): "INVALID");
#endif

			if(!pass && (match->GetFlags()&WDS_BYPASS_FLAG_TAKE) )
				{
#if WDS_BYPASS_DEBUG
				printf(" wants to take\n");
#endif

				// NOTE: if taken, will first allow event to propagate normally, but with used and bypass flags set
				if(!idlemouse)
					{
					event->SetUsed(EW_USED_ALL);
					event->SetWidgetFlags( event->GetWidgetFlags() | WDS_BYPASS_EVENT);
					}

				match->SetExclusion(FALSE);

				if(taken && !idlemouse)
					EW_PRINT(EW_EVENT,EW_ERROR,"intersecting exclusive bypasses");
				else
					taken=TRUE;
				}
			else if(pass)
				{
#if WDS_BYPASS_DEBUG
				printf(" not exclusive\n");
#endif
				// add WDS_BYPASS_EVENT flag
				bypass_copy.SetWidgetFlags( bypass_copy.GetWidgetFlags() | WDS_BYPASS_EVENT);

				if( SendBypass(match,&bypass_copy) )
					return_value=TRUE;

				// remove WDS_BYPASS_EVENT flag
				bypass_copy.SetWidgetFlags( (bypass_copy.GetWidgetFlags() | WDS_BYPASS_EVENT) ^ WDS_BYPASS_EVENT);

#if WDS_BYPASS_PROPOGATE
//				bypass_copy.CopyDataToEvent(event);
				osdMemcpy(event,&bypass_copy,sizeof(EW_Event));
#endif
				}

			// tamper proofing
			bypass_list.SetCurrent(match);
			bypass_list.PostIncrement();
			}
		}

#if WDS_BYPASS_DEBUG
	printf("normal pass: idlemouse=%d (%s)\n",idlemouse,GetNameString()->Buffer());
	event->Print((long)this);
#endif

	// enable all bypasses on list	( <-WHAT? )

	if(event->GetSource())
		GetWidgetList()->Descend(event,EW_DESCEND_IN_CHILD_WIDGET);

#if WDS_BYPASS_DEBUG
	printf("normal complete: (%s)\n",GetNameString()->Buffer());
#endif

	bypass_list.ToHead();
	while( (idlemouse && (match=(WDS_Bypass *)bypass_list.PostIncrement()) != NULL) ||
																		(match=bypass_list.MatchEvent(event,TRUE)) != NULL )
		{
#if WDS_BYPASS_DEBUG
		printf("post (%s)",match->DestinationIsValid()? match->GetDestination()->GetNameString()->Buffer(): "INVALID");
#endif

		if( ( (idlemouse && event->InWidget(this)) || (!idlemouse && match->GetFlags()&WDS_BYPASS_FLAG_TAKE) ) &&
																									!(match->GetExclusion()) )
			{
#if WDS_BYPASS_DEBUG
			printf(" taken exclusively or idlemouse\n");
#endif

			if(idlemouse)
				{
				osdMemcpy(&bypass_copy,match,sizeof(WDS_Bypass));

				bypass_copy.SetWidgetFlags( bypass_copy.GetWidgetFlags() | WDS_BYPASS_IN_FOCUS);
				bypass_copy.SetUsed(FALSE);
				}
			else
				{
//				bypass_copy.CopyDataFromEvent(event);
				osdMemcpy(&bypass_copy,event,sizeof(EW_Event));

				bypass_copy.SetUsed(FALSE);
				}

			// add WDS_BYPASS_EVENT flag
			bypass_copy.SetWidgetFlags( bypass_copy.GetWidgetFlags() | WDS_BYPASS_EVENT);

			if( SendBypass(match,&bypass_copy) )
				return_value=TRUE;

			ExcludeAncestors(&bypass_copy);

			// remove WDS_BYPASS_EVENT flag
			bypass_copy.SetWidgetFlags( (bypass_copy.GetWidgetFlags() | WDS_BYPASS_EVENT) ^ WDS_BYPASS_EVENT);

#if WDS_BYPASS_PROPOGATE
			if(!idlemouse)
				{
//				bypass_copy.CopyDataToEvent(event);
				osdMemcpy(event,&bypass_copy,sizeof(EW_Event));
				}
#endif
			}
#if WDS_BYPASS_DEBUG
		else printf(" NOT taken\n");
#endif

		// tamper proofing
		bypass_list.SetCurrent(match);
		bypass_list.PostIncrement();
		}

#if WDS_BYPASS_DEBUG
	printf("bypass done\n");
	event->Print((long)this);
#endif

	return return_value;
	}



/******************************************************************************
long	WDS_Scope::SendBypass(WDS_Bypass *match,WDS_Bypass *bypass)

Keywords:
******************************************************************************/
long WDS_Scope::SendBypass(WDS_Bypass *match,WDS_Bypass *bypass)
	{
	long value=FALSE;

#if WDS_SENDBYPASS_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Scope::SendBypass() to (%s) of",
									match->DestinationIsValid()? match->GetDestination()->GetNameString()->Buffer(): "INVALID");
	bypass->Print((long)bypass);
#endif

	if(!match->DestinationIsValid())
		{
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Scope::SendBypass() invalid destination");
		}
	else if(match->GetFlags() & WDS_BYPASS_FLAG_DESCEND)
		{
		if(match->GetDestination()->Descend(bypass))
			value=TRUE;
		}
	else
		{
		if(match->GetDestination()->PreOperate(bypass))
			value=TRUE;
		}

	return value;
	}



/******************************************************************************
void	WDS_Scope::ExcludeAncestors(WDS_Bypass *bypass)

Keywords:
******************************************************************************/
void WDS_Scope::ExcludeAncestors(WDS_Bypass *bypass)
	{
	WDS_Scope *scope=this;
	WDS_Bypass *match;

	while((scope=(WDS_Scope *)scope->FindWidgetType(GetTypeCode(),TRUE)) != NULL)
		scope->bypass_list.ResolveConflicts(bypass,TRUE);
	}



/******************************************************************************
WDS_Bypass *WDS_BypassList::MatchEvent(EW_Event *event,long if_enabled)

	Note: no Rewind(), searches from current position

Keywords:
******************************************************************************/
WDS_Bypass *WDS_BypassList::MatchEvent(EW_Event *event,long if_enabled)
	{
	WDS_Bypass *next;

#if WDS_MATCH_DEBUG
	printf("\nMatch\n");
	event->Print(NULL);
#endif

	while( (next=PostIncrement()) != NULL )
		{
#if WDS_MATCH_DEBUG
		printf("Versus enabled=%d\n",next->GetEnabled());
		next->Print(NULL);
#endif
		if(!next->DestinationIsValid())
			{
#if !EW_RELEASE_GRADE
			EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Scope::MatchEvent() invalid bypass to 0x%x being deleted",next);
#endif

#if WDS_VALIDIFIER_DEBUG
			Print("Before");
#endif

			RemoveBypass(next);

#if WDS_VALIDIFIER_DEBUG
			Print("After");
#endif
			}
		else if( (!if_enabled || next->GetEnabled()) && next->Is(event->GetSource(),event->GetItem(),event->GetState()) )
			return next;
		}

	return NULL;
	}



/******************************************************************************
void WDS_BypassList::ConnectBypass(WDS_Bypass *bypass)

Keywords:
******************************************************************************/
void WDS_BypassList::ConnectBypass(WDS_Bypass *bypass)
	{
	ResolveConflicts(bypass,FALSE);

	AppendNode(bypass);
	}



/******************************************************************************
void	WDS_BypassList::ResolveConflicts(WDS_Bypass *bypass,long mark)

	if mark==TRUE, mark the exclusion flag instead of removing them

Keywords:
******************************************************************************/
void WDS_BypassList::ResolveConflicts(WDS_Bypass *bypass,long mark)
	{
	WDS_Bypass *match;

	EW_Event eventdata;

	Rewind();
	while( (match=MatchEvent(bypass,FALSE)) != NULL )
		{
		if( match->GetFlags() & WDS_BYPASS_FLAG_TAKE )
			{
			if(mark)
				match->SetExclusion(TRUE);
			else
				{
				/* turn off WDS_BYPASS_FLAG_TAKE for any conflicting bypasses */
				match->SetFlags( match->GetFlags() ^ WDS_BYPASS_FLAG_TAKE );

				/* if the match is from the same widget */
				if( match->GetDestination() == bypass->GetDestination() )
					{
					/* remove old bypass */
					RemoveBypass(match);
					}
				else
					{
					/* inform other widget that its bypass was degraded */
					match->CopyDataToEvent(&eventdata);
					eventdata.SetWidgetFlags( eventdata.GetWidgetFlags() | WDS_BYPASS_EVENT | WDS_BYPASS_TAKE_LOST );

					if(match->DestinationIsValid())
						match->GetDestination()->PreOperate(&eventdata);
					}
				}
			}
		}
	}



/******************************************************************************
void WDS_BypassList::RemoveBypass(WDS_Bypass *bypass)

Keywords:
******************************************************************************/
void WDS_BypassList::RemoveBypass(WDS_Bypass *bypass)
	{
//	printf("%s0x%x delete 0x%x%s\n",ewColorCode(1),this,bypass,ewColorCode(0));

	RemoveNode(bypass);
	bypass->Invalidate();
	delete bypass;
	}



/******************************************************************************
		WDS_Scope::~WDS_Scope(void)

******************************************************************************/
WDS_Scope::~WDS_Scope(void)
	{
//	printf("WDS_Scope::~WDS_Scope() 0x%x (%s)\n",this,GetName());
	}



/******************************************************************************
		WDS_BypassList::~WDS_BypassList(void)

******************************************************************************/
WDS_BypassList::~WDS_BypassList(void)
	{
/*
	printf("WDS_BypassList::~WDS_BypassList() 0x%x\n",this);
	Print("Destruct");
*/

	WDS_Bypass *node;

	ToHead();
	while( (node=PostIncrement()) != NULL )
		{
		RemoveBypass(node);
		}
	}



/******************************************************************************
void	WDS_BypassList::Print(char name)

Keywords:
******************************************************************************/
void WDS_BypassList::Print(char *name)
	{
	char message[200];

	WDS_Bypass *next;

	sprintf(message,"---------- BYPASS LIST: (%s) ----------",name);
	EW_PRINT(EW_WIDGET,EW_LOG,message);

	Rewind();
	while( (next = Next()) != NULL )
		{
		sprintf(message,"BYPASS LIST ENTRY: flags=%d widget=0x%x valid=%d/0x%x (%s)",next->GetFlags(),next->GetDestination(),
							next->GetValidifier()->GetValid(),next->GetValidifier(),
							next->DestinationIsValid()? next->GetDestination()->GetNameString()->Buffer(): "INVALID");
		EW_PRINT(EW_WIDGET,EW_LOG,message);

		next->EW_Event::Print((long)next->GetDestination());
		}

	sprintf(message,"---------- END OF LIST: (%s) ----------",name);
	EW_PRINT(EW_WIDGET,EW_LOG,message);
	}
