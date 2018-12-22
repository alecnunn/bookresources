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
#include "securetransport.h"

BufferPool   *SecureTransport::header_pool = NULL;
Random       *SecureTransport::random = NULL;
/***************************************************************/
void SecureTransport::Begin(Random *new_random)
{
   random = new_random;
   header_pool = new BufferPool();
}
/***************************************************************/
void SecureTransport::End()
{
   delete header_pool;
}
/***************************************************************/
SecureTransport::SecureTransport()
{
   assert(header_pool != NULL);

   send_fsm1 = new FSM(0xabcdef12,5001);
   receive_fsm1 = new FSM(0xabcdef12,5001);

   send_fsm2 = new FSM(0x829e351a,17053);
   receive_fsm2 = new FSM(0x829e351a,17053);

   send_fsm3 = new FSM(0x61c8ee2f,351);
   receive_fsm3 = new FSM(0x61c8ee2f,351);

   send_fsm4 = new FSM(0xb4a307cf,65383);
   receive_fsm4 = new FSM(0xb4a307cf,65383);
}
/***************************************************************/
SecureTransport::~SecureTransport()
{
   delete send_fsm1;
   delete receive_fsm1;

   delete send_fsm2;
   delete receive_fsm2;

   delete send_fsm3;
   delete receive_fsm3;

   delete send_fsm4;
   delete receive_fsm4;
}
/***************************************************************/
bool SecureTransport::Send(Buffer *blist, int32 max_bytes)
{
   assert(max_bytes >= 0);

   Buffer *b = header_pool->GetBuffer(MAX_LEN_HEADER);
   if (b == NULL)
   {
      printf("Unable to allocate buffer for secure buffer list sending\n");
      return false;
   }

   int32 len_data = AddHeader(b, blist, max_bytes);

   b->SetNext(blist);

   // encrypt starting 8 bytes in (don't encrypt the length or fsm2)
   int32 processed = 0;
   uint32 key;
   Buffer *walk_buf = b;
   while (processed < len_data)
   {
      int32 process_now = min(walk_buf->GetDataLength(), len_data - processed);
      byte *ptr = walk_buf->GetDataStart();

      for (int32 i=0; i < process_now; i++)
      {
	 if ((processed & 3) == 0)
	 {
	    send_fsm4->UpdateState();
	    key = send_fsm4->GetState();
	 }
	 
	 if (processed >= 8)   // Skip length & FSM2 fields
	 {
	    int32 byte_num = processed & 3;   // rotate through bytes of key
	    *ptr = *ptr ^ ((key & (0xff << (byte_num * 8))) >> (byte_num * 8));
	 }
	 
	 processed++;
	 ptr++;
      }

      walk_buf = walk_buf->GetNext();
   }

   bool retval = super::Send(b,len_data);

   send_fsm1->UpdateState();
   send_fsm2->UpdateState();
   send_fsm3->UpdateState();
   send_fsm4->UpdateState();
   return retval;
}
/***************************************************************/
void SecureTransport::SetState(int32 send_state1,int32 send_state2,int32 send_state3,
			       int32 receive_state1,int32 receive_state2,int32 receive_state3)
{
   send_fsm1->SetState(send_state1);
   send_fsm2->SetState(send_state2);
   send_fsm3->SetState(send_state3);
   send_fsm4->SetState(send_state1 + send_state2 - send_state3);

   receive_fsm1->SetState(receive_state1);
   receive_fsm2->SetState(receive_state2);
   receive_fsm3->SetState(receive_state3);
   receive_fsm4->SetState(receive_state1 + receive_state2 - receive_state3);
}
/***************************************************************/
bool SecureTransport::ClearPool()
{
   if (header_pool != NULL)
      header_pool->FreeAll();
   return true;
}

#define TRYREAD(a) if (!receive_list->Read(a)) return false;
/***************************************************************/
bool SecureTransport::ParseReceiveList()
{
   int32 len_message;
   int32 len_available;
   int32 state2;

   receive_list->InitRead();

   TRYREAD(&len_message);
   TRYREAD(&state2);

   state2 ^= len_message;

   if (state2 != receive_fsm2->GetState())
   {
      // Unknown how much to discard; further successful communication is unlikely.
      receive_list = Buffer::RemoveDataFromList(receive_list, receive_list->GetListReadableLength());
    
      printf("Invalid state2 for secure message of length %d.\n", len_message);
      return false;
   }

//   dprintf("trying to decode len_message with %i\n",receive_fsm1->GetState());
   len_message = (~(len_message ^ receive_fsm1->GetState())) & 0x7fffffff;
//   dprintf("after decoding trying to read len %i message\n",len_message);
   len_available = receive_list->GetListReadableLength() + 
      sizeof(len_message) + sizeof(state2);

   if (len_message < 0)
   {
      receive_list =
         Buffer::RemoveDataFromList(receive_list, sizeof(len_message) + sizeof(state2));
      printf("Invalid message length");
      return false;
   }

   if (len_available < len_message)
      return false;

   // Decrypt message (the length was already read)
   int32 i, processed = 0;
   uint32 key;
   Buffer *b = receive_list;
   while (processed < len_message)
   {
      int32 process_now = min(b->GetDataLength(), len_message - processed);
      byte *ptr = b->GetDataStart();

      for (i=0; i < process_now; i++)
      {
	 if ((processed & 3) == 0)
	 {
	    receive_fsm4->UpdateState();
	    key = receive_fsm4->GetState();
	 }

	 if (processed >= 8)   // Skip length and FSM2 fields
	 {
	    int32 byte_num = processed & 3;   // rotate through bytes of key
	    *ptr = *ptr ^ ((key & (0xff << (byte_num * 8))) >> (byte_num * 8));
	 }
	 ptr++;
	 processed++;
      }
      b = b->GetNext();
   }

   int32 digest,calc_digest;

   byte *digest_ptr;

   digest_ptr = receive_list->GetListReadPointer();
   TRYREAD(&digest);

   receive_list->Set(digest_ptr,(int32)0);
   calc_digest = GetBufferListDigest(receive_list,len_message);
   if (digest != calc_digest)
   {
      receive_list = Buffer::RemoveDataFromList(receive_list, len_message);
      printf("Invalid digest for secure message of length %d.\n", len_message);
      return false;
   }

   int32 len_pad = 1 + receive_fsm3->GetState() % 11;

   receive_fsm1->UpdateState();
   receive_fsm2->UpdateState();
   receive_fsm3->UpdateState();
   receive_fsm4->UpdateState();

   int32 len_header = MIN_LEN_HEADER + len_pad;
      
   receive_list = Buffer::RemoveDataFromList(receive_list,len_header);

   if (receive_list == NULL)
   {
      printf("SecureTransport got empty message.\n");
      return true;
   }

   receive_list->InitRead(len_message - len_header);

   // XXX Process message here (it's in receive_list)

   receive_list = 
      Buffer::RemoveDataFromList(receive_list,len_message - len_header);

   return true;
}
#undef TRYREAD
/***************************************************************/
int32 SecureTransport::AddHeader(Buffer *header, Buffer *data, int32 max_bytes)
{
   assert(header->GetNext() == NULL);
   assert(random != NULL);

   int32 len_pad = 1 + send_fsm3->GetState() % 11;

   int32 len_data = MIN_LEN_HEADER + len_pad + 
      min(max_bytes,data->GetListDataLength());
//   dprintf("passed in message max length %i, sending %i plus header of %i\n",
//	   max_bytes,len_data-MIN_LEN_HEADER-len_pad,MIN_LEN_HEADER+len_pad);
   int32 scrambled_len_data = (~len_data ^ send_fsm1->GetState()) & 0x7fffffff;

   header->Add(scrambled_len_data);
   header->Add(send_fsm2->GetState() ^ scrambled_len_data);

//   dprintf("encoding len_message with %i\n",send_fsm1->GetState());

   byte *digest_ptr = header->GetListAddPointer();

   header->Add((int32)0);
   
   for (int32 i=0; i<len_pad; i++)
   {
#if 1      
      header->Add((byte) random->Generate());
#else
      header->Add((byte) 0xce);   // Constant for debugging purposes
#endif
   }

   // we didn't check all the adds, but we're sure we didn't overflow the buffer
   assert(header->GetNext() == NULL);

   // Calculate and patch in the digest of the header AND the data
   header->SetNext(data);
   header->Set(digest_ptr, GetBufferListDigest(header, len_data));
   header->SetNext(NULL);

   return len_data;
}
/***************************************************************/
int32 SecureTransport::GetBufferListDigest(Buffer *blist,int32 count)
{
   MD5Context context;
   Buffer *b;
   int32 processed,process_now;
   int32 digest[4];

   //dprintf("Getting buffer digest of %i bytes\n",count);
   MD5Init(&context);

   processed = 0;
   b = blist;
   while (processed < count)
   {
      process_now = min(b->GetDataLength(),count-processed);

      // only update if this buffer has data, because passing in 0 bytes
      // actually DOES change the state
      if (process_now > 0)
	 MD5Update(&context,b->GetDataStart(),process_now);

      b = b->GetNext();
      processed += process_now;
   }

   MD5Final((byte *)&digest,&context);

   return digest[0] ^ digest[1] ^ digest[2] ^ digest[3];
}
