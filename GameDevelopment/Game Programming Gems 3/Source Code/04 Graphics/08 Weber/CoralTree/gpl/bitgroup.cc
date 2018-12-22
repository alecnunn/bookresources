
/******************************************************************************

Coral Tree gpl/bitgroup.cc
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

#define	GPL_MAXIMUM_SIZE(x)	(((x) + (OSD_BITS_PER_WORD - 1)) / OSD_BITS_PER_WORD)
#define	GPL_MAJOR(x)	((x) / OSD_BITS_PER_WORD)
#define	GPL_MINOR(x)	((x) % OSD_BITS_PER_WORD)

/***********************************************************************
class GPL_Bit
***********************************************************************/
inline GPL_Boolean GPL_Bit::operator==(const GPL_Bit & b) const
{
	return bitGroup->GetBit(index) == b.bitGroup->GetBit(b.index);
}
inline GPL_Boolean GPL_Bit::operator==(const GPL_Boolean b) const
{
	return bitGroup->GetBit(index) == b;
}
inline GPL_Boolean GPL_Bit::operator!=(const GPL_Bit & b) const
{
	return bitGroup->GetBit(index) != b.bitGroup->GetBit(b.index);
}
inline GPL_Boolean GPL_Bit::operator!=(const GPL_Boolean b) const
{
	return bitGroup->GetBit(index) != b;
}
inline GPL_Bit & GPL_Bit::operator=(const GPL_Boolean b)
{
	// To account for cases where integer values are
	// silently converted in GPL_Boolean values,
	// compare with zero.
	if (FALSE == b)
	{
		bitGroup->ClearBit(index);
	}
	else
	{
		bitGroup->SetBit(index);
	}
	return *this;
}

/***********************************************************************
class GPL_BitGroup
***********************************************************************/
GPL_BitGroup::GPL_BitGroup(void)
	: bits((uint32 *) GPL_BAD_ADDRESS), size(0), length(0)
{
}

GPL_BitGroup::GPL_BitGroup(const GPL_BitGroup & bitGroup)
	: bits((uint32 *) GPL_BAD_ADDRESS), size(0), length(0)
{
	operator=(bitGroup);
}

GPL_BitGroup::~GPL_BitGroup(void)
{
	Clear();
}

GPL_BitGroup & GPL_BitGroup::operator=(const GPL_BitGroup & bitGroup)
{
	Clear();
	size = bitGroup.size;
	length = bitGroup.length;
	bits = new uint32[size];
	osdMemcpy(bits, bitGroup.bits, sizeof(uint32) * size);
	return *this;
}

GPL_Boolean GPL_BitGroup::GetBit(const unsigned long i) const
{
	if (i < length)
	{
		return bits[GPL_MAJOR(i)] & (0x01 << GPL_MINOR(i));
	}
	return FALSE;
}

uint32 GPL_BitGroup::GetGroup(const unsigned long offset, const uint32 mask)
{
	// If this offset if out-of-bounds...
	if (offset + OSD_BITS_PER_WORD >= length)
	{
		return 0;
	}

	// If this is aligned on a word boundary...
	if (0 == GPL_MINOR(offset))
	{
		return mask & (bits[GPL_MINOR(offset)]);
	}

	register uint32 lower = bits[GPL_MAJOR(offset)];
	lower = lower >> GPL_MINOR(offset);
	register uint32 upper = bits[GPL_MAJOR(offset) + 1];
	upper = upper << (OSD_BITS_PER_WORD - GPL_MINOR(offset));
	return mask & (upper | lower);
}

GPL_Boolean GPL_BitGroup::IsClear(void) const
{
	for (register unsigned long i = 0; size > i; ++i)
	{
		if (bits[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

long GPL_BitGroup::Compare(const GPL_BitGroup & bitGroup) const
{
	if (length == bitGroup.length)
	{
		register const unsigned long n = size;
		for (register unsigned long i = 0; n > i; ++i)
		{
			if (bits[i] != bitGroup.bits[i])
			{
				return (long) bits[i] - (long) bitGroup.bits[i];
			}
		}
		return 0;
	}
	return (long) length - (long) bitGroup.length;
}

GPL_BitGroup GPL_BitGroup::Invert(void) const
{
	GPL_BitGroup b;
	b.Resize(length);
	for (register unsigned long i = 0; size > i; ++i)
	{
		b.bits[i] = ~(bits[i]);
	}
	return b;
}

GPL_BitGroup GPL_BitGroup::Join(const GPL_BitGroup & bitGroup) const
{
	GPL_BitGroup b;
	b.Resize((length < bitGroup.length) ? bitGroup.length : length);
	register unsigned long i = 0;
	while (b.size > i)
	{
		b.bits[i++] = 0;
	}
	i = 0;
	while (size > i)
	{
		b.bits[i] = b.bits[i] | bits[i];
		++i;
	}
	i = 0;
	while (bitGroup.size > i)
	{
		b.bits[i] = b.bits[i] | bitGroup.bits[i];
		++i;
	}
	return b;
}

GPL_BitGroup GPL_BitGroup::Intersect(const GPL_BitGroup & bitGroup) const
{
	GPL_BitGroup b;
	b.Resize((length < bitGroup.length) ? length : bitGroup.length);
	register unsigned long i = 0;
	while (b.size > i)
	{
		b.bits[i] = bits[i] & bitGroup.bits[i];
		++i;
	}
	return b;
}

GPL_BitGroup GPL_BitGroup::Subtract(const GPL_BitGroup & bitGroup) const
{
	return Intersect(bitGroup.Invert());
}

void GPL_BitGroup::Clear(void)
{
	if ((uint32 *) GPL_BAD_ADDRESS != bits)
	{
		delete [] bits;
		bits = (uint32 *) GPL_BAD_ADDRESS;
	}
	size = 0;
	length = 0;
}

void GPL_BitGroup::SetBit(const unsigned long i)
{
	if (length > i)
	{
		bits[GPL_MAJOR(i)] = bits[GPL_MAJOR(i)] | (0x01 << GPL_MINOR(i));
	}
}

void GPL_BitGroup::SetRange(const unsigned long from, const unsigned long to)
{
	// If the indices make no sense...
	if (from > to)
	{
		return;
	}

	// If the bits to set do not span words...
	register const unsigned long f = GPL_MAJOR(from);
	register const unsigned long t = GPL_MAJOR(to);
	if (f == t)
	{
		bits[f] = bits[f] |
			((0xFFFFFFFF >> (OSD_BITS_PER_WORD - (GPL_MINOR(to) + 1))) &
				(0xFFFFFFFF << GPL_MINOR(from)));
		return;
	}

	// Set the prefix.
	bits[f] = bits[f] | (0xFFFFFFFF << GPL_MINOR(from));

	// Set the postfix.
	bits[t] = bits[t] | (0xFFFFFFFF >> (OSD_BITS_PER_WORD - (GPL_MINOR(to) + 1)));

	// Set the infix.
	for (register unsigned long i = f + 1; t > i; ++i)
	{
		bits[i] = 0xFFFFFFFF;
	}
}

void GPL_BitGroup::ClearBit(const unsigned long i)
{
	if (length > i)
	{
		bits[GPL_MAJOR(i)] = bits[GPL_MAJOR(i)] & ~(0x01 << GPL_MINOR(i));
	}
}

void GPL_BitGroup::ClearRange(const unsigned long from, const unsigned long to)
{
	// Clear the prefix.
	register const unsigned long f = GPL_MAJOR(from);
	bits[f] = bits[f] & ~(0xFFFFFFFF << GPL_MINOR(from));

	// Clear the postfix.
	register const unsigned long t = GPL_MAJOR(to);
	bits[t] = bits[t] & ~(0xFFFFFFFF >> (OSD_BITS_PER_WORD - GPL_MINOR(to)));

	// Clear the intermediaries.
	for (register unsigned long i = f + 1; t > i; ++i)
	{
		bits[i] = 0x00000000;
	}
}

void GPL_BitGroup::ClearAllBits(void)
{
	for (register unsigned long i = 0; size > i; ++i)
	{
		bits[i] = 0;
	}
}

void GPL_BitGroup::SetGroup(const unsigned long offset,
							const uint32 group, const uint32 mask)
{
	// If this offset if out-of-bounds...
	if (offset + OSD_BITS_PER_WORD >= length)
	{
		return;
	}

	// If this is aligned on a word boundary...
	if (0 == GPL_MINOR(offset))
	{
		bits[GPL_MAJOR(offset)] = (bits[GPL_MAJOR(offset)] & ~mask) |
									(group & mask);
		return;
	}

	bits[GPL_MAJOR(offset)] =
		(bits[GPL_MAJOR(offset)] & ~(mask << GPL_MINOR(offset))) |
		((group & mask) << GPL_MINOR(offset));
	bits[GPL_MAJOR(offset) + 1] =
		(bits[GPL_MAJOR(offset) + 1] & ~(mask >> (OSD_BITS_PER_WORD - GPL_MINOR(offset)))) |
		((group & mask) >> (OSD_BITS_PER_WORD - GPL_MINOR(offset)));
}

void GPL_BitGroup::Resize(const unsigned long s)
{
	register const unsigned long newSize = GPL_MAXIMUM_SIZE(s);
	register uint32 *newBits = new uint32[newSize];
	osdMemset(newBits, 0, sizeof(uint32) * newSize);
	register const unsigned long n = GPL_MAXIMUM_SIZE(s < length ? s : length);
	for (register unsigned long i = 0; n > i; ++i)
	{
		newBits[i] = bits[i];
	}
	Clear();
	bits = newBits;
	size = newSize;
	length = s;
}

GPL_Boolean GPL_BitGroup::Print(GPL_String & string,
								const GPL_Verbosity verbosity) const
{
	for (register unsigned long i = 0; length > i; ++i)
	{
		string.Append(GetBit(i) ? '1' : '0');
	}
	return TRUE;
}

GPL_Boolean GPL_BitGroup::Parse(const GPL_String & string)
{
	register const unsigned long n = string.GetLength();
	Resize(n);
	for (register unsigned long i = 0; n > i; ++i)
	{
		if ('0' == string[i])
		{
			ClearBit(i);
		}
		else
		{
			SetBit(i);
		}
	}
	return TRUE;
}
