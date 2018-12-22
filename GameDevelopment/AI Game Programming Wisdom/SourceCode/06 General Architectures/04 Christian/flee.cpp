
#include "character.h"
#include "Flee.h"
#include "gametime.h"
#include "util.h"

Flee::Flee()
{
    m_name      = "Flee"; // must match class name
    m_owner     = NULL;
}

void Flee::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}

bool Flee::start ()
{
    m_state = Go;

    m_owner->animStart ( CA_Run );

    m_enemy = m_owner->getEnemy();

    printf ( "%s meows and takes out running.\n", m_owner->getName() );

    m_owner->setMotionSpeed ( m_owner->getRunSpeed() );

    m_clock = 0.0f;
    m_changeDirTime = 3.0f;

    return false;
}

bool Flee::update () 
{
    switch ( m_state )
    {

    case Go:

        m_clock += GameTime::dt;

        if ( m_clock >= m_changeDirTime )
        {

            m_clock = 0.0f;

            // get enemies position
            int ex, ey;
            m_enemy->getPos ( ex, ey );

            // get our position
            int ox, oy;
            m_owner->getPos ( ox, oy );

            // set movement direction away from enemy (only x or y, not both)
            int xdir, ydir;

            if ( abs( ex - ox ) < abs (ey - oy ) )
            {
                ydir = 0;

                if ( ex > ox )
                    xdir = -1;
                else
                    xdir = 1;
            }
            else
            {
                xdir = 0;

                if ( ey > oy )
                    ydir = -1;
                else
                    ydir = 1;
            }

            m_owner->setDirection ( xdir, ydir );
        }

        // move
        m_owner->move ();

        if ( m_owner->inNewPos() )
        {
            int x, y;
            m_owner->getPos ( x, y );

            printf ( "%s fleeing %s at %d %d\n",
                m_owner->getName(), m_enemy->getName(), x, y );
        }

        m_owner->burnEnergy();

        break;

    }

    return true;
}

bool Flee::finish ()
{
    return true;
}

IEOwner * Flee::getOwner ()
{
    return m_owner;
}

const char * Flee::getName()
{
    return m_name.c_str();
}

