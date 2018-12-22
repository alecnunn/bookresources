
#include "character.h"
#include "Chased.h"
#include "gametime.h"
#include "util.h"

Chased::Chased()
{
    m_name      = "Chased";
    m_owner     = NULL;
}

void Chased::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}


bool Chased::update () 
{
    return m_owner->beingChased();
}


IEOwner * Chased::getOwner ()
{
    return m_owner;
}

const char * Chased::getName()
{
    return m_name.c_str();
}

