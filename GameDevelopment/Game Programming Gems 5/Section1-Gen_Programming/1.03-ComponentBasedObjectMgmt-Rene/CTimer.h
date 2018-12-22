
#ifndef __CTIMER_H
#define __CTIMER_H

#include "types.h"

// Feel free to drop this *high performance* timer into your own projects... 8)

class CTimer
{
public:
	CTimer();

	void Reset();
	uint32 GetSeconds();

private:

	uint32 mCurrentTime;
	uint32 mBaseTime;
};


#endif //__CTIMER_H



