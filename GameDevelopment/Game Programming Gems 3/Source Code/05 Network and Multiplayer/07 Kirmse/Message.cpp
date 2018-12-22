#include "main.h"

/***************************************************************/
Message::Message(uint32 new_length) :
      mAction(Action::NONE)
{
   miLength = new_length;

   mpData = (byte *) malloc(miLength);
}
/***************************************************************/
Message::Message(const Message &msg) :
      mAction(Action::NONE)
{
   miLength = msg.miLength;
   mpData   = (byte *) malloc(miLength);
   mAction  = msg.mAction;
   memcpy(mpData, msg.mpData, miLength);
}
/***************************************************************/
Message::~Message()
{
   free(mpData);
}
