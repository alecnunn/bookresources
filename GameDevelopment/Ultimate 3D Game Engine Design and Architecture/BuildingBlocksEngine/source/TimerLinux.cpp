/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<time.h>
#include<NameSpace.h>


DECLARE_ENGINE_NAMESPACE

float g_start = 0;


float GetTimeMs()
{
   clock_t time = clock();

   if(g_start == 0)
      g_start = (float)clock();

   return (float)time - g_start;
}


float GetTimeSeconds()
{
   clock_t time = GetTimeMs() / CLOCKS_PER_SEC;
   return (float)time;
}


END_ENGINE_NAMESPACE
