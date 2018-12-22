/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#ifndef _BUFFERPOOL_H
#define _BUFFERPOOL_H

#include "buffer.h"
#include "pool.h"
#include <list>

typedef list<Buffer *> buffer_store_type;

class BufferPool : public Pool<Buffer>
{
public:

   Buffer * GetBuffer(int32 min_size);
   Buffer * Get()  { return GetBuffer(Buffer::MAX_SIZE); }

   void Release(Buffer *b);

private:

};

#endif

