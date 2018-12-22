/*
 * Action
 *
 * Describes an action taken by a filter, and can produce a
 * human-readable version.
 */

#ifndef _ACTION_H
#define _ACTION_H

class Action
{
public:

   enum ActionType
   {
      NONE,
      DROP,
      DELAY,
      WAIT_CONNECT,
      SEND_NOW,
      DUPLICATE,
      REORDER
   };
   
   Action(ActionType new_type, int32 new_value = 0);
   Action(const Action &that);
   ~Action();

   void       ToString(char *str) const;
   
   ActionType GetType() const
   {
      return mType;
   }

private:
   
   ActionType mType;
   int32      miValue;
};

#endif
