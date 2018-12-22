#include "drivers/wintel32/wintel32core.hpp"
#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"
#include "drivers/wintel32/core/time/wintel32timer.hpp"

CHighTimer *
CPCHighTimer::Get()
{
    if (!ms_pSingleton) {
        ms_pSingleton = new CPCHighTimer;
    }

    return ms_pSingleton;
}

CPCHighTimer::CPCHighTimer() : CHighTimer()
{
    // period is the number of counter increments that occur over 1 second
    if (FALSE == QueryPerformanceFrequency(&m_Frequency)) {
        sgxTrace("Can not query performance timer.");
    } else {
        m_FrequencyTime = (double)m_Frequency.QuadPart;
    }
}

CPCHighTimer::~CPCHighTimer()
{
}


void 
CPCHighTimer::Start()
{
    CHighTimer::Start();
    QueryPerformanceCounter(&m_Start);
}

tREAL32 
CPCHighTimer::GetTime()
{
LARGE_INTEGER curr;
double t;

    QueryPerformanceCounter(&curr);

    t = (double)(curr.QuadPart - m_Start.QuadPart) * 1000.0;
    t /= m_FrequencyTime;

    return (tREAL32)t;
}
