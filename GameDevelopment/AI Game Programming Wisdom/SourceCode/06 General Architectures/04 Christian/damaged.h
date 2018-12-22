

#ifndef _Damaged_H_
#define _Damaged_H_

#include "ieexec.h"

class Character;

class Damaged : public IEExec
{

public:

    Damaged();

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
