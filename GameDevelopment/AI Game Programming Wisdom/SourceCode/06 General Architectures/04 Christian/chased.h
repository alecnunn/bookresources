

#ifndef _Chased_H_
#define _Chased_H_

#include "ieexec.h"

class Character;

class Chased : public IEExec
{

public:

    Chased();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
    
};

#endif
