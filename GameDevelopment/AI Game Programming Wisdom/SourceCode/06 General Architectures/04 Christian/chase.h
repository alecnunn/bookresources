

#ifndef _Chase_H_
#define _Chase_H_

#include "ieexec.h"

class Character;

class Chase : public IEExec
{

public:

    Chase();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum ChaseStates
    {
        Go
    };

    void newDirection ();

private:
    std::string m_name;
    Character * m_owner;

    Character * m_enemy;
};

#endif
