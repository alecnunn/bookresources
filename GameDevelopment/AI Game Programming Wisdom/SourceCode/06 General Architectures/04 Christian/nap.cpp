
#include "character.h"
#include "nap.h"
#include "gametime.h"

Nap::Nap()
{
    m_name      = "Nap"; // must match class name
    m_timeSnore = 3.0f;
    m_owner     = NULL;
}

void Nap::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}

bool Nap::start ()
{
    m_state = LieDown;

    m_owner->animStart ( CA_LieDown );

    printf ( "%s is tired and lies down.\n", m_owner->getName() );

    m_done = false;

    return false;
}

bool Nap::update () 
{
    switch ( m_state )
    {

    case LieDown:
        if ( m_owner->animDone () )
        {
            printf ( "%s yawns.\n", m_owner->getName() );

            m_owner->animStart ( CA_Yawn );
            m_state = DownYawn;
        }
        break;

    case DownYawn:
        if ( m_owner->animDone () )
        {
            printf ( "%s falls asleep.\n", m_owner->getName() );

            m_owner->animStart ( CA_Rest );
            m_state = Rest;
            m_clock = 0.0f;
        }
        break; 

    case Rest:
        m_clock += GameTime::dt;

        m_owner->restoreEnergy();

        if ( m_clock > m_timeSnore )
        {
            printf ( "%s snores.\n", m_owner->getName() );
            m_clock = 0.0f;
        }

        if ( m_owner->rested() )
        {
            printf ( "%s wakes up.\n", m_owner->getName() );

            m_owner->animStart ( CA_GetUp );
            m_state = GetUp;
        }

        break;

    case GetUp:
        if ( m_owner->animDone () )
        {
            printf ( "%s stands up and yawns.\n", m_owner->getName() );

            m_owner->animStart ( CA_Yawn );
            m_state = UpYawn;            
        }

        break;

    case UpYawn:
        if ( m_owner->animDone () )
        {
            printf ( "%s stretchs.\n", m_owner->getName() );

            m_owner->animStart ( CA_Stretch );
            m_state = Stretch;
        }
        break;

    case Stretch:
        if ( m_owner->animDone () )
        {
            printf ( "%s is wide awake and alert.\n", m_owner->getName() );

            m_done  = true;  // flag universal done for inference engine
            m_state = Done;
        }
        break;

    case Done:
        return false;
        break;

    }

    return true;
}

bool Nap::finish ()
{
    return true;
}

IEOwner * Nap::getOwner ()
{
    return m_owner;
}

const char * Nap::getName()
{
    return m_name.c_str();
}
