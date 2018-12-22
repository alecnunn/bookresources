

#ifndef _Done_H_
#define _Done_H_

#include "ieexec.h"

class Character;

class Done : public IEExec
{

public:

    Done();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
};

#endif
