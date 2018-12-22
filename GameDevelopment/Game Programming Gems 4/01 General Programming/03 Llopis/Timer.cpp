
#include "Timer.h"
#include "Clock.h"


Timer::Timer(Clock & clock) :
    m_clock (clock),
    m_currentTime (0),
    m_frameTime (0),
    m_bPaused (false),
    m_fScale (1.0f)
{
    m_clock.AddObserver(this);
}


Timer::~Timer()
{
    m_clock.RemoveObserver(this);
}


void Timer::Notify ()
{
    if (!m_bPaused)
    {
        m_frameTime = m_clock.GetFrameDuration() * m_fScale;
        m_currentTime += m_frameTime;
    }
}



void Timer::Pause (bool bOn)
{
    m_bPaused = bOn;
}


void Timer::SetScale(float fScale)
{
    m_fScale = fScale;
}



float Timer::GetScale () const
{
    return m_fScale;
}

bool Timer::IsPaused () const
{
    return m_bPaused;
}
