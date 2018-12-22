#include "main.h"

/***************************************************************/
Action::Action(ActionType new_type, int32 new_value) :
   mType(new_type),
   miValue(new_value)
{
}
/***************************************************************/
Action::Action(const Action &that)
{
   mType   = that.mType;
   miValue = that.miValue;
}
/***************************************************************/
Action::~Action()
{
}
/***************************************************************/
void Action::ToString(char *str) const
{
   switch (mType)
   {
   case NONE:
      strcpy(str, "");
      break;
      
   case DROP:
      strcpy(str, "Dropped");
      break;
      
   case DELAY:
      sprintf(str, "Delayed %d ms", miValue);
      break;

   case WAIT_CONNECT:
      strcpy(str, "Wait for connect");
      break;

   case SEND_NOW:
      strcpy(str, "Sent immediately");
      break;

   case DUPLICATE:
      strcpy(str, "Duplicated");
      break;
      
   case REORDER:
      strcpy(str, "Reordered");
      break;
      
   default:
      strcpy(str, "?");
      break;
   }
}
