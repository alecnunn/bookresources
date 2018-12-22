

#ifndef _WonFight_H_
#define _WonFight_H_

#include "ieexec.h"

class Character;

class WonFight : public IEExec
{

public:

    WonFight();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
};

#endif
