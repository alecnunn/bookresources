
#include "character.h"
#include "WonFight.h"
#include "gametime.h"
#include "util.h"

WonFight::WonFight()
{
    m_name      = "WonFight";
    m_owner     = NULL;
}

void WonFight::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool WonFight::update () 
{
    Character * enemy = m_owner->getEnemy();

    if ( enemy )
	{
		if ( enemy->getHealth() <= 0 )
		{
			return true;
		}
	}

	return false;
}


IEOwner * WonFight::getOwner ()
{
    return m_owner;
}

const char * WonFight::getName()
{
    return m_name.c_str();
}

