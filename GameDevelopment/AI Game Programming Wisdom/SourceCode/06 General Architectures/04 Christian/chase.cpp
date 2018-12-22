
#include "character.h"
#include "Chase.h"
#include "gametime.h"
#include "util.h"

Chase::Chase()
{
    m_name      = "Chase"; // must match class name
    m_owner     = NULL;
}

void Chase::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}

bool Chase::start ()
{
    m_state = Go;

    m_owner->animStart ( CA_Run );

    m_enemy = m_owner->getEnemy();

    printf ( "%s barks at %s.\n", m_owner->getName(), m_enemy->getName() );

    m_enemy->alertChase ( m_owner );

    m_owner->setMotionSpeed ( m_owner->getRunSpeed() );

    return false;
}

bool Chase::update () 
{
    switch ( m_state )
    {

    case Go:

        // get enemies position
        int ex, ey;
        m_enemy->getPos ( ex, ey );

        // get our position
        int ox, oy;
        m_owner->getPos ( ox, oy );

        // set movement direction towards it (only x or y, not both)
        int xdir, ydir;

        if ( abs( ex - ox ) > abs (ey - oy ) )
        {
            ydir = 0;

            if ( ex > ox )
                xdir = 1;
            else
                xdir = -1;
        }
        else
        {
            xdir = 0;

            if ( ey > oy )
                ydir = 1;
            else
                ydir = -1;
        }

        m_owner->setDirection ( xdir, ydir );

        // move
        m_owner->move ();

        if ( m_owner->inNewPos() )
        {
            printf ( "%s chasing %s at %d %d\n",
                m_owner->getName(), m_enemy->getName(), ox, oy );
        }

        m_owner->burnEnergy();

        break;

    }

    return true;
}

bool Chase::finish ()
{
    return true;
}

IEOwner * Chase::getOwner ()
{
    return m_owner;
}

const char * Chase::getName()
{
    return m_name.c_str();
}

