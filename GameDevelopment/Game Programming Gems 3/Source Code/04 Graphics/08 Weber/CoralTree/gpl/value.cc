
/******************************************************************************

Coral Tree gpl/value.cc
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

#include <gpl/gpl.h>

/******************************************************************************
class GPL_Value
******************************************************************************/
GPL_Value::GPL_Value(void)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
}

GPL_Value::GPL_Value(const GPL_Real r)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
	SetReal(r);
	SetPrecision(0.0);
}

GPL_Value::GPL_Value(const GPL_Real r, const GPL_Real p)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
	SetReal(r);
	SetPrecision(p);
}

GPL_Value::GPL_Value(const GPL_String & s)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
	SetString(s);
}

GPL_Value::GPL_Value(const GPL_Block & b)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
	SetBlock(b);
}

GPL_Value::GPL_Value(const GPL_Message & m)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
	SetMessage(m);
}

GPL_Value::GPL_Value(const GPL_Value & v)
{
	type = GPL_VALUETYPE_NULL;
	Clear();
	operator=(v);
}

GPL_Value::~GPL_Value(void)
{
	Clear();
}

GPL_Value & GPL_Value::operator=(const GPL_Value & v)
{
	Clear();
	type = v.type;
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		integer = v.integer;
		break;
	case GPL_VALUETYPE_REAL:
		real = v.real;
		precision = v.precision;
		break;
	case GPL_VALUETYPE_STRING:
		// Falls through...
	case GPL_VALUETYPE_BLOCK:
		length = v.length;
		address = (GPL_Address) osdMalloc(length);
		osdMemcpy(address, v.address, length);
		break;
	case GPL_VALUETYPE_MESSAGE:
		message = new GPL_Message();
		*message = v.GetMessage();
		break;
	default:
		// NOP
		break;
	}
	return *this;
}

GPL_Boolean GPL_Value::IsNull(void) const
{
	return GPL_VALUETYPE_NULL == type;
}

GPL_Integer GPL_Value::GetInteger(void) const
{
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		return integer;
	case GPL_VALUETYPE_REAL:
		return (GPL_Integer) real;
	case GPL_VALUETYPE_STRING:
		return (GPL_Integer) atol((char *) address);
	case GPL_VALUETYPE_BLOCK:
		return (GPL_Integer) length;
	case GPL_VALUETYPE_MESSAGE:
		return (GPL_Integer)(message->Length());
	default:
		return (GPL_Integer) 0;
	}
}

GPL_Real GPL_Value::GetReal(void) const
{
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		return (GPL_Real) integer;
	case GPL_VALUETYPE_REAL:
		return real;
	case GPL_VALUETYPE_STRING:
		return (GPL_Real) atof((char *) address);
	case GPL_VALUETYPE_BLOCK:
		return (GPL_Real) length;
	case GPL_VALUETYPE_MESSAGE:
		return (GPL_Real)(message->Length());
	default:
		return (GPL_Real) 0.0;
	}
}

GPL_Real GPL_Value::GetPrecision(void) const
{
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		return (GPL_Real) 0.0;
	case GPL_VALUETYPE_REAL:
		return precision;
	case GPL_VALUETYPE_STRING:
		return (GPL_Real) 0.0;
	case GPL_VALUETYPE_BLOCK:
		return (GPL_Real) 0.0;
	case GPL_VALUETYPE_MESSAGE:
		return (GPL_Real) 0.0;
	default:
		return (GPL_Real) 0.0;
	}
}

GPL_String GPL_Value::GetString(void) const
{
	GPL_String s;
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		s.Printf("%d", integer);
		break;
	case GPL_VALUETYPE_REAL:
		s.Printf("%f", real);
		break;
	case GPL_VALUETYPE_STRING:
		// Falls through...
	case GPL_VALUETYPE_BLOCK:
		s = GPL_String((const char *) address, length);
		break;
	case GPL_VALUETYPE_MESSAGE:
		s = message->ToString();
	default:
		break;
	}
	return s;
}

GPL_Address GPL_Value::GetBlock(unsigned long & l) const
{
	switch(type)
	{
	case GPL_VALUETYPE_STRING:
		// Falls through...
	case GPL_VALUETYPE_BLOCK:
		l = length;
		return address;
	case GPL_VALUETYPE_MESSAGE:
		return message->GetBuffer(&l);
	default:
		l = 0;
		return GPL_BAD_ADDRESS;
	}
}

void GPL_Value::GetBlock(GPL_Block & b) const
{
	char *a;
	unsigned long l;
	switch(type)
	{
	case GPL_VALUETYPE_STRING:
		// Falls through...
	case GPL_VALUETYPE_BLOCK:
		b.Set(address, length);
		return;
	case GPL_VALUETYPE_MESSAGE:
		a = message->GetBuffer(&l);
		b.Set(a,l);
		return;
	default:
		b.Set(GPL_BAD_ADDRESS, 0);
		return;
	}
}

GPL_Message &GPL_Value::GetMessage(void) const
{
	static GPL_Message dummy;
	switch(type)
	{
		case GPL_VALUETYPE_MESSAGE:
			return *message;
		default:
			return dummy;
	}
}

long GPL_Value::Compare(const GPL_Value & v) const
{
	if (type != v.type)
	{
		return (long) type - (long) v.type;
	}
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		return integer - v.integer;
	case GPL_VALUETYPE_REAL:
		return (long) real - (long) v.real;
	case GPL_VALUETYPE_STRING:
		if (length != v.length)
		{
			return length - v.length;
		}
		return strncmp((const char *) address, (const char *) v.address, length);
	case GPL_VALUETYPE_BLOCK:
		if (length != v.length)
		{
			return length - v.length;
		}
		return osdMemcmp((const char *) address, (const char *) v.address, length);
	case GPL_VALUETYPE_MESSAGE:
		char *a1, *a2;
		unsigned long l1, l2;
		a1 = message->GetBuffer(&l1);
		a2 = v.message->GetBuffer(&l2);
		if(l1 != l2)
		{
			return l1 - l2;
		}
		return osdMemcmp((const char *)a1, (const char *)a2, l1);
	default:
		return 0;
	}
}

void GPL_Value::SetNull(void)
{
	Clear();
}

void GPL_Value::SetInteger(const GPL_Integer i)
{
	Clear();
	type = GPL_VALUETYPE_INTEGER;
	integer = i;
}

void GPL_Value::SetReal(const GPL_Real r)
{
	Clear();
	type = GPL_VALUETYPE_REAL;
	real = r;
	precision = 0.0;
}

void GPL_Value::SetReal(const GPL_Real r, const GPL_Real p)
{
	Clear();
	type = GPL_VALUETYPE_REAL;
	real = r;
	precision = p;
}

void GPL_Value::SetPrecision(const GPL_Real p)
{
	if(type == GPL_VALUETYPE_REAL)
	{
		precision = p;
	}
}

void GPL_Value::SetString(const GPL_String & s)
{
	Clear();
	type = GPL_VALUETYPE_STRING;
	length = s.GetLength();
	address = osdMalloc(length);
	osdMemcpy(address, s.GetData(), length);
}

void GPL_Value::SetBlock(GPL_Address b, const unsigned long l)
{
	Clear();
	type = GPL_VALUETYPE_BLOCK;
	length = l;
	address = osdMalloc(length);
	osdMemcpy(address, b, length);
}

void GPL_Value::SetBlock(const GPL_Block & b)
{
	Clear();
	type = GPL_VALUETYPE_BLOCK;
	length = b.GetLength();
	address = osdMalloc(length);
	osdMemcpy(address, b.GetAddress(), length);
}

void GPL_Value::SetMessage(const GPL_Message &m)
{
	Clear();
	type = GPL_VALUETYPE_MESSAGE;
	message = new GPL_Message(m);
}

void GPL_Value::SetPerType(const GPL_String &s)
{
	if(s.IsNull()) return;
	GPL_ValueType t;
	t = type;
	Clear();
	type = t;
	switch(type)
	{
		case GPL_VALUETYPE_INTEGER:
			integer = atol((const char *)s);
			break;
		case GPL_VALUETYPE_REAL:
			real = atof((const char *)s);
			break;
		case GPL_VALUETYPE_STRING:
			length = s.GetLength();
			address = osdMalloc(length);
			osdMemcpy(address, s.GetData(), length);
			break;
		case GPL_VALUETYPE_BLOCK:
			break;
		case GPL_VALUETYPE_MESSAGE:
			break;
		default:
			break;
	}
	
}

void GPL_Value::Clear(void)
{
	if ((GPL_VALUETYPE_STRING == type || GPL_VALUETYPE_BLOCK == type) &&
		GPL_BAD_ADDRESS != address)
	{
		osdFree(address);
	}
	else if(GPL_VALUETYPE_MESSAGE == type)
	{
		delete message;
		message = NULL;
	}
	type = GPL_VALUETYPE_NULL;
	precision = (GPL_Real) 0.0;
	real = (GPL_Real) 0.0;
}

long GPL_Value::Pack(GPL_Message & msg) const
{
	char *a;unsigned long l;
	msg.AppendUnsignedChar((unsigned char) type);
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		msg.AppendUnsignedLong((unsigned long) integer);
		break;
	case GPL_VALUETYPE_REAL:
		msg.AppendDouble((double) real);
		msg.AppendDouble((double) precision);
		break;
	case GPL_VALUETYPE_STRING:
	case GPL_VALUETYPE_BLOCK:
		msg.AppendBlock((char *) address, length);
		break;
	case GPL_VALUETYPE_MESSAGE:
		a = message->GetBuffer(&l);
		msg.AppendBlock(a, l);
		break;
	default:
		// NOP
		break;
	}
	return TRUE;
}

long GPL_Value::Unpack(GPL_Message & msg)
{
	char *a;unsigned long l;
	Clear();
	type = (GPL_ValueType) msg.ReadUnsignedChar();
	switch(type)
	{
	case GPL_VALUETYPE_INTEGER:
		integer = (GPL_Integer) msg.ReadUnsignedLong();
		precision = (GPL_Real)0.0;
		break;
	case GPL_VALUETYPE_REAL:
		real = (GPL_Real) msg.ReadDouble();
		precision = (GPL_Real) msg.ReadDouble();
		break;
	case GPL_VALUETYPE_STRING:
	case GPL_VALUETYPE_BLOCK:
		address = (GPL_Address) msg.ReadBlock(&length);
		break;
	case GPL_VALUETYPE_MESSAGE:
		message = new GPL_Message();
		a = msg.ReadBlock(&l);
		message->SetBuffer(a, l, 0);
		break;
	default:
		// NOP
		break;
	}
	return TRUE;
}

GPL_OBJECT_IMPLEMENTATION(GPL_Value, 0x40000004, "GPL_Value");

