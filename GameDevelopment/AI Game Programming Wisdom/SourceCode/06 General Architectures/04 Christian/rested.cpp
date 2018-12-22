
#include "character.h"
#include "Rested.h"
#include "gametime.h"
#include "util.h"

Rested::Rested()
{
    m_name      = "Rested";
    m_owner     = NULL;
}

void Rested::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool Rested::update () 
{
    if ( m_owner->getEnergy() == m_owner->getMaxEnergy() )
        return true;
    else
        return false;
}


IEOwner * Rested::getOwner ()
{
    return m_owner;
}

const char * Rested::getName()
{
    return m_name.c_str();
}

