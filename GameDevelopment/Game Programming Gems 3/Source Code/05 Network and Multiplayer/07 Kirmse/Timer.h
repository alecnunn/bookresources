#ifndef _TIMER_H
#define _TIMER_H

class CTimer
{
public:

   CTimer();
   ~CTimer();

   // Return time since creation, in milliseconds
   float     GetDuration     ();

private:

   float  mfSecondsPerTick;
   int64  miStartTime;
};


#endif
