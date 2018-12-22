
#ifndef TIMESOURCECUSTOM_H_
#define TIMESOURCECUSTOM_H_


#include "TimeSource.h"
#include <deque>


class TimeSourceCustom : public TimeSource
{
public:
    TimeSourceCustom();
    virtual double GetTime() const;

    void QueueTime (double time);
    void QueueStep (double time);

private:
    mutable std::deque<double> m_timeQueue;
};



#endif