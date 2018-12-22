#include "drivers/null/nullcore.hpp"
#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"
#include "drivers/null/core/time/nulltimer.hpp"

CHighTimer *
CNullHighTimer::Get()
{
    if (!ms_pSingleton) {
        ms_pSingleton = new CNullHighTimer;
    }

    return ms_pSingleton;
}

CNullHighTimer::CNullHighTimer() : CHighTimer()
{
	// NULLSTUB
}

CNullHighTimer::~CNullHighTimer()
{
}


void 
CNullHighTimer::Start()
{
	// NULLSTUB
}

tREAL32 
CNullHighTimer::GetTime()
{
	// NULLSTUB

    return 0;
}
