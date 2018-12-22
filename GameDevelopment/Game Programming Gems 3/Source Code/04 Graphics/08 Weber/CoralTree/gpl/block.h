#ifndef GPL_BLOCK_H
#define GPL_BLOCK_H

/******************************************************************************

Coral Tree gpl/block.h
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

#include <gpl/depict.h>

/***********************************************************************
class GPL_Block
***********************************************************************/
class GPL_Block : public GPL_Array<GPL_Byte>, public GPL_Depictable
{
	public:
		GPL_Block(void);
		GPL_Block(const GPL_Array<GPL_Byte> & b);
		GPL_Block(GPL_Address a, const unsigned long l);
virtual ~GPL_Block(void);

		GPL_Block & operator=(const GPL_Array<GPL_Byte> & b);

		GPL_Opaque CheckSum(void) const;
		GPL_Block ShiftLeft(const unsigned long shift) const;
		GPL_Block ShiftRight(const unsigned long shift) const;

		// GPL_Depictable
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity =
										GPL_VERBOSITY_CONCISE) const;
		virtual GPL_Boolean Parse(const GPL_String & string);

		// Stream-like
		unsigned long pointer;
		void Rewind() { pointer = 0; }
		GPL_Boolean CopyNext(GPL_Byte *chunk, unsigned long *sz);
		GPL_Boolean CopyTo(GPL_Byte delim, GPL_Byte *chunk, unsigned long *sz);

		// Historical
		GPL_Address GetAddress(void) const;
		unsigned long GetLength(void) const;
		GPL_Address Get(unsigned long & l);
		void Set(GPL_Address a, const unsigned long l);
};

#endif /* GPL_BLOCK_H */
