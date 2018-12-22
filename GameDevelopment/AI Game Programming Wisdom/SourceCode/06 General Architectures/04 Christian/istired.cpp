
#include "character.h"
#include "IsTired.h"
#include "gametime.h"
#include "util.h"

IsTired::IsTired()
{
    m_name      = "IsTired";
    m_owner     = NULL;
}

void IsTired::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);

    m_fatigueMax = 5;
}


bool IsTired::update () 
{
    if ( m_owner->getFatigue() >= m_fatigueMax )
        return true;
    else
        return false;
}


IEOwner * IsTired::getOwner ()
{
    return m_owner;
}

const char * IsTired::getName()
{
    return m_name.c_str();
}

