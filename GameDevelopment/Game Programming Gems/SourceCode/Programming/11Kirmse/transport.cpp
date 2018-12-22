/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#include "transport.h"
#include <assert.h>

/***************************************************************/
Transport::Transport()
{
   receive_list = NULL;
   receive_busy_list = NULL;
}
/***************************************************************/
Transport::~Transport()
{
   receive_list->RemoveListReference();
   receive_busy_list->RemoveListReference();

   buffer_store_type::iterator it;

   // remove reference from all send_busy_buffers
   it = send_busy_buffers.begin();
   while (it != send_busy_buffers.end())
   {
      (*it)->RemoveListReference();

      it = send_busy_buffers.erase(it);
   }
}
/***************************************************************/
bool Transport::Send(Buffer *blist,int32 max_bytes)
{
   Buffer *ptr;
   socket_buffer sb[MAX_SEND_BUFFERS];
   int32 len_sb, list_len;
   int32 bytes_remaining = max_bytes;
   int32 bytes_sent = 0;

   assert(blist != NULL);

   len_sb = 0;
   list_len = 0;
   ptr = blist;
   // Build up an array of pointers from the buffer list.  Skip empty buffers.
   while (ptr != NULL && bytes_remaining > 0)
   {
      sb[len_sb].buf = ptr->GetDataStart();
      sb[len_sb].len = ptr->GetDataLength();

      // list_len counts ALL buffers in the list, so that when we get to SendNotify,
      // we know how many buffers to remove a reference from (even empty ones).
      list_len++;
      
      if ((int32) sb[len_sb].len > bytes_remaining)
	 sb[len_sb].len = bytes_remaining;
      bytes_remaining -= sb[len_sb].len;
      
      bytes_sent += sb[len_sb].len;

      if (sb[len_sb].len != 0)
	 len_sb++;

      if (len_sb == MAX_SEND_BUFFERS)
      {
         // Pretend that buffers are sent
         blist->AddListReference(max_bytes);
         blist->RemoveListReference(max_bytes);
	 return false;
      }
      ptr = ptr->GetNext();
   }

   assert(bytes_remaining != max_bytes); // if sent only 0 length buffers

   // Add references to all buffers in the list that are sent (including empty ones)
   Buffer *b = blist;
   while (b != ptr)
   {
      b->AddReference();
      b = b->GetNext();
   }
   
   send_busy_buffers.push_back(blist);

   // XXX Actually send buffer list over the socket, e.g.
   // as->Send(sb, len_sb, blist, list_len);
   return true;
}
/***************************************************************/
void Transport::Receive(Buffer *b)
{
   socket_buffer sb[1];
   int32 len_sb;

   assert(b != NULL);
   assert(receive_busy_list == NULL);

   sb[0].buf = b->GetDataStart() + b->GetDataLength();
   sb[0].len = b->GetMaxDataLength() - b->GetDataLength();
   len_sb = 1;

   receive_busy_list = b;

   // XXX Actually receive data from socket, e.g.
   // as->Receive(sb,len_sb,b);
}
