
#ifndef TIMESOURCEHRWIN32_H_
#define TIMESOURCEHRWIN32_H_


#include "TimeSource.h"


class TimeSourceHRWin32 : public TimeSource
{
public:
    TimeSourceHRWin32();
    virtual double GetTime() const;


private:
    double GetSecondsPerTick() const;

    double m_secsPerTick;
};



#endif