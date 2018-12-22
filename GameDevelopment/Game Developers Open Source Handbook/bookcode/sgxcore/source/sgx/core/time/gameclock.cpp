#include "sgx/core/time/gameclock.hpp"

CGameClock *CGameClock::sm_pSingleton = NULL;

CGameClock *
CGameClock::Get()
{
    if (sm_pSingleton == NULL) {
        sm_pSingleton = new CGameClock;
    }

    return sm_pSingleton;
}


CGameClock::CGameClock()
{
    m_fLastTime = 0;
    m_fFractionalSpeed = 1.0f;
    m_fGivenTime = 0;
    m_fConstantPeriod = 1.0f/60.0f;
    m_fClampedMin = 1.0f/15.0f;
    m_fClampedMax = 1.0f/60.0f;
    m_ClockMode = Realtime;
	m_fLastTelaps = 0;
}

tREAL32 
CGameClock::ApplyRealTimeToClock(const tREAL32 current_time)
{
tREAL32 time_elapsed;

    time_elapsed = current_time - m_fLastTime;

    switch(m_ClockMode)
    {
    case	Realtime:
        time_elapsed *= m_fFractionalSpeed;
        break;

    case	AsGiven:
        time_elapsed = m_fGivenTime;
        break;

    case	Constant:
        time_elapsed = m_fConstantPeriod;
        break;

    case	Clamped:
        if (time_elapsed < m_fClampedMax) {
            time_elapsed = m_fClampedMax;
        } else if (time_elapsed > m_fClampedMin) {
            time_elapsed = m_fClampedMin;
        }
        time_elapsed *= m_fFractionalSpeed;
        break;
    }

    m_fLastTime = current_time;
	m_fLastTelaps = time_elapsed;

    return time_elapsed;
}

void 
CGameClock::SetModeRealtime()
{
    m_ClockMode = Realtime;
}

void 
CGameClock::SetModeConstant(const tREAL32 period)
{
    m_fConstantPeriod = period;
    m_ClockMode = Constant;
}

void 
CGameClock::SetModeConstantFPS(const tREAL32 fps)
{
    m_fConstantPeriod = 1.0f/fps;
    m_ClockMode = Constant;
}

void 
CGameClock::SetModeClamped(const tREAL32 min_fps, const tREAL32 max_fps)
{
    m_fClampedMin = 1.0f/min_fps;
    m_fClampedMax = 1.0f/max_fps;
    m_ClockMode = Clamped;
}

void 
CGameClock::SetModeClampedTelaps(const tREAL32 min_telaps, const tREAL32 max_telaps)
{
    m_fClampedMin = min_telaps;
    m_fClampedMax = max_telaps;
    m_ClockMode = Clamped;
}

tREAL32 
CGameClock::Update(const tREAL32 telaps)
{
    return ApplyRealTimeToClock(telaps + m_fLastTime);
}

tREAL32 
CGameClock::GetTime()
{
    return m_fLastTime;
}

tREAL32 
CGameClock::GetLastTelaps()
{
    return m_fLastTelaps;
}

tREAL32 
CGameClock::GetClampedMin()
{
    return m_fClampedMin;
}

tREAL32 
CGameClock::GetClampedMax()
{
    return m_fClampedMax;
}
