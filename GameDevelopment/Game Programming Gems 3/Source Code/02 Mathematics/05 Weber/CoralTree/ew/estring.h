/******************************************************************************

Coral Tree ew/estring.h
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



/******************************************************************************
class	EW_String

******************************************************************************/
class EW_String
	{
	private:
		long	charsize;	// in bits
		char	*buffer;

	public:
				EW_String(void);

virtual			~EW_String();

virtual void	BufferChanged(void)	{ };

		void	SetCharSize(long bits);
		long	GetCharSize(void) { return charsize; };

		unsigned int	Length(void);

		long	GetLength(void)	{ return strlen(buffer); };
		void	NewLength(unsigned int newsize);

		long	NewBuffer(const char *newbuffer);
		void	CopyFromString(EW_String *otherstring);
		void	CopyToBuffer(char *outbuffer);

		char	*Buffer(void)
					{
					// note, this gives direct access to string buffer
					// may be invalid following any other EW_String Op
					// If possible, use Compare(char *),
					//   GetString(char *), or Buffer(long).
					return buffer;
					};

		long	Buffer(long position);

		long	Compare(char *otherbuffer);
		long	Compare(EW_String *otherstring);

		long	InsertChar(long position,char c);
		long	InsertBuffer(long position,char *insertion);

		long	DeleteChar(long position);
		long 	DeleteRange(long position,long number);

		void	ConvertToNativeFormat(void);
		void	ConvertFromNativeFormat(void);
	};
