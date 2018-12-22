#include "sgx/core/types/types.hpp"
#include "sgx/core/time/timer.hpp"

CHighTimer *CHighTimer::ms_pSingleton = NULL;

CHighTimer *CHighTimer::Get()
{
    if (!ms_pSingleton) {
        ms_pSingleton = new CHighTimer;
    }

    return ms_pSingleton;
}

