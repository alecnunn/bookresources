/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#include "defs.h"
#include "bufferpool.h"

class Transport
{
public:
   Transport();
   virtual ~Transport();

   // The caller should not change the data in blist once it has been
   // passed to Send.  
   // Return true on success.  
   // Sends up to max_bytes over the transport from the buffer list.
   virtual bool Send(Buffer *blist,int32 max_bytes);

   void Receive(Buffer *b);

private:

   enum
   {
      MAX_SEND_BUFFERS = 3000,
   };

   // buffers passed to Socket::Receive() that haven't been released in
   // ReceiveNotify yet.
   Buffer *receive_busy_list;

   // buffers passed to Socket::Send() that haven't been released in
   // SendNotify yet.
   buffer_store_type send_busy_buffers;

protected:

   struct socket_buffer 
   {
      uint32 len;   // Length of buffer in bytes
      byte  *buf;   // Actual buffer data
   };
   
   // returns true if it parsed a message, false if not.
   virtual bool ParseReceiveList() = 0;

   // buffers we've got in ReceiveNotify() that comprise the beginning
   // part of a message that may not be complete yet.  They are linked by
   // their GetNext() pointers.
   Buffer *receive_list;
};

#endif
