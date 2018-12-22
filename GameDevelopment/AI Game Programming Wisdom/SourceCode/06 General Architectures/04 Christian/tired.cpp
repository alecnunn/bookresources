
#include "character.h"
#include "Tired.h"
#include "gametime.h"
#include "util.h"

Tired::Tired()
{
    m_name      = "Tired";
    m_owner     = NULL;
}

void Tired::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool Tired::update () 
{
    if ( m_owner->getEnergy() <= 0.0f )
        return true;
    else
        return false;
}


IEOwner * Tired::getOwner ()
{
    return m_owner;
}

const char * Tired::getName()
{
    return m_name.c_str();
}

