/******************************************************************************

Coral Tree wds/getstr.h
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



// element WDS_GETSTR_SCOPE_SHALLOW
#define	WDS_GETSTR_NO_FLAGS			0
#define	WDS_GETSTR_SCOPE_SHALLOW	1
#define	WDS_GETSTR_CLIPBOARD_AUTO	2



/*****************************************************************************
class	WDS_GetString

A string entry widget

*****************************************************************************/
class WDS_GetString : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_GetString);

	private:

		long			focused,last_focused;
		long			draw_pending;
		GPL_BitGroup	*keyset;

	protected:

		WDS_String		buffer;			// backup when editing string
		WDS_String		string;			// current/last string

		long	selection;					// first press position
		long	cursor;						// starting-from position
		long	range_mark;					// to-mark position
		long 	maxlength;
		long	justification;
		long	depth;
		long 	selecting;
		long 	requesting;
		long 	keystroke_entered;
		long	acceptitem,abortitem;

		void	RegisterKeyboard(long on);
		void	RegisterMouse(long on);
		void	BroadcastSelection(void);
		void	RequestSelection(void);

	public:
				WDS_GetString(void);

virtual	void	ChangeState(long newstate,EW_Event *event);

virtual	long	Operate(EW_Event *event);
virtual	void	Draw(EW_Redraw *redraw);
virtual	long	Resize(EW_Resize *resize);

		void	SetAcceptKeys(long set)		{ acceptitem=set; };
		long	GetAcceptKeys(void)			{ return acceptitem; };

		void	SetAbortKeys(long set)		{ abortitem=set; };
		long	GetAbortKeys(void)			{ return abortitem; };

		void	SetMaxLength(long set)		{ maxlength=set; };
		long	GetMaxLength(void)			{ return maxlength; };

		void	SetString(char *a_string);

		void			SetKeySet(GPL_BitGroup *set)	{ keyset=set; }; 
		GPL_BitGroup	*GetKeySet(void)				{ return keyset; };

		WDS_String *GetStoreBuffer(void)		{ return &buffer; };
		WDS_String *GetString(void)			{ return &string; };

		void	DeleteRange(void);

		void	SetStringDisplayChar(char set)
					{
					buffer.SetDisplayChar(set);
					string.SetDisplayChar(set);
					};

		void	StartEdit(long cursor_position,EW_Event *event);
		void	StopEdit(long restore,EW_Event *event);
		void	SelectWord(long index);
		void	SelectAll(void);

				// accounting for string display character
		long	ModCharWidth(char *data);
		long	ModStringWidth(char *data);

		long	AllowClipboard(void);
	};
