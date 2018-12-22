/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Carbon/Carbon.h>
#include<Timer.h>


DECLARE_ENGINE_NAMESPACE


float GetTimeMs()
{
   float time = 0;

   //time = (unsigned)TickCount();
   AbsoluteTime current;
   Duration currentDur;
   current = UpTime();
   currentDur = AbsoluteToDuration(current);
   time = (float)currentDur;

   return time;
}


float GetTimeSeconds()
{
   float time = 0;

   //time = (unsigned)TickCount() * 60;
   AbsoluteTime current;
   Duration currentDur;
   current = UpTime();
   currentDur = AbsoluteToDuration(current);
   time = (float)(0.001 * currentDur);

   return time;
}


END_ENGINE_NAMESPACE