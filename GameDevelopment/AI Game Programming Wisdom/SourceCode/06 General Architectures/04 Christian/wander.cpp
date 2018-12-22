
#include "character.h"
#include "Wander.h"
#include "gametime.h"
#include "util.h"

Wander::Wander()
{
    m_name      = "Wander"; // must match class name
    m_owner     = NULL;
}

void Wander::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);

    m_minWanderTime = 5.0f;
}

bool Wander::start ()
{
    m_state = Go;

    m_owner->animStart ( CA_Walk );

    printf ( "%s starts walking.\n", m_owner->getName() );

    m_dirClock  = 0.0f;
    newDirection();

    m_owner->setMotionSpeed ( m_owner->getWalkSpeed() );

    return false;
}

bool Wander::update () 
{
    switch ( m_state )
    {

    case Go:
        m_owner->move ();

        m_dirClock += GameTime::dt;

        if ( m_dirClock > m_timeChangeDir )
        {
            newDirection();
            m_dirClock = 0.0f;
        }

        if ( m_owner->inNewPos() )
        {
            int x, y;
            m_owner->getPos ( x, y );
            printf ( "%s wandering at %d %d\n",
                m_owner->getName(), x, y );
        }

        m_owner->burnEnergy();

        break;
    }

    return true;
}

bool Wander::finish ()
{
    return true;
}

IEOwner * Wander::getOwner ()
{
    return m_owner;
}

const char * Wander::getName()
{
    return m_name.c_str();
}

void Wander::newDirection ()
{
    float r = frand();

    int xdir, ydir;

    // x direction
    if ( r < 0.5f )
    {
        r = frand();

        if ( r < 0.5f )
            xdir = 1;
        else
            xdir = -1;

        ydir = 0;
    }

    // else y direction
    else 
    {
        r = frand();

        if ( r < 0.5f )
            ydir = 1;
        else
            ydir = -1;

        xdir = 0;
    }

    //printf ( "changed dir %d %d\n", xdir, ydir );

    // figure the time to change next direction
    r = frand();

    m_timeChangeDir = m_minWanderTime + ( r * 10.0f );

    m_owner->setDirection ( xdir, ydir );
}