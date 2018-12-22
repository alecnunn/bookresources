
#include "character.h"
#include "Dying.h"
#include "gametime.h"
#include "util.h"

Dying::Dying()
{
    m_name      = "Dying";
    m_owner     = NULL;
}

void Dying::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool Dying::update () 
{
    int curHealth = m_owner->getHealth();

	if ( curHealth <= 0 )
		return true;
	else
		return false;
}


IEOwner * Dying::getOwner ()
{
    return m_owner;
}

const char * Dying::getName()
{
    return m_name.c_str();
}

