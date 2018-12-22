#ifndef GPL_DESC_H
#define GPL_DESC_H

/******************************************************************************

Coral Tree gpl/desc.h
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

#include <gpl/typedefs.h>
#include <gpl/persist.h>
#include <gpl/list.h>
#include <gpl/str.h>

/***********************************************************************
class GPL_Description

Provide a description of data using abstract types.
***********************************************************************/
class GPL_Description : public GPL_Persistent
{
	public:
		GPL_Description(void);
		virtual ~GPL_Description(void);

		GPL_Boolean operator==(const GPL_Description & d) const
		{
			return 0 == Compare(d);
		}
		GPL_Boolean operator!=(const GPL_Description & d) const
		{
			return 0 == !Compare(d);
		}
		GPL_Description & operator=(const GPL_Description & d)
		{
			values.Clear();
			format.Clear();
			Append(d);
			return *this;
		}
		GPL_Description & operator+=(const GPL_Description & d)
		{
			Append(d);
			return *this;
		}

		GPL_String GetFormat(const GPL_Boolean elide = FALSE) const;

		long Compare(const GPL_Description & d) const;
		GPL_Boolean Matches(const GPL_Description & d) const;

		void Set(const char *f, ...);
		void VSet(const char *f, va_list *ap);
		void Append(const GPL_Description & d);

		// GPL_Persistent
		virtual long Pack(GPL_Message & msg) const;
		virtual long Unpack(GPL_Message & msg);

		GPL_List<GPL_Value> &GetValueList(void) { return values; }

		// GPL_Object
		GPL_OBJECT_INTERFACE(GPL_Description);

	protected:
		GPL_String format;
		GPL_List<GPL_Value> values;
};

#endif /* GPL_DESC_H */
