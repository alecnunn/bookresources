/******************************************************************************

Coral Tree gpl/desc.cc
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

/***********************************************************************
class GPL_Description
***********************************************************************/
GPL_Description::GPL_Description(void)
{
}

GPL_Description::~GPL_Description(void)
{
}

GPL_String GPL_Description::GetFormat(const GPL_Boolean elide) const
{
	GPL_String f = format;
	if (elide)
	{
		register const unsigned long n = f.GetLength();
		for (register unsigned long i = 0; n > i; ++i)
		{
			switch(f[i])
			{
			case 'B':
				f[i] = 'b';
				break;
			case 'I':
				f[i] = 'i';
				break;
			case 'N':
				f[i] = 'n';
				break;
			case 'R':
				f[i] = 'r';
				break;
			case 'S':
				f[i] = 's';
				break;
			case 'M':
				f[i] = 'm';
				break;
			default:
				// NOP
				break;
			}
		}
	}
	return f;
}

long GPL_Description::Compare(const GPL_Description & d) const
{
	// Check for identical formats.
	long equivalence = format.Compare(d.format);
	if (0 != equivalence)
	{
		return equivalence;
	}

	// Check for identical values.
	//return values.Compare(d.values);
	return !(values == d.values);
}

#define GPL_DESC_CHECK_ONE(t)	if (values.GetCount() <= k || (t) != values[k].GetType()) \
{ return FALSE; } \
if (d.values.GetCount() <= l || (t) != d.values[l].GetType()) \
{ return FALSE; }
#define	GPL_DESC_CHECK_TWO(a,b)	if ((a) != (b)) { return FALSE; }

GPL_Boolean GPL_Description::Matches(const GPL_Description & d) const
{
	register unsigned long i = 0;	// index into format
	register unsigned long j = 0;	// index into d.format
	register char *f = format.GetData();
	register char *g = d.format.GetData();
	const unsigned long m = format.GetLength();
	const unsigned long n = d.format.GetLength();
	unsigned long k = 0;	// index into values
	unsigned long l = 0;	// index into d.values
	GPL_Address o = GPL_BAD_ADDRESS;	// values[k].GetBlock()
	GPL_Address p = GPL_BAD_ADDRESS;	// d.values[l].GetBlock()
	unsigned long q = 0;	// length of values[k].GetBlock()
	unsigned long r = 0;	// length of d.values[l].GetBlock()
	GPL_Real the_diff;
	GPL_Boolean hunting = FALSE;
	while (m > i && n > j)
	{
		// Wild-cards are not allowed on the right-hand side.
		if ('.' == g[j] || '*' == g[j])
		{
			return FALSE;
		}
		if (hunting)
		{
			if (f[i] == g[j])
			{
				hunting = FALSE;
				++i;
			}
			if ('I' == g[j] || 'R' == g[j] || 'S' == g[j] || 'B' == g[j] || 'M' == g[j])
			{
				++l;
			}
			++j;
		}
		else
		{
			switch(f[i])
			{
			case '.':
				// Matches anything.
				if ('I' == g[j] || 'R' == g[j] || 'S' == g[j] || 'B' == g[j] || 'M' == g[j])
				{
					++l;
				}
				break;
			case '*':
				// If there are no more characters...
				if (m <= i + 1)
				{
					return TRUE;
				}
				hunting = (f[i + 1] != g[j]);
				if ('I' == g[j] || 'R' == g[j] || 'S' == g[j] || 'B' == g[j] || 'M' == g[j])
				{
					++l;
				}
				break;
			case 'N':
				if ('N' == g[i])
				{
					GPL_DESC_CHECK_ONE(GPL_VALUETYPE_NULL);
					GPL_DESC_CHECK_TWO(values[k].IsNull(), d.values[l].IsNull());
					++k;
					++l;
				}
				else
				{
					return FALSE;
				}
				break;
			case 'I':
				if ('I' == g[i])
				{
					GPL_DESC_CHECK_ONE(GPL_VALUETYPE_INTEGER);
					GPL_DESC_CHECK_TWO(values[k].GetInteger(), d.values[l].GetInteger());
					++k;
					++l;
				}
				else
				{
					return FALSE;
				}
				break;
			case 'R':
				if ('R' == g[i])
				{
					GPL_DESC_CHECK_ONE(GPL_VALUETYPE_REAL);
					the_diff = values[k].GetReal() - d.values[l].GetReal();
					if((the_diff > values[k].GetPrecision()) || (the_diff < -(values[k].GetPrecision())))
					{
						return FALSE;
					}
					//GPL_DESC_CHECK_TWO(values[k].GetReal(), d.values[l].GetReal());
					++k;
					++l;
				}
				else
				{
					return FALSE;
				}
				break;
			case 'S':
				if ('S' == g[i])
				{
					GPL_DESC_CHECK_ONE(GPL_VALUETYPE_STRING);
					GPL_DESC_CHECK_TWO(values[k].GetString(), d.values[l].GetString());
					++k;
					++l;
				}
				else
				{
					return FALSE;
				}
				break;
			case 'B':
				if ('B' == g[i])
				{
					GPL_DESC_CHECK_ONE(GPL_VALUETYPE_BLOCK);
					o = values[k].GetBlock(q);
					p = d.values[l].GetBlock(r);
					GPL_DESC_CHECK_TWO(q, r);
					if (0 != osdMemcmp(o, p, q))
					{
						return FALSE;
					}
					++k;
					++l;
				}
				else
				{
					return FALSE;
				}
				break;
			case 'M':
				if ('M' == g[i])
				{
					if(0 != values[k].Compare(d.values[l]))
					{
						return FALSE;
					}
					++k;
					++l;
				}
				else
				{
					return FALSE;
				}
				break;
			case 'n':
				if ('N' == g[i])
				{
					++l;
				}
				else if ('n' != g[i])
				{
					return FALSE;
				}
				break;
			case 'i':
				if ('I' == g[i])
				{
					++l;
				}
				else if ('i' != g[i])
				{
					return FALSE;
				}
				break;
			case 'r':
				if ('R' == g[i])
				{
					++l;
				}
				else if ('r' != g[i])
				{
					return FALSE;
				}
				break;
			case 's':
				if ('S' == g[i])
				{
					++l;
				}
				else if ('s' != g[i])
				{
					return FALSE;
				}
				break;
			case 'b':
				if ('B' == g[i])
				{
					++l;
				}
				else if ('b' != g[i])
				{
					return FALSE;
				}
				break;
			case 'm':
				if ('M' == g[i])
				{
					++l;
				}
				else if ('m' != g[i])
				{
					return FALSE;
				}
				break;
			default:
				return FALSE;
			}
			++i;
			++j;
		}
	}

	return (m <= i && n <= j);
}

void GPL_Description::Set(const char *f, ...)
{
	va_list ap;
	va_start(ap, f);
	VSet(f, &ap);
	va_end(ap);
}

void GPL_Description::VSet(const char *f, va_list *ap)
{
	// Clear the previous format and values.
	// TODO: this full copy of old should be more efficient...
	//       replace values instead of full copy and start over.
	GPL_String old_format;
	GPL_List<GPL_Value> old_values;
	old_format = format;
	old_values = values;
	format = f;
	values.Clear();

	GPL_Address c_ptr;
	unsigned long len;
	GPL_Real r1, r2;
	GPL_Message *m;

	GPL_Value value;
	register const unsigned long o = old_format.GetLength();
	register const unsigned long n = format.GetLength();
	for (register unsigned long i = 0; n > i; ++i)
	{
		switch(f[i])
		{
		case 'i':
		case 'n':
		case 'f':
		case 'r':
		case 's':
		case 'b':
		case 'm':
			gplLog("GPL WARNING","lowercase specifier in GPL_Description::VSet, reverting to \'.\'");
		case '.':
			if(i < o)
			{
				format[i] = old_format[i];
				value = old_values[i];
				values.Append(value);
			}
			else
			{
				format[i] = 'N';
				value.SetNull();
				values.Append(value);
			}
			break;
		case '*':
			if(i < o)
			{
				format.Resize(old_format.GetLength());
			}
			else
			{
				format.Resize(format.GetLength()-1);
			}
			while(i < o)
			{
				
				format[i] = old_format[i];
				value = old_values[i];
				values.Append(value);
				i++;
			}
			i = n; // Force Finish
			break;
		case 'N':
			value.SetNull();
			values.Append(value);
			break;
		case 'I':
			value.SetInteger((GPL_Integer) va_arg(*ap, long));
			values.Append(value);
			break;
		case 'F':
			// separated va_arg calls to different lines.  VC++5 has problem
			r1 = (GPL_Real) va_arg(*ap, double);
			r2 = (GPL_Real) va_arg(*ap, double);
			value.SetReal(r1,r2);
			format[i] = 'R';
			values.Append(value);
			break;
		case 'R':
			value.SetReal((GPL_Real) va_arg(*ap, double));
			values.Append(value);
			break;
		case 'S':
			value.SetString((GPL_String) va_arg(*ap, char*));
			values.Append(value);
			break;
		case 'B':
			// separated va_arg calls to different lines.  VC++5 has problem
			c_ptr = (GPL_Address)va_arg(*ap, char*);
			len = va_arg(*ap, unsigned long);
			value.SetBlock(c_ptr, len);
			values.Append(value);
			break;
		case 'M':
			m = (GPL_Message *)va_arg(*ap, char*);
			value.SetMessage(*m);
			values.Append(value);
			break;
		default:
			break;
		}
	}
}

void GPL_Description::Append(const GPL_Description & d)
{
	format.Append(d.format);
	register const unsigned long n = d.values.GetCount();
	for (register unsigned long i = 0; n > i; ++i)
	{
		values.Append(d.values[i]);
	}
}

long GPL_Description::Pack(GPL_Message & msg) const
{
	msg.AppendString(format);
	register const unsigned long n = values.GetCount();
	msg.AppendUnsignedLong(n);
	for (register unsigned long i = 0; n > i; ++i)
	{
		values[i].Pack(msg);
	}
	return TRUE;
}

long GPL_Description::Unpack(GPL_Message & msg)
{
	format = msg.ReadString();
	register const unsigned long n = msg.ReadUnsignedLong();
	register unsigned long i;
	values.Clear();
	GPL_Value dummy;
	for(i = 0; i < n; ++i)
	{
		dummy.Unpack(msg);
		values.Append(dummy);
	}
	return TRUE;
}

GPL_OBJECT_IMPLEMENTATION(GPL_Description, 0x40000000, "GPL_Description");

