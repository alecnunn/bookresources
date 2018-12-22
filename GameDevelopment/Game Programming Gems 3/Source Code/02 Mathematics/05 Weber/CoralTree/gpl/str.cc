
/******************************************************************************

Coral Tree gpl/str.cc
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
class GPL_String
***********************************************************************/
/******************************************************************************

******************************************************************************/
GPL_String::GPL_String(char * s)
{
	SetData((const char *) s, GPL_STRLEN(s));
}

/******************************************************************************

******************************************************************************/
GPL_String::GPL_String(const char * s)
{
	SetData(s, GPL_STRLEN(s));
}

/******************************************************************************

******************************************************************************/
GPL_String::GPL_String(char * s, const unsigned long l)
{
	SetData((const char *) s, l);
}

/******************************************************************************

******************************************************************************/
GPL_String::GPL_String(const char * s, const unsigned long l)
{
	SetData(s, l);
}

/******************************************************************************

******************************************************************************/
GPL_String::GPL_String(const unsigned long n, const char c)
{
	Resize(n, c);
}

/******************************************************************************

******************************************************************************/
GPL_String::GPL_String(const GPL_String & s) : data(s.data)
{
}

/******************************************************************************

******************************************************************************/
GPL_String::~GPL_String(void)
{
}

/******************************************************************************

******************************************************************************/
GPL_String &
GPL_String::operator =(const GPL_String & s)
{
	data = s.data;
	return *this;
}

/******************************************************************************

******************************************************************************/
GPL_String &
GPL_String::operator =(char * s)
{
	SetData((const char *) s, GPL_STRLEN(s));
	return *this;
}

/******************************************************************************

******************************************************************************/
GPL_String &
GPL_String::operator =(const char * s)
{
	SetData(s, GPL_STRLEN(s));
	return *this;
}

/******************************************************************************

******************************************************************************/
long GPL_String::operator==(const char * s) const
{
	// If both are null...
	register const long isNull = IsNull();
	if (isNull && (const char *) 0 == s)
	{
		return TRUE;
	}

	// If only one is null...
	if (isNull || (const char *) 0 == s)
	{
		return FALSE;
	}

	// If neither is null...
	return 0 == strcmp(data.GetData(), s);
}

/******************************************************************************

******************************************************************************/
unsigned long GPL_String::GetLength(void) const
{
	return IsNull() ? 0 : GPL_STRLEN(data.GetData());
}

/******************************************************************************

******************************************************************************/
unsigned long GPL_String::GetSize(void) const
{
	return IsNull() ? 0 : data.GetSize();
}

/******************************************************************************

******************************************************************************/
GPL_String GPL_String::GetSubString(const unsigned long from,
									const unsigned long to) const
{
	return data.GetRange(from, to);
}

/******************************************************************************

******************************************************************************/
GPL_String GPL_String::Duplicate(void) const
{
	GPL_String string;
	register const unsigned long length = GetLength();
	if (0 < length)
	{
		string.Resize(length);
		osdMemcpy(string.data.GetData(), data.GetData(), length);
	}
	return string;
}


/******************************************************************************

******************************************************************************/
long GPL_String::Compare (const char *s, const unsigned long l) const
{
	if (IsNull ())
	{
		if ((const char *) 0 == s)
		{
			return 0;
		}
		return -1;
	}
	if ((const char *) 0 == s)
	{
		return 1;
	}
	const char *d = GetData ();
	register const unsigned long n = GetLength ();
	register unsigned long i = 0;
	while (n > i && l > i) {

		if (s[i] < d[i]) {

			return 1;
		}
		else if (s[i] > d[i]) {

			return -1;
		}

		++i;
	}

	if (l < n) {

		return 1;
	}
	else if (l > n) {

		return -1;
	}

	return 0;
}


/******************************************************************************

******************************************************************************/
long GPL_String::SubCompare (const char *s, const unsigned long l) const
{
	if (IsNull ())
	{
		if ((const char *) 0 == s || 0 == l)
		{
			return 0;
		}
		return -1;
	}
	if ((const char *) 0 == s || 0 == l)
	{
		return 1;
	}
	return strncmp(data.GetData(), s, l);
}

/******************************************************************************

******************************************************************************/
long GPL_String::Find(const char *s, const unsigned long l) const
{
	if ((const char *) 0 != s && 0 < l)
	{
		register const unsigned long n = GetLength();
		register char *d = data.GetData();
		for (register long i = 0; n > (unsigned long) i + (l - 1); ++i, ++d)
		{
			if (0 == strncmp(s, d, l))
			{
				return i;
			}
		}
	}
	return -1;
}

/******************************************************************************

******************************************************************************/
void GPL_String::SetData(const char *s, const unsigned long l)
{
	if ((char *) 0 == s)
	{
		data.Clear();
	}
	else
	{
		// Expand the data to fit the incoming characters.
		data.Resize(1 + l);

		// Copy only the valid characters from the incoming
		// string since it may be a substring and s[l] may
		// *not* be the null character.
		data.SetRange(s, l, 0);

		// Just in case s is a substring...
		data[l] = (char) 0;
	}
}

/******************************************************************************

******************************************************************************/
void GPL_String::Clear(void)
{
	data.Clear();
}

/******************************************************************************

******************************************************************************/
void GPL_String::Resize(const unsigned long l, const char c)
{
	register const unsigned long length = GetLength();
	data.Resize(1 + l);

	// This is not very nice but faster than operator[].
	if (l > length)
	{
		osdMemset((data.GetData() + length), c, (size_t) (l - length));
	}

	// Always null-terminate.
	data[l] = 0;
}

/******************************************************************************

******************************************************************************/
void GPL_String::Append(const char *string, const unsigned long l)
{
	// If the array has no data...
	register const unsigned long length = GetLength();
	if (0 == length)
	{
		SetData(string, l);
	}

	// If the array has data...
	else
	{
		Insert(length, string, l);
	}
}

/******************************************************************************

******************************************************************************/
void GPL_String::Insert(const unsigned long at, const char *string,
						const unsigned long l)
{
	// If at is greater than or equal to GetLength() (i.e., the null
	// character), the insertion will appear to fail because the null
	// will be left in place, inadvertently prematurely terminating
	// the new string.
	data.InsertRange(string, l, at);
}

/******************************************************************************

******************************************************************************/
void GPL_String::Remove(const unsigned long at, const unsigned long l)
{
	data.RemoveRange(at, at + l - 1);
}

/******************************************************************************

******************************************************************************/
long GPL_String::ToLong(void) const
{
	return atol(data.GetData());
}


/******************************************************************************

******************************************************************************/
long GPL_String::Scanf(char *format, ...) const
{
	register char *s = data.GetData();
	if ((char *) 0 == s)
	{
		return -1;
	}
	va_list args;
	va_start(args, format);
	register const long rc = osdVSScanf(s, format, args);
	va_end(args);
	return rc;
}

/******************************************************************************

******************************************************************************/
long GPL_String::VScanf(char *format, va_list args) const
{
	register char *s = data.GetData();
	if ((char *) 0 == s)
	{
		return -1;
	}
	return osdVSScanf(s, format, args);
}
/******************************************************************************

******************************************************************************/
void GPL_String::Printf(char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *s = osdVSPrintf(format, args);
	operator=(s);
	osdFree(s);
	va_end(args);
}

/******************************************************************************

******************************************************************************/
void GPL_String::VPrintf(char *format, va_list args)
{
	char *s = osdVSPrintf(format, args);
	operator=(s);
	osdFree(s);
}

/******************************************************************************

******************************************************************************/
void GPL_String::Join(const GPL_String & delimiter, const GPL_String & string)
{
	if (0 < GetLength())
	{
		Append(delimiter);
	}
	Append(string);
}

/******************************************************************************

******************************************************************************/
GPL_String GPL_String::Join(const GPL_String & delimiter,
								const GPL_Array<GPL_String> & strings)
{
	GPL_String string;
	register const unsigned long n = strings.GetCount();
	for (register unsigned long i = 0; n > i; ++i)
	{
		string.Join(delimiter, strings[i]);
	}
	return string;
}

/******************************************************************************

******************************************************************************/
void GPL_String::Split(const GPL_String & delimiter, GPL_String & first,
						GPL_String & second) const
{
	Split(*this, delimiter, first, second);
}

/******************************************************************************

******************************************************************************/
void GPL_String::Split(const GPL_String & string, const GPL_String & delimiter,
						GPL_String & first, GPL_String & second)
{
	register const long i = string.Find(delimiter);
	if (-1 == i)
	{
		first = string;
		second = GPL_String();
	}
	else
	{
		if (0 == i)
		{
			first = GPL_String();
		}
		else
		{
			first = string.GetSubString(0, i - 1);
		}
		second = string.GetSubString(i + delimiter.GetLength(),
										string.GetLength() - 1);
	}
}

/******************************************************************************

******************************************************************************/
unsigned long GPL_String::HashFunction(const GPL_String & string)
{
	register unsigned long hash = 0;
	register const char *s = string.GetData();
	while (s && *s)
	{
		hash += (unsigned long) *s;
		s++;
	}
	return hash;
}

GPL_String GPL_String::Escape(const GPL_String & string,
								const GPL_CharPtr & chars)
{
	GPL_String buffer(2 * string.GetLength(), 0);
	register char *s = string.GetData();
	if ((char *) 0 != s)
	{
		register unsigned long i = 0;
		register const char *c = chars.GetData();
		while ('\0' != *s)
		{
			// If this character should be escaped...
			if ((char *) 0 != strchr(c, *s))
			{
				// Add the back-slash.
				buffer.data[i++] = '\\';
			}

			// Add the character no matter what.
			buffer.data[i++] = *s;

			// Next!
			++s;
		}
	}
	return buffer;
}

GPL_String GPL_String::Unescape(const GPL_String & string,
								const GPL_CharPtr & chars)
{
	GPL_String buffer(string.GetLength(), 0);
	register char *s = string.GetData();
	if ((char *) 0 != s)
	{
		register unsigned long i = 0;
		register const char *c = chars.GetData();
		while ('\0' != *s)
		{
			// If this character should be unescaped...
			if ('\\' == *s && 0 != *(s +1) && (char *) 0 != strchr(c, *(s + 1)))
			{
				// Remove the back-slash.
				++s;
			}

			// Add the character no matter what.
			buffer.data[i++] = *s;

			// Next!
			++s;
		}
	}
	return buffer;
}

/***********************************************************************
class GPL_StringTokenizer
***********************************************************************/
/***********************************************************************
void GPL_StringTokenizer::Reset(const GPL_String & s)
***********************************************************************/
void GPL_StringTokenizer::Reset(const GPL_String & s)
{
	// Duplicate to ensure a private copy.
	buffer = s.Duplicate();
	internalString = buffer.GetData();
	next_index = 0;
}

/***********************************************************************
void GPL_StringTokenizer::Reset(const GPL_String & s, const GPL_String & d,
								GPL_Boolean m)
***********************************************************************/
void GPL_StringTokenizer::Reset(const GPL_String & s, const GPL_String & d,
								const GPL_Boolean m)
{
	// Duplicate to ensure a private copy.
	strict = m;
	buffer = s.Duplicate();
	internalString = buffer.GetData();
	delimiters = d;
	next_index = 0;
}

/***********************************************************************
GPL_String GPL_StringTokenizer::Next(const GPL_String & d)
***********************************************************************/
GPL_String GPL_StringTokenizer::Next(const GPL_String & d)
{
#ifdef GPL_USE_STRTOK_IN_STRINGTOKENIZER
	register char *token = strtok(internalString, d.GetData());
	internalString = (char *) 0;
	return token;
#else
	register long len;
	long i,start_index;
	if(!internalString) 
	{
		return (char *) 0;
	}
	if(next_index == -1) 
	{
		return (char *) 0;
	}
	len = d.GetLength();

	long hit_delim_at_all = FALSE;
	long hit_delim_this_time;
	long have_non_null_token = FALSE;;
	start_index = next_index;

	do 
	{
		hit_delim_this_time = FALSE;
		if(internalString[next_index] == 0)
		{
			next_index = -1;
			break;
		}
		for(i=0;i<len;i++)
		{
			if(internalString[(unsigned long)next_index] == d[(unsigned long)i])
			{
				hit_delim_at_all = TRUE;
				hit_delim_this_time = TRUE;
				internalString[next_index] = 0;
				break;
			}
		}
		if(hit_delim_at_all)
		{
			if(strict) 
			{
				next_index++;
				break;
			}
			if(!hit_delim_this_time)
			{
				if(have_non_null_token) {break;}
				else 
				{
					start_index = next_index;
					hit_delim_at_all = FALSE;
					have_non_null_token = TRUE;
				}
			}
		}
		else
		{
			have_non_null_token = TRUE;
		}
	} while(++next_index);

	if(!strict && !have_non_null_token) return (char *) 0; 
	return &(internalString[start_index]);
#endif
}
