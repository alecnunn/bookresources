/******************************************************************************

Coral Tree wds/wstr.h
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

#define WDS_STRING_CENTER			0x0001
#define WDS_STRING_RIGHT			0x0002
#define WDS_STRING_SINGLELINE		0x0004
#define WDS_STRING_HALF_SHIFT		0x0008
#define WDS_STRING_CURSOR_UP		0x0010
#define WDS_STRING_CURSOR_DOWN		0x0020
#define WDS_STRING_CURSOR_STARTLINE	0x0040
#define WDS_STRING_CURSOR_ENDLINE	0x0080
#define WDS_STRING_CURSOR_OP		0x00F0
#define WDS_STRING_DRAW				0x0100

#define WDS_STRING_STYLE_NORMAL		0
#define WDS_STRING_STYLE_CURSOR		1
#define WDS_STRING_STYLE_RANGE		2

#define WDS_STRING_STYLE_NULL		0
#define WDS_STRING_STYLE_3D			1
#define WDS_STRING_STYLE_UNDERLINE	2
#define WDS_STRING_STYLE_BOLD		4
#define WDS_STRING_STYLE_DEFAULT	WDS_STRING_STYLE_NULL

#define WDS_STRING_MISS				-1
#define WDS_STRING_MISS_TO_LEFT		-2

// for SetHighlight()
#define WDS_HIGHLIGHT_OFF			0
#define WDS_HIGHLIGHT_LINE_OUT		1
#define WDS_HIGHLIGHT_BACKFILL		2

// for SetConfirmCallback(fp,???)
#define WDS_CONFIRM_NULL			0x0000
#define WDS_CONFIRM_ALLOW_EMPTY		0x0001
#define WDS_CONFIRM_MARK_INVALID	0x0002
#define WDS_CONFIRM_AUTO_HIGHLIGHT	0x0004

// date
#define WDS_CONFIRM_LEAD_ZERO	0x0008

#define WDS_CONFIRM_USA			0x0000
#define WDS_CONFIRM_ISO			0x0010

#define WDS_CONFIRM_DASH		0x0000
#define WDS_CONFIRM_STROKE		0x0020

// time
#define WDS_CONFIRM_AMPM		0x0000
#define WDS_CONFIRM_24HR		0x0040
#define WDS_CONFIRM_ELAPSED		0x0080
#define WDS_CONFIRM_SECONDS		0x0100

// phone
#define WDS_CONFIRM_PRETTY		0x0008
#define WDS_CONFIRM_NANP		0x0010
#define WDS_CONFIRM_NANP_PRETTY	(WDS_CONFIRM_NANP|WDS_CONFIRM_PRETTY)
#define WDS_CONFIRM_CCITT		0x0020
#define WDS_CONFIRM_MAP_CHAR	0x0040

#define WDS_CONFIRM_INVALID_RESPONSE	"Invalid"


typedef long (*WDS_String_ConfirmCallback)(WDS_String *,long);

typedef GPL_DoubleList<WDS_String_ConfirmCallback>
												WDS_String_ConfirmCallbackList;



/******************************************************************************
class	WDS_String : public EW_String

	confirmation function should potentially correct the string
		and return non-zero if changed

Keywords:
******************************************************************************/
class WDS_String : public EW_String
	{
	private:
        long	(*confirm_function)(WDS_String *,long);

		long 	firstline;
		long	cursor;
		long	cursorx;
		long	rangemark;
		long	cursorbg;
		long	cursorfg;
		long	style;
		long	editable;
		long	highlight;		// highlighting occurs in owner, marked here
		long	confirming;
		long	confirm_flags;
		long	allow_confirm;
		long	is_invalid;

		long 	numberlines;	// use Operate() to update
		long 	widest;			// use Operate() to update
		char	displaychar;

		long	RemoveInvalid(char *buffer);
		void	InvalidateTo(char *buffer);

	public:
				WDS_String();

		void	CopyFromWDS_String(WDS_String *other);

		long	Operate(EW_Window *root,long x,long y,long w,long h,
												long mx,long my,long flags);

		void	SetStyle(long set)				{ style = set; }
		void	SetStyleIncluding(long set)		{ style |= set; }
		void	SetStyleExcluding(long set)		{ style ^= (style&set); }
		long	GetStyle(void)					{ return style; }

virtual void	BufferChanged(void)
					{
					// prevent loops
					if(!confirming && allow_confirm)
						{
						confirming=TRUE;
						Confirm();
						confirming=FALSE;
						}
					};

		void	SetConfirmCallback(WDS_String_ConfirmCallback fp,long flags)
					{
					confirm_function=fp;
					confirm_flags=flags;
					BufferChanged();
					};

		long	Confirm(void);

		void	DenyConfirm()	{ allow_confirm=FALSE;	};
		void	AllowConfirm()
					{
					if(editable)
						allow_confirm=TRUE;
					BufferChanged();
					};

		long	GetNumberLines(EW_Window *root,long maxwidth);
		long	GetStringSpace(EW_Window *root,long maxwidth,
													long *width,long *height);

		void	SetEditable(long set)
					{
					editable=set;
					if(!editable)
						DenyConfirm();
					};
		long	GetEditable(void)				{ return editable; };

		void	SetHighlight(long set)			{ highlight=set; };
		long	GetHighlight(void)				{ return highlight; };

		void	SetInvalid(long set)			{ is_invalid=set; };
		long	GetInvalid(void)				{ return is_invalid; };

		void	SetDisplayChar(char set)		{ displaychar=set; };
		char	GetDisplayChar(void)			{ return displaychar; };

				// accounting for string display character
		long	ModCharWidth(EW_Window *root,char data);
		long	ModStringWidth(EW_Window *root,char *data);

		void	SetCursorColor(long fg,long bg)
					{
					cursorfg=fg;
					cursorbg=bg;
					};
		void	GetCursorColor(long *fg,long *bg)
					{
					*fg=cursorfg;
					*bg=cursorbg;
					};

		long	GetLastCursorX(void)			{ return cursorx; };

		long	GetFirstLine(void)				{ return firstline; }
		void	SetFirstLine(long x)			{ firstline = x; }

		void    SetCursorPosition(long x)		{ cursor=x; };
		long    GetCursorPosition(void)			{ return cursor; };

		void    SetRangeMarkPosition(long x)	{ rangemark=x; };
		long    GetRangeMarkPosition(void)		{ return rangemark; };

static	float	SuffixToFloat(char *buffer);

static	long	ConfirmDate(WDS_String *string,long flags);
static	long	ConfirmTime(WDS_String *string,long flags);
static	long	ConfirmDateTime(WDS_String *string,long flags);
static	long	ConfirmPhone(WDS_String *string,long flags);
static	long	ConfirmSSN(WDS_String *string,long flags);
static	long	ConfirmLong(WDS_String *string,long flags);
static	long	ConfirmFloat(WDS_String *string,long flags);
static	long	ConfirmDouble(WDS_String *string,long flags);
	};

