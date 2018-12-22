/******************************************************************************

Coral Tree wds/textlist.cc
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


#define WDS_TEXTLIST_SELECT_DEBUG	FALSE
#define WDS_TEXTLIST_RESIZE_DEBUG	FALSE
#define	WDS_TEXTLIST_TAKE_DEBUG		FALSE

#define WDS_TEXTLIST_DEBUG_NAME		"Table TL 0"

#define WDS_TEST_STRING				FALSE
#define WDS_TEST_STRINGS			26
#define WDS_TEXTLIST_BEVEL			TRUE

static char wdsTestText[WDS_TEST_STRINGS][32]=
	{
	"Alpha",	"Bravo",	"Charlie",	"Delta",	"Echo",		"Foxtrot",	"Golf",
	"Hotel",	"India",	"Juliet",	"Kilo",		"Lima",		"Mike",		"November",
	"Oscar",	"Papa",		"Quebec",	"Romeo",	"Sierra",	"Tango",	"Uniform",
	"Victor",	"Whiskey",	"X-ray",	"Yankee",	"Zulu"
	};



/******************************************************************************
		WDS_TextList::WDS_TextList(void)

Keywords:
******************************************************************************/
WDS_TextList::WDS_TextList(void)
	{
	SetName("TextList");

	stringwidget.SetName("TL GetString");

	selected_number= -1;
	SetEditable(WDS_TEXTLIST_EDITABLE_NONE);
	SetShielded(0);
	SetOffset(0.0);
	SetDrawOffset(0);
	SetDrawLength(0);
	SetFlags(WDS_TEXTLIST_NULL);
	SetStringList(&defaultlist);

	// arbitrary
//	SetPrefSize(80,0);
//	SetSize(80,0);

	WDS_Activation activation;
	activation.SetTargetWidget(this);
	activation.SetCode(0);

	stringwidget.SetStateChangeCallback(WDS_STATE_0,&WDS_TextList::StringDeActivationCallback,&activation);

    SetEventMask(GetLocalEventMask());
    GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS);

#if WDS_TEST_STRING
	stringlist=new WDS_StringList;
	long m;
	WDS_String *string;
	for(m=0;m<WDS_TEST_STRINGS;m++)
		{
		string=new WDS_String;
		string->NewBuffer(wdsTestText[m]);
		stringlist->AppendNode(string);
		}
#endif
	}



/******************************************************************************
void	WDS_TextList::TransferString(long done)

	done:
		FALSE = start editing string
		TRUE  = done editing string

Keywords:
******************************************************************************/
void WDS_TextList::TransferString(long done)
	{
	long entrynumber=GetSelectedNumber();

	// no transfers on uneditable fields
	if(!IsEditable(entrynumber))
		return;

//	printf("0x%x edit %d,%d\n",(long)this,done,entrynumber);

	WDS_String *node=stringlist->ToIndex(entrynumber);

	if(node==NULL)
		{
		char message[128];

		sprintf(message,"WDS_TextList::TransferString(%d) selected number %d beyond list length %d",
																			done,entrynumber,stringlist->GetNumberNodes());
		EW_PRINT(EW_WIDGET,EW_ERROR,message);

		return;
		}

	if(done)
		node->CopyFromString(stringwidget.GetString());
	else
		stringwidget.SetString(node->Buffer());
	}



/******************************************************************************
void	WDS_TextList::SetSelectedNumber(long set)

******************************************************************************/
void WDS_TextList::SetSelectedNumber(long set)
	{
	if(set==selected_number)
		return;

	last_selected=selected_number;

	if(GetRootWindow()->GetDoubleBuffer() || GetDirty()&EW_DIRTY_PARTIAL)
		SetDirty(EW_DIRTY_COMPLETE);
	else
		SetDirty(EW_DIRTY_PARTIAL);

	if(selected_number>=0)
		TransferString(TRUE);

	selected_number=set;

	if(selected_number>=0)
		TransferString(FALSE);
	}



/******************************************************************************
char	*WDS_TextList::GetSelectedString(void)

******************************************************************************/
char *WDS_TextList::GetSelectedString(void)
{
	long index;
	char *returnval;
	index = GetSelectedNumber();

	WDS_String *s;
	stringlist->ToHead();
	while(s = stringlist->PostIncrement())
	{
		if(!index)
		{
			returnval = strdup(s->Buffer());
			return returnval;
		}
		index--;
	}
	return NULL;
}



/******************************************************************************
void	WDS_TextList::StringDeActivationCallback(WDS_Activation *activation)

	end editing string

Keywords:
******************************************************************************/
void WDS_TextList::StringDeActivationCallback(WDS_Activation *activation)
	{
	WDS_TextList *textlist=(WDS_TextList *)activation->GetTargetWidget();

	textlist->SetSelectedNumber(-1);

	textlist->ChangeState(WDS_STATE_0,activation->GetEvent());
	}



/******************************************************************************
long	WDS_TextList::GetHeaderHeight(void)

Keywords:
******************************************************************************/
long WDS_TextList::GetHeaderHeight(void)
	{
	return GetHeight(WDS_STATE_0);
	}



/******************************************************************************
long	WDS_TextList::GetEntryHeight(void)

Keywords:
******************************************************************************/
long WDS_TextList::GetEntryHeight(void)
	{
	return GetHeight(WDS_STATE_3);
	}



/******************************************************************************
long	WDS_TextList::GetHeight(void)

Keywords:
******************************************************************************/
long WDS_TextList::GetHeight(long state)
	{
#if TRUE

	SetLabel("Qy,");

	long size[2],maxheight=0;

	long m;
	for(m=0;m<3;m++)
		{
		size[0]= -1;
		size[1]= -1;

		FindSizeByFlags(state+m,WDS_WIDGET_FLAGS_AUTOSIZE,size);

		if(m==0 || maxheight<size[1])
			maxheight=size[1];
		}

	return maxheight;

#else

	long inset=	EW_MAX( InterpretElement(state,WDS_ELEMENT_INSET),
						InterpretElement(state+1,WDS_ELEMENT_INSET) )+
				EW_MAX( InterpretElement(state,WDS_ELEMENT_INSET_MODY),
						InterpretElement(state+1,WDS_ELEMENT_INSET_MODY) );

	long depth=EW_MAX( InterpretElement(state,WDS_ELEMENT_DEPTH),
						InterpretElement(state+1,WDS_ELEMENT_DEPTH) );

	return 2*(depth+inset)+GetRootWindow()->GetFontHeight();

#endif
	}



/******************************************************************************
void	WDS_TextList::GetAlteredGeometry(long *ax,long *ay,long *asx,long *asy)

	get true geometry altered by GetDrawOffset() and GetDrawLength()
	alter only if GetDrawLength()>0

Keywords:
******************************************************************************/
void WDS_TextList::GetAlteredGeometry(long *ax,long *ay,long *asx,long *asy)
	{
	GetTrueGeometry(ax,ay,asx,asy);

	// alter "true geometry"
	if(GetDrawLength()>0)
		{
		*ay+= *asy-GetDrawLength()-GetDrawOffset();
		*asy=GetDrawLength();
		}
	}



/******************************************************************************
void	WDS_TextList::AlterRowHighlight(long row,long on)

******************************************************************************/
void WDS_TextList::AlterRowHighlight(long row,long on)
	{
	WDS_String *string=stringlist->ToIndex(row);

	if(string)
		string->SetHighlight(on);
	}



/******************************************************************************
void	WDS_TextList::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_TextList::Draw(EW_Redraw *redraw)
	{
	EW_Window *root=GetRootWindow();
	WDS_String *string;

	long truex,truey,sx,sy;
	long altx,alty,asx,asy;
	long clipx,clipy,clipsx,clipsy;
	long m=0;
	long offset_l;
	long dy=1;

	long save_active=GetWidgetState();

	long partial= (!redraw->GetForce() && GetDirty()&EW_DIRTY_PARTIAL);

	offset_l=(long)(GetOffset());

	GetTrueGeometry(&truex,&truey,&sx,&sy);
	GetAlteredGeometry(&altx,&alty,&asx,&asy);

    long rx,ry,rsx,rsy;
    GetRegion(FALSE,&rx,&ry,&rsx,&rsy);
	ry+= alty-truey;
	rsy+= asy-sy;

	root->PushClipRegion();
	root->IntersectClipRegion(altx,alty,asx,asy);

	long cly=GetHeaderHeight();

	stringlist->ToHead();
	while( m<GetShielded() && ((string=stringlist->PostIncrement()) != NULL) )
		{
		dy=asy-cly*(m+1);

		if(GetRootWindow()->ClipRegionIntersectsBounds(altx,alty+dy,asx,cly))
			{
			if(IsEditable(m))
				SetWidgetState(WDS_STATE_0);
			else
				SetWidgetState( (m==GetSelectedNumber())? WDS_STATE_2: WDS_STATE_1 );

			// restore index
			stringlist->SetCurrent(string);
			stringlist->PostIncrement();

			if(!partial || m==selected_number || m==last_selected)
				{
#if WDS_TEXTLIST_BEVEL
				BevelBox(TRUE,TRUE,string,altx,alty+dy,asx,cly);
#else
				string->Operate(root,altx,alty+dy,asx,cly,0,0,WDS_STRING_DRAW);
#endif
				}
			}
		m++;
		}

	root->PushClipRegion();
	root->IntersectClipRegion(altx,alty,asx,asy-cly*GetShielded());
	root->GetClipBounds(&clipx,&clipy,&clipsx,&clipsy);

/*
printf("TL alt %d,%d %d,%d-%d clip %d,%d %d,%d (%s)\n",
				altx,alty,asx,asy,cly*GetShielded(),clipx,clipy,clipsx,clipsy,GetName());
*/

	cly=GetEntryHeight();

	long dy1=(long)(asy+cly*GetOffset());

	while( ((string=stringlist->PostIncrement()) != NULL) )
		{
		dy=dy1-cly*(m+1);

		if(GetRootWindow()->ClipRegionIntersectsBounds(altx,alty+dy,asx,cly) &&
																		m>=offset_l+GetShielded() && (alty+dy+cly)>clipy )
			{
			if(IsEditable(m))
				SetWidgetState(WDS_STATE_3);
			else
				SetWidgetState( (m==GetSelectedNumber())? WDS_STATE_5: WDS_STATE_4 );

			long highlightbg=InterpretElement(GetWidgetState(),WDS_ELEMENT_TEXTLIST_HIGHLIGHT_BG);

			// restore index
			stringlist->SetCurrent(string);
			stringlist->PostIncrement();

			if(!partial || m==selected_number || m==last_selected)
				{
				BevelBox(TRUE,TRUE,NULL,altx,alty+dy,asx,cly);

				if(string->GetHighlight())
					GetRootWindow()->LineOut(altx,alty+dy,asx,cly,0,3,highlightbg);

				GetRootWindow()->SetColor(EW_COLOR_PEN,InterpretElement(GetWidgetState(),EW_COLOR_PEN));
#if FALSE
				// string not aligned according to look properties
				string->Operate(root,altx,alty+dy,asx,cly,0,0,WDS_STRING_DRAW);
#else
				long border=GetLocalElement(GetWidgetState(),WDS_ELEMENT_BORDER);
				BevelBox(FALSE,FALSE,string,altx,alty+dy,asx,cly);
				SetLocalElement(GetWidgetState(),WDS_ELEMENT_BORDER,border);
#endif
				}
			}
		m++;
		}

	root->PopClipRegion();
	root->PopClipRegion();

#if FALSE
	root->RectWire(truex,truey,truex+sx-1,truey+sy-1,EW_GREEN);
	root->RectWire(altx,alty,altx+asx-1,alty+asy-1,EW_LIGHTGREEN);

	if( !GetNameString()->Compare(WDS_TEXTLIST_DEBUG_NAME) )
		printf("%d,%d %d,%d -> %d,%d %d,%d (%d,%d)\n",truex,truey,sx,sy,altx,alty,asx,asy,GetDrawOffset(),GetDrawLength());
#endif

	SetWidgetState(save_active);

	// set clipping for children (GetString)
	SetClipRegion(altx,alty,asx,asy-cly*GetShielded());
	SetClipping( (GetSelectedNumber()>=GetShielded())? EW_CLIPPING_CHILDREN: EW_CLIPPING_OFF);
	}



/******************************************************************************
long	WDS_TextList::NumberOfLocation(long mousey)

	returns header/entry number based on mouse y location assumed within list

Keywords:
******************************************************************************/
long WDS_TextList::NumberOfLocation(long mousey)
	{
	long pos[2];
	long size[2];

	GetAlteredGeometry(&pos[0],&pos[1],&size[0],&size[1]);

	long dy=pos[1]+size[1]-1-mousey;

	long cly=GetHeaderHeight();
	long entry=dy/cly;

	if(entry<GetShielded())
		return entry;

	dy-=GetShielded()*cly;

	cly=GetEntryHeight();
	entry=(long)(GetShielded()+dy/(float)cly+GetOffset());

	return entry;
	}



/******************************************************************************
long	WDS_TextList::LocationOfNumber(long number)

	returns y location based on header/entry number

Keywords:
******************************************************************************/
long WDS_TextList::LocationOfNumber(long number)
	{
	long pos[2];
	long size[2];

	GetAlteredGeometry(&pos[0],&pos[1],&size[0],&size[1]);

	long cly=GetHeaderHeight();
	long dy=pos[1]+size[1];
	long location=pos[1]+size[1]-cly*(number+1);

	if(number<GetShielded())
		return location;

	location=dy-cly*(GetShielded()+1);

	cly=GetEntryHeight();

	location-=(long)(cly*(number-GetShielded()-GetOffset()));

	return location;
	}



/******************************************************************************
long	WDS_TextList_GetString::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_TextList_GetString::Operate(EW_Event *event)
	{
	// if disconnected, do not act upon event (probably residually bypassed)
	if(GetRootWindow()==NULL)
		{
#if WDS_TEXTLIST_SELECT_DEBUG
#endif
		EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TextList_GetString::Operate() detached widget received event (residually bypassed?)");
		EW_PRINT(EW_WIDGET,EW_LOG,"   (%s) string (%s) -> (%s)",GetNameString()->Buffer(),
																			GetStoreBuffer()->Buffer(),GetString()->Buffer());
		event->Print((long)this);

		StopEdit(FALSE,event);
		return FALSE;
		}

#if WDS_TEXTLIST_SELECT_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TextList_GetString::Operate()");
	event->Print((long)this);
#endif

	return WDS_GetString::Operate(event);
	}



/******************************************************************************
long	WDS_TextList::IsEditable(long index)

******************************************************************************/
long WDS_TextList::IsEditable(long index)
	{
	WDS_String *string;

	return ( (GetEditable()&(index<GetShielded()? WDS_TEXTLIST_EDITABLE_HEADER: WDS_TEXTLIST_EDITABLE_ENTRY)) &&
															(string=stringlist->ToIndex(index)) && string->GetEditable() );
	}



/******************************************************************************
long	WDS_TextList::Operate(EW_Event *event)

******************************************************************************/
long WDS_TextList::Operate(EW_Event *event)
	{
	if(event->GetUsed())
		return FALSE;

#if WDS_TEXTLIST_SELECT_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_TextList::Operate()");
	event->Print((long)this);

	WDS_String *string;

	if(selected_number>=0)
		string=GetStringList()->ToIndex(selected_number);
	else
		string=NULL;

	printf("selected entry = %d (%s)\n",selected_number,string? string->Buffer(): "<NULL>");
#endif

	long mouse[2];
	event->GetMousePosition(&mouse[0],&mouse[1]);

	long lastentrynumber=GetSelectedNumber();
	long entrynumber=NumberOfLocation(mouse[1]);

	long editable=IsEditable(entrynumber);

	if(event->GetWidgetFlags()&WDS_BYPASS_EVENT)
		{
		if( (event->GetWidgetFlags() & WDS_BYPASS_TAKE_LOST) )
			{
#if WDS_TEXTLIST_SELECT_DEBUG
			printf("take lost state=%d\n",GetWidgetState());
#endif

			TakeKeyboard(FALSE);

			SetSelectedNumber(-1);
			AdjustStringWidget();
			}

		return FALSE;
		}

	long idleecho=InterpretElement(GetWidgetState(),WDS_ELEMENT_TEXTLIST_IDLE_ECHO);

	if( ((idleecho && event->IsIdleMouse()) || event->IsMouseMotion()) &&
														event->InWidget(this) && !editable && entrynumber>=GetShielded())
		{
		if(GetSelectedNumber()!=entrynumber)
			{
			SetSelectedNumber(entrynumber);

			ChangeState(WDS_STATE_3,event);
			}

		event->SetUsed(EW_USED_ALL);
		}
	else if(event->InWidget(this) && event->IsLeftMouseButton())
		{
#if WDS_TEXTLIST_SELECT_DEBUG
		printf("new entrynumber %d\n",entrynumber);
#endif

		TakeKeyboard(FALSE);

		if(!editable)
			{
			if(entrynumber>=GetShielded())
				{
				TakeKeyboard(TRUE);
				TakeKeyboard(FALSE);

				SetSelectedNumber(entrynumber);

				if(event->IsLeftMousePress())
					{
					// if clicked from different area, not really a multi-click
					if(entrynumber!=lastentrynumber || event->GetLastChoice()!=this)
						event->SetRepetition(0);

					ChangeState(WDS_STATE_2,event);
					}
				else
					ChangeState(WDS_STATE_4,event);

				event->SetUsed(EW_USED_ALL);
				}
			}
//		else if(entrynumber!=GetSelectedNumber() && event->IsLeftMousePress())
		else
			{
			// steal from previous take
			TakeKeyboard(TRUE);
			TakeKeyboard(FALSE);

			if(stringwidget.GetWidgetState())
				stringwidget.ChangeState(WDS_STATE_0,event);

			SetSelectedNumber(entrynumber);
			ChangeState(WDS_STATE_1,event);

			// don't use: let WDS_TextList_GetString pick it up
//			event->SetUsed(EW_USED_ALL);
			}

		AdjustStringWidget();
		}
	else
		last_selected= -1;

#if WDS_TEXTLIST_SELECT_DEBUG
		printf(" entrynumber remains %d\n",GetSelectedNumber());
#endif

	return FALSE;
	}



/******************************************************************************
void	WDS_TextList::TakeKeyboard(long on)

	take keyboard focus away from any other widget who has it

	used to make uneditable entries follow the same behavior as
		editable entries

******************************************************************************/
void WDS_TextList::TakeKeyboard(long on)
	{
#if WDS_TEXTLIST_TAKE_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG," WDS_TextList::TakeKeyboard(%d) from (%s) flags=%d/%d",on,GetNameString()->Buffer(),
																			GetFlags(),WDS_TEXTLIST_UNEDITABLE_GRAB_KEYBD);
#endif

	if(!GetFlags()&WDS_TEXTLIST_UNEDITABLE_GRAB_KEYBD)
		return;

	EW_Event event;
    long shallow=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_SCOPE_SHALLOW;

	event.SetSIS(EW_EVENT_KEYBOARD,EW_KEY_ALL,EW_STATE_ANY);

	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_TAKE,shallow);
	}



/******************************************************************************
void	WDS_TextList::AdjustStringWidget(void)

Keywords:
******************************************************************************/
void WDS_TextList::AdjustStringWidget(void)
	{
	long pos[2];
	long size[2];
	long apos[2];
	long asize[2];

	GetTrueGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	GetAlteredGeometry(&apos[0],&apos[1],&asize[0],&asize[1]);

	// adjust stringwidget
	long entrynumber=GetSelectedNumber();
	long editable=IsEditable(entrynumber);

	if(!editable)
		{
		entrynumber=0;
		stringwidget.SetDormant(TRUE);
		stringwidget.SetInvisible(TRUE);
		}
	else
		{
		stringwidget.SetDormant(FALSE);
		stringwidget.SetInvisible(FALSE);
		}

	if(entrynumber>=0 && editable)
		{
		long location=LocationOfNumber(entrynumber);
		long height= (entrynumber<GetShielded())? GetHeaderHeight(): GetEntryHeight();

		// if not a child, add
		if(!GetWidgetList()->SetCurrent(&stringwidget))
			AddWidget(&stringwidget);

		// relative position
		stringwidget.SetGeometry(0,location-pos[1],size[0],height);
		}
	else
		{
		// if a child, remove
		if(GetWidgetList()->SetCurrent(&stringwidget))
			RemoveWidget(&stringwidget);
		}
	}



/******************************************************************************
void	WDS_TextList::SuggestPrefSize(long prefx,long prefy)

******************************************************************************/
void WDS_TextList::SuggestPrefSize(long prefx,long prefy)
	{
	long lastpref[2];

	GetPrefSize(&lastpref[0],&lastpref[1]);

	SetPrefSize(prefx,lastpref[1]);
	}



/******************************************************************************
long	WDS_TextList::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_TextList::Resize(EW_Resize *resize)
	{
	long pref[2];

#if WDS_TEXTLIST_RESIZE_DEBUG
	if( !GetNameString()->Compare(WDS_TEXTLIST_DEBUG_NAME) )
		resize->Print((long)this);
#endif

	GetPrefSize(&pref[0],&pref[1]);

	pref[1]=0;

	if(GetStringList())
		pref[1]+=GetEntryHeight()*GetStringList()->GetNumberNodes();

	SetPrefSize(pref[0],pref[1]);

#if WDS_TEXTLIST_RESIZE_DEBUG
	if( !GetNameString()->Compare(WDS_TEXTLIST_DEBUG_NAME) )
		{
		printf("TL %d,%d ",pref[0],pref[1]);
		GetSize(&pref[0],&pref[1]);
		printf("%dx%d ",pref[0],pref[1]);
		resize->GetPrefSize(&pref[0],&pref[1]);
		printf("(%d,%d) -> ",pref[0],pref[1]);
		}
#endif

    long resized=CheckSize(resize);

	AdjustStringWidget();

    widgetlist->Resize(resize);

#if WDS_TEXTLIST_RESIZE_DEBUG
	if( !GetNameString()->Compare(WDS_TEXTLIST_DEBUG_NAME) )
		{
		GetSize(&pref[0],&pref[1]);
		printf("%dx%d\n",pref[0],pref[1]);
		}
#endif

    return FALSE;
	}
