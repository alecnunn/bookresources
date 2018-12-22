
#include "character.h"
#include "Die.h"
#include "gametime.h"
#include "util.h"

Die::Die()
{
    m_name      = "Die"; // must match class name
    m_owner     = NULL;
}

void Die::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}

bool Die::start ()
{
    m_state = Go;

    m_owner->animStart ( CA_Die );

	printf ( "%s is dying...\n", m_owner->getName() );

    return false;
}

bool Die::update () 
{
    switch ( m_state )
    {

    case Go:

		if ( m_owner->animDone() )
		{
			printf ( "%s is dead.\n", m_owner->getName() );
			m_state = Done;
		}
        break;

	case Done:
		break;

    }

    return true;
}

bool Die::finish ()
{
    return true;
}

IEOwner * Die::getOwner ()
{
    return m_owner;
}

const char * Die::getName()
{
    return m_name.c_str();
}

