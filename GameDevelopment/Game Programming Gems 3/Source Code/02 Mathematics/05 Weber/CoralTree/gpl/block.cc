/******************************************************************************

Coral Tree gpl/block.cc
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

#include <gpl/block.h>

#define UPPERBITS(x,y)	((x) & (~(y)))
#define LOWERBITS(x,y)	((x) & (y))

/***********************************************************************
class GPL_Block
***********************************************************************/
GPL_Block::GPL_Block(void)
{
	pointer = 0;
}

GPL_Block::GPL_Block(const GPL_Array<GPL_Byte> & b)
	: GPL_Array<GPL_Byte>(b)
{
	pointer = 0;
}

GPL_Block::GPL_Block(GPL_Address a, const unsigned long l)
{
	pointer = 0;
	Set(a, l);
}

GPL_Block::~GPL_Block(void)
{
}

GPL_Block & GPL_Block::operator=(const GPL_Array<GPL_Byte> & b)
{
	GPL_Array<GPL_Byte>::operator=(b);
	return *this;
}

GPL_Address GPL_Block::Get(unsigned long & l)
{
	l = GetCount();
	return (GPL_Address) GetData();
}

GPL_Address GPL_Block::GetAddress(void) const
{
	return (GPL_Address) GetData();
}

unsigned long GPL_Block::GetLength(void) const
{
	return GetCount();
}

void GPL_Block::Set(GPL_Address a, const unsigned long l)
{
	SetData((GPL_Byte *) a, l);
}

GPL_Opaque GPL_Block::CheckSum(void) const
{
	GPL_Opaque sum = 0;
	const unsigned long n = GetCount();
	for (unsigned long i = 0; n > i; ++i)
	{
		sum += operator[](i);
	}
	return sum;
}

GPL_Block GPL_Block::ShiftLeft(const unsigned long shift) const
{
	GPL_Block b;
	b.Resize(GetSize());
	if (0 < GetSize())
	{
		const GPL_Byte mask = 0xff >> shift;
		const unsigned long unshift = 8 - shift;
		const GPL_Byte saved = UPPERBITS(operator[](0),mask) >> unshift;
		const unsigned long n = GetSize() - 1;
		for (unsigned long i = 0; n > i; ++i)
		{
			b.operator[](i) = (LOWERBITS(operator[](i),mask) << shift) |
								(UPPERBITS(operator[](1 + i),mask) >> unshift);
		}
		b.operator[](n) = (LOWERBITS(operator[](n),mask) << shift) | saved;
	}
	return b;
}

GPL_Block GPL_Block::ShiftRight(const unsigned long shift) const
{
	GPL_Block b;
	b.Resize(GetSize());
	if (0 < GetSize())
	{
		const unsigned long unshift = 8 - shift;
		const GPL_Byte mask = 0xff >> unshift;
		const unsigned long n = GetSize() - 1;
		const GPL_Byte saved = LOWERBITS(operator[](n),mask) << unshift;
		for (unsigned long i = n; 0 < i; --i)
		{
			b.operator[](i) = (LOWERBITS(operator[](i - 1),mask) << unshift) |
								(UPPERBITS(operator[](i),mask) >> shift);
		}
		b.operator[](0) = saved | (UPPERBITS(operator[](0),mask) >> shift);
	}
	return b;
}

GPL_Boolean GPL_Block::CopyNext(GPL_Byte *chunk, unsigned long *sz)
{
	const unsigned long n = GetCount();
	char *a = (char *)GetData();
	GPL_Boolean returnVal = TRUE;
	
	if(pointer >= n) { return FALSE; }
	if(pointer + *sz >= n) 
	{ 
		*sz = n - pointer; 
		returnVal = FALSE;
	}
	osdMemcpy((void *)chunk, (void *)(&(a[pointer])), *sz);

	pointer += *sz;

	return TRUE;
}

GPL_Boolean GPL_Block::CopyTo(GPL_Byte delim, GPL_Byte *chunk, unsigned long *sz)
{
	const unsigned long n = GetCount();
	unsigned long max = *sz;
	unsigned long index;
	char *a = (char *)GetData();

	*sz = 0;

	if(pointer >= n) { return FALSE; }

	GPL_Byte *root = (GPL_Byte *)(&(a[pointer]));
	for(index = 0; index < max; index++)
	{
		chunk[index] = root[index];
		pointer++;
		(*sz)++;
		if(root[index] == delim)
		{
			break;
		}
		if(pointer >= n) break;
	}

	//osdMemcpy((void *)chunk, (void *)(root), *sz);

	return TRUE;
}

GPL_Boolean GPL_Block::Print(GPL_String & string,
								const GPL_Verbosity verbosity) const
{
	const unsigned long n = GetCount();
	string.Resize(2 * n);
	unsigned long i = 0;
	unsigned long j = 0;
	GPL_Boolean hiByte = TRUE;
	GPL_Byte c = 0;
	while (n > i)
	{
		if (hiByte)
		{
			c = (0xF0 & (operator[](i))) >> 4;
			hiByte = FALSE;
		}
		else
		{
			c = 0x0F & (operator[](i++));
			hiByte = TRUE;
		}
		switch(c)
		{
			case 0x01:
				string[j++] = '1';
				break;
			case 0x02:
				string[j++] = '2';
				break;
			case 0x03:
				string[j++] = '3';
				break;
			case 0x04:
				string[j++] = '4';
				break;
			case 0x05:
				string[j++] = '5';
				break;
			case 0x06:
				string[j++] = '6';
				break;
			case 0x07:
				string[j++] = '7';
				break;
			case 0x08:
				string[j++] = '8';
				break;
			case 0x09:
				string[j++] = '9';
				break;
			case 0x0A:
				string[j++] = 'A';
				break;
			case 0x0B:
				string[j++] = 'B';
				break;
			case 0x0C:
				string[j++] = 'C';
				break;
			case 0x0D:
				string[j++] = 'D';
				break;
			case 0x0E:
				string[j++] = 'E';
				break;
			case 0x0F:
				string[j++] = 'F';
				break;
			default:
				string[j++] = '0';
				break;
		}
	}
	return TRUE;
}

GPL_Boolean GPL_Block::Parse(const GPL_String & string)
{
	const unsigned long n = string.GetLength();
	Resize(n / 2);
	unsigned long i = 0;
	unsigned long j = 0;
	GPL_Boolean hiByte = TRUE;
	GPL_Byte c = 0;
	while (n > i)
	{
		switch(toupper(string[i++]))
		{
			case '1':
				c = 0x1;
				break;
			case '2':
				c = 0x2;
				break;
			case '3':
				c = 0x3;
				break;
			case '4':
				c = 0x4;
				break;
			case '5':
				c = 0x5;
				break;
			case '6':
				c = 0x6;
				break;
			case '7':
				c = 0x7;
				break;
			case '8':
				c = 0x8;
				break;
			case '9':
				c = 0x9;
				break;
			case 'A':
				c = 0xA;
				break;
			case 'B':
				c = 0xB;
				break;
			case 'C':
				c = 0xC;
				break;
			case 'D':
				c = 0xD;
				break;
			case 'E':
				c = 0xE;
				break;
			case 'F':
				c = 0xF;
				break;
			default:
				c = 0x0;
				break;
		}
		if (hiByte)
		{
			operator[](j) = c << 4;
			hiByte = FALSE;
		}
		else
		{
			operator[](j++) |= c;
			hiByte = TRUE;
		}
	}
	return TRUE;
}
