

#ifndef _Dying_H_
#define _Dying_H_

#include "ieexec.h"

class Character;

class Dying : public IEExec
{

public:

    Dying();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
    
	int m_lastHealth;
};

#endif
