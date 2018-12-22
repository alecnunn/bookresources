

#ifndef _Flee_H_
#define _Flee_H_

#include "ieexec.h"

class Character;

class Flee : public IEExec
{

public:

    Flee();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum FleeStates
    {
        Go
    };

    void newDirection ();

private:
    std::string m_name;
    Character * m_owner;

    Character * m_enemy;

    float m_clock;
    float m_changeDirTime;
};

#endif
