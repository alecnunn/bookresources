#include "ie.h"

#include "Cat.h"
#include "util.h"

#include <math.h>

Cat::Cat ( char * name )
{
    setName ( name );
    registerAnims ();
    makeBrain     ();

    m_alignment  = A_Cats;
    m_sightRange = 15;

}

void Cat::registerAnims ()
{
    m_anims[ CA_LieDown ] = new Anim ( 1.0f );
    m_anims[ CA_Yawn    ] = new Anim ( 1.0f );
    m_anims[ CA_Rest    ] = new Anim ( 2.0f, true );
    m_anims[ CA_GetUp   ] = new Anim ( 1.0f );
    m_anims[ CA_Stretch ] = new Anim ( 0.5f );
    m_anims[ CA_Walk    ] = new Anim ( 1.0f, true );
    m_anims[ CA_Run     ] = new Anim ( 1.0f, true );
    m_anims[ CA_Fight   ] = new Anim ( 1.0f, true );
    m_anims[ CA_Die     ] = new Anim ( 1.5f );
};

const char * Cat::getAttack ( int & damage, float & prepareTime )
{
	const int numAttacks = 4;

	static const char * attacks[numAttacks] = {"bites", "scratchs", "snarls at", "swipes"};
	static int          damagel[numAttacks] = {15, 10, 0, 5};
    static float        ptime  [numAttacks] = {0.9f, 0.6f, 0.1f, 0.3f};

	float r = frand();

	int idx = (int)floor(r * (float)numAttacks);

	damage      = damagel[ idx ];
    prepareTime = ptime[ idx ];

	return attacks[ idx ];

}

const char * Cat::getDamageReact ( float & rtime )
{
	const int numReact = 3;

	static const char * desc[ numReact ] = {"screeches", "yowls", "jumps"};
    static float        time[ numReact ] = {0.2f, 0.5f, 0.6f};

	float r = frand();

	int idx = (int)floor(r * (float)numReact);

    rtime = time[ idx ];

	return desc[idx];
}


IE_START(Cat)

    GOAL (Wander)

        IF (Tired)      GOSUB (Nap)
        IF (Chased)     GOTO  (Flee)

    GOAL (Nap)

        IF (Rested)     RETURN

    GOAL (Flee)
        IF (Caught)     GOTO (Fight)

    GOAL (Fight)

		IF (Dying)      GOTO  (Die)
		IF (Damaged)    GOSUB (DamageReact)
		IF (WonFight)   GOTO  (Wander)

	GOAL (DamageReact)

		IF (Done)		RETURN

	GOAL (Die)

IE_END
