
#ifndef TIMESOURCE_H_
#define TIMESOURCE_H_


class TimeSource
{
public:
    virtual ~TimeSource() {};
    virtual double GetTime() const = 0;
};



#endif