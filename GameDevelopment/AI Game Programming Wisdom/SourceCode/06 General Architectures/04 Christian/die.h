

#ifndef _Die_H_
#define _Die_H_

#include "ieexec.h"

class Character;

class Die : public IEExec
{

public:

    Die();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum DieStates
    {
        Go,
		Done
    };


private:
    std::string m_name;
    Character * m_owner;

};

#endif
