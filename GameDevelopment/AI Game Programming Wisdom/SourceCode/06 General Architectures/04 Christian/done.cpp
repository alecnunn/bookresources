
#include "character.h"
#include "Done.h"
#include "gametime.h"
#include "util.h"

Done::Done()
{
    m_name      = "Done";
    m_owner     = NULL;
}

void Done::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool Done::update () 
{
	if ( m_owner->getDone() )
	{
		m_owner->setDone ( false ); // reset for the next guy
		return true;
	}
	else
		return false;
}


IEOwner * Done::getOwner ()
{
    return m_owner;
}

const char * Done::getName()
{
    return m_name.c_str();
}

