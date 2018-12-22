/******************************************************************************

Coral Tree gpl/message.cc
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

long GPL_Message::count = 0;

// TODO: remove all hard coded 4s and 3s due to the sizeof a long or unsigned L

/******************************************************************************
GPL_Message::GPL_Message(long fore_buffer_size)
******************************************************************************/
GPL_Message::GPL_Message(long fore_buffer_size)
{
	buffer = NULL;
	fore = NULL;
	length = 0;
	position = 0;
	size = 0;
	foreSize = fore_buffer_size;
	flags = 0;
	count++;
}

/******************************************************************************
GPL_Message::GPL_Message(const GPL_Message & msg)
******************************************************************************/
GPL_Message::GPL_Message(const GPL_Message & msg)
{
	buffer = NULL;
	fore = NULL;
	length = 0;
	position = 0;
	size = 0;
	foreSize = 0;
	flags = 0;
	count++;
	operator=(msg);
}

/******************************************************************************
GPL_Message::~GPL_Message(void)
******************************************************************************/
GPL_Message::~GPL_Message(void)
{
	count--;
	if (fore && !(GPL_MESSAGE_COPY & flags))
	{
		osdFree(fore);
	}
}

/******************************************************************************
GPL_Message & GPL_Message::operator=(const GPL_Message & msg)
******************************************************************************/
GPL_Message & GPL_Message::operator=(const GPL_Message & msg)
{
	if (this != &msg)
	{
		Clear();

		length = msg.length;
		position = msg.position;
		flags = msg.flags;
		foreSize = msg.foreSize;
		if (!(GPL_MESSAGE_COPY & msg.flags))
		{
			CheckSize(length);
			osdMemcpy((void *) buffer, (const void *) msg.buffer, length);
		}
		else
		{
			fore = msg.fore;
			buffer = msg.buffer;
		}
	}

	return *this;
}

/******************************************************************************
long GPL_Message::Compare(const GPL_Message &m) const
******************************************************************************/
long GPL_Message::Compare(const GPL_Message &m) const
{
	if (length != m.length)
	{
		return length - m.length;
	}
	if ((char *) 0 == buffer && (char *) 0 == m.buffer)
	{
		return 0;
	}
	if ((char *) 0 == buffer || (char *) 0 == m.buffer)
	{
		return ((long) buffer) - ((long) m.buffer);
	}
	return osdMemcmp(buffer, m.buffer, length); 
}

/******************************************************************************
void GPL_Message::CheckSize(long size_to_check)
******************************************************************************/
void GPL_Message::CheckSize(long size_to_check)
{
	while (size_to_check > size)
	{
		if ((GPL_MESSAGE_READONLY | GPL_MESSAGE_COPY) & flags)
		{
			flags = flags | GPL_MESSAGE_INVALID;
			return;
		}

		fore = (char *) osdRealloc((void *) fore,
			(int) size + GPL_MESSAGE_BUFFER_CHUNK_SIZE + foreSize);
		if (!fore)
		{
			gplLog("GPL ERROR", "out of memory");
		}

		buffer = fore + foreSize;

		size += GPL_MESSAGE_BUFFER_CHUNK_SIZE;
	}
}

/******************************************************************************
void GPL_Message::RightShift(long size_to_shift, long at_index)
******************************************************************************/
void GPL_Message::RightShift(long size_to_shift, long at_index)
{
	if (0 >= size_to_shift)
	{
		return;
	}

	if (at_index > length)
	{
		gplLog("GPL ERROR","attempt to shift message from index beyond length");
	}

	if ((GPL_MESSAGE_READONLY | GPL_MESSAGE_COPY) & flags)
	{
		flags = flags | GPL_MESSAGE_INVALID;
		return;
	}

	// Use fore if possible if prepending
	if((at_index == 0) && fore && (size_to_shift <= foreSize))
	{
		buffer -= size_to_shift;
		size += size_to_shift;
		length += size_to_shift;
		foreSize -= size_to_shift;
	}
	else
	{
		while ((length + size_to_shift) > size)
		{
			fore = (char *) osdRealloc((void *) fore,
									(int) size + GPL_MESSAGE_BUFFER_CHUNK_SIZE + foreSize);
			if (!fore)
			{
				gplLog("GPL ERROR", "out of memory");
			}

			buffer = fore + foreSize;

			size += GPL_MESSAGE_BUFFER_CHUNK_SIZE;
		}

		if (length != at_index)
		{
			osdMemmove(
				(void *)(	(unsigned long)buffer + 
							(unsigned long)size_to_shift + 
							(unsigned long)at_index), 
				(void *)(	(unsigned long)buffer + 
							(unsigned long)at_index), 
				(unsigned int)(length - at_index));
		}

		length += size_to_shift;
	}
}

/******************************************************************************
void GPL_Message::DeleteRange(long index_1, long index_2)
******************************************************************************/
void GPL_Message::DeleteRange(long index_1, long index_2)
{
	long tmp_index, move_size;
	
	/* MAKE SURE index_1 <= index_2 */
	if(index_2 < index_1)
	{
		tmp_index = index_1;
		index_1 = index_2;
		index_2 = tmp_index;
	}

	move_size = length - 1 - index_2;

	if (0 < move_size)
	{
		if ((GPL_MESSAGE_READONLY | GPL_MESSAGE_COPY) & flags)
		{
			flags = flags | GPL_MESSAGE_INVALID;
			return;
		}

		osdMemmove(
			(void *)(	(unsigned long)buffer + 
						(unsigned long)index_1), 
			(void *)(	(unsigned long)buffer + 
						(unsigned long)index_2 + 1), 
			(unsigned int)(move_size));
	}
	else if (0 > move_size)
	{
		gplLog("GPL ERROR", "attempt to delete beyond end of message");
	}

	length -= index_2 - index_1 + 1;

	while ((size - length) >= GPL_MESSAGE_BUFFER_CHUNK_SIZE)
	{
		fore = (char *) osdRealloc((void *) fore,
							(int) size - GPL_MESSAGE_BUFFER_CHUNK_SIZE + foreSize);
		if (!fore)
		{
			gplLog("GPL ERROR", "memory deallocation problem");
		}
		buffer = fore + foreSize;

		size -= GPL_MESSAGE_BUFFER_CHUNK_SIZE;
	}
	Rewind();
}

/******************************************************************************
void GPL_Message::AppendChar(char c)
******************************************************************************/
void GPL_Message::AppendChar(char c)
{
	InsertChar(c, length);
}

/******************************************************************************
void GPL_Message::InsertChar(char c, long at_index)
******************************************************************************/
void GPL_Message::InsertChar(char c, long at_index)
{
	RightShift(1, at_index);

	buffer[at_index] = c;
}

/******************************************************************************
char GPL_Message::ReadChar(void)
******************************************************************************/
char GPL_Message::ReadChar(void)
{
	char c;

	if (0 == length)
	{
		return 0;
	}

	if ((position + 1) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	c = buffer[position];

	position++;

	return c;
}

/******************************************************************************
void GPL_Message::AppendUnsignedChar(unsigned char c)
******************************************************************************/
void GPL_Message::AppendUnsignedChar(unsigned char c)
{
	InsertUnsignedChar(c, length);
}

/******************************************************************************
void GPL_Message::InsertUnsignedChar(unsigned char c, long at_index)
******************************************************************************/
void GPL_Message::InsertUnsignedChar(unsigned char c, long at_index)
{
	RightShift(1, at_index);

	buffer[at_index] = *((char *) &c);
}

/******************************************************************************
unsigned char GPL_Message::ReadUnsignedChar(void)
******************************************************************************/
unsigned char GPL_Message::ReadUnsignedChar(void)
{
	unsigned char c;

	if (0 == length)
	{
		return 0;
	}

	if ((position + 1) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	c = *((unsigned char *) buffer + position);

	position++;

	return c;
}

/******************************************************************************
void GPL_Message::AppendShort(short hs)
******************************************************************************/
void GPL_Message::AppendShort(short hs)
{
	InsertShort(hs, length);
}

/******************************************************************************
void GPL_Message::InsertShort(short hs, long at_index)
******************************************************************************/
void GPL_Message::InsertShort(short hs, long at_index)
{
	unsigned short ns;

	ns = htons(*((unsigned short *) &hs));

	RightShift(2, at_index);

	osdMemcpy((void *) &buffer[at_index], (void *) &ns, 2);
}

/******************************************************************************
short GPL_Message::ReadShort(void)
******************************************************************************/
short GPL_Message::ReadShort(void)
{
	if (0 == length)
	{
		return 0;
	}

	unsigned short ns;
	short hs;

	if ((position + 2) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	osdMemcpy((void *) &ns, (void *) &buffer[position], 2);

	position += 2;

	ns = ntohs(ns);
	hs = *((short *) &ns);

	return hs;
}

/******************************************************************************
void GPL_Message::AppendUnsignedShort(unsigned short hs)
******************************************************************************/
void GPL_Message::AppendUnsignedShort(unsigned short hs)
{
	InsertUnsignedShort(hs, length);
}

/******************************************************************************
void GPL_Message::InsertUnsignedShort(unsigned short hs, long at_index)
******************************************************************************/
void GPL_Message::InsertUnsignedShort(unsigned short hs, long at_index)
{
	unsigned short ns;

	ns = htons(hs);

	RightShift(2, at_index);

	osdMemcpy((void *) &buffer[at_index], (void *) &ns, 2);
}

/******************************************************************************
unsigned short GPL_Message::ReadUnsignedShort(void)
******************************************************************************/
unsigned short GPL_Message::ReadUnsignedShort(void)
{
	if (0 == length)
	{
		return 0;
	}

	unsigned short ns;
	short hs;

	if ((position + 2) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	osdMemcpy((void *) &ns, (void *) &buffer[position], 2);

	position += 2;

	hs = ntohs(ns);

	return hs;
}

/******************************************************************************
void GPL_Message::AppendLong(long hl)
******************************************************************************/
void GPL_Message::AppendLong(long hl)
{
	InsertLong(hl, length);
}

/******************************************************************************
void GPL_Message::InsertLong(long hl, long at_index)
******************************************************************************/
void GPL_Message::InsertLong(long hl, long at_index)
{
	unsigned long nl;

	nl = htonl(*((unsigned long *) &hl));

	RightShift(4, at_index);

	osdMemcpy((void *) &buffer[at_index], (void *) &nl, 4);
}

/******************************************************************************
long GPL_Message::ReadLong(void)
******************************************************************************/
long GPL_Message::ReadLong(void)
{
	if (0 == length)
	{
		return 0;
	}

	unsigned long nl;
	long hl;

	if ((position + 4) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	osdMemcpy((void *) &nl, (void *) &buffer[position], 4);

	position += 4;

	nl = ntohl(nl);
	hl = *((long *) &nl);

	return hl;
}

/******************************************************************************
void GPL_Message::AppendUnsignedLong(unsigned long hl)
******************************************************************************/
void GPL_Message::AppendUnsignedLong(unsigned long hl)
{
	InsertUnsignedLong(hl, length);
}

/******************************************************************************
void GPL_Message::InsertUnsignedLong(unsigned long hl, long at_index)
******************************************************************************/
void GPL_Message::InsertUnsignedLong(unsigned long hl, long at_index)
{
	unsigned long nl;

	nl = htonl(hl);

	RightShift(4, at_index);

	osdMemcpy((void *) &buffer[at_index], (void *) &nl, 4);
}

/******************************************************************************
unsigned long GPL_Message::ReadUnsignedLong(void)
******************************************************************************/
unsigned long GPL_Message::ReadUnsignedLong(void)
{
	if (0 == length)
	{
		return 0;
	}

	unsigned long nl;
	long hl;

	if ((position + 4) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	osdMemcpy((void *) &nl, (void *) &buffer[position], 4);

	position += 4;

	hl = ntohl(nl);

	return hl;
}

/******************************************************************************
void GPL_Message::AppendFloat(float hf)
******************************************************************************/
void GPL_Message::AppendFloat(float hf)
{
	InsertFloat(hf, length);
}

/******************************************************************************
void GPL_Message::InsertFloat(float hf, long at_index)
******************************************************************************/
void GPL_Message::InsertFloat(float hf, long at_index)
{
	int e = 0;
	long l = length;
	InsertLong ((long) (frexp (hf, &e) * 0xfffffff), at_index);
	InsertLong (e, at_index + (length - l));
}

/******************************************************************************
float GPL_Message::ReadFloat (void)
******************************************************************************/
float GPL_Message::ReadFloat (void)
{
	double d;
	int i;
	d = ((double) ReadLong () / 0xfffffff);
	i = (int) ReadLong ();
	return (float)ldexp (d, i);
}

/******************************************************************************
void GPL_Message::AppendDouble(double hd)
******************************************************************************/
void GPL_Message::AppendDouble(double hd)
{
	InsertDouble(hd, length);
}

/******************************************************************************
void GPL_Message::InsertDouble(double hd, long at_index)
******************************************************************************/
void GPL_Message::InsertDouble(double hd, long at_index)
{
	int e = 0;
	long l = length;
	InsertLong ((long) (frexp (hd, &e) * 0xfffffff), at_index);
	InsertLong (e, at_index + (length - l));
}

/******************************************************************************
double GPL_Message::ReadDouble (void)
******************************************************************************/
double GPL_Message::ReadDouble (void)
{
	double d;
	int i;
	d = ((double) ReadLong () / 0xfffffff);
	i = (int) ReadLong ();
	return (float)ldexp (d, i);
}

/******************************************************************************
void GPL_Message::AppendInetAddress(unsigned long hl)
******************************************************************************/
void GPL_Message::AppendInetAddress(unsigned long hl)
{
	InsertInetAddress(hl, length);
}

/******************************************************************************
void GPL_Message::InsertInetAddress(unsigned long hl, long at_index)
******************************************************************************/
void GPL_Message::InsertInetAddress(unsigned long hl, long at_index)
{
	RightShift(4, at_index);

	osdMemcpy((void *) &buffer[at_index], (void *) &hl, 4);
}

/******************************************************************************
unsigned long GPL_Message::ReadInetAddress(void)
******************************************************************************/
unsigned long GPL_Message::ReadInetAddress(void)
{
	if (0 == length)
	{
		return 0;
	}

	unsigned long nl;

	if ((position + 4) > length)
	{
		gplLog("GPL ERROR", "attempt to read past end of message");
	}

	osdMemcpy((void *) &nl, (void *) &buffer[position], 4);

	position += 4;

	return nl;
}

/******************************************************************************
void GPL_Message::AppendString(const GPL_String & string)
******************************************************************************/
void GPL_Message::AppendString(const GPL_String & string)
{
	InsertString(string, length);
}

/******************************************************************************
void GPL_Message::InsertString(const GPL_String & string, long at_index)
******************************************************************************/
void GPL_Message::InsertString(const GPL_String & string, long at_index)
{
	long string_length;

	if (string.IsNull())
	{
		InsertLong(0, at_index);
	}
	else
	{
		string_length = string.GetLength();
		InsertBlock((char *) string.GetData(), string_length, at_index);
	}
}

/******************************************************************************
GPL_String GPL_Message::ReadString(void)
******************************************************************************/
GPL_String GPL_Message::ReadString(void)
{
	long string_length;
	GPL_String new_string;

	string_length = ReadLong();

	if (string_length)
	{
		new_string = GPL_String(buffer + position, string_length);
		if (new_string.IsNull())
		{
			gplLog("GPL ERROR", "could not allocate memory for string");
		}

		position += string_length;
	}

	return new_string;
}

/******************************************************************************
void GPL_Message::AppendCharPtr(const GPL_CharPtr & string)
******************************************************************************/
void GPL_Message::AppendCharPtr(const GPL_CharPtr & string)
{
	InsertCharPtr(string, length);
}

/******************************************************************************
void GPL_Message::InsertCharPtr(const GPL_CharPtr & string, long at_index)
******************************************************************************/
void GPL_Message::InsertCharPtr(const GPL_CharPtr & string, long at_index)
{
	long string_length;

	if(!string.IsValid())
	{
		InsertLong(0, at_index);
	}
	else
	{
		string_length = string.GetCount();
		InsertBlock((char *) string, string_length, at_index);
	}
}

/******************************************************************************
GPL_CharPtr GPL_Message::ReadCharPtr(void)
******************************************************************************/
GPL_CharPtr GPL_Message::ReadCharPtr(void)
{
	long string_length;
	GPL_CharPtr new_string;

	string_length = ReadLong();

	if (string_length)
	{

		new_string = GPL_CharPtr(buffer + position, string_length);
		if (!new_string.IsValid())
		{
			gplLog("GPL ERROR", "could not allocate memory for string");
		}

		position += string_length;
	}

	return new_string;
}

/******************************************************************************
void GPL_Message::AppendCharStar(char *string)
******************************************************************************/
void GPL_Message::AppendCharStar(char *string)
{
	InsertCharStar(string, length);
}

/******************************************************************************
void GPL_Message::InsertCharStar(char *string, long at_index)
******************************************************************************/
void GPL_Message::InsertCharStar(char *string, long at_index)
{
	long string_length;

	if (string)
	{
		string_length = strlen(string);
		InsertBlock(string, string_length, at_index);
	}
	else
	{
		InsertLong(0, at_index);
	}
}

/******************************************************************************
char *GPL_Message::ReadCharStar(void)
******************************************************************************/
char *GPL_Message::ReadCharStar(void)
{
	long string_length;
	char *new_string;

	string_length = ReadLong();

	if (!string_length)
	{
		return NULL;
	}

	new_string = (char *) osdMalloc((unsigned int) (string_length + 1));
	if (NULL == new_string)
	{
		gplLog("GPL ERROR", "could not allocate memory for string");
	}

	osdMemcpy((void *) new_string, (void *) &buffer[position],
				(unsigned int) string_length);
	position += string_length;

	new_string[string_length] = '\0';


	return new_string;
}

/******************************************************************************
void GPL_Message::AppendByteArray(const GPL_Array<GPL_Byte> & array)
******************************************************************************/
void GPL_Message::AppendByteArray(const GPL_Array<GPL_Byte> & array)
{
	InsertByteArray(array, length);
}

/******************************************************************************
void GPL_Message::InsertByteArray(const GPL_Array<GPL_Byte> & array,
									long at_index)
******************************************************************************/
void GPL_Message::InsertByteArray(const GPL_Array<GPL_Byte> & array,
									long at_index)
{
	InsertUnsignedLong(array.GetCount(), at_index);
	at_index += 4;
	RightShift(array.GetCount(), at_index);
	if ((GPL_Byte *) 0 != array.GetData() && 0 < array.GetCount())
	{
		osdMemcpy((void *) &buffer[at_index], (void *) array.GetData(),
					(unsigned int) array.GetCount());
	}
}

/******************************************************************************
GPL_Array<GPL_Byte> GPL_Message::ReadByteArray(void)
******************************************************************************/
GPL_Array<GPL_Byte> GPL_Message::ReadByteArray(void)
{
	const long n = ReadLong();
	GPL_Array<GPL_Byte> array;
	array.SetData((GPL_Byte *) (&buffer[position]), n);
	position += n;
	return array;
}

/******************************************************************************
void GPL_Message::AppendBlock(char *block, unsigned long block_length)
******************************************************************************/
void GPL_Message::AppendBlock(char *block, unsigned long block_length)
{
	InsertBlock(block, block_length, length);
}


/******************************************************************************
void GPL_Message::InsertBlock(char *block, unsigned long block_length, long at_index)
******************************************************************************/
void GPL_Message::InsertBlock(char *block, unsigned long block_length, long at_index)
{
	InsertUnsignedLong(block_length, at_index);
	at_index+=4;
	RightShift(block_length, at_index);
	if (0 < block_length)
	{
		osdMemcpy((void *) &buffer[at_index], (void *) block,
					(unsigned int) block_length);
	}
}

/******************************************************************************
unsigned long GPL_Message::QueryBlockLength(void)
******************************************************************************/
unsigned long GPL_Message::QueryBlockLength(void)
{
	unsigned long out_block_length;
	out_block_length = ReadLong();
	position -= 4;
	return out_block_length;
}

/******************************************************************************
char *GPL_Message::SkipBlock(unsigned long *in_block_length)
******************************************************************************/
char *GPL_Message::SkipBlock(unsigned long *in_block_length)
{
	char *block;
	unsigned long a_long;
	unsigned long *block_length;
	if (!in_block_length)
	{
		block_length = &a_long;
	}
	else
	{
		block_length = in_block_length;
	}
	*block_length = ReadUnsignedLong();
	block = &buffer[position];
	position += *block_length;
	return block;
}

/******************************************************************************
unsigned long GPL_Message::CopyBlock(char *block, unsigned long block_length)
******************************************************************************/
unsigned long GPL_Message::CopyBlock(char *block, unsigned long block_length)
{
	unsigned long out_block_length;
	unsigned long position_shift;
	out_block_length = ReadLong();

	position_shift = out_block_length;

	if (out_block_length > block_length)
	{
		out_block_length = block_length;
	}

	osdMemcpy((void *) block, (void *) &buffer[position],
				(unsigned int) out_block_length);

	position += position_shift;

	return out_block_length;
}

/******************************************************************************
char *GPL_Message::ReadBlock(unsigned long *block_length)
******************************************************************************/
char *GPL_Message::ReadBlock(unsigned long *block_length)
{
	char *block;
	*block_length = ReadLong();

	if (!*block_length)
	{
		return NULL;
	}

	block = (char *) osdMalloc((unsigned int) (*block_length));
	if (NULL == block)
	{
		gplLog("GPL ERROR", "could not allocate memory for buffer");
	}

	osdMemcpy((void *) block, (void *) &buffer[position],
				(unsigned int) (*block_length));

	position += *block_length;

	return block;
}

/******************************************************************************
void GPL_Message::AppendMessage(GPL_Message *other)
******************************************************************************/
void GPL_Message::AppendMessage(GPL_Message *other)
{
	InsertMessage(other, length);
}

/******************************************************************************
void GPL_Message::InsertMessage(GPL_Message *other, long at_index)
******************************************************************************/
void GPL_Message::InsertMessage(GPL_Message *other, long at_index)
{
	long block_length = other->length - other->position;
	char *block = other->buffer + other->position;
	RightShift(block_length, at_index);
	if (block_length > 0)
	{
		osdMemcpy((void *) (buffer + at_index),
					(const void *) block, (unsigned int) block_length);
	}
}

/******************************************************************************
void GPL_Message::Rewind(void)
******************************************************************************/
void GPL_Message::Rewind(void)
{
	position = 0;
}

/******************************************************************************
void GPL_Message::Clear(void)
******************************************************************************/
void GPL_Message::Clear(void)
{
	if (flags & GPL_MESSAGE_COPY)
	{
		buffer = NULL;
		size = 0;
	}
	length = 0;
	position = 0;
	flags = flags & ~GPL_MESSAGE_INVALID;
	flags = flags & ~GPL_MESSAGE_COPY;
}

/******************************************************************************
GPL_Boolean GPL_Message::Print(GPL_String & string,
								const GPL_Verbosity verbosity) const
******************************************************************************/
GPL_Boolean GPL_Message::Print(GPL_String & string,
								const GPL_Verbosity verbosity) const
{
	GPL_Array<GPL_Byte> bytes;
	CopyRemainder(bytes);
	GPL_String image;
	if (gplByteArrayPrint(bytes, image))
	{
		string.Append(image);
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_Message::Parse(const GPL_String & string)
******************************************************************************/
GPL_Boolean GPL_Message::Parse(const GPL_String & string)
{
	GPL_Array<GPL_Byte> bytes;
	if (gplByteArrayParse(string, bytes))
	{
		SetBuffer(bytes);
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
void GPL_Message::Print(void)
******************************************************************************/
void GPL_Message::Print(void)
{
	long i;

	fprintf(stderr, ">");
	
	for(i = 0; i < length;i++)
	{
		if((buffer[i] > 32) && (buffer[i] < 126))
			fprintf(stderr, "%c", buffer[i]);
		else 
			fprintf(stderr, ".");
		
	}
	fprintf(stderr, "<\n");
}

/******************************************************************************
char *GPL_Message::CopyBuffer(unsigned long *buffer_size)
******************************************************************************/
char *GPL_Message::CopyBuffer(unsigned long *buffer_size)
{
	*buffer_size = length;
	char *tmp;
	tmp = (char *) osdMalloc(length);
	osdMemcpy((void *) tmp, (void *) buffer, length);
	return tmp;
}

/******************************************************************************
void GPL_Message::CopyBuffer(GPL_Array<GPL_Byte> & bytes)
******************************************************************************/
void GPL_Message::CopyBuffer(GPL_Array<GPL_Byte> & bytes) const
{
	bytes.SetData((GPL_Byte *) buffer, length);
}

/******************************************************************************
char *GPL_Message::CopyRemainder(unsigned long *buffer_size)
******************************************************************************/
char *GPL_Message::GetRemainder(unsigned long *buffer_size)
{
	*buffer_size = length - position;
	char *tmp;
	tmp = (buffer + position);
	return tmp;
}

/******************************************************************************
char *GPL_Message::CopyRemainder(unsigned long *buffer_size)
******************************************************************************/
char *GPL_Message::CopyRemainder(unsigned long *buffer_size)
{
	*buffer_size = length;
	char *tmp;
	tmp = (char *) osdMalloc(length - position);
	osdMemcpy((void *) tmp, (void *) (buffer + position), length - position);
	return tmp;
}

/******************************************************************************
void GPL_Message::CopyRemainder(GPL_Array<GPL_Byte> & bytes)
******************************************************************************/
void GPL_Message::CopyRemainder(GPL_Array<GPL_Byte> & bytes) const
{
	bytes.SetData((GPL_Byte *) (buffer + position), length - position);
}

/******************************************************************************
char *GPL_Message::GetBuffer(unsigned long *buffer_size)
******************************************************************************/
char *GPL_Message::GetBuffer(unsigned long *buffer_size)
{
	*buffer_size = length;
	return buffer;
}

/******************************************************************************
void GPL_Message::SetBuffer(char *buffer_ptr, unsigned long buffer_size,
							const long duplicate)
******************************************************************************/
void GPL_Message::SetBuffer(char *buffer_ptr, unsigned long buffer_size,
							const long duplicate)
{
	length = buffer_size;
	position = 0;
	flags = flags & ~GPL_MESSAGE_INVALID;
	if (duplicate)
	{
		flags = flags & ~GPL_MESSAGE_COPY;
		CheckSize(length);
		osdMemcpy((void *) buffer, (void *) buffer_ptr, length);
	}
	else
	{
		buffer = buffer_ptr;
		size = buffer_size;
		flags = flags | GPL_MESSAGE_COPY;
	}
}

/******************************************************************************
void GPL_Message::SetBuffer(const GPL_Array<GPL_Byte> & bytes)
******************************************************************************/
void GPL_Message::SetBuffer(const GPL_Array<GPL_Byte> & bytes)
{
	length = bytes.GetCount();
	position = 0;
	flags = flags & ~GPL_MESSAGE_INVALID;
	flags = flags & ~GPL_MESSAGE_COPY;
	CheckSize(length);
	osdMemcpy((void *) buffer, (void *) bytes.GetData(), length);
}

/******************************************************************************
long GPL_Message::Position(long jump)
******************************************************************************/
long GPL_Message::Position(long jump)
{
	/* AVOID CHECKS FOR NO JUMP */
	if(!jump) return position;

	if(position + jump >= length)
	{
		gplLog("GPL WARNING", "attempt to reposition beyond length");
		return position;
	}
	if(position + jump < 0)
	{
		gplLog("GPL WARNING", "attempt to reposition before zero");
		return position;
	}
	position += jump;
	return position;
}

void GPL_Message::ResetHome(GPL_Boolean to_end)
{
	if(!fore) { return; }  // no buffer yet
	if(to_end && length){ position = length; }
	buffer += position;
	size -= position;
	foreSize += position;
	length -= position;
	position = 0;
}

/******************************************************************************
GPL_Boolean gplByteArrayPrint(const GPL_Array<GPL_Byte> & bytes, GPL_String & string)
******************************************************************************/
GPL_Boolean gplByteArrayPrint(const GPL_Array<GPL_Byte> & bytes, GPL_String & string)
{
	register const unsigned long n = bytes.GetCount();
	string.Resize(2 * n);
	register unsigned long i = 0;
	register unsigned long j = 0;
	GPL_Boolean hiByte = TRUE;
	GPL_Byte c = 0;
	while (n > i)
	{
		if (hiByte)
		{
			c = (0xF0 & (bytes[i])) >> 4;
			hiByte = FALSE;
		}
		else
		{
			c = 0x0F & (bytes[i++]);
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

/******************************************************************************
GPL_Boolean gplByteArrayParse(const GPL_String & string, GPL_Array<GPL_Byte> & bytes)
******************************************************************************/
GPL_Boolean gplByteArrayParse(const GPL_String & string, GPL_Array<GPL_Byte> & bytes)
{
	register const unsigned long n = string.GetLength();
	bytes.Resize(n / 2);
	register unsigned long i = 0;
	register unsigned long j = 0;
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
			bytes[j] = c << 4;
			hiByte = FALSE;
		}
		else
		{
			bytes[j++] |= c;
			hiByte = TRUE;
		}
	}
	return TRUE;
}

