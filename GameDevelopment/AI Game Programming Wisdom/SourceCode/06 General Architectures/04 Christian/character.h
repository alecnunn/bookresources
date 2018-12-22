
#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "ieowner.h"
#include "world2d.h"
#include "characterbrainlib.h"
#include "anim.h"

enum CA_Ids
{
    CA_LieDown,
    CA_Yawn,
    CA_Rest,
    CA_GetUp,
    CA_Stretch,
    CA_Walk,
    CA_Run,
    CA_Fight,
	CA_Die,

    CA_NUM      // keep last
};

enum Alignments
{
    A_Dogs,
    A_Cats
};

//
// character class, contains common AI executors
// for characters
//
class Character : public IEOwner, public WorldObj
{

public:

    Character();

    virtual void  registerAnims () = 0;
    virtual void  makeBrain () = 0;

	virtual const char * getAttack            ( int & damage, float & prepareTime );
	virtual const char * getDamageReact       ( float & time );

    void  addRest       ( float rest );
    bool  rested        ();
    void  animStart     ( int id );
    bool  animDone      ();
    void  update        ();
    void  updateAnim    ();
    void  initEnergy    ( float burnRate, float recoveRate );
    void  addEnergy     ( float energy );
    void  burnEnergy    ();
    void  restoreEnergy ();
    float getMaxEnergy  ();
    float getEnergy     ();
    void  setSpeeds     ( float walkSpeed, float runSpeed );
    float getWalkSpeed  ();
    float getRunSpeed   ();
    bool  isEnemy       ( Character * other );
    int   getAlignment  () { return m_alignment; }
    void  setSightRange ( int range ) { m_sightRange = range; }
    int   getSightRange () { return m_sightRange; }
    void  setEnemy      ( Character * other ) { m_enemy = other; }

    Character * getEnemy () { return m_enemy; }

    void  alertChase     ( Character * other );
    void  alertFight     ( Character * other );

    bool  beingChased    () { return m_beingChased; }
    bool  fighting       () { return m_fighting; }
	int   getHealth      () { return m_health; }
	void  setDone        ( bool done ) { m_done = done; }
	bool  getDone        () { return m_done; }
	void  setDamage      ( int damage ) { m_health -= damage; }

protected:

    float     m_energy;
    float     m_energyBurnRate;
    float     m_energyRecoveryRate;
    float     m_maxEnergy;
    float     m_walkSpeed;
    float     m_runSpeed;

    int       m_alignment;
    int       m_sightRange;
	int       m_health;

	bool      m_done;  // generic done

    Character * m_enemy;

    Anim  * m_anims [ CA_NUM ];
    int     m_curAnim;

    bool    m_beingChased;
    bool    m_fighting;
};

#endif