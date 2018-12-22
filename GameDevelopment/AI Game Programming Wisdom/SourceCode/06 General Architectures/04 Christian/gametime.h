
#ifndef _GAMETIME_H_
#define _GAMETIME_H_

#include "time.h"

class GameTime
{

public:

    GameTime();

    void start ();
    void end   ();

    static double dt;

private:
    time_t       m_startTime;
    time_t       m_endTime;
    double       m_elapsedTime;
};

#endif