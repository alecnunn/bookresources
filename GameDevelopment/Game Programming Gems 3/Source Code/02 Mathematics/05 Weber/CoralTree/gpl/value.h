#ifndef GPL_VALUE_H
#define GPL_VALUE_H

/******************************************************************************

Coral Tree gpl/value.h
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
#include <gpl/str.h>
#include <gpl/message.h>

/***********************************************************************
enum GPL_ValueType
***********************************************************************/
enum GPL_ValueType
{
	GPL_VALUETYPE_NULL		= 0,
	GPL_VALUETYPE_INTEGER	= 1,
	GPL_VALUETYPE_REAL		= 2,
	GPL_VALUETYPE_STRING	= 3,
	GPL_VALUETYPE_BLOCK		= 4,
	GPL_VALUETYPE_MESSAGE	= 5
};

/******************************************************************************
class GPL_Value

Provide a value using the following abstract types:
	Null
	Integer
	Real
	String
	Block
******************************************************************************/
class GPL_Value : public GPL_Persistent
{
	public:
		GPL_Value(void);
		GPL_Value(const GPL_Value & v);
		GPL_Value(const GPL_Real r);
		GPL_Value(const GPL_Real r, const GPL_Real p);
		GPL_Value(const GPL_String & s);
		GPL_Value(const GPL_Block & b);
		GPL_Value(const GPL_Message & m);
		virtual ~GPL_Value(void);

		GPL_Value & operator=(const GPL_Value & v);
		GPL_Boolean operator==(const GPL_Value & v) const
		{
			return 0 == Compare(v);
		}

		GPL_ValueType GetType(void) const { return type; }
		GPL_Boolean IsNull(void) const;
		GPL_Integer GetInteger(void) const;
		GPL_Real GetReal(void) const;
		GPL_Real GetPrecision(void) const;
		GPL_String GetString(void) const;
		GPL_Address GetBlock(unsigned long & l) const;
		void GetBlock(GPL_Block &b) const;
		GPL_Message &GetMessage(void) const;

		virtual long Compare(const GPL_Value & v) const;

		void SetNull(void);
		void SetInteger(const GPL_Integer i);
		void SetReal(const GPL_Real r);
		void SetReal(const GPL_Real r, const GPL_Real p);
		void SetPrecision(const GPL_Real p);
		void SetString(const GPL_String & s);
		void SetBlock(GPL_Address b, const unsigned long l);
		void SetBlock(const GPL_Block &b);
		void SetMessage(const GPL_Message &m);
		void SetPerType(const GPL_String &s);

		// GPL_Persistent
		virtual GPL_Boolean Pack(GPL_Message & msg) const;
		virtual GPL_Boolean Unpack(GPL_Message & msg);

		// GPL_Object
		GPL_OBJECT_INTERFACE(GPL_Value);

	protected:
		GPL_ValueType type;
		union
		{
			GPL_Integer integer;
			GPL_Real real;
			GPL_Address address;

			// THIS IS JUST A POINTER TO KEEP GPL_Value LIGHTWEIGHT
			GPL_Message *message;
		};
		union
		{
			unsigned long length;
			GPL_Real precision;
		};

		void Clear(void);
};

#endif /* GPL_VALUE_H */
