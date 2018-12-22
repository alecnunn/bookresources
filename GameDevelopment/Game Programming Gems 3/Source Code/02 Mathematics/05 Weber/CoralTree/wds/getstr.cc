/******************************************************************************

Coral Tree wds/getstr.cc
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


#define WDS_GETSTR_DEBUG			FALSE
#define WDS_GETSTR_BYPASS_DEBUG		FALSE
#define WDS_GETSTR_STARTSTOP_DEBUG	FALSE
#define WDS_GETSTR_DRAW_FLASH		FALSE

#define WDS_GETSTR_FOCUS_DOTS		FALSE
#define WDS_GETSTR_FOCUS_DOT_COLOR	EW_RED

#define WDS_GETSTR_MINSIZEX	16
#define WDS_GETSTR_MINSIZEY	16



/******************************************************************************
		WDS_GetString::WDS_GetString()

Keywords:
******************************************************************************/
WDS_GetString::WDS_GetString()
	{
	DB("WDS_GetString::WDS_GetString()");

/*
	buffer[0]='\0';
	string[0]='\0';
*/

	SetName("GetString");

	depth=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH);
	justification=0;
	cursor=0;
	selection=0;
//	range_mark= -1;
	range_mark=0;
	selecting=FALSE;
	requesting=FALSE;
	keystroke_entered=FALSE;
	focused=FALSE;
	last_focused=FALSE;
	maxlength= -1;

	SetBounds(EW_BOUNDS_IGNORE_PARENT);
	SetAcceptKeys(EW_KEY_RETURN);
	SetAbortKeys(EW_KEY_ESC);
	SetKeySet(NULL);

	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS|EW_EVENT_CLIPBOARD);

//	SetEventMask(GetFullEventMask());

	SetAnnotatable(TRUE);
//	SetMinSize(WDS_GETSTR_MINSIZEX,WDS_GETSTR_MINSIZEY);
	}



/******************************************************************************
void	WDS_GetString::ChangeState(long newstate,EW_Event *event)

******************************************************************************/
void WDS_GetString::ChangeState(long newstate,EW_Event *event)
	{
#if WDS_GETSTR_DEBUG
	char message[200];
	sprintf(message,"WDS_GetString::ChangeState(%d) 0x%x string=0x%x(%s) buffer=0x%x(%s)",newstate,(long)this,
																(long)&string,string.Buffer(),(long)&buffer,buffer.Buffer());
	EW_PRINT(EW_WIDGET,EW_LOG,message);
#endif

	cursor=0;

	if(newstate)
		{
		buffer.NewBuffer(string.Buffer());
		string.DenyConfirm();
		}
	else
		{
		string.AllowConfirm();

		range_mark= -1;
		buffer.NewLength(0);
		}

	keystroke_entered=FALSE;
	RegisterKeyboard(newstate!=0);

	WDS_Widget::ChangeState(newstate,event);
	}



/******************************************************************************
void WDS_GetString::RegisterKeyboard(long on)

	bypass keyboard events into this widget

	if on==FALSE, then release keyboard

******************************************************************************/
void WDS_GetString::RegisterKeyboard(long on)
	{
#if WDS_GETSTR_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_GetString::RegisterKeyboard(%d) 0x%x (%s)",
									on,(long)this,GetNameString()->Buffer());
#endif

	EW_Event event;
	long shallow=InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_SCOPE_SHALLOW;

	event.SetSIS(EW_EVENT_KEYBOARD,EW_KEY_ALL,EW_STATE_ANY);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_TAKE,shallow);
	}



/******************************************************************************
void WDS_GetString::RegisterMouse(long on)

	bypass mouse events into this widget

******************************************************************************/
void WDS_GetString::RegisterMouse(long on)
	{
#if WDS_GETSTR_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_GetString::RegisterMouse(%d) 0x%x (%s)",on,(long)this,GetName());
#endif

	EW_Event event;

	event.SetSIS(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,EW_STATE_RELEASE);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_NULL,FALSE);

	event.SetSIS(EW_EVENT_MOUSEPOSITION,EW_ITEM_ALL,EW_STATE_ANY);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_TAKE,FALSE);
	}



/******************************************************************************
long	WDS_GetString::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_GetString::Operate(EW_Event *event)
	{
	EW_Window *root=GetRootWindow();

	short done=FALSE;
	short ok=FALSE;
	long length;
	long mx, my, position;

	long rx,ry,rsx,rsy;
	GetRegion(FALSE,&rx,&ry,&rsx,&rsy);

	event->GetMousePosition(&mx,&my);

	if(event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS)
		{
		if(event->IsKey(EW_KEY_ALL))
			{
#if WDS_GETSTR_BYPASS_DEBUG
			printf("bypass (%s) (%s)->(%s)\n",GetNameString()->Buffer(),
								GetStoreBuffer()->Buffer(),GetString()->Buffer());
			event->Print(NULL);
#endif

#if WDS_GETSTR_BYPASS_DEBUG
				printf("  -> %s focused dirty %d->",last_focused? "still": "now",GetDirty());
#endif

			focused=TRUE;
			SetDirty( ((last_focused!=focused)? EW_DIRTY_COMPLETE: EW_DIRTY_NOT) | EW_DIRTY_PENDING | EW_DIRTY_IF_PENDING);

#if WDS_GETSTR_BYPASS_DEBUG
				printf("%d\n",GetDirty());
#endif

			}

		event->SetUsed(EW_USED_ALL);
		}

	if(event->IsIdleMouse())
		{
		if(!GetWidgetState())
			return FALSE;

#if WDS_GETSTR_BYPASS_DEBUG
		printf("bypass (%s)\n",GetNameString()->Buffer());
#endif

#if WDS_GETSTR_BYPASS_DEBUG
		printf("  -> %s not focused dirty %d->",!last_focused? "still": "now",GetDirty());
#endif
		focused=FALSE;

		SetDirty( ((last_focused!=focused)? EW_DIRTY_COMPLETE: EW_DIRTY_NOT) | EW_DIRTY_PENDING | EW_DIRTY_IF_PENDING);
#if WDS_GETSTR_BYPASS_DEBUG
		printf("%d\n",GetDirty());
#endif

#if WDS_GETSTR_BYPASS_DEBUG
		printf("  done\n");
#endif
		event->SetUsed(EW_USED_ALL);
		}

	/* if the bypass was just lost, deactivate the widget */
	if( event->IsKey(EW_KEY_ALL) && GetWidgetState() && (event->GetWidgetFlags() & WDS_BYPASS_TAKE_LOST) )
		{
#if WDS_GETSTR_BYPASS_DEBUG
		printf("  -> take lost\n");
#endif
		StopEdit(FALSE,event);
		SetDirty(EW_DIRTY_COMPLETE);
		event->SetUsed(EW_USED_ALL);
		}

	if(event->IsClipboard())
		{
		if(AllowClipboard() && event->IsClipboardReady() && requesting)
			{
			// use clipboard text
			char *cliptext=GetRootWindow()->GetEventContext()->CopyTextFromClipboard();

//			printf("getstring clipboard (%s)\n",cliptext);

			DeleteRange();
			string.InsertBuffer(cursor,cliptext);
			cursor+=strlen(cliptext);
			range_mark=cursor;
			
			SetDirty(EW_DIRTY_COMPLETE);

			osdFree(cliptext);

			if(maxlength>=0 && string.GetLength()>maxlength)
				string.NewLength(maxlength);
			}

		requesting=FALSE;
		return FALSE;
		}

	if(event->GetUsed())
		return FALSE;

	// mark the event as consumed  ----> should this be allowed: set for anything remaining
	event->SetUsed(EW_USED_ALL);

#if WDS_GETSTR_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_GetString::Operate()");

	event->Print((long)this);
#endif

	focused=TRUE;

	if(event->IsLeftMousePress() && InRegion(event,TRUE))
		{
		if(GetString()->GetEditable() )
			{
			if(!GetWidgetState())
				{
				ChangeState(WDS_STATE_1,event);
				}

			if(InRegion(event,TRUE))
				{
				position=string.Operate(root,rx,ry,rsx,rsy,mx,my,justification|WDS_STRING_HALF_SHIFT);

#if WDS_GETSTR_DEBUG
				fprintf(stderr,"PRESS %d\n",position);
#endif

				if(position >= 0)
					{
					selecting=TRUE;
					range_mark=position;

					if(!(event->GetItem()&EW_KEY_SHIFT))
						{
						cursor=position;
						selection=position;

						switch( event->GetRepetition()%3 )
							{
							case 1:
								SelectWord(position);
								break;
							case 2:
								SelectAll();
								break;
							}
						}

					RegisterMouse(TRUE);
					}
				}

			SetDirty(EW_DIRTY_COMPLETE);

			return FALSE;
			}
		}

	if(event->IsLeftMouseRelease())
		{
		if(selecting)
			{
			selecting=FALSE;
			position=string.Operate(root,rx,ry,rsx,rsy,mx,my,justification|WDS_STRING_HALF_SHIFT);

#if WDS_GETSTR_DEBUG
			fprintf(stderr,"RELEASE %d C:%d\n",position,cursor);
#endif

#if FALSE
			if(c != position && position >= 0)
				{
				range_mark=position-1;
				}
#endif

			if(range_mark>=0 && range_mark!=cursor &&
										InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_CLIPBOARD_AUTO)
				{
				BroadcastSelection();
				}

			RegisterMouse(FALSE);

//			SetDirty(EW_DIRTY_COMPLETE);
			return FALSE;
			}
		}

	if(!GetWidgetState())
		requesting=FALSE;
	else if(event->IsMiddleMousePress() && InRegion(event,TRUE))
		{
		if(InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_CLIPBOARD_AUTO)
			{
			// request clipboard text
			RequestSelection();
			}

		return FALSE;
		}

	if(selecting && (event->Is(EW_EVENT_MOUSEPOSITION,EW_ITEM_ALL,EW_STATE_ANY) ||
														(event->IsLeftMousePress() && (event->GetItem()&EW_KEY_SHIFT)) ) )
	{
		position=string.Operate(root,rx,ry,rsx,rsy,mx,my,justification|WDS_STRING_HALF_SHIFT);

		if(position==WDS_STRING_MISS_TO_LEFT)
			position=0;

		if(position < 0)
		{
			/* NOOP */
		}
		else if(position == selection)
		{
//			range_mark= -1;
			range_mark=selection;
			cursor=selection;
		}
		else if(position < selection)
		{
			range_mark=position;
//			cursor=selection-1;
			cursor=selection;
		}
		else if(position > selection)
		{
//			range_mark=position - 1;
			range_mark=position;
			cursor=selection;
		}
#if WDS_GETSTR_DEBUG
		fprintf(stderr,"MOUSE POS position=%d cursor=%d range_mark=%d\n",position,cursor,range_mark);
#endif

		SetDirty(EW_DIRTY_COMPLETE);
		return FALSE;
	}

	length=string.Length();

	if(GetWidgetState() && !(event->GetState()&EW_STATE_RELEASE) )
		{
		switch(event->GetSource())
			{
			case EW_EVENT_KEYBOARD:
#if FALSE
				printf("WDS_GetString Keyboard Press:\n");
				event->Print(NULL);
#endif

#if WDS_KEYPRESS_TEST
				if(TRUE)
#else
				if( event->IsKeyTyped(EW_KEY_ALL) )
#endif
					{
					keystroke_entered=TRUE;

					if(event->IsKeyTyped(GetAbortKeys()))
							done=TRUE;
					else if(event->IsKeyTyped(GetAcceptKeys()))
							{
							done=TRUE;
							ok=TRUE;
							}
					else
						{
						long item=event->GetItem();

						switch(item)
							{
							///////////////////////////////////////////////////////
							// DON'T MESS WITH THESE WITHOUT BEING VERY CAREFUL  //
							// Many sequences are carefully tuned to match Win32 //
							///////////////////////////////////////////////////////

							// note generally:
							// SHIFT mod = maintain selection
							// CTRL  mod = larger granularity

							case EW_KEY_BACKSPACE|EW_KEY_CONTROL:
								// back-delete word
								if(cursor>0)
									{
									cursor=EW_MIN(cursor,range_mark);

									long save_mark=cursor;
									SelectWord(--cursor);
									range_mark=save_mark;
									}
							case EW_KEY_BACKSPACE:
								if(range_mark>=0 && range_mark!=cursor)
									DeleteRange();
								else if(cursor>0)
									{
									cursor--;
									string.DeleteChar(cursor);
									}
								
								range_mark=cursor;
								break;

							case EW_KEY_DELETE|EW_KEY_CONTROL:
								{
								// delete word
								if(cursor!=range_mark)
									DeleteRange();

								range_mark=cursor;

								long save_mark=cursor;
								SelectWord(cursor);
								cursor=save_mark;
								}
							case EW_KEY_DELETE:
								if(range_mark>=0 && range_mark!=cursor)
									DeleteRange();
								else if(cursor!=length)
									{
									string.DeleteChar(cursor);
									}

								range_mark=cursor;
								break;

							case (EW_KEY_CONTROL|'a'):
								// select all
								SelectAll();
								break;

							case (EW_KEY_CONTROL|'c'):
								// copy
								BroadcastSelection();
								break;

							case (EW_KEY_CONTROL|'x'):
								// cut
								BroadcastSelection();
								DeleteRange();
								range_mark=cursor;
								break;

							case (EW_KEY_CONTROL|'v'):
								// paste
								RequestSelection();
								break;

							case EW_KEY_HOME:
							case EW_KEY_HOME|EW_KEY_SHIFT:
								// beginning of line
								{
								long savemark=cursor;

								string.Operate(root,rx,ry,rsx,rsy,mx,my,WDS_STRING_CURSOR_STARTLINE|justification);
								range_mark=string.GetCursorPosition();

								cursor=savemark;

								if( !(item&EW_KEY_SHIFT) )
									cursor=range_mark;
								}
								break;

							case EW_KEY_END:
							case EW_KEY_END|EW_KEY_SHIFT:
								// end of line
								{
								long savemark=cursor;
								string.Operate(root,rx,ry,rsx,rsy,mx,my,WDS_STRING_CURSOR_ENDLINE|justification);
								range_mark=string.GetCursorPosition();

								cursor=savemark;

								if( !(item&EW_KEY_SHIFT) )
									cursor=range_mark;
								}
								break;

							case EW_KEY_HOME|EW_KEY_CONTROL:
							case EW_KEY_HOME|EW_KEY_CONTROL|EW_KEY_SHIFT:
								// beginning of text
								range_mark=0;

								if( !(item&EW_KEY_SHIFT) )
									cursor=range_mark;
								break;

							case EW_KEY_END|EW_KEY_CONTROL:
							case EW_KEY_END|EW_KEY_CONTROL|EW_KEY_SHIFT:
								// end of text
								range_mark=length;

								if( !(item&EW_KEY_SHIFT) )
									cursor=range_mark;
								break;

							case EW_KEY_CURSOR_UP:
							case EW_KEY_CURSOR_UP|EW_KEY_SHIFT:
								string.Operate(root,rx,ry,rsx,rsy,mx,my,WDS_STRING_CURSOR_UP|justification);
								cursor=string.GetCursorPosition();

								if( !(item&EW_KEY_SHIFT) )
									range_mark=cursor;
								break;

							case EW_KEY_CURSOR_DOWN:
							case EW_KEY_CURSOR_DOWN|EW_KEY_SHIFT:
								string.Operate(root,rx,ry,rsx,rsy,mx,my,WDS_STRING_CURSOR_DOWN|justification);
								cursor=string.GetCursorPosition();

								if( !(item&EW_KEY_SHIFT) )
									range_mark=cursor;
								break;

							case EW_KEY_CURSOR_LEFT:
							case EW_KEY_CURSOR_LEFT|EW_KEY_SHIFT:
								if( range_mark>0 && ((item&EW_KEY_SHIFT) || range_mark==cursor) )
									range_mark--;

								if( !(item&EW_KEY_SHIFT) )
									{
									cursor=EW_MIN(cursor,range_mark);
									range_mark=cursor;
									}
								break;

							case EW_KEY_CURSOR_RIGHT:
							case EW_KEY_CURSOR_RIGHT|EW_KEY_SHIFT:
								if( (item&EW_KEY_SHIFT) || range_mark==cursor )
									range_mark++;

								if(range_mark>length)
									range_mark=length;

								if( !(item&EW_KEY_SHIFT) )
									{
									cursor=EW_MAX(cursor,range_mark);
									range_mark=cursor;
									}
								break;

							case EW_KEY_CURSOR_LEFT|EW_KEY_CONTROL:
							case EW_KEY_CURSOR_LEFT|EW_KEY_CONTROL|EW_KEY_SHIFT:
								// move by word
								if(range_mark>0)
									{
									long save_cursor=cursor;

									range_mark--;
									SelectWord(range_mark);

									while(!EW_IS_ALPHANUM(string.Buffer(cursor)))
										{
										cursor--;
										SelectWord(cursor);
										}

									range_mark=cursor;

									if( (item&EW_KEY_SHIFT) )
										cursor=save_cursor;

									if(InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_CLIPBOARD_AUTO)
										BroadcastSelection();
									}
								break;

							case EW_KEY_CURSOR_RIGHT|EW_KEY_CONTROL:
							case EW_KEY_CURSOR_RIGHT|EW_KEY_CONTROL|EW_KEY_SHIFT:
								// move by word
								{
								long save_cursor=cursor;
								long length=string.GetLength();

								SelectWord(range_mark);

								while(range_mark<length && !EW_IS_ALPHANUM(string.Buffer(range_mark)))
									SelectWord(range_mark);

								if( !(item&EW_KEY_SHIFT) )
									cursor=range_mark;
								else
									cursor=save_cursor;

								if(InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_CLIPBOARD_AUTO)
									BroadcastSelection();
								}
								break;

							default:	// other ascii
								{
								long ascii= (item&EW_KEY_ALL_ASCII);

								if(keyset==NULL || keyset->GetBit(ascii))
									{
									if(range_mark >= 0)
										{
										long char_range;
										if(range_mark >= cursor)
											{
//											char_range=range_mark -cursor+ 1;
											char_range=range_mark -cursor;
											string.DeleteRange(cursor,char_range);
											}
										else if(range_mark < cursor)
											{
//											char_range=cursor- range_mark + 1;
											char_range=cursor- range_mark;
											string.DeleteRange(range_mark,char_range);
											cursor=range_mark;
											}
//										range_mark= -1;
										}

									if( maxlength<0 || length<maxlength )
									// ASCII 8 to 127
									if( (ascii>=EW_KEY_BACKSPACE) && (ascii<EW_KEY_DELETE) )
										{
										string.InsertChar(cursor,ascii);
										cursor++;
										}

									range_mark=cursor;
									}
								else
									root->RingBell();
								}
								break;
							}
						}
					}
				break;

			default:
				return FALSE;
			}

		if(done)
			StopEdit(!ok,event);

#if WDS_GETSTR_DEBUG
		fprintf(stderr,"RELEASE\n");
#endif
		SetDirty(EW_DIRTY_COMPLETE);
		}

	return FALSE;
	}



/******************************************************************************
void	WDS_GetString::StartEdit(long cursor_position,EW_Event *event)

******************************************************************************/
void WDS_GetString::StartEdit(long cursor_position,EW_Event *event)
	{
#if WDS_GETSTR_STARTSTOP_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_GetString::StartEdit(%d, ) state=%d (%s) string (%s) -> (%s)",cursor_position,GetWidgetState(),
											GetNameString()->Buffer(),GetStoreBuffer()->Buffer(),GetString()->Buffer());
	event->Print((long)this);
#endif

	if(!GetWidgetState())
		ChangeState(WDS_STATE_1,event);

	cursor=cursor_position;
	range_mark=cursor;
	}



/******************************************************************************
void	WDS_GetString::StopEdit(long restore,EW_Event *event)

******************************************************************************/
void WDS_GetString::StopEdit(long restore,EW_Event *event)
	{
#if WDS_GETSTR_STARTSTOP_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_GetString::StopEdit(%d, ) state=%d (%s) string (%s) -> (%s)",restore,GetWidgetState(),
											GetNameString()->Buffer(),GetStoreBuffer()->Buffer(),GetString()->Buffer());
	event->Print((long)this);
#endif

	if(GetWidgetState())
		{
		if(restore)
			string.NewBuffer(buffer.Buffer());

		ChangeState(WDS_STATE_0,event);
		}

	selection=0;
	}



/******************************************************************************
void	WDS_GetString::Draw(EW_Redraw *redraw)

	note that string is not drawn as bevelbox label but through a separate
	string routine

Keywords:
******************************************************************************/
void WDS_GetString::Draw(EW_Redraw *redraw)
	{
//	printf("Draw (%s)\n",GetNameString()->Buffer());

    long truex,truey,sx,sy;
    GetTrueGeometry(&truex,&truey,&sx,&sy);

	long rx,ry,rsx,rsy;
	GetRegion(FALSE,&rx,&ry,&rsx,&rsy);

#if WDS_GETSTR_BYPASS_DEBUG
	printf("Draw last_focused %d->%d (%s)\n",last_focused,focused,GetNameString()->Buffer());
#endif
	last_focused=focused;

#if WDS_GETSTR_DRAW_FLASH
	GetRootWindow()->RectFill(truex,truey,truex+sx-1,truey+sy-1,EW_RED);
	GetRootWindow()->Flush(TRUE);
	osdMilliSleep(50);
#endif

	long index_pen=GetRootWindow()->GetColor(EW_COLOR_PEN);

	long apparent_state=WDS_STATE_0;
	if(GetWidgetState())
		apparent_state= (focused? WDS_STATE_2: WDS_STATE_1);

	long textfg=InterpretElement(apparent_state,WDS_ELEMENT_GETSTR_UNSELECT_FG);
	long colorfg=InterpretElement(apparent_state,WDS_ELEMENT_GETSTR_SELECT_FG);
	long colorbg=InterpretElement(apparent_state,WDS_ELEMENT_GETSTR_SELECT_BG);
	long highlightbg=InterpretElement(apparent_state,WDS_ELEMENT_GETSTR_HIGHLIGHT_BG);

	string.SetCursorColor(colorfg,colorbg);

	GetRootWindow()->SetColor(EW_COLOR_PEN,textfg);

	if(GetWidgetState())
		{
		string.SetCursorPosition(cursor);
		string.SetRangeMarkPosition(range_mark);

		SetWidgetState(focused? WDS_STATE_2: WDS_STATE_1);
		BevelBox(TRUE,NULL);
		SetWidgetState(WDS_STATE_1);

#if WDS_GETSTR_FOCUS_DOTS
		GetRootWindow()->RectFill(truex,		truey,		truex+1,	truey+1,	WDS_GETSTR_FOCUS_DOT_COLOR);
		GetRootWindow()->RectFill(truex+sx-2,	truey,		truex+sx-1,	truey+1,	WDS_GETSTR_FOCUS_DOT_COLOR);
		GetRootWindow()->RectFill(truex,		truey+sy-2,	truex+1,	truey+sy-1,	WDS_GETSTR_FOCUS_DOT_COLOR);
		GetRootWindow()->RectFill(truex+sx-2,	truey+sy-2,	truex+sx-1,	truey+sy-1,	WDS_GETSTR_FOCUS_DOT_COLOR);
#endif

		if(string.GetHighlight()==WDS_HIGHLIGHT_LINE_OUT)
			GetRootWindow()->LineOut(rx,ry,rsx,rsy,0,3,highlightbg);
		else if(string.GetHighlight()==WDS_HIGHLIGHT_BACKFILL)
			GetRootWindow()->RectFill(rx-1,ry-1,rx+rsx,ry+rsy,highlightbg);

		string.Operate(GetRootWindow(),rx,ry,rsx,rsy,0,0,WDS_STRING_DRAW|justification);
		}
	else
		{
		if( !GetString()->GetEditable() )
			SetWidgetState(WDS_STATE_3);

		string.SetCursorPosition(-1);
		string.SetRangeMarkPosition(-1);
		BevelBox(TRUE,NULL);

		SetWidgetState(WDS_STATE_0);

		if(string.GetHighlight()==WDS_HIGHLIGHT_LINE_OUT)
			GetRootWindow()->LineOut(rx,ry,rsx,rsy,0,3,highlightbg);
		else if(string.GetHighlight()==WDS_HIGHLIGHT_BACKFILL)
			GetRootWindow()->RectFill(rx-1,ry-1,rx+rsx,ry+rsy,highlightbg);

		string.Operate(GetRootWindow(),rx,ry,rsx,rsy,0,0,WDS_STRING_DRAW|justification);
		}

	GetRootWindow()->SetColor(EW_COLOR_PEN,index_pen);
	}



/******************************************************************************
void	WDS_GetString::SetString(char *a_string)

Keywords:
******************************************************************************/
void WDS_GetString::SetString(char *a_string)
	{
	DB("WDS_GetString::SetString(char *a_string)");

//	ChangeState(WDS_STATE_0);

	string.NewBuffer(a_string);
	SetDirty(EW_DIRTY_COMPLETE);

#if WDS_GETSTR_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"SetString() %d %d",
					strlen(a_string),string.Length());
#endif
	}



/******************************************************************************
void	WDS_GetString::SelectWord(long index)

******************************************************************************/
void WDS_GetString::SelectWord(long index)
	{
	char *pointer=string.Buffer();
	long len=string.GetLength();

	long iswhite=EW_IS_WHITESPACE(pointer[index]);
	long isalpha=EW_IS_ALPHANUM(pointer[index]);

//	printf("index=%d/%d (%s)\n",index,len,pointer);

	cursor=index;
	range_mark=index;

	SetDirty(EW_DIRTY_COMPLETE);

	if(!iswhite && !isalpha)
		{
		range_mark++;

		return;
		}

	while(cursor>0 && iswhite==EW_IS_WHITESPACE(pointer[cursor-1]) && isalpha==EW_IS_ALPHANUM(pointer[cursor-1]))
		cursor--;

	while(range_mark<len && iswhite==EW_IS_WHITESPACE(pointer[range_mark]) && isalpha==EW_IS_ALPHANUM(pointer[range_mark]))
		range_mark++;

//	range_mark--;

	if(InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_CLIPBOARD_AUTO)
		BroadcastSelection();
	}



/******************************************************************************
void	WDS_GetString::SelectAll(void)

******************************************************************************/
void WDS_GetString::SelectAll(void)
	{
	cursor=0;
	range_mark=string.GetLength();

	if(InterpretElement(WDS_STATE_ALL,WDS_ELEMENT_GETSTR_FLAGS)&WDS_GETSTR_CLIPBOARD_AUTO)
		BroadcastSelection();

	SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void	WDS_GetString::DeleteRange(void)

******************************************************************************/
void WDS_GetString::DeleteRange(void)
	{
	if(range_mark<0 || range_mark==cursor)
		return;

	long char_range;
	if(range_mark >= cursor)
		{
		char_range=range_mark -cursor;
		string.DeleteRange(cursor,char_range);
		range_mark=cursor;
		}
	else if(range_mark < cursor)
		{
		char_range=cursor- range_mark;
		string.DeleteRange(range_mark,char_range);
		cursor=range_mark;
		}
	}



/******************************************************************************
void	WDS_GetString::BroadcastSelection(void)

******************************************************************************/
void WDS_GetString::BroadcastSelection(void)
	{
	if(!AllowClipboard())
		return;

	WDS_String tempstring;

	long start=EW_MIN(cursor,range_mark);
	long len=abs(cursor-range_mark);

	tempstring.NewBuffer(&string.Buffer()[start]);
	tempstring.NewLength(len);

	GetRootWindow()->GetEventContext()->CopyTextToClipboard(tempstring.Buffer());
	}



/******************************************************************************
void	WDS_GetString::RequestSelection(void)

******************************************************************************/
void WDS_GetString::RequestSelection(void)
	{
	if(!AllowClipboard())
		return;

	GetRootWindow()->GetEventContext()->RequestTextFromClipboard(GetRootWindow());
	requesting=TRUE;
	}



/******************************************************************************
long	WDS_GetString::AllowClipboard(void)

******************************************************************************/
long WDS_GetString::AllowClipboard(void)
	{
	return (GetString()->GetDisplayChar()==0);
	}



/******************************************************************************
long	WDS_GetString::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_GetString::Resize(EW_Resize *resize)
	{
	//EW_Window *root=GetRootWindow();

	if(GetLabelString()->Compare(&string))
		GetLabelString()->CopyFromWDS_String(&string);

	if(GetWidgetState() && !GetString()->GetEditable())
		StopEdit(FALSE,NULL);

	return WDS_Widget::Resize(resize);
	}
