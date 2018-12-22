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
#include "bufferpool.h"

/***************************************************************/
Buffer * BufferPool::GetBuffer(int32 min_size)
{
   store_type::iterator it;
   Buffer *b;

   /* look for a big enough buffer in the store */
   for (it = store.begin(); it != store.end(); it++)
   {
      b = *it;
      if (b->GetSize() >= min_size)
      {
	 store.erase(it);
   
	 return b;
      }
   }

   /* no available buffers big enough; create a new one */

   b = new Buffer(this,min_size);
   if (b->GetSize() >= min_size)
      allocated_count++;
   else
   {
      delete b;
      b = NULL;
   }

   return b;
}
/***************************************************************/
void BufferPool::Release(Buffer *b)
{
   if (b->GetReferenceCount() != 0)
      printf("Freeing buffer with reference count != 0\n");
   
   b->ResetData();
   
   store.push_front(b);
}
