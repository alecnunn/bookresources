

#include "character.h"
#include "gametime.h"

Character::Character()
{
	m_energy             = 100.0f;
    m_maxEnergy          = 100.0f;
    m_energyBurnRate     = 1.0f;  // per second
    m_energyRecoveryRate = 10.0f; // per second
    m_walkSpeed          = 1.0f;  // meters per second
    m_runSpeed           = 5.0f;  // meters per second
    m_enemy              = NULL;  // current enemy focused on
    m_sightRange         = 10;    // meters
	m_health             = 100;
	m_done				 = false; // generic done flag

    m_curAnim = -1;

    for ( int i=0; i<CA_NUM; i++ )
    {
        m_anims[i] = NULL;
    }

    m_beingChased = false;
}

void Character::animStart ( int animId )
{
    m_curAnim = animId;
}

bool Character::animDone ()
{
    if ( m_curAnim >= 0 && m_anims[m_curAnim] )
    {
        return m_anims[m_curAnim]->done();
    }

    return true;
}

void Character::addRest ( float rest )
{
    m_energy += rest;

    if ( m_energy >= m_maxEnergy )
        m_energy = m_maxEnergy;
}

bool Character::rested ()
{
    if ( m_energy == m_maxEnergy )
        return true;
    else
        return false;
}

void Character::update ()
{
    think();

    updateAnim();
}

void Character::updateAnim()
{
    if ( m_curAnim > -1 )
    {
        m_anims[m_curAnim]->update();
    }
}

void Character::initEnergy ( float burnRate, float recoveRate )
{
    m_energy = m_maxEnergy;
    m_energyBurnRate = burnRate;
    m_energyRecoveryRate = recoveRate;
}

void Character::addEnergy ( float energy )
{
    m_energy += energy;
}

void Character::burnEnergy ()
{
    m_energy -= (GameTime::dt * m_energyBurnRate);

    if ( m_energy < 0.0f )
        m_energy = 0.0f;
}

void Character::restoreEnergy ()
{
    m_energy += (GameTime::dt * m_energyRecoveryRate);

    if ( m_energy > m_maxEnergy )
        m_energy = m_maxEnergy;
}

float Character::getMaxEnergy ()
{
    return m_maxEnergy;
}

float Character::getEnergy()
{
    return m_energy;
}

void Character::setSpeeds ( float walkSpeed, float runSpeed )
{
    m_walkSpeed = walkSpeed;
    m_runSpeed  = runSpeed;
}

float Character::getWalkSpeed ()
{
    return m_walkSpeed;
}

float Character::getRunSpeed ()
{
    return m_runSpeed;
}

bool Character::isEnemy ( Character * other )
{
    if ( m_alignment == other->getAlignment() )
        return false;
    else
        return true;
}

void Character::alertChase ( Character * other )
{
    m_beingChased = true;
    m_enemy = other;
}

void Character::alertFight ( Character * other )
{
    m_fighting = true;
    m_enemy = other;
    m_beingChased = false;
}

const char * Character::getAttack ( int & damage, float & prepareTime )
{
	damage = 3;
    prepareTime = 0.5f;
	return "attacks";
}

const char * Character::getDamageReact ( float & time)
{
    time = 0.3f;
	return "cries out";
}