
#include <windows.h>

#include "CTimer.h"

CTimer::CTimer()
{
}

void CTimer::Reset()
{
	mBaseTime = GetTickCount();
}

uint32 CTimer::GetSeconds()
{
	mCurrentTime = GetTickCount();
	return (mCurrentTime - mBaseTime) / 1000;
}



