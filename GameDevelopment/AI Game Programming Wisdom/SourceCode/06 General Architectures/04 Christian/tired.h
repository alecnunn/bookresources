

#ifndef _Tired_H_
#define _Tired_H_

#include "ieexec.h"

class Character;

class Tired : public IEExec
{

public:

    Tired();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
    
    int m_fatigueMax;
};

#endif
