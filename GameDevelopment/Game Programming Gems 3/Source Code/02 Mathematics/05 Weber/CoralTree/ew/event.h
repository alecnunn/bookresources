/******************************************************************************

Coral Tree ew/event.h
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

// activate mouse motion event even without buttons pressed
// (they should still be fitered in each window-dependent event loop)
// required for sub-focus to work properly
#define EW_EVENT_BUTTONLESS_MOTION	TRUE


// event sources
#define EW_EVENT_NONE			0x000
#define EW_EVENT_MOUSEPOSITION	0x001
#define EW_EVENT_MOUSEBUTTON	0x002
#define EW_EVENT_KEYBOARD		0x004
#define EW_EVENT_CLIPBOARD		0x008
#define EW_EVENT_FOCUS			0x010
#define EW_EVENT_PERIODIC		0x020
#define EW_EVENT_SYSTEM			0x040
#define EW_EVENT_WIDGET			0x080	// for widget set, EW will never create
#define EW_EVENT_APPLICATION	0x100	// for application, EW will never create

#define EW_EVENT_INTERNAL		0x3F	// except WIDGET,APPLICATION
#define EW_EVENT_ANY			0x7FFFFFFF

#define EW_EVENT_COMMON			(EW_EVENT_MOUSEPOSITION|EW_EVENT_MOUSEBUTTON| \
								EW_EVENT_KEYBOARD|EW_EVENT_WIDGET|            \
								EW_EVENT_APPLICATION)

// event sources accepted when unmapped
#define	EW_EVENT_WHEN_UNMAPPED	(EW_EVENT_PERIODIC|EW_EVENT_SYSTEM| \
								EW_EVENT_WIDGET|EW_EVENT_APPLICATION)

// event sources occuring at locations in space (subject to descend mode)
#define	EW_EVENT_LOCATIONAL		(EW_EVENT_MOUSEPOSITION|EW_EVENT_MOUSEBUTTON| \
								EW_EVENT_KEYBOARD)

// event sources from expective user input
#define	EW_EVENT_EXPECTIVE		(EW_EVENT_MOUSEBUTTON|EW_EVENT_KEYBOARD)

// event sources blocked by a modal window
#define	EW_EVENT_MODAL			(EW_EVENT_MOUSEBUTTON|EW_EVENT_KEYBOARD| \
								EW_EVENT_MOUSEPOSITION)
#define	EW_EVENT_MODAL_BEEP		(EW_EVENT_EXPECTIVE)

// event sources that trigger a following <EW_EVENT_FOCUS,EW_ITEM_IDLEMOUSE>
// note also after system for exposures
#define EW_EVENT_IDLESOURCE		(EW_EVENT_COMMON|EW_EVENT_FOCUS|EW_EVENT_SYSTEM)


// event items
#define EW_ITEM(x)				( ((x)>=0 && (x)<32)? (1<<(x)): 0 )
#define EW_ITEM_NONE			0
#define EW_ITEM_LEFT			EW_ITEM(0)	// EW_EVENT_MOUSEBUTTON
#define EW_ITEM_MIDDLE			EW_ITEM(1)
#define EW_ITEM_RIGHT			EW_ITEM(2)
#define EW_ITEM_WHEEL			EW_ITEM(3)
#define EW_ITEM_X				EW_ITEM(0)	// EW_EVENT_MOUSEPOSITION
#define EW_ITEM_Y				EW_ITEM(1)
#define EW_ITEM_CLIPBOARD_FAIL	EW_ITEM(0)	// EW_EVENT_CLIPBOARD
#define EW_ITEM_CLIPBOARD_READY	EW_ITEM(1)
#define EW_ITEM_CLOSEWINDOW		EW_ITEM(0)	// EW_EVENT_SYSTEM
#define EW_ITEM_EXPOSURE		EW_ITEM(1)
#define EW_ITEM_QUIT_APP		EW_ITEM(2)
#define EW_ITEM_WORK			EW_ITEM(0)	// EW_EVENT_PERIODIC
#define EW_ITEM_TIMER			EW_ITEM(1)
#define EW_ITEM_FOCUSCHANGE		EW_ITEM(0)	// EW_EVENT_FOCUS
#define EW_ITEM_IDLEMOUSE		EW_ITEM(1)
#define EW_ITEM_ALL				0x7FFFFFFF

// keyboard item groups and specials (regular char items are in ascii)
// note: mouse events can use key mod masks
#define EW_KEY_BACKSPACE		8
#define EW_KEY_RETURN			'\n'		// 10, officially a linefeed (LF)
#define EW_KEY_CR				13			// should be converted (CR -> LF)
#define EW_KEY_ESC				27
#define EW_KEY_DELETE			127
#define EW_KEY_ALL_ASCII		0x000000FF
#define EW_KEY_ASCII_NOT		0x7FFFFF00

#define EW_KEY_NON_ASCII_FLAG	(1<<8)
#define EW_KEY_CURSOR_FLAG		((1<<9)|EW_KEY_NON_ASCII_FLAG)
#define EW_KEY_CURSOR_UP		(EW_KEY_CURSOR_FLAG+0)
#define EW_KEY_CURSOR_DOWN		(EW_KEY_CURSOR_FLAG+1)
#define EW_KEY_CURSOR_LEFT		(EW_KEY_CURSOR_FLAG+2)
#define EW_KEY_CURSOR_RIGHT		(EW_KEY_CURSOR_FLAG+3)
#define EW_KEY_ANY_CURSOR		( EW_KEY_CURSOR_UP|EW_KEY_CURSOR_DOWN| \
									EW_KEY_CURSOR_LEFT|EW_KEY_CURSOR_RIGHT )

#define EW_KEY_NON_ASCII2		((1<<11)|EW_KEY_NON_ASCII_FLAG)
#define EW_KEY_HOME				(EW_KEY_NON_ASCII2+0)
#define EW_KEY_END				(EW_KEY_NON_ASCII2+1)
#define EW_KEY_INSERT			(EW_KEY_NON_ASCII2+2)
#define EW_KEY_TAB				(EW_KEY_NON_ASCII2+3)
#define EW_KEY_PAUSE			(EW_KEY_NON_ASCII2+4)
#define EW_KEY_PAGE_UP			(EW_KEY_NON_ASCII2+5)
#define EW_KEY_PAGE_DOWN		(EW_KEY_NON_ASCII2+6)

#define EW_KEY_F_KEYS			0x000F0000
#define EW_KEY_F_KEY_SHIFT		16
#define EW_KEY_F(x)				((((x)&15)<<EW_KEY_F_KEY_SHIFT)| \
														EW_KEY_NON_ASCII_FLAG)
#define EW_KEY_INDEXED_KEYS		0x000FFFFF

#define EW_KEY_NO_MOD_MASK		0x00FFFFFF
#define EW_KEY_MOD_MASK			0x07000000
#define EW_KEY_SHIFT			(1<<24)
#define EW_KEY_CAPS_LOCK		(1<<25)
#define EW_KEY_CONTROL			(1<<26)
#define EW_KEY_UNUSED_MASK		0xF800B000
#define EW_KEY_ALL				EW_ITEM_ALL


/* event states */
#define EW_STATE_NULL			0
#define EW_STATE_PRESS			0x00000001
#define EW_STATE_REPEAT			0x00000002
#define EW_STATE_RELEASE		0x00000004
#define EW_STATE_ANY			0x7FFFFFFF


// EW_Event::widget_flags values
#define EW_WIDGET_FLAGS_NONE	0x00000000
#define EW_WIDGET_FLAGS_ALL		0x7FFFFFFF


// EW_Event::used values (can use X and Y independently)
#define EW_USED_NONE		FALSE
#define EW_USED_TRUE		TRUE	// old TRUE/FALSE, prefer EW_USED_ALL
#define EW_USED_GRABX		0x02	// grab=new access
#define EW_USED_GRABY		0x04
#define EW_USED_DRAGX		0x08	// drag=continued access
#define EW_USED_DRAGY		0x10
#define EW_USED_X			( EW_USED_GRABX | EW_USED_DRAGX )
#define EW_USED_Y			( EW_USED_GRABY | EW_USED_DRAGY )
#define EW_USED_ALL			0x7FFFFFFF


// EW_Event::GetMouseButtons() OR'ed values
#define EW_MOUSEBUTTONS_LEFT	1
#define EW_MOUSEBUTTONS_MIDDLE	2
#define EW_MOUSEBUTTONS_RIGHT	4
#define EW_MOUSEBUTTONS_ALL		7




class EW_Event
	{
	private:
		EW_Window		*destination;	// intended target window
		EW_ClipRegion	*clipregion;	// for exposure event
		EW_Widget		*choice;		// last in the descension
		EW_Widget		*lastchoice;	// for repeats

		long		source;			// signal source
		long		item;			// item(s) in source to which event occured
		long		state;			// state(s) to which item(s) changed

		long		mx,my;			// mouse location
		long		mousebuttons;	// mouse buttons
		long		used;			// set to TRUE after used by a widget
		long		widget_flags;	// widget-system defined flags
									// (EW can't use)
		long		repetition;		// previous consec. instances of same event
		long		dislocated;		// when TRUE, non-locational outside
									// otherwise locational bounds
		long		time;			// relative time

	public:
				EW_Event();
virtual			~EW_Event();
		void	Reset(void);

		void	MapControlCharacters(void);
		void	CopyFrom(EW_Event *other);

		/* data member access functions */
		void		SetDestination(EW_Window *set)	{ destination=set; };
		EW_Window	*GetDestination(void)			{ return destination; };

		void	SetSource(long x)			{	source=x;		};
		long	GetSource(void)				{	return source;	};

		void	SetItem(long x)				{	item=x;			};
		long	GetItem(void)				{	return item;	};

		void	SetState(long x)			{	state=x;		};
		long	GetState(void)				{	return state;	};

		void	SetTimeAutomatically(void)	{ SetTime(osdTimer()); };
		void	SetTime(long set)			{ time=set; };
		long	GetTime(void)				{ return time; };

		void			SetClipRegion(EW_ClipRegion *set)	{ clipregion=set; };
		EW_ClipRegion	*GetClipRegion(void)			{ return clipregion; };

		void		SetChoice(EW_Widget *set)	{ choice=set; };
		EW_Widget	*GetChoice(void)			{ return choice; };

		void		SetLastChoice(EW_Widget *set)	{ lastchoice=set; };
		EW_Widget	*GetLastChoice(void)			{ return lastchoice; };

		void	SetRepetition(long set)		{ repetition=set; };
		long	GetRepetition(void)			{ return repetition; };

		void	SetDislocated(long set)		{ dislocated=set; };
		long	GetDislocated(void)			{ return dislocated; };

		void	SetUsed(long set)			{	used=set;			};
		long	GetUsed(void)				{	return used;	};

		//		widget system only - NOT FOR USE BY EW
		long	GetWidgetFlags(void)				{ return widget_flags;	};
		void	SetWidgetFlags(long set)			{ widget_flags=set;		};
		void	SetWidgetFlagsIncluding(long set)	{ widget_flags|=set;	};
		void	SetWidgetFlagsExcluding(long set)
					{
					widget_flags^= (widget_flags&set);
					};

		void	SetMousePosition(long x,long y)			{	mx=x;	my=y;	};
		void	GetMousePosition(long *x,long *y)		{	*x=mx;	*y=my;	};
		void	InvertMousePosition(long windowheight)
					{
					my=EW_INVERT_Y(my,windowheight);
					};

		void	SetMouseButtons(long x)		{	mousebuttons=x;			};
		long	GetMouseButtons(void)		{	return mousebuttons;	};

		/* convenience functions */
		void	SetSIS(long _source,long _item,long _state)
					{
					SetSource(_source);
					SetItem(_item);
					SetState(_state);
					};

		void	SetSISW(long _source,long _item,long _state,long _widget_flags)
					{
					SetSIS(_source,_item,_state);
					SetWidgetFlags(_widget_flags);
					};

		long	InWidget(EW_Widget *widget)
					{
					long truex,truey,sx,sy;

					widget->GetTrueGeometry(&truex,&truey,&sx,&sy);

					return InBounds(truex,truey,sx,sy);
					};

		long	InBounds(long x,long y,long sx,long sy)
					{
					return EW_WITHIN(mx,my,x,y,sx,sy);
					};

		long	IsUsed(long part)
					{
					// determine if specified part of event has been used
					// accounts for old EW_USED_TRUE possibility
					return ( GetUsed()==EW_USED_TRUE || (GetUsed() & part) );
					};

		long	IsDragged(void)
					{
					// note, also specifies: used, but not just grabbed
					return ( (GetUsed()&EW_USED_DRAGX) ||
												(GetUsed()&EW_USED_DRAGY) );
					};

		long	Matches(EW_Event *other)
					{
					return IsOnly(other->GetSource(),other->GetItem(),
															other->GetState());
					}

		long	Intersects(EW_Event *mask)
					{
					return Is(mask->GetSource(),mask->GetItem(),
														mask->GetState()) &&
		( !GetWidgetFlags() || (GetWidgetFlags() & mask->GetWidgetFlags()) );
					}

		long	Is(long _source,long _item,long _state)
					{
					return (( _source==EW_EVENT_ANY || (source & _source) )&&
							( _item==EW_ITEM_ALL || (item & (_item) ) ) &&
							( _state==EW_STATE_ANY || (state & (_state) ) ) );
					};

		long	IsOnly(long _source,long _item,long _state)
					{
					return	(source == _source) &&
							(item == (_item) ) &&
							(state == (_state) );
					};

		long	IsKey(long key)
					{
					return Is(EW_EVENT_KEYBOARD,key,EW_STATE_ANY);
					};

		long	IsKeyOfState(long key,long state);

		long	IsKeyTyped(long key)
					{ return IsKeyOfState(key,EW_STATE_PRESS|EW_STATE_REPEAT);};
		long	IsKeyPressed(long key)
					{ return IsKeyOfState(key,EW_STATE_PRESS); };
		long	IsKeyRepeated(long key)
					{ return IsKeyOfState(key,EW_STATE_REPEAT); };
		long	IsKeyReleased(long key)
					{ return IsKeyOfState(key,EW_STATE_RELEASE); };

		long	IsMouse(void)
					{
					return ( IsMouseMotion() || IsMouseButton() );
					};

		long	IsMouseMotion(void)
					{
					return Is(EW_EVENT_MOUSEPOSITION,EW_ITEM_ALL,EW_STATE_ANY);
					};

		long	IsMouseButton(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_ALL,EW_STATE_ANY);
					};

		long	IsMousePress(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_ALL,EW_STATE_PRESS);
					};

		long	IsMouseRelease(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_ALL,EW_STATE_RELEASE);
					};

		long	IsLeftMouseButton(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,EW_STATE_ANY);
					};

		long	IsLeftMousePress(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,EW_STATE_PRESS);
					};

		long	IsLeftMouseRelease(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_LEFT,
															EW_STATE_RELEASE);
					};

		long	IsMiddleMouseButton(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_MIDDLE,EW_STATE_ANY);
					};

		long	IsMiddleMousePress(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_MIDDLE,
															EW_STATE_PRESS);
					};

		long	IsLeftMouseMotion(void)
					{
					return IsMouseMotion() && (mousebuttons & EW_ITEM_LEFT);
					};

		long	IsMiddleMouseRelease(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_MIDDLE,
															EW_STATE_RELEASE);
					};

		long	IsMiddleMouseMotion(void)
					{
					return IsMouseMotion() && (mousebuttons & EW_ITEM_MIDDLE);
					};

		long	IsRightMouseButton(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_RIGHT,EW_STATE_ANY);
					};

		long	IsRightMousePress(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_RIGHT,
															EW_STATE_PRESS);
					};

		long	IsRightMouseRelease(void)
					{
					return Is(EW_EVENT_MOUSEBUTTON,EW_ITEM_RIGHT,
															EW_STATE_RELEASE);
					};

		long	IsRightMouseMotion(void)
					{
					return IsMouseMotion() && (mousebuttons & EW_ITEM_RIGHT);
					};

		long	IsWhenUnMapped(void)
					{
					return Is(EW_EVENT_WHEN_UNMAPPED,EW_ITEM_ALL,EW_STATE_ANY);
					};

		long	IsLocational(void)
					{
					return Is(EW_EVENT_LOCATIONAL,EW_ITEM_ALL,EW_STATE_ANY);
					};

		long	IsClipboard(void)
					{
					return Is(EW_EVENT_CLIPBOARD,EW_ITEM_ALL,EW_STATE_ANY);
					};

		long	IsClipboardReady(void)
					{
					return Is(EW_EVENT_CLIPBOARD,
										EW_ITEM_CLIPBOARD_READY,EW_STATE_ANY);
					};

		long	IsClipboardFail(void)
					{
					return Is(EW_EVENT_CLIPBOARD,
										EW_ITEM_CLIPBOARD_FAIL,EW_STATE_ANY);
					};

		long	IsExpective(void)
					{
					return (Is(EW_EVENT_EXPECTIVE,EW_ITEM_ALL,EW_STATE_ANY));
					};

		long	IsModal(void)
					{
					return (Is(EW_EVENT_MODAL,EW_ITEM_ALL,EW_STATE_ANY));
					};

		long	IsModalBeep(void)
					{
					return (Is(EW_EVENT_MODAL_BEEP,EW_ITEM_ALL,EW_STATE_PRESS));
					};

		long	IsSystem(void)
					{
					return Is(EW_EVENT_SYSTEM,EW_ITEM_ALL,EW_STATE_ANY);
					};

		long	IsCloseWindow(void)
					{
					return Is(EW_EVENT_SYSTEM,EW_ITEM_CLOSEWINDOW,EW_STATE_ANY);
					};

		long	IsQuitApplication(void)
					{
					return Is(EW_EVENT_SYSTEM,EW_ITEM_QUIT_APP,EW_STATE_ANY);
					};

		long	IsExposure(void)
					{
					return Is(EW_EVENT_SYSTEM,EW_ITEM_EXPOSURE,EW_STATE_ANY);
					};

		long	IsWork(void)
					{
					return Is(EW_EVENT_PERIODIC,EW_ITEM_WORK,EW_STATE_ANY);
					};

		long	IsTimer(long index=EW_STATE_ANY)
					{
					return ( IsOnly(EW_EVENT_PERIODIC,EW_ITEM_TIMER,index) ||
								(index==EW_STATE_ANY &&
								Is(EW_EVENT_PERIODIC,EW_ITEM_TIMER,index)) );
					};

		long	IsFocusChange(void)
					{
					return Is(EW_EVENT_FOCUS,EW_ITEM_FOCUSCHANGE,EW_STATE_ANY);
					};

		long	IsIdleMouse(void)
					{
					return Is(EW_EVENT_FOCUS,EW_ITEM_IDLEMOUSE,EW_STATE_ANY);
					};

		long	Print(long hexcode);
	};



/******************************************************************************
class	EW_EventQueue


Keywords:
******************************************************************************/
class EW_EventQueue : public GPL_DoubleList<EW_Event>
	{
	public:
				EW_EventQueue(void)		{};
virtual			~EW_EventQueue(void)	{};

		void	AppendCopyOf(EW_Event *other);
		void	ProcessAllEvents(void);
	};



#if EW_WL==EW_MEX

#define EW_MAX_MEX_KEY			1024
#define EW_MAX_MEX_DEVICE		1024
#define EW_MAX_MEX_CROSS_KEY	69
const unsigned char ewMexCrossMapping[EW_MAX_MEX_CROSS_KEY][2]=
	{
	ZEROKEY,		'0',
	ONEKEY,			'1',
	TWOKEY,			'2',
	THREEKEY,		'3',
	FOURKEY,		'4',
	FIVEKEY,		'5',
	SIXKEY,			'6',
	SEVENKEY,		'7',
	EIGHTKEY,		'8',
	NINEKEY,		'9',
	AKEY,			'a',
	BKEY,			'b',
	CKEY,			'c',
	DKEY,			'd',
	EKEY,			'e',
	FKEY,			'f',
	GKEY,			'g',
	HKEY,			'h',
	IKEY,			'i',
	JKEY,			'j',
	KKEY,			'k',
	LKEY,			'l',
	MKEY,			'm',
	NKEY,			'n',
	OKEY,			'o',
	PKEY,			'p',
	QKEY,			'q',
	RKEY,			'r',
	SKEY,			's',
	TKEY,			't',
	UKEY,			'u',
	VKEY,			'v',
	WKEY,			'w',
	XKEY,			'x',
	YKEY,			'y',
	ZKEY,			'z',
	SPACEKEY,		' ',
	SEMICOLONKEY,	';',
	PERIODKEY,		'.',
	COMMAKEY,		',',
	QUOTEKEY,		'\'',
	ACCENTGRAVEKEY,	'`',
	MINUSKEY,		'-',
	VIRGULEKEY,		'/',
	BACKSLASHKEY,	'\\',
	EQUALKEY,		'=',
	LEFTBRACKETKEY,	'[',
	RIGHTBRACKETKEY,']',
	PAD0,			'0',
	PAD1,			'1',
	PAD2,			'2',
	PAD3,			'3',
	PAD4,			'4',
	PAD5,			'5',
	PAD6,			'6',
	PAD7,			'7',
	PAD8,			'8',
	PAD9,			'9',
	PADPERIOD,		'.',
	PADMINUS,		'-',
	PADCOMMA,		',',
	PADVIRGULEKEY,	'/',
	PADASTERKEY,	'*',
	PADPLUSKEY,		'+',
	DELKEY,			127,
	ESCKEY,			27,
	RETKEY,			13,
	PADENTER,		13,
	BACKSPACEKEY,	8,
	};

#endif
