
#include "character.h"
#include "Damaged.h"
#include "gametime.h"
#include "util.h"

Damaged::Damaged()
{
    m_name      = "Damaged";
    m_owner     = NULL;
}

void Damaged::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);

	m_lastHealth = m_owner->getHealth();
}


bool Damaged::update () 
{
    int curHealth = m_owner->getHealth();

	if ( curHealth != m_lastHealth )
	{
		m_lastHealth = curHealth;
		return true;
	}
	else
		return false;
}


IEOwner * Damaged::getOwner ()
{
    return m_owner;
}

const char * Damaged::getName()
{
    return m_name.c_str();
}

