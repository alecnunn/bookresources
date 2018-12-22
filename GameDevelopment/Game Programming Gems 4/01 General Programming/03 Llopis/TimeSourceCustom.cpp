
#include "TimeSourceCustom.h"



TimeSourceCustom::TimeSourceCustom()
{
}


double TimeSourceCustom::GetTime() const
{
    if (m_timeQueue.empty())
        return 0;
    
    double time = m_timeQueue.front();
    m_timeQueue.pop_front();
    return time;
}


void TimeSourceCustom::QueueTime (double time)
{
    m_timeQueue.push_back(time);
}

void TimeSourceCustom::QueueStep (double step)
{
    if (m_timeQueue.empty())
        m_timeQueue.push_back(step);
    else
        m_timeQueue.push_back(step + m_timeQueue.back());
}
