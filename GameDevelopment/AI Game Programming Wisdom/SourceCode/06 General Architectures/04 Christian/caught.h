

#ifndef _Caught_H_
#define _Caught_H_

#include "ieexec.h"

class Character;

class Caught : public IEExec
{

public:

    Caught();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
    
};

#endif
