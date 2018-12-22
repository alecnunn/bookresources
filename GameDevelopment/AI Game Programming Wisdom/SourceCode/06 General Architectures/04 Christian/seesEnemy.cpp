
#include "character.h"
#include "SeesEnemy.h"
#include "gametime.h"
#include "util.h"

SeesEnemy::SeesEnemy()
{
    m_name      = "SeesEnemy";
    m_owner     = NULL;
}

void SeesEnemy::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool SeesEnemy::update () 
{
    int px;
    int py;

    m_owner->getPos ( px, py );

    int xdir;
    int ydir;

    m_owner->getDirection ( xdir, ydir );

    int dist = m_owner->getSightRange();

    Character * enemy = lookForEnemy ( px, py, xdir, ydir, dist );

    if ( enemy )
    {
        m_owner->setEnemy ( enemy );
        return true;
    }
    else
    {
        m_owner->setEnemy ( NULL );
        return false;
    }
}


IEOwner * SeesEnemy::getOwner ()
{
    return m_owner;
}

const char * SeesEnemy::getName()
{
    return m_name.c_str();
}

Character * SeesEnemy::lookForEnemy ( int x, int y, int xdir, int ydir, int range )
{
    int  i;
    int  sx;
    int  sy;

    Character * enemy = NULL;

    World2D  * world = m_owner->getWorld();
    
    int width  = world->getWidth();
    int length = world->getLength();

    if ( xdir > 0 )
    {
        for ( i=x+1; i<=x+range; i++ )
        {
            sy = y;

            if ( i >= width )
                sx = 0 + (i - width);
            else
                sx = i;

            enemy = testEnemy ( sx, sy );
            if ( enemy )
                return enemy;
        }
    }
    else if ( xdir < 0 )
    {
        for ( i=x-1; i>=x-range; i-- )
        {
            sy = y;

            if ( i < 0 )
                sx = width + i;
            else
                sx = i;

            enemy = testEnemy ( sx, sy );
            if ( enemy )
                return enemy;
        }
    }
    else if ( ydir > 0 )
    {
        for ( i=y+1; i<=y+range; i++ )
        {
            sx = x;

            if ( i >= length )
                sy = 0 + (i - length);
            else
                sy = i;

            enemy = testEnemy ( sx, sy );
            if ( enemy )
                return enemy;
        }
    }
    else if ( ydir < 0 )
    {
        for ( i=y-1; i>=y-range; i-- )
        {
            sx = x;

            if ( i < 0 )
                sy = range + i;
            else
                sy = i;

            enemy = testEnemy ( sx, sy );
            if ( enemy )
                return enemy;
        }
    }

    return NULL;
}

Character * SeesEnemy::testEnemy ( int x, int y )
{
    Character * enemy = NULL;

    WorldPos * wpos;
    WorldObj * obj;
    World2D  * world = m_owner->getWorld();
    
    wpos = world->getPos ( x, y );

    obj  = wpos->getObj();

    if ( obj )
    {
        Character * other = (Character *)obj;

        if ( other )
        {
            if ( m_owner->isEnemy ( other ) )
            {
                enemy = other;
            }
        }
    }

    return enemy;
}