/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<windows.h>
#include<mmsystem.h>
#include<Timer.h>

#pragma comment(lib, "winmm.lib")


DECLARE_ENGINE_NAMESPACE


float GetTimeMs()
{
   return (float)timeGetTime();
}


float GetTimeSeconds()
{
   return (float)timeGetTime() * 0.001f;
}


END_ENGINE_NAMESPACE