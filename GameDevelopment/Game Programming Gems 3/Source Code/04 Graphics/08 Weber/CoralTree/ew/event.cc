/******************************************************************************

Coral Tree ew/event.cc
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

#include "ew.h"


#define	EW_EVENT_PRINTSOURCE	(EW_EVENT_ANY)	// events reported by EW_Event::Print() try EW_EVENT_ANY or EW_EVENT_COMMON
#define	EW_EVENT_QUEUE_DEBUG	FALSE



/******************************************************************************
		EW_Event::EW_Event(void)

******************************************************************************/
EW_Event::EW_Event(void)
	{
	clipregion=NULL;
	Reset();
	}



/******************************************************************************
		EW_Event::~EW_Event(void)

******************************************************************************/
EW_Event::~EW_Event(void)
	{
	if(clipregion)
		delete clipregion;
	}



/******************************************************************************
void	EW_Event::Reset(void)

******************************************************************************/
void EW_Event::Reset(void)
	{
	destination=NULL;
	source=EW_EVENT_NONE;
	item=EW_ITEM_NONE;
	state=EW_STATE_NULL;
	choice=NULL;
	lastchoice=NULL;
	mx= -1;
	my= -1;
	mousebuttons=0;
	widget_flags=0;
	time=0;
	repetition=0;
	dislocated=FALSE;
	used=FALSE;

	if(clipregion)
		{
		delete clipregion;
		clipregion=NULL;
		}
	}



/******************************************************************************
void    EW_Event::CopyFrom(EW_Event *other)

******************************************************************************/
void EW_Event::CopyFrom(EW_Event *other)
	{
	destination=other->destination;
	source=other->source;
	item=other->item;
	state=other->state;
	choice=other->choice;
	lastchoice=other->lastchoice;
	mx=other->mx;
	my=other->my;
	mousebuttons=other->mousebuttons;
	widget_flags=other->widget_flags;
	time=other->time;
	repetition=other->repetition;
	used=other->used;

	if(clipregion)
		{
		delete clipregion;
		clipregion=NULL;
		}

	if(other->clipregion)
		{
		clipregion= new EW_ClipRegion;
		clipregion->CopyFrom(other->clipregion);
		}
	}



/******************************************************************************
long	EW_Event::IsKeyOfState(long key,long state)

******************************************************************************/
long EW_Event::IsKeyOfState(long key,long state)
	{
	long mod=item;
	long item2= (item&EW_KEY_NO_MOD_MASK);
	long key2=  (key&EW_KEY_NO_MOD_MASK);

	if(mod>='A' && mod<='Z')
		mod|=EW_KEY_SHIFT;

	if(key>='A' && key<='Z')
		key|=EW_KEY_SHIFT;

	long same_nonindexed= ( (item2>=EW_KEY_INDEXED_KEYS || key2>=EW_KEY_INDEXED_KEYS) && (item2&key2));

	long same_mod= ( (mod&EW_KEY_MOD_MASK) == (key&EW_KEY_MOD_MASK) || mod==EW_KEY_ALL || key==EW_KEY_ALL);

//	printf("mod=0x%x key=0x%x same_nonindexed=%d same_mod=%d\n",mod,key,same_nonindexed,same_mod);

	return (Is(EW_EVENT_KEYBOARD,EW_ITEM_ALL,state) && ((item2==key2 || same_nonindexed) && same_mod) );
	}



/******************************************************************************
void    EW_Event::MapControlCharacters(void)

    map control codes into just original character plus control bit

******************************************************************************/
void EW_Event::MapControlCharacters(void)
	{
//  EW_PRINT(EW_WINDOW,EW_LOG,"source=0x%x",source);

	if(GetSource()!=EW_EVENT_KEYBOARD)
		return;

	// convert carriage-return's to '\n'
	if(GetItem()==EW_KEY_CR)
		SetItem(EW_KEY_RETURN);

	long item= GetItem();
	long ascii= (item&EW_KEY_ALL_ASCII);

//  EW_PRINT(EW_WINDOW,EW_LOG,"ascii=%d",ascii);

	// leave recognized keys alone
	if(ascii==EW_KEY_BACKSPACE || ascii==EW_KEY_RETURN || ascii==EW_KEY_CR || ascii==EW_KEY_ESC || ascii==EW_KEY_DELETE )
		return;
	if(item&EW_KEY_NON_ASCII_FLAG)
		return;

	// zero is OK only if not a cursor or F-key
	if(ascii==0x00 && (item & (EW_KEY_F_KEYS|EW_KEY_ANY_CURSOR)) )
		return;

	// control codes
	if(ascii<0x20)
		{
		if(ascii<0x1b)          // `a-z
			ascii+='`';
		else if(ascii<0x1e)     // [\]
			ascii+='@';
		else if(ascii==0x1e)    // ~/
			ascii='~';
		else // 0x1f
			ascii='/';

		// make sure shifted chars are uppercase
		if((item&(EW_KEY_SHIFT|EW_KEY_CAPS_LOCK)) && ascii>='a' && ascii<='z')
			ascii += ('A'-'a');

		item &= EW_KEY_ASCII_NOT;
		item |= EW_KEY_CONTROL;
		item |= ascii;

//      EW_PRINT(EW_WINDOW,EW_LOG," to %d item=0x%x",ascii,item);

		SetItem(item);
		}
	}




/******************************************************************************
long    EW_Event::Print(long hexcode)

    print event data for debugging

    returns TRUE if printed (not masked out)

Keywords:
******************************************************************************/
long EW_Event::Print(long hexcode)
	{
	char ew_print_message[1024];
	char source_note[64];
	char item_note[64];
	char state_note[64];
	char used_note[64];
	char widget_note[64];
	char mouse_note[64];
	char temp[256];

	source_note[0]=0;
	item_note[0]=0;
	state_note[0]=0;
	used_note[0]=0;
	widget_note[0]=0;
	mouse_note[0]=0;

	EW_Event mask;
	mask.SetSISW(EW_EVENT_PRINTSOURCE,EW_ITEM_ALL,EW_STATE_ANY,EW_WIDGET_FLAGS_ALL);
	if(!Intersects(&mask))
		return FALSE;

	if(item==EW_ITEM_ALL)
		sprintf(item_note,"ALL");
	else
		sprintf(item_note,"???");

	if(state==EW_STATE_ANY)
		sprintf(state_note,"ANY");
	else if(source&EW_EVENT_COMMON)
		{
		if(state==EW_STATE_NULL)
			sprintf(state_note,"NULL");
		else if(state==EW_STATE_PRESS)
			sprintf(state_note,"PRESS");
		else if(state==EW_STATE_REPEAT)
			sprintf(state_note,"REPEAT");
		else if(state==EW_STATE_RELEASE)
			sprintf(state_note,"RELEASE");
		}

	if(source==EW_EVENT_NONE)
		sprintf(source_note,"NONE");
	else if(source==EW_EVENT_MOUSEPOSITION)
		{
		sprintf(source_note,"POS");
		if( (item&EW_KEY_NO_MOD_MASK) == (EW_ITEM_X|EW_ITEM_Y) )
			sprintf(item_note,"XY");
		}
	else if(source==EW_EVENT_MOUSEBUTTON)
		{
		sprintf(source_note,"BUTTON");
		if((item&EW_KEY_NO_MOD_MASK)==EW_ITEM_LEFT)
			sprintf(item_note,"LEFT");
		else if((item&EW_KEY_NO_MOD_MASK)==EW_ITEM_MIDDLE)
			sprintf(item_note,"MIDDLE");
		else if((item&EW_KEY_NO_MOD_MASK)==EW_ITEM_RIGHT)
			sprintf(item_note,"RIGHT");
		else if((item&EW_KEY_NO_MOD_MASK)==EW_ITEM_WHEEL)
			{
			sprintf(item_note,"WHEEL");
			if(state>0)
				sprintf(state_note,"UP");
			else if(state<0)
				sprintf(state_note,"DOWN");
			}
		}
	else if(source==EW_EVENT_KEYBOARD)
		{
		sprintf(source_note,"KEY");

		long item2= (item&EW_KEY_NO_MOD_MASK);

		if(item==EW_KEY_ALL)
			sprintf(item_note,"ALL_KEYS");
		else if(item2==EW_KEY_BACKSPACE)
			sprintf(item_note,"BS");
		else if(item2==EW_KEY_RETURN)
			sprintf(item_note,"RET");
		else if(item2==EW_KEY_CR)
			sprintf(item_note,"CR (warning!)");
		else if(item2==EW_KEY_ESC)
			sprintf(item_note,"ESC");
		else if(item2==EW_KEY_DELETE)
			sprintf(item_note,"DEL");
		else if(item2==EW_KEY_ALL_ASCII)
			sprintf(item_note,"ALL_ASCII");
		else if(item2==EW_KEY_HOME)
			sprintf(item_note,"HOME");
		else if(item2==EW_KEY_END)
			sprintf(item_note,"END");
		else if(item2==EW_KEY_INSERT)
			sprintf(item_note,"INSERT");
		else if(item2==EW_KEY_TAB)
			sprintf(item_note,"TAB");
		else if(item2==EW_KEY_PAUSE)
			sprintf(item_note,"PAUSE");
		else if(item2==EW_KEY_PAGE_UP)
			sprintf(item_note,"PgUp");
		else if(item2==EW_KEY_PAGE_DOWN)
			sprintf(item_note,"PgDn");
		else if(item2==EW_KEY_F_KEYS)
			sprintf(item_note,"F-KEYS");
		else if(item2&EW_KEY_F_KEYS && (item2>>EW_KEY_F_KEY_SHIFT)>0 && (item2>>EW_KEY_F_KEY_SHIFT)<17 )
			sprintf(item_note,"F%d",(int)(item2>>EW_KEY_F_KEY_SHIFT));
		else if(item2==EW_KEY_CURSOR_UP)
			sprintf(item_note,"UP");
		else if(item2==EW_KEY_CURSOR_DOWN)
			sprintf(item_note,"DOWN");
		else if(item2==EW_KEY_CURSOR_LEFT)
			sprintf(item_note,"LEFT");
		else if(item2==EW_KEY_CURSOR_RIGHT)
			sprintf(item_note,"RIGHT");
		else if(item2==EW_KEY_ANY_CURSOR)
			sprintf(item_note,"ANY_CURSOR");
		else if(item&EW_KEY_UNUSED_MASK)
			sprintf(item_note,"UNUSED_MASK?");
		else if(item2&EW_KEY_ANY_CURSOR)
			sprintf(item_note,"CURSOR");
		else if(item2<128)
			sprintf(item_note,"%c",(char)(item));
		else
			sprintf(item_note,"MASK?");
		}
	else if(source==EW_EVENT_FOCUS)
		{
		sprintf(source_note,"FOCUS");
		if(item==EW_ITEM_FOCUSCHANGE)
			{
			sprintf(item_note,"CHANGE");

			EW_Window *target=(EW_Window *)state;
			if(target)
				sprintf(state_note,"(%s)",target->GetTitle());
			else
				sprintf(state_note,"LEAVE");
			}
		else if(item==EW_ITEM_IDLEMOUSE)
			sprintf(item_note,"IDLE");
		}
	else if(source==EW_EVENT_CLIPBOARD)
		{
		sprintf(source_note,"CLIPBOARD");
		if(item==EW_ITEM_CLIPBOARD_READY)
			sprintf(item_note,"READY");
		else if(item==EW_ITEM_CLIPBOARD_FAIL)
			sprintf(item_note,"FAIL");
		}
	else if(source==EW_EVENT_PERIODIC)
		{
		sprintf(source_note,"PERIODIC");
		if(item==EW_ITEM_WORK)
			sprintf(item_note,"WORK");
		else if(item==EW_ITEM_TIMER)
			sprintf(item_note,"TIMER");
		}
	else if(source==EW_EVENT_SYSTEM)
		{
		sprintf(source_note,"SYS");
		if(item==EW_ITEM_CLOSEWINDOW)
			sprintf(item_note,"CLOSE");
		else if(item==EW_ITEM_QUIT_APP)
			sprintf(item_note,"QUIT_APP");
		else if(item==EW_ITEM_EXPOSURE)
			{
			sprintf(item_note,"EXPO");

			EW_Window *target=(EW_Window *)state;
			if(target)
				sprintf(state_note,"(%s)",target->GetTitle());
			}
		}
	else if(source&EW_EVENT_INTERNAL)
		{
		sprintf(source_note,"MASK");
		}
	else
		{
		if(source==EW_EVENT_WIDGET)
			sprintf(source_note,"WIDGET");
		else if(source==EW_EVENT_APPLICATION)
			sprintf(source_note,"APP");
		else if(source==EW_EVENT_ANY)
			sprintf(source_note,"ANY");
		else
			sprintf(source_note,"MASK?");
		}


	if(source==EW_EVENT_KEYBOARD || source==EW_EVENT_MOUSEBUTTON || source==EW_EVENT_MOUSEPOSITION)
		{
		if(item!=EW_KEY_ALL)
			{
			strcpy(temp,item_note);
			sprintf(item_note,"%s%s%s%s%s", (item&EW_KEY_SHIFT)? "S":"",
											(item&EW_KEY_CAPS_LOCK)? "L":"",
											(item&EW_KEY_CONTROL)? "C":"",
											(item&(EW_KEY_SHIFT|EW_KEY_CAPS_LOCK|EW_KEY_CONTROL))? " ":"",
											temp);
			}
		}

	if(used==EW_USED_NONE)
		strcat(used_note,"NONE");
	else if(used==EW_USED_TRUE)
		strcat(used_note,"TRUE");
	else if(used==EW_USED_ALL)
		strcat(used_note,"ALL");
	else
		{
		if(used&EW_USED_GRABX && used&EW_USED_DRAGX)
			strcat(used_note,"ax");
		else if(used&EW_USED_GRABX)
			strcat(used_note,"gx");
		else if(used&EW_USED_DRAGX)
			strcat(used_note,"dx");

		if(used&EW_USED_GRABY && used&EW_USED_DRAGY)
			strcat(used_note,"ay");
		else if(used&EW_USED_GRABY)
			strcat(used_note,"gy");
		else if(used&EW_USED_DRAGY)
			strcat(used_note,"dy");
		}

	// officially, EW knows nothing about these, but this is really convenient
	if(widget_flags)
		{
		// WDS_BYPASS_EVENT
		if(widget_flags&0x00000001)
			strcat(widget_note,"Bypass");

		// WDS_BYPASS_TAKE_LOST
		if(widget_flags&0x00000002)
			strcat(widget_note,"Lost");

		// WDS_BYPASS_IN_FOCUS
		if(widget_flags&0x00000004)
			strcat(widget_note,"Focus");

		strcat(widget_note,"?");
		}

	if(mousebuttons)
		{
		if(mousebuttons&0x01)
			strcat(mouse_note,"L");
		if(mousebuttons&0x02)
			strcat(mouse_note,"M");
		if(mousebuttons&0x04)
			strcat(mouse_note,"R");
		}

	if(source_note[0])
		{
		strcpy(temp,source_note);
		sprintf(source_note," %s%s%s",ewColorCode(EW_CODE_RED),temp,ewColorCode(EW_CODE_WHITE));
		}
	if(item_note[0])
		{
		strcpy(temp,item_note);
		sprintf(item_note," %s%s%s",ewColorCode(EW_CODE_GREEN),temp,ewColorCode(EW_CODE_WHITE));
		}
	if(state_note[0])
		{
		strcpy(temp,state_note);
		sprintf(state_note," %s%s%s",ewColorCode(EW_CODE_YELLOW),temp,ewColorCode(EW_CODE_WHITE));
		}
	if(used_note[0])
		{
		strcpy(temp,used_note);
		sprintf(used_note," %s%s%s",ewColorCode(EW_CODE_MAGENTA),temp,ewColorCode(EW_CODE_WHITE));
		}
	if(widget_note[0])
		{
		strcpy(temp,widget_note);
		sprintf(widget_note," %s%s%s",ewColorCode(EW_CODE_CYAN),temp,ewColorCode(EW_CODE_WHITE));
		}

	if(mouse_note[0])
		{
		strcpy(temp,mouse_note);
		sprintf(mouse_note," %s%s%s",ewColorCode(EW_CODE_RED),temp,ewColorCode(EW_CODE_WHITE));
		}


	sprintf(ew_print_message,"%sEVENT:src=0x%x%s item=0x%x%s state=0x%x%s used=0x%x%s widget_flags=0x%x%s time=%d%s",
		ewColorCode(EW_CODE_WHITE),
		(unsigned int)(source),source_note,(unsigned int)(item),item_note,
		(unsigned int)state,state_note,(unsigned int)used,used_note,
		(unsigned int)widget_flags,widget_note,(int)time,
		ewColorCode(EW_CODE_NORMAL));

	EW_PRINT(EW_EVENT,EW_LOG,ew_print_message);

	sprintf(ew_print_message,"%s      rep=%s%d%s choice=0x%x/0x%x mouse=(%d,%d)0x%x%s code=0x%x for 0x%x (%s)%s",
		ewColorCode(EW_CODE_WHITE),
		repetition? ewColorCode(EW_CODE_GREEN): "",
		(int)repetition,ewColorCode(EW_CODE_WHITE),
		(int)choice,(int)lastchoice,(int)mx,(int)my,(int)mousebuttons,mouse_note,(int)hexcode,
		(int)destination,destination? destination->GetTitle(): "",
		ewColorCode(EW_CODE_NORMAL));

	EW_PRINT(EW_EVENT,EW_LOG,ew_print_message);


	if(clipregion)
		{
		sprintf(ew_print_message,"%sClip:%s",ewColorCode(EW_CODE_YELLOW),ewColorCode(EW_CODE_NORMAL));

		EW_Rectangle *node;
		long x,y,sx,sy;

		clipregion->ToHead();
		while( (node=clipregion->PostIncrement()) != NULL )
			{
			node->GetGeometry(&x,&y,&sx,&sy);

			sprintf(temp," (%d,%d %d,%d)",(int)x,(int)y,(int)sx,(int)sy);
			strcat(ew_print_message,temp);
			}

		EW_PRINT(EW_EVENT,EW_LOG,ew_print_message);
		}

	return TRUE;
	}



/******************************************************************************
void	EW_EventQueue::AppendCopyOf(EW_Event *other)

******************************************************************************/
void EW_EventQueue::AppendCopyOf(EW_Event *other)
	{
	EW_Event *event=new EW_Event;

	event->CopyFrom(other);
	AppendNode(event);
	}



/******************************************************************************
void	EW_EventQueue::ProcessAllEvents(void)

******************************************************************************/
void EW_EventQueue::ProcessAllEvents(void)
	{
	if(!GetNumberNodes())
		return;

	EW_Event *event;
	EW_Window *target;

#if	EW_EVENT_QUEUE_DEBUG
//	EW_PRINT(EW_EVENT,EW_LOG,"%sEW_EventQueue::ProcessAllEvents()%s begin",ewColorCode(4),ewColorCode(EW_CODE_NORMAL));
#endif

	long m=0;
	while( (event=ToHead()) != NULL)
		{
#if	EW_EVENT_QUEUE_DEBUG
		EW_PRINT(EW_EVENT,EW_LOG,"%sEW_EventQueue::ProcessAllEvents()%s processing %d/%d",
										ewColorCode(EW_CODE_BLUE),ewColorCode(EW_CODE_NORMAL),m+1,GetNumberNodes());
		event->Print(m);
#endif

		if((target=event->GetDestination()))
			target->ProcessEvent(event,TRUE);

		RemoveNode(event);
		delete event;

		m++;
		}

#if	EW_EVENT_QUEUE_DEBUG
//	EW_PRINT(EW_EVENT,EW_LOG,"%sEW_EventQueue::ProcessAllEvents()%s complete",ewColorCode(4),ewColorCode(EW_CODE_NORMAL));
#endif
	}
