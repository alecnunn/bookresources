
#include "character.h"
#include "Caught.h"
#include "gametime.h"
#include "util.h"

Caught::Caught()
{
    m_name      = "Caught";
    m_owner     = NULL;
}

void Caught::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool Caught::update () 
{
    Character * enemy = m_owner->getEnemy();

    if ( enemy )
    {
        // get enemy position
        int ex, ey;
        enemy->getPos ( ex, ey );

        // get our position
        int ox, oy;
        m_owner->getPos ( ox, oy );

        if ( ex == ox && ey == oy )
        {
            if ( ! m_owner->beingChased() )
            {
                printf ( "%s catches %s\n", m_owner->getName(), 
                    enemy->getName() );
            }

            return true;
        }
    }

    return false;
}


IEOwner * Caught::getOwner ()
{
    return m_owner;
}

const char * Caught::getName()
{
    return m_name.c_str();
}

