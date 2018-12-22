

#ifndef _Rested_H_
#define _Rested_H_

#include "ieexec.h"

class Character;

class Rested : public IEExec
{

public:

    Rested();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
};

#endif
