
#include "character.h"
#include "Fight.h"
#include "gametime.h"
#include "util.h"

Fight::Fight()
{
    m_name      = "Fight"; // must match class name
    m_owner     = NULL;
}

void Fight::init   ( IEOwner * owner )
{
    m_owner = dynamic_cast<Character *> (owner);
}

bool Fight::start ()
{
    m_state = Go;

    m_owner->animStart ( CA_Run );

    m_enemy = m_owner->getEnemy();

    m_enemy->alertFight ( m_owner );

    m_owner->setMotionSpeed ( m_owner->getRunSpeed() );

    m_attackClock       = 0.0f;
    m_justAttackedDelay = 0.25f;

	m_attackDesc = m_owner->getAttack( m_damage, m_attackTime );

    m_attackTime += getEnergyFact();

    return false;
}

bool Fight::update () 
{
    switch ( m_state )
    {

    case Go:

        m_attackClock += GameTime::dt;

        if ( m_attackClock >= m_attackTime )
        {

            printf ( "%s %s %s\n", m_owner->getName(), m_attackDesc, m_enemy->getName() );
            m_attackClock = 0.0f;

			m_enemy->setDamage ( m_damage );

            m_attackDesc = m_owner->getAttack ( m_damage, m_attackTime );

            // since just attacked, add extra to give opponent time to respond
            m_attackTime += m_justAttackedDelay;

            // factor in energy
            m_attackTime += getEnergyFact();
        }

        m_owner->burnEnergy();

        break;

    }

    return true;
}

float Fight::getEnergyFact ()
{
    float energy = m_owner->getEnergy();

    const float maxFact = 0.5f;

    float factor;
    
    if ( energy == 0.0f )
        factor = maxFact;
    else
        factor = maxFact - ((maxFact * energy) / 100.0f);

    return factor;
}

bool Fight::finish ()
{
    return true;
}

IEOwner * Fight::getOwner ()
{
    return m_owner;
}

const char * Fight::getName()
{
    return m_name.c_str();
}

