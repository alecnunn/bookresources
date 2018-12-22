#ifndef GPL_STR_H
#define GPL_STR_H

/******************************************************************************

Coral Tree gpl/str.h
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

#include <string.h>

#include <gpl/array.h>
#include <gpl/tools.h>

#define	GPL_STRLEN(s)	(((char *) 0 == (s)) ? 0 : strlen(s))

/***********************************************************************
class GPL_String

Reference-counted string object.
***********************************************************************/
class GPL_String
{

	public:
						GPL_String(void) {}
						GPL_String(char * s);
						GPL_String(const char * s);
						GPL_String(char * s, const unsigned long l);
						GPL_String(const char * s, const unsigned long l);
						GPL_String(const unsigned long n, const char c = 0);
						GPL_String(const GPL_CharPtr & d) : data(d) {}
						GPL_String(const GPL_String & s);
virtual					~GPL_String(void);

						operator char *(void) { return data.GetData(); }
						operator const char *(void) const { return data.GetData(); }

		GPL_String		& operator=(const GPL_String & s);
		GPL_String		& operator=(char * s);
		GPL_String		& operator=(const char * s);

		/**********************************************************************
		These operators blindly step into the data; if the object is null or
		the index out-of-bounds, a segmentation violation *WILL* follow.
		Caveat coder.
		**********************************************************************/
		char			& operator[](const unsigned long i)
							{
							return data[i];
							}
const	char			& operator[](const unsigned long i) const
							{ return data[i]; }

inline	GPL_Boolean		operator==(const GPL_String & s) const
							{ return operator==(s.GetData()); }
inline	GPL_Boolean		operator==(char * s) const
							{ return operator==((const char *) s); }
		GPL_Boolean		operator==(const char * s) const;
inline	GPL_Boolean		operator!=(const GPL_String & s) const
							{ return !(operator==(s.GetData())); }
inline	GPL_Boolean		operator!=(char * s) const
							{ return !(operator==((const char *) s)); }
inline	GPL_Boolean		operator!=(const char * s) const
							{ return !(operator==(s)); }
inline	GPL_Boolean		operator<(const GPL_String & s) const
							{ return 0 > Compare (s.GetData (), s.GetLength()); }
inline	GPL_Boolean		operator<(char * s) const
							{ return 0 > Compare ((const char *) s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator<(const char * s) const
							{ return 0 > Compare (s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator<=(const GPL_String & s) const
							{ return 0 >= Compare (s.GetData (), s.GetLength()); }
inline	GPL_Boolean		operator<=(char * s) const
							{ return 0 >= Compare ((const char *) s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator<=(const char * s) const
							{ return 0 >= Compare (s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator>(const GPL_String & s) const
							{ return 0 < Compare (s.GetData (), s.GetLength()); }
inline	GPL_Boolean		operator>(char * s) const
							{ return 0 < Compare ((const char *) s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator>(const char * s) const
							{ return 0 < Compare (s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator>=(const GPL_String & s) const
							{ return 0 <= Compare (s.GetData (), s.GetLength()); }
inline	GPL_Boolean		operator>=(char * s) const
							{ return 0 <= Compare ((const char *) s, GPL_STRLEN(s)); }
inline	GPL_Boolean		operator>=(const char * s) const
							{ return 0 <= Compare (s, GPL_STRLEN(s)); }

inline	GPL_Boolean		IsNull(void) const
							{
							return (char *) 0 == data.GetData();
							}
		GPL_CharPtr &	GetCharPtr(void) { return data; }
const	GPL_CharPtr &	GetCharPtr(void) const { return data; }
		char			*GetData(void) { return data.GetData(); }
		char			*GetData(void) const { return data.GetData(); }
		unsigned long	GetLength(void) const;
		unsigned long	GetSize(void) const;
		GPL_String		GetSubString(const unsigned long from,
									const unsigned long to) const;
		GPL_String		Duplicate(void) const;
		long			Compare (const GPL_String & s) const
							{ return Compare (s.GetData (), s.GetLength()); }
		long			Compare (char *s) const
							{ return Compare ((const char *) s, GPL_STRLEN(s)); }
		long			Compare (const char *s) const
							{ return Compare (s, GPL_STRLEN(s)); }
		long			Compare (const char *s, const unsigned long l) const;
		long			SubCompare (const char *s, const unsigned long l) const;
		long			Find(const char c) const
							{ return Find(&c, 1); }
		long			Find(const GPL_String & s) const
							{ return Find(s.GetData()); }
		long			Find(const char *s) const
							{ return Find(s, GPL_STRLEN(s)); }
		long			Find(const char *s, const unsigned long l) const;

inline	void			SetData(char *s)
							{
							SetData((const char *) s, GPL_STRLEN(s));
							}
inline	void			SetData(const char *s)
							{
							SetData(s, GPL_STRLEN(s));
							}
inline	void			SetData(char *s, const unsigned long l)
							{
							SetData((const char *) s, l);
							}
		void			SetData(const char *s, const unsigned long l);
		void			Clear(void);
		void			Resize(const unsigned long l, const char c = 0);
		void			Prepend(const char c)
							{
							Insert(0, &c, 1);
							}
		void			Prepend(const GPL_String & string)
							{
							Insert(0, string.data.GetData(), string.GetLength());
							}
		void			Prepend(const char *string)
							{
							Insert(0, string, GPL_STRLEN(string));
							}
		void			Prepend(const char *string, const unsigned long l)
							{
							Insert(0, string, l);
							}
		void			Append(const char c)
							{
							Append(&c, 1);
							}
		void			Append(const GPL_String & string)
							{
							Append(string.data.GetData(), string.GetLength());
							}
		void			Append(const char *string)
							{
							Append(string, GPL_STRLEN(string));
							}
		void			Append(const char *string, const unsigned long l);
		void			Insert(const unsigned long at, const char c)
							{
							Insert(at, &c, 1);
							}
		void			Insert(const unsigned long at,
								const GPL_String & string)
							{
							Insert(at, string.data.GetData(),
									string.GetLength());
							}
		void			Insert(const unsigned long at, const char *string)
							{
							Insert(at, string, GPL_STRLEN(string));
							}
		void			Insert(const unsigned long at, const char *string,
								const unsigned long l);
		void			Remove(const unsigned long at, const unsigned long l);

		long			ToLong(void) const;

		long			Scanf(char *format, ...) const;
		long			VScanf(char *format, va_list ap) const;
		void			Printf(char *format, ...);
		void			VPrintf(char *format, va_list ap);

		void			Join(const GPL_String & delimiter,
								const GPL_String & string);
static	GPL_String		Join(const GPL_String & delimiter,
								const GPL_Array<GPL_String> & strings);
		void			Split(const GPL_String & delimiter,
							GPL_String & first, GPL_String & second) const;
static	void			Split(const GPL_String & string,
								const GPL_String & delimiter,
								GPL_String & first, GPL_String & second);
		GPL_String		Escape(const GPL_CharPtr & chars) const
							{
							return Escape(*this, chars);
							}
static	GPL_String		Escape(const GPL_String & string,
								const GPL_CharPtr & chars);
		GPL_String		Unescape(const GPL_CharPtr & chars) const
							{
							return Unescape(*this, chars);
							}
static	GPL_String		Unescape(const GPL_String & string,
								const GPL_CharPtr & chars);

static	unsigned long	HashFunction(const GPL_String & string);

	protected:
		GPL_CharPtr	data;
};

/***********************************************************************
class GPL_StringTokenizer

String tokenizer.
***********************************************************************/
class GPL_StringTokenizer
{
	public:
		GPL_StringTokenizer(void) : internalString((char *) 0), strict(FALSE) {}
		virtual ~GPL_StringTokenizer(void) {}

		void Reset(const GPL_String & s);
		void Reset(const GPL_String & s, const GPL_String & d)
		{
			Reset(s, d, strict);
		}
		void Reset(const GPL_String & s, const GPL_String & d,
					const GPL_Boolean m);
		GPL_String Next(void)
		{
			return Next(delimiters);
		}
		GPL_String Next(const GPL_String & d);
		GPL_String Extract(const int position)
		{
			return Extract(delimiters, position);
		}
		GPL_String Extract(const GPL_String & d, const int position)
		{
			register char *token =
				gplExtract(buffer.GetData(), NULL, d.GetData(), position);
			return token;
		}

	protected:
		GPL_String buffer;
		GPL_String delimiters;
		char *internalString;
		GPL_Boolean strict;
		long next_index;
};

/***********************************************************************
class GPL_StringTokenizerStdin

String tokenizer for standard input.
***********************************************************************/
class GPL_StringTokenizerStdin : public GPL_StringTokenizer
{
	private:
		GPL_Array<GPL_String>	line;
	public:
		GPL_StringTokenizerStdin(){ delimiters = " \t\n"; }
virtual	~GPL_StringTokenizerStdin(){}

		GPL_Array<GPL_String> &GetLine()
		{
			char *tmp;
			tmp = osdGetStdin(NULL);
			buffer = tmp;
			internalString = buffer.GetData();
			osdFree(tmp);
			next_index = 0;

			GPL_String s;
			line.Clear();
			//while(s = Next())
			//{
			//	line.AppendElement(s);
			//}
			for(s = Next(); !(s.IsNull()); s = Next())
			{
				line.AppendElement(s);
			}

			return line;
		}
};

#endif /* GPL_STR_H */
