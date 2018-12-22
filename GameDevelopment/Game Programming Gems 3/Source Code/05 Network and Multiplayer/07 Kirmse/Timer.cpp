
#include "main.h"

CTimer::CTimer()
{
   int64 iNumTicksPerSecond;
   QueryPerformanceFrequency((LARGE_INTEGER*) &iNumTicksPerSecond);
   mfSecondsPerTick = 1.0f / ((float) iNumTicksPerSecond);
   
   QueryPerformanceCounter((LARGE_INTEGER*) &miStartTime);
}


CTimer::~CTimer()
{
}


float CTimer::GetDuration()
{
   int64 iNow;
   QueryPerformanceCounter((LARGE_INTEGER*) &iNow);
   return ((int64) (iNow - miStartTime) * mfSecondsPerTick) * 1000.0f;
}

