#ifndef GPL_MESSAGE_H
#define GPL_MESSAGE_H

/******************************************************************************

Coral Tree gpl/message.h
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
#include <gpl/array.h>
#include <gpl/str.h>
#include <gpl/depict.h>

/***********************************************************************
TODO: Right now message is a single buffer.  It should be a 'tree' like 
structure so the the insert function doesn't have to move a block
of memory.

However, Blocks should be kept in single chunks because the interface
returns  pointers to blocks within messages through the SkipBlock call.
***********************************************************************/


#define GPL_MESSAGE_BUFFER_CHUNK_SIZE	64
#define GPL_MESSAGE_INVALID				0x1
#define GPL_MESSAGE_READONLY			0x2
#define GPL_MESSAGE_COPY				0x4

class GPL_Message : public GPL_Depictable
{
	public:
		static long				count;


	/******************************************************************
	WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
	The following is protected because NL_Message uses them directly.
	The reason for this is efficiency, it avoids an extra memcpy.
	If GPL_Message changes structure, be sure to update NL_Message.

	WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
	Update:  It looks like DNA, NRM, and NCS have started becoming
	dependent on GPL_Message's structure.  It will therefore probably
	remain the same.
	******************************************************************/
	protected:
		char *buffer;
		char *fore;
		long foreSize;
		long length;
		long size;
		long position;
		unsigned long flags;
	/*****************************************************************/

	private:
		void RightShift(long size_to_shift, long at_index);

	protected:
		void CheckSize(long size_to_check);

	public:
		GPL_Message(long fore_buffer_size = 0);
		GPL_Message(const GPL_Message & msg);
		virtual ~GPL_Message(void);

		GPL_Message & operator=(const GPL_Message & msg);

		GPL_Boolean operator==(const GPL_Message &m) const
		{
			return 0 == Compare(m);
		}
		GPL_Boolean operator!=(const GPL_Message &m) const
		{
			return 0 != Compare(m);
		}

		long Compare(const GPL_Message &m) const;

		void DeleteRange(long index_1, long index_2);

		void AppendChar(char c);
		void InsertChar(char c, long at_index);
		char ReadChar(void);

		void AppendUnsignedChar(unsigned char c);
		void InsertUnsignedChar(unsigned char c, long at_index);
		unsigned char ReadUnsignedChar(void);

		void AppendShort(short hs);
		void InsertShort(short hs, long at_index);
		short ReadShort(void);

		void AppendUnsignedShort(unsigned short hs);
		void InsertUnsignedShort(unsigned short hs, long at_index);
		unsigned short ReadUnsignedShort(void);

		void AppendLong(long hl);
		void InsertLong(long hl, long at_index);
		long ReadLong(void);

		void AppendUnsignedLong(unsigned long hl);
		void InsertUnsignedLong(unsigned long hl, long at_index);
		unsigned long ReadUnsignedLong(void);

		void AppendFloat(float hf);
		void InsertFloat(float hf, long at_index);
		float ReadFloat(void);

		void AppendDouble(double hd);
		void InsertDouble(double hd, long at_index);
		double ReadDouble(void);

		void AppendInetAddress(unsigned long hl);
		void InsertInetAddress(unsigned long hl, long at_index);
		unsigned long ReadInetAddress(void);

		
		void AppendString(const GPL_String & string);
		GPL_String ReadString(void);
		void InsertString(const GPL_String & string, long at_index);

		void AppendCharPtr(const GPL_CharPtr & string);
		GPL_CharPtr ReadCharPtr(void);
		void InsertCharPtr(const GPL_CharPtr & string, long at_index);

		void AppendCharStar(char *string); 	// NULL terminated
		char *ReadCharStar(void);//returns NULL terminated string. NULL for 0 length
		void InsertCharStar(char *string, long at_index);

		void AppendByteArray(const GPL_Array<GPL_Byte> & array);
		void InsertByteArray(const GPL_Array<GPL_Byte> & array, long at_index);
		GPL_Array<GPL_Byte> ReadByteArray(void);

		void AppendBlock(char *block, unsigned long block_length);
		void InsertBlock(char *block,unsigned long block_length,long at_index);
		char *SkipBlock(unsigned long *block_length);
		unsigned long CopyBlock(char *block, unsigned long block_length);
		char *ReadBlock(unsigned long *block_length);
		void AppendBlock(char *block, long block_length)
			{ AppendBlock(block, (unsigned long)block_length); }
		void InsertBlock(char *block,long block_length,long at_index)
			{ InsertBlock(block, (unsigned long)block_length, at_index); }
		char *SkipBlock(long *block_length)
			{ return SkipBlock((unsigned long *)block_length); }
		long CopyBlock(char *block, long block_length)
			{ return (long)CopyBlock(block, (unsigned long)block_length); }
		char *ReadBlock(long *block_length)
			{ return ReadBlock((unsigned long *)block_length); }
		unsigned long QueryBlockLength(void);

		void AppendMessage(GPL_Message *other);
		void InsertMessage(GPL_Message *other,long at_index);
		
		void Rewind(void);
		void Clear(void);
		void Print(void);
		long Length(void) { return length; }
		long Position(long jump);

		void ResetHome(GPL_Boolean to_end = FALSE);

		char *GetBuffer(unsigned long *buffer_size);
		char *CopyBuffer(unsigned long *buffer_size);
		void CopyBuffer(GPL_Array<GPL_Byte> & bytes) const;
		char *CopyRemainder(unsigned long *buffer_size);
		char *GetRemainder(unsigned long *buffer_size);
		void CopyRemainder(GPL_Array<GPL_Byte> & bytes) const;
		void SetBuffer(char *buffer_ptr, unsigned long buffer_size,
						const long duplicate = 1);
		void SetBuffer(const GPL_Array<GPL_Byte> & bytes);

		// GPL_Depictable
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity) const;
		virtual GPL_Boolean Parse(const GPL_String & string);
};

extern GPL_Boolean gplByteArrayPrint(const GPL_Array<GPL_Byte> & bytes, GPL_String & string);
extern GPL_Boolean gplByteArrayParse(const GPL_String & string, GPL_Array<GPL_Byte> & bytes);

#endif /* GPL_MESSAGE_H */
