/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
// supports a secure transport class
//
// The format of each message is as follows:
//
// 4 bytes     length of message, including header, encoded with fsm1, hi bit set to 0
// 4 bytes     output of fsm2 xor-ed with the previous 4 bytes
// 4 bytes     message digest of message, including header, with digest set to 0
// 1-11 bytes  random junk (length 1 + (fsm3 % 11))

// As an anti-hacking mechanism, the encrypt and decrypt routines are not
// split out into separate functions.  Unfortunately, this means that
// their code is duplicated in some places.

#ifndef _SECURETRANSPORT_H
#define _SECURETRANSPORT_H

#include "fsm.h"
#include "md5.h"
#include "transport.h"
#include "random.h"

class SecureTransport : public Transport
{
   typedef Transport super;

public:

   // Set up/delete buffer pool
   static void Begin(Random *new_random);
   static void End();

   SecureTransport();
   ~SecureTransport();

   // Note: Send alters the buffer.
   bool Send(Buffer *blist, int32 max_bytes);

   void SetState(int32 send_state1,int32 send_state2,int32 send_state3,
		 int32 receive_state1,int32 receive_state2,int32 receive_state3);

protected:

   enum
   {
      MAX_LEN_HEADER = 24,
      MIN_LEN_HEADER = 12,
   };

   virtual bool ParseReceiveList();

   // Add a header for the given data buffer to "header".
   // header must be a single buffer (NOT a buffer list, i.e. next == NULL).
   // max_bytes is the maximum # of data bytes to send.
   // Return the TOTAL length of the message (header and data).
   int32 AddHeader(Buffer *header, Buffer *data, int32 max_bytes);

   int32 GetBufferListDigest(Buffer *blist,int32 count);

   FSM *send_fsm1;     // Used to scramble length of the message
   FSM *send_fsm2;     // Compared at client and server for validation
   FSM *send_fsm3;     // Determines number of bytes of junk padding
   FSM *send_fsm4;     // Used to encrypt the message.  Never actually sent.

   // Corresponding state machines for incoming messages.
   FSM *receive_fsm1;
   FSM *receive_fsm2;
   FSM *receive_fsm3;
   FSM *receive_fsm4;

   // A pool of small buffers for message headers
   static BufferPool *header_pool;

   // Clear buffer pool.
   // Handles the EVENT_CLEAR_POOL event.
   static bool ClearPool();

   // Random number generator
   static Random *random;
};

#endif
