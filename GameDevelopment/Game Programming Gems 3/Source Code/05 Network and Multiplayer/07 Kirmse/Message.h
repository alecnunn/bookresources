/*
 * Message
 *
 * Holds a single message from the network.
 */

#ifndef _MESSAGE_H
#define _MESSAGE_H

class Message
{
public:

   explicit Message(uint32 new_length);
   Message(const Message &msg);
   ~Message();

   byte  *GetData() const
   {
      return mpData;
   }
   uint32 GetLength() const
   {
      return miLength;
   }

   // Get/Set the clock time when we should send this message
   uint32 GetSendTime() const
   {
      return miSendTime;
   }
   void   SetSendTime(uint32 new_send_time)
   {
      miSendTime = new_send_time;
   }

   const Action &GetAction() const
   {
      return mAction;
   }

   void          SetAction(const Action &action)
   {
      mAction = action;
   }
   
private:
   
   byte  *mpData;
   uint32 miLength;

   uint32 miSendTime;
   Action mAction;
};

#endif
