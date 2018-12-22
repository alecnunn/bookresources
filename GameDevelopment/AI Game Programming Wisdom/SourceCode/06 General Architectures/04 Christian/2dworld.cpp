
#include "2dworld.h"


WorldObj::WorldObj ( 2DWorld * world, char * name )
{
    m_name  = name;
    m_world = world;
}

void WorldObj::setPos ( int x, int y )
{
    m_world->setPos ( x, y, this );   
}
