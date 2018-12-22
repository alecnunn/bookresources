

#ifndef _Wander_H_
#define _Wander_H_

#include "ieexec.h"

class Character;

class Wander : public IEExec
{

public:

    Wander();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum WanderStates
    {
        Go
    };

    void newDirection ();

private:
    std::string m_name;
    Character * m_owner;

    float m_dirClock;
    float m_minWanderTime;
    float m_timeChangeDir;
};

#endif
