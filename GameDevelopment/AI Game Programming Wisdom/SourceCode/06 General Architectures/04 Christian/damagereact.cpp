
#include "character.h"
#include "DamageReact.h"
#include "gametime.h"
#include "util.h"

DamageReact::DamageReact()
{
    m_name      = "DamageReact"; // must match class name
    m_owner     = NULL;
}

void DamageReact::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}

bool DamageReact::start ()
{
    m_state = Go;

  
    const char * desc = m_owner->getDamageReact ( m_time );
	printf ( "%s %s\n", m_owner->getName(), desc );

    m_clock = 0.0f;

    return false;
}

bool DamageReact::update () 
{
    switch ( m_state )
    {

    case Go:

        m_clock += GameTime::dt;

		if ( m_clock >= m_time )
		{
			m_state = Done;
			m_owner->setDone ( true );
		}
        break;

	case Done:
		break;

    }

    return true;
}

bool DamageReact::finish ()
{
    return true;
}

IEOwner * DamageReact::getOwner ()
{
    return m_owner;
}

const char * DamageReact::getName()
{
    return m_name.c_str();
}

