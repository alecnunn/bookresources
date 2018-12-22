/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#ifndef _BUFFER_H
#define _BUFFER_H

// A buffer is a general-purpose block of memory.  They are reference counted so
// that when they are no longer used, they're returned to a BufferPool.
//
// Buffers can also form linked lists to hold more data than would fit in a single
// buffer.  The Buffer class has functions that operate on just a single buffer,
// or on the linked list of Buffers starting with this one.  Most functions that
// operate on the list contain List in their names.

#include "defs.h"
#include <string>

class BufferPool;

class Buffer
{
public:

   enum
   {
      MAX_SIZE = 100,  // A single buffer will have at most this many data bytes.
   };

   Buffer(BufferPool *new_bp);
   Buffer(BufferPool *new_bp,int32 min_size);
   ~Buffer();

   // This default constructor is for compatability with Pool; 
   // do NOT actually call it!
   Buffer() {}

   int32 GetSize();

   // Each buffer has a reference count, initially 0.  When the
   // reference count decrements to 0, the buffer is returned to
   // the buffer pool's free store and should no longer be referenced.
   void AddReference();
   void RemoveReference();
   int32 GetReferenceCount();

   // Add/remove a reference to every buffer in the list.
   void AddListReference();
   void RemoveListReference();

   // Add/Remove a reference for every buffer until we reach max_bytes of data in the
   // buffer list.
   void AddListReference(int32 max_bytes);
   void RemoveListReference(int32 max_bytes);

   byte *GetDataStart() { return start_data; }
   int32 GetDataLength() { return len_data; }
   int32 GetMaxDataLength();
   void  SetDataLength(int32 new_length);
   void  AddDataLength(int32 length);
   void  ResetData();
   byte *GetReadPosition() { return read_pos; }

   // Return the number of data bytes in the linked list.
   int32 GetListDataLength();
   // Return the last buffer in the linked list.
   Buffer *GetListEnd();  

   // Unlink the last buffer from blist and return it.
   static Buffer * RemoveListEnd(Buffer *& blist);

   // Return the number of Buffers in the linked list.
   int32 GetListLength(); 

   // Advance the data pointer in the buffer list by len bytes.
   void AdvanceListDataStart(int32 len);

   Buffer * GetNext();
   void SetNext(Buffer *b);
   void Append(Buffer *b);   // Add b to the end of the linked list

   void Release();
   void ReleaseList();

   // these functions allow post-patching of int32s
   byte * GetListAddPointer();
   // Returns current read pointer of list (NOT necessarily of this buffer),
   // or NULL if no bytes are available for reading.
   byte * GetListReadPointer();
   bool Set(byte *ptr,int32 data) { return Set(ptr,(byte *)&data,4); }
   bool Set(byte *ptr,int16 data) { return Set(ptr,(byte *)&data,2); }

   // returns true on success (if spanning 2 buffers, second buffer must be there)
   bool Set(byte *ptr,byte *buffer,int32 len_buffer);

   // regular adding to a buffer list
   Buffer *Add(uint32 data)  { return Add((byte *) &data, 4); }
   Buffer *Add(int32 data)   { return Add((byte *) &data, 4); }
   Buffer *Add(uint16 data)  { return Add((byte *) &data, 2); }
   Buffer *Add(int16 data)   { return Add((byte *) &data, 2); }
   Buffer *Add(int8 data)    { return Add((byte *) &data, 1); }
   Buffer *Add(byte data)    { return Add((byte *) &data, 1); }
   Buffer *Add(bool data)    { return Add((byte *) &data, 1); }
   Buffer *Add(char *string) { return Add((byte *) string, strlen(string)); }
   Buffer *AddTerminatedString(char *string) 
   { return Add((byte *) string, strlen(string) + 1); }
   
   // Add buffer to the end of our buffer, getting a new buffer from the pool
   // if needed (accessible with GetNext()).
   // Returns the last buffer in the list (which can be useful for doing
   // a sequence of consecutive Add calls efficiently).
   Buffer *Add(byte *buffer, int32 len_buffer);

   // remove count bytes from blist, freeing any empty buffers
   static Buffer * RemoveDataFromList(Buffer *blist,int32 count);

   // initializes read pointers for Read() functions to work on
   // entire data buffers
   void InitRead();
   // initializes read pointers for Read() functions to work only
   // for len_total bytes
   void InitRead(int32 len_total);

   int32 GetListReadableLength();
   int32 GetReadableLength();
   // Set the read pointer to the pos-th byte of the buffer list, if possible.
   // Returns true if 0 <= pos <= GetListReadableLength().
   bool SetListReadPosition(int32 pos);
   
   bool Read(uint32 *data) { return Read((byte *) data, 4); }
   bool Read(int32 *data)  { return Read((byte *) data, 4); }
   bool Read(uint16 *data) { return Read((byte *) data, 2); }
   bool Read(int16 *data)  { return Read((byte *) data, 2); }
   bool Read(byte *data)   { return Read((byte *) data, 1); }
   bool Read(char *data)   { return Read((byte *) data, 1); }
   bool Read(bool *data)   { return Read((byte *) data, 1); }

   // buffer must not be NULL.
   bool Read(byte *buffer,int32 len_buffer);

   // "Peek" is similar to "Read" except that it does not advance the read pointer.
   bool Peek(uint32 *data) { return Peek((byte *) data, 4); }
   bool Peek(int32 *data)  { return Peek((byte *) data, 4); }
   bool Peek(byte *data)   { return Peek((byte *) data, 1); }
   bool Peek(char *data)   { return Peek((byte *) data, 1); }

   bool Peek(byte *buffer,int32 len_buffer);

   // Read len_buffer bytes into buffer, starting from start_data (i.e., ignoring
   // the read pointer).  The state of the buffer is unchanged.
   // Return true iff len_buffer bytes are available to read.
   bool PeekData(byte *buffer, int32 len_buffer);

   bool PeekData(int32 *data) { return PeekData((byte *) data, 4); }

private:

   BufferPool *bp;

   byte *memory;
   int32 len_memory;

   /* keep track of real data start and length */
   byte *start_data;
   int32 len_data;  

   /* auxiliary data pointer and length from start_data used for reading */
   byte *read_pos;    // points to next byte read
   int32 len_read;    // # of bytes available to read; NOT affected by Read()

   int32 references;

   Buffer *next;

   void Init(BufferPool *new_bp); // the "real" constructor

   // If update_pos is true, does a Read, otherwise does a Peek.
   bool ReadOrPeek(byte *buffer,int32 len_buffer,bool update_pos);

   // Advance the data pointer by len bytes.  Assumes that len_data >= len.
   void AdvanceDataStart(int32 len);
};

#endif

