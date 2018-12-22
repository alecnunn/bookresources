
#ifndef TIMER_H_
#define TIMER_H_


class Clock;
#include "Observer.h"



class Timer : private IObserver
{
public:
    Timer(Clock & clock);
    ~Timer();

    double GetTime() const { return m_currentTime; }
    double GetFrameDuration() const { return m_frameTime; }

    void Pause (bool bOn);
    void SetScale (float fScale);   
    
    bool IsPaused () const;
    float GetScale () const;

private:
    virtual void Notify();
    
    Clock & m_clock;

    double  m_currentTime;
    double  m_frameTime;

    bool    m_bPaused;
    float   m_fScale;
};



#endif