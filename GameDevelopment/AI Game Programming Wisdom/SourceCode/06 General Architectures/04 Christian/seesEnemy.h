

#ifndef _SeesEnemy_H_
#define _SeesEnemy_H_

#include "ieexec.h"

class Character;

class SeesEnemy : public IEExec
{

public:

    SeesEnemy();

    void init   ( IEOwner * owner );
    bool update ();

    IEOwner * getOwner ();

    const char * getName();

private:
    std::string m_name;
    Character * m_owner;
    
    Character * lookForEnemy ( int x, int y, int xdir, int ydir, int range );
    Character * testEnemy    ( int x, int y );
};

#endif
