//
// Timer class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <windows.h>
#include "Core/Timer.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif



                                                    
PTCAPI Timer::Timer()
{
    // get timer frequency
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);

    // defaults
    m_old = 0.0;
    m_time = 0.0;
    m_start = 0.0;
    m_current = 0.0;
    m_running = false;
}


PTCAPI Timer::Timer(double time)
{
    // get timer frequency
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);

    // defaults
    m_old = 0.0;
    m_time = 0.0;
    m_start = 0.0;
    m_current = 0.0;
    m_running = false;

    // set time
    set(time);
}


PTCAPI Timer::Timer(const Timer &timer)
{
    // copy constructor
    *this = timer;
}


PTCAPI Timer::~Timer()
{
    // stop
    stop();
}




void PTCAPI Timer::set(double time)
{
    // set time
    m_current = time;
    m_start = clock();
    m_time = m_start + time;
    m_old = m_time - delta();
}




void PTCAPI Timer::start()
{
    // check if not running
    if (!m_running)
    {
        // set start time
        m_start = clock();

        // set old time
        m_old = clock();

        // timer is running
        m_running = true;
    }
}


void PTCAPI Timer::stop()
{
    // timer is not running
    m_running = false;
}




double PTCAPI Timer::time()
{
    // check if running
    if (m_running)
    {
        // get current clock time
        const double time = clock();

        // assign if time has advanced
        if (time>m_time) m_time = time;

        // calculate current time
        m_current = m_time - m_start;
    }

    // get current time
    return m_current;
}


double PTCAPI Timer::delta()
{
    // check if running
    if (m_running)
    {
        // get current clock time
        const double time = clock();

        // calculate delta
        double delta = time - m_old;

        // update old time
        m_old = time;

        // force positive delta
        if (delta<0.0) delta = 0.0; 

        // get delta
        return delta;
    }
    else
    {
        // not running
        return 0.0;
    }
}


double PTCAPI Timer::resolution()
{
    // get timer resolution
    return 1.0 / (double) m_frequency;
}




Timer& PTCAPI Timer::operator =(const Timer &timer)
{
    // check for self assignment
    if (this==&timer) throw Error("self assignment is not allowed");
        
    // assignment
    m_old = timer.m_old;
    m_time = timer.m_time;
    m_start = timer.m_start;
    m_current = timer.m_current;
    m_running = timer.m_running;
    return *this;
}


bool PTCAPI Timer::operator ==(const Timer &timer) const
{
    // is equal to
    if (m_old==timer.m_old && 
        m_time==timer.m_time && 
        m_start==timer.m_start && 
        m_current==timer.m_current &&
        m_running==timer.m_running) return true;

    // failure
    return false;
}


bool PTCAPI Timer::operator !=(const Timer &timer) const
{
    // not equal to
    return !(*this==timer);
}




double Timer::clock()
{
    // get time
    __int64 time;
    QueryPerformanceCounter((LARGE_INTEGER*)&time);

    // convert to seconds and return
    return (double) time / (double) m_frequency;
}
