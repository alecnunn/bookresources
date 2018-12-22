
#include "world2d.h"
#include "gametime.h"

#include <assert.h>

WorldObj::WorldObj()
{
    m_xdir = 0;
    m_ydir = 0;
}

void WorldObj::setWorld ( World2D * world, char * name )
{
    m_name  = name;
    m_world = world;
}

// setPos -- sets the world object x,y
void WorldObj::setPos ( int x, int y )
{
    m_pos = m_world->setPos ( x, y, this );   

    m_xdist = 0.5f;
    m_ydist = 0.5f;
}

// getPos -- gets the xy of the object
void WorldObj::getPos ( int & x, int & y )
{
    x = m_pos->m_x;
    y = m_pos->m_y;
}

const char * WorldObj::getWOName ()
{
    return m_name.c_str();
}

void WorldObj::move ()
{
    float xdist = (m_speed * GameTime::dt * m_xdir);
    float ydist = (m_speed * GameTime::dt * m_ydir);

    m_xdist += xdist;
    m_ydist += ydist;
    
    // see if new world coordinates
    int new_x = m_pos->m_x;
    int new_y = m_pos->m_y;

    m_lastx = new_x;
    m_lasty = new_y;

    if ( m_xdist < 0.0f )
    {        
        int xdist = 1 + (int)abs(m_xdist);

        new_x -= xdist;
        m_xdist = 1.0f + (m_xdist + (float)(xdist-1));

    }
    else if ( m_xdist >= 1.0f )
    {
        int xdist = 1 + (int)m_xdist;
        new_x += xdist;
        m_xdist = 0.0f + (m_xdist - (float)(xdist-1));
    }

    if ( m_ydist < 0.0f )
    {
        int ydist = 1 + (int)abs(m_ydist);
        new_y -= ydist;
        m_ydist = 1.0f + (m_ydist + (float)(ydist-1));
    }
    else if ( m_ydist >= 1.0f )
    {
        int ydist = 1 + (int)m_ydist;
        new_y += ydist;
        m_ydist = 0.0f + (m_ydist - (float)(ydist-1));
    }

    if ( new_x < 0 )
        new_x = m_world->m_width + new_x;
    else if ( new_x >= m_world->m_width )
        new_x = 0 + (new_x - m_world->m_width);

    if ( new_y < 0 )
        new_y = m_world->m_length + new_y;
    else if ( new_y >= m_world->m_length )
        new_y = 0 + (new_y - m_world->m_length );

    // extra clamp in case time is really great
    // like in the case of debugging
    if ( new_y < 0 ) new_y = 0;
    else if ( new_y >= m_world->m_length ) new_y = m_world->m_length - 1;
    if ( new_x < 0 ) new_x = 0;
    else if ( new_x >= m_world->m_width ) new_x = m_world->m_width - 1;

    // if new xy position
    if ( new_x != m_pos->m_x || new_y != m_pos->m_y )
    {
        // null out old position
        m_pos->setObject ( NULL );

        // set new position
        m_pos = m_world->setPos ( new_x, new_y, this );
    }
}

bool WorldObj::inNewPos ()
{
    if ( m_lastx != m_pos->m_x || m_lasty != m_pos->m_y )
        return true;
    else
        return false;
}

WorldPos::WorldPos()
{
    m_id   = -1;
    m_obj  = NULL;
}

void WorldPos::setObject  ( WorldObj * obj )
{
    m_obj = obj;
}

WorldObj * WorldPos::getObj ()
{
    return m_obj;
}

World2D::World2D( int width, int length, float tileDist )
{
    m_width  = width;
    m_length = length;
    m_tileDist = tileDist;

    m_posArray = (WorldPos **)malloc ( sizeof ( WorldPos * ) * length * width );

    int i, num = m_length * m_width;

    int x = 0;
    int y = 0;

    for ( i=0; i<num; i++ )
    {
        m_posArray[i] = new WorldPos;

        m_posArray[i]->m_id   = i;
        m_posArray[i]->m_obj  = NULL;


        m_posArray[i]->m_x = x;
        m_posArray[i]->m_y = y;

        ++x;
        if ( x >= m_width )
        {
            x = 0;
            ++y;
        }
    }
}

World2D::~World2D()
{
    int i, num = m_length * m_width;

    for ( i=0; i<num; i++ )
    {
        delete m_posArray[i];
    }

    free ( m_posArray );
}

WorldPos * World2D::setPos ( int x, int y, WorldObj * obj )
{
    // get the pos
    WorldPos * pos = getPos ( x, y );
    
    pos->setObject ( obj );

    return pos;
}

WorldPos * World2D::getPos ( int x, int y )
{
    assert ( x >= 0 && x <= m_width );
    assert ( y >= 0 && y <= m_length );

    int idx = y * m_width + x;

    return m_posArray[idx];
}
