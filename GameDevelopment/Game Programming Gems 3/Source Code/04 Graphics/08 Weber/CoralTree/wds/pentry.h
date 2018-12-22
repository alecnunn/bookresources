/******************************************************************************

Coral Tree wds/pentry.h
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



// WDS_PointerEntry::SetPointerType()
#define WDS_LONG	0
#define WDS_FLOAT	1
#define WDS_BUFFER	2
#define WDS_STRING	3



/******************************************************************************
class	WDS_PointerEntry

******************************************************************************/
class WDS_PointerEntry : public WDS_GetString
	{
	WDS_DECLARE_TYPE(WDS_PointerEntry);

	private:

		long		copying;

		long		ref_type;

		void		*ref_buffer;
		long		*ref_long,last_long;
		float		*ref_float,last_float;
		char		*ref_char;
		WDS_String	*ref_string,last_string;

		long		ValueChanged(void);

		void		CopyValueToBuffer(void);
		void		CopyBufferToValue(void);

	public:
					WDS_PointerEntry(void);

virtual	void		ChangeState(long newstate,EW_Event *event);
virtual	long		Resize(EW_Resize *resize);

		void		SetPointer(void *set);
		void		*GetPointer(void)			{ return ref_buffer; };

		void		SetPointerType(long set);
		long		GetPointerType(void)		{ return ref_type; };
	};
