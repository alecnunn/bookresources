

#ifndef _Fight_H_
#define _Fight_H_

#include "ieexec.h"

class Character;

class Fight : public IEExec
{

public:

    Fight();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum FightStates
    {
        Go
    };

    void newDirection ();

private:
    std::string m_name;
    Character * m_owner;

    Character * m_enemy;

    float m_attackTime;
    float m_attackClock;
    float m_justAttackedDelay;

    int   m_damage;

    const char * m_attackDesc;

    float getEnergyFact ();
};

#endif
