#include "ie.h"

#include "dog.h"
#include "util.h"

#include <math.h>

Dog::Dog ( char * name )
{
    setName ( name );
    registerAnims ();
    makeBrain     ();

    m_alignment   = A_Dogs;
    m_sightRange  = 30;
}

void Dog::registerAnims ()
{
    m_anims[ CA_LieDown ] = new Anim ( 1.0f );
    m_anims[ CA_Yawn    ] = new Anim ( 1.0f );
    m_anims[ CA_Rest    ] = new Anim ( 2.0f, true );
    m_anims[ CA_GetUp   ] = new Anim ( 1.0f );
    m_anims[ CA_Stretch ] = new Anim ( 0.5f );
    m_anims[ CA_Walk    ] = new Anim ( 1.0f, true );
    m_anims[ CA_Run     ] = new Anim ( 1.0f, true );
    m_anims[ CA_Fight   ] = new Anim ( 1.0f, true );
    m_anims[ CA_Die     ] = new Anim ( 2.0f );
};


const char * Dog::getAttack ( int & damage, float & prepareTime )
{
	const int numAttacks = 4;

	static const char * attacks[numAttacks] = {"bites", "snaps at", "barks at", "paws"};
	static int          damagel[numAttacks] = {20, 8, 0, 4};
    static float        ptime  [numAttacks] = {1.0f, 0.5f, 0.2f, 0.4f};

	float r = frand();

	int idx = (int)floor(r * (float)numAttacks);

	damage      = damagel[ idx ];
    prepareTime = ptime[ idx ];

	return attacks[ idx ];

}

const char * Dog::getDamageReact( float & rtime )
{
	const int numReact = 3;

	static const char * desc[ numReact ] =  {"howls", "yelps", "whimpers"};
    static float        time[ numReact ] =  {0.5f, 0.3f, 0.2f};

	float r = frand();

	int idx = (int)floor(r * (float)numReact);

    rtime = time[ idx ];

	return desc[idx];
}

IE_START(Dog)

    GOAL (Wander)

        IF (Tired)      GOSUB (Nap)
        IF (SeesEnemy)  GOTO  (Chase)

    GOAL (Nap)

        IF (Rested)     RETURN

    GOAL (Chase)
        
        IF (Caught)     GOTO  (Fight)

    GOAL (Fight)

		IF (Dying)      GOTO  (Die)
		IF (Damaged)    GOSUB (DamageReact)
		IF (WonFight)   GOTO  (Wander)

	GOAL (DamageReact)

		IF (Done)		RETURN

	GOAL (Die)

IE_END
