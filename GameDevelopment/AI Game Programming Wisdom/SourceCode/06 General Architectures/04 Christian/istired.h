

#ifndef _IsTired_H_
#define _IsTired_H_

#include "ieexec.h"

class Character;

class IsTired : public IEExec
{

public:

    IsTired();

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
