#ifndef GPL_BITGROUP_H
#define GPL_BITGROUP_H

/******************************************************************************

Coral Tree gpl/bitgroup.h
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
#include <gpl/str.h>
#include <gpl/depict.h>

class GPL_BitGroup;

/***********************************************************************
class GPL_Bit

A bit constituent of a GPL_BitGroup.
Note: No checking is done on the address of the bit group given in
the constructor.  Null pointers *will* cause severe problems.
***********************************************************************/
class GPL_Bit
{
	public:
		GPL_Bit(const unsigned long i, GPL_BitGroup *b) : index(i), bitGroup(b) {}
		virtual ~GPL_Bit(void) {}

		inline GPL_Boolean operator==(const GPL_Bit & b) const;
		inline GPL_Boolean operator==(const GPL_Boolean b) const;
		inline GPL_Boolean operator!=(const GPL_Bit & b) const;
		inline GPL_Boolean operator!=(const GPL_Boolean b) const;
		inline GPL_Bit & operator=(const GPL_Boolean b);

	private:
		unsigned long index;
		GPL_BitGroup *bitGroup;

		GPL_Bit(void) {}
};

/***********************************************************************
class GPL_BitGroup

A bit group of arbitrary length.
***********************************************************************/
class GPL_BitGroup : public GPL_Depictable
{
	public:
		GPL_BitGroup(void);
		GPL_BitGroup(const GPL_BitGroup & bitGroup);
		virtual ~GPL_BitGroup(void);

		GPL_BitGroup	& operator=(const GPL_BitGroup & bitGroup);
		inline GPL_BitGroup operator~(void) const
		{
			return Invert();
		}
		inline GPL_BitGroup operator|(const GPL_BitGroup & bitGroup) const
		{
			return Join(bitGroup);
		}
		inline GPL_BitGroup operator&(const GPL_BitGroup & bitGroup) const
		{
			return Intersect(bitGroup);
		}
		inline GPL_BitGroup operator-(const GPL_BitGroup & bitGroup) const
		{
			return Subtract(bitGroup);
		}
		inline GPL_Boolean operator==(const GPL_BitGroup & bitGroup) const
		{
			return 0 == Compare(bitGroup);
		}
		inline GPL_Boolean operator!=(const GPL_BitGroup & bitGroup) const
		{
			return 0 != Compare(bitGroup);
		}
		inline GPL_Bit operator[](const unsigned long i)
		{
			return GPL_Bit(i, this);
		}

		inline unsigned long GetCount(void) const { return length; }
		GPL_Boolean GetBit(const unsigned long i) const;
		uint32 GetGroup(const unsigned long offset,
								const uint32 mask);
		GPL_Boolean IsClear(void) const;

		long Compare(const GPL_BitGroup & bitGroup) const;
		GPL_BitGroup Invert(void) const;
		GPL_BitGroup Join(const GPL_BitGroup & bitGroup) const;
		GPL_BitGroup Intersect(const GPL_BitGroup & bitGroup) const;
		GPL_BitGroup Subtract(const GPL_BitGroup & bitGroup) const;

		void Resize(const unsigned long s);
		void Clear(void);
		void SetBit(const unsigned long i);
		void SetRange(const unsigned long from, const unsigned long to);
		void ClearBit(const unsigned long i);
		void ClearRange(const unsigned long from, const unsigned long to);
		void ClearAllBits(void);
		void SetGroup(const unsigned long offset, const uint32 group,
						const uint32 mask);

		// GPL_Depictable
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity) const;
		virtual GPL_Boolean Parse(const GPL_String & string);

	protected:
		uint32 *bits;
		unsigned long size;
		unsigned long length;
};

#endif /* GPL_BITGROUP_H */
