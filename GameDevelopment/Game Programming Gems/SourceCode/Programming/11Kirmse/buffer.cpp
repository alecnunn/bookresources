/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#include <stdio.h>
#include <assert.h>
#include "buffer.h"
#include "bufferpool.h"

/***************************************************************/
Buffer::Buffer(BufferPool *new_bp)
{
   Init(new_bp);
}

/***************************************************************/
Buffer::Buffer(BufferPool *new_bp,int32 min_size)
{
   Init(new_bp);

   assert(min_size <= MAX_SIZE && min_size > 0);

   memory = (byte *) malloc(min_size);
   len_memory = min_size;
   start_data = memory;
}
/***************************************************************/
Buffer::~Buffer()
{
   free(memory);
}
/***************************************************************/
void Buffer::Init(BufferPool *new_bp)
{
   assert(new_bp != NULL);

   bp = new_bp;

   memory = NULL;
   len_memory = 0;

   ResetData();
}
/***************************************************************/
int32 Buffer::GetSize()
{
   return len_memory;
}

/***************************************************************/
void Buffer::AddReference()
{
   references++;
}

/***************************************************************/
void Buffer::RemoveReference()
{
   references--;
   if (references == 0)
      Release();
}
/***************************************************************/
void Buffer::AddListReference()
{
   Buffer *blist;
   for (blist = this; blist != NULL; blist = blist->next)
      blist->AddReference();
}

/***************************************************************/
void Buffer::RemoveListReference()
{
   Buffer *blist = this;
   while (blist != NULL)
   {
      Buffer *nxt = blist->next;
      blist->RemoveReference();
      blist = nxt;
   }
}
/***************************************************************/
void Buffer::AddListReference(int32 max_bytes)
{
   Buffer *blist = this;
   while (blist != NULL && max_bytes > 0)
   {
      max_bytes -= blist->GetDataLength();
      
      Buffer *nxt = blist->next;
      blist->AddReference();
      blist = nxt;      
   }
}
/***************************************************************/
void Buffer::RemoveListReference(int32 max_bytes)
{
   Buffer *blist = this;
   while (blist != NULL && max_bytes > 0)
   {
      max_bytes -= blist->GetDataLength();
      
      Buffer *nxt = blist->next;
      blist->RemoveReference();
      blist = nxt;      
   }
}
/***************************************************************/
int32 Buffer::GetReferenceCount()
{
   return references;
}
/***************************************************************/
void Buffer::ResetData()
{

#ifdef IG_MEM_DEBUG
   memset(memory,0xfe,len_memory);
#endif
   start_data = memory;
   len_data = 0;
   
   references = 0;
   SetNext(NULL);
}
/***************************************************************/
int32 Buffer::GetMaxDataLength()
{
   return memory + len_memory - start_data;
}

/***************************************************************/
int32 Buffer::GetListDataLength()
{
   Buffer *b;
   int32 length;

   length = 0;
   b = this;
   while (b != NULL)
   {
      length += b->GetDataLength();
      b = b->GetNext();
   }

   return length;
}
/***************************************************************/
Buffer *Buffer::GetListEnd()
{
   Buffer *b;

   b = this;
   while (b->GetNext() != NULL)
      b = b->GetNext();

   return b;
}
/***************************************************************/
Buffer * Buffer::RemoveListEnd(Buffer *& blist)
{
   Buffer *b,*prev;

   if (blist->GetNext() == NULL)
   {
      b = blist;
      blist = NULL;
      return b;
   }

   prev = blist;
   b = blist->GetNext();
   while (b->GetNext() != NULL)
   {
      prev = b;
      b = b->GetNext();
   }

   prev->SetNext(NULL);
   return b;
}
/***************************************************************/
int32 Buffer::GetListLength()
{
   Buffer *b;
   int32 length;

   length = 0;
   b = this;
   while (b != NULL)
   {
      length++;
      b = b->GetNext();
   }

   return length;
}
/***************************************************************/
void Buffer::AdvanceListDataStart(int32 len)
{
   assert(len >= 0);
   if (len > GetListDataLength())
   {
      printf("AdvanceListDataStart trying to advance %d bytes when there are only %d\n",
	      len, GetListDataLength());
      return;
   }
   Buffer *blist = this;
   while (blist != NULL)
   {
      int32 data_len = blist->GetDataLength();
      if (len <= data_len)
      {
	 blist->AdvanceDataStart(len);
	 return;
      }
      blist->AdvanceDataStart(data_len);
      len -= data_len;
      blist = blist->next;
   }
   assert(0);   // Unreachable
}
/***************************************************************/
void Buffer::SetDataLength(int32 new_length)
{
   if ((start_data + new_length) > (memory + len_memory))
   {
      printf("SetDataLength %i would go past end of buffer\n",new_length);
      new_length = memory + len_memory - start_data;
   }
   len_data = new_length;
}
/***************************************************************/
void Buffer::AddDataLength(int32 length)
{
   if ((start_data + length) > (memory + len_memory))
      printf("Going past end of buffer memory\n");

   len_data += length;
}
/***************************************************************/
Buffer * Buffer::GetNext()
{
   return next;
}
/***************************************************************/
void Buffer::SetNext(Buffer *b)
{
   next = b;
}

/***************************************************************/
void Buffer::Append(Buffer *b)
{
   Buffer *blist;

   blist = this;

   while (blist->GetNext() != NULL)
   {
      blist = blist->GetNext();
   }
   blist->SetNext(b);
}
/***************************************************************/
void Buffer::Release()
{
   bp->Release(this);
}
/***************************************************************/
void Buffer::ReleaseList()
{
   Buffer *blist = this;
   while (blist != NULL)
   {
      Buffer *nxt = blist->next;
      blist->Release();
      blist = nxt;
   }
}
/***************************************************************/
byte * Buffer::GetListAddPointer()
{
   Buffer *blist = this;
   // Search through list until we find one that isn't filled
   // (unless the last one is filled)
   while (blist != NULL)
   {
      byte *add_ptr = blist->start_data + blist->len_data;
      Buffer *next = blist->GetNext();
      if (add_ptr < blist->memory + blist->len_memory || next == NULL)
	 return add_ptr;
      blist = next;
   }
   return NULL;
}
/***************************************************************/
byte * Buffer::GetListReadPointer()
{
   Buffer *blist = this;
   while (blist != NULL)
   {
      if (blist->GetReadableLength() > 0)
	 return blist->read_pos;
      blist = blist->GetNext();
   }
   return NULL;
}

/***************************************************************/
bool Buffer::Set(byte *ptr,byte *buffer,int32 len_buffer)
{
   int32 len_copy,rest_copy;

   // First find the buffer into which ptr points
   Buffer *blist = this;
   for (blist = this; blist != NULL; blist = blist->GetNext())
      if (ptr >= blist->start_data && ptr <= blist->start_data + blist->len_data)
	 break;
   if (blist == NULL)
      return false;

   len_copy = len_buffer;

   if ((blist->start_data + blist->len_data) < (ptr + len_buffer))
      len_copy = (blist->start_data + blist->len_data) - ptr;

   memcpy(ptr,buffer,len_copy);

   if (len_copy < len_buffer)
   {
      // If it didn't fit in this buffer, add the rest to the next buffer
      rest_copy = len_buffer - len_copy;

      blist = blist->GetNext();
      if (blist == NULL)
      {
	 printf("Set overflowing buffer with no next\n");
	 return false;
      }

      return blist->Set(blist->GetDataStart(),buffer + len_copy,rest_copy);
   }
   return true;
}

/***************************************************************/
Buffer *Buffer::Add(byte *buffer,int32 len_buffer)
{
   int32 len_copy,rest_copy;

   len_copy = len_buffer;

   if ((memory + len_memory) < (start_data + len_data + len_buffer))
      len_copy = (memory + len_memory) - (start_data + len_data);

   memcpy(start_data + len_data,buffer,len_copy);
   len_data += len_copy;

   if (len_copy < len_buffer)
   {
      rest_copy = len_buffer - len_copy;

      Buffer *b;
      if (GetNext() != NULL)
      {
	 b = GetNext();
	 while (b->GetNext() != NULL)
	    b = b->GetNext();

	 return b->Add(buffer + len_copy, rest_copy);
      }
      
      b = bp->GetBuffer(MAX_SIZE);
      SetNext(b);
      return b->Add(buffer + len_copy, rest_copy);
   }

   return this;
}

/***************************************************************/
Buffer * Buffer::RemoveDataFromList(Buffer *blist,int32 count)
{
   int32 removed,remove_now;
   Buffer *b;

   assert(blist != NULL);

   removed = 0;
   b = blist;
   while (removed < count)
   {
      assert(b != NULL);

      remove_now = min(b->GetDataLength(),count-removed);

      b->start_data += remove_now;
      b->len_data -= remove_now;
      
      if (b->len_data == 0)
      {
	 Buffer *temp;

	 temp = b->GetNext();
	 b->RemoveReference();
	 b = temp;
      }
      removed += remove_now;
   }
   return b;
}

/***************************************************************/
void Buffer::InitRead()
{
   Buffer *b = this;
   while (b != NULL)
   {
      b->read_pos = b->start_data;
      b->len_read = b->len_data;

      // XXX Tracking down a bug
      assert(b != b->GetNext());

      b = b->GetNext();
   }
}

/***************************************************************/
void Buffer::InitRead(int32 len_total)
{
   Buffer *b = this;
   while (b != NULL)
   {
      b->read_pos = b->start_data;
      b->len_read = min(b->len_data, len_total);

      len_total -= b->len_read;
      
      b = b->GetNext();
   }
}

/***************************************************************/
int32 Buffer::GetListReadableLength()
{
   Buffer *b;
   int32 length;

   length = 0;
   b = this;
   while (b != NULL)
   {
      length += b->len_read - (b->read_pos - b->start_data);
      b = b->GetNext();
   }

   return length;
}

/***************************************************************/
int32 Buffer::GetReadableLength()
{
   return len_read - (read_pos - start_data);
}
/***************************************************************/
bool Buffer::SetListReadPosition(int32 pos)
{
   if (pos < 0 || pos > GetListDataLength())
      return false;
   
   Buffer *b = this;
   int32 length = 0;

   while (b != NULL)
   {
      int32 buf_length = b->GetDataLength();
      if (buf_length >= pos)
      {
	 b->read_pos = b->start_data + pos;

	 // Set all later buffers to the beginning
	 b = b->GetNext();
	 while (b != NULL)
	 {
	    b->read_pos = b->start_data;
	    b = b->GetNext();
	 }
	 return true;
      }

      // Mark this buffer as already read, and move to the next
      pos -= buf_length;
      b->read_pos = b->start_data + buf_length;
      b = b->GetNext();
   }

   // Unreachable, since pos was in range
   assert(false);
   return false;  
}

/***************************************************************/
bool Buffer::Read(byte *buffer,int32 len_buffer)
{
   return ReadOrPeek(buffer, len_buffer, true);
}
/***************************************************************/
bool Buffer::Peek(byte *buffer,int32 len_buffer)
{
   return ReadOrPeek(buffer, len_buffer, false);   
}
/***************************************************************/
bool Buffer::ReadOrPeek(byte *buffer,int32 len_buffer,bool update_pos)
{
   assert(buffer != NULL);

   Buffer *b = this;

   while (b != NULL)
   {
      int32 len_copy = len_buffer;
      
      if ((b->start_data + b->len_read) < (b->read_pos + len_buffer))
         len_copy = (b->start_data + b->len_read) - b->read_pos;
      
      memcpy(buffer, b->read_pos, len_copy);
      
      if (update_pos)
         b->read_pos += len_copy;
      
      if (len_copy == len_buffer)
         return true;

      b = b->GetNext();
      buffer += len_copy;
      len_buffer -= len_copy;
   }
   
   return false;  // Requested amount of data not available to read
}
/***************************************************************/
bool Buffer::PeekData(byte *buffer, int32 len_buffer)
{
   assert(buffer != NULL);

   Buffer *b = this;

   while (b != NULL)
   {
      int32 len_copy = min(len_buffer, b->len_data);
      
      memcpy(buffer, b->start_data, len_copy);
      
      if (len_copy == len_buffer)
         return true;

      b = b->GetNext();
      buffer += len_copy;
      len_buffer -= len_copy;
   }
   
   return false;  // Requested amount of data not available to read
}
/***************************************************************/
void Buffer::AdvanceDataStart(int32 len)
{
   assert(len <= len_data);
   start_data += len;
   len_data -= len;
}
