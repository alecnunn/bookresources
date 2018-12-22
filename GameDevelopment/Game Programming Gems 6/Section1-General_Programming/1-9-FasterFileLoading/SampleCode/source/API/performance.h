#ifndef _PERFORMANCE_H_
#define _PERFORMANCE_H_

#ifdef _MSC_VER
	#include <windows.h>

	// Performance profiling 
	LARGE_INTEGER nFrequency;
	LARGE_INTEGER nStartTime;
	LARGE_INTEGER nStopTime;
	#define START_TIME() QueryPerformanceFrequency(&nFrequency); QueryPerformanceCounter(&nStartTime);
	#define STOP_TIME() QueryPerformanceCounter(&nStopTime);
	#define TIME_IN_MILLI ((float)(nStopTime.QuadPart - nStartTime.QuadPart) / (float) nFrequency.QuadPart * 1000.0f)
#elif defined __GNUC__
	#include <sys/time.h>
	#include <unistd.h>
	struct timeval _tstart, _tend;
	struct timezone tz; 
	#define START_TIME() gettimeofday(&_tstart, &tz);
	#define STOP_TIME() gettimeofday(&_tend,&tz);
	#define TIME_IN_MILLI (((double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000)) - ((double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000)))
#endif

#endif
