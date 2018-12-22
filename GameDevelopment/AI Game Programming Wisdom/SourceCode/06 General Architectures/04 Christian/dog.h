

#ifndef _DOG_H_
#define _DOG_H_

#include "character.h"

class Dog : public Character
{
public:
    Dog ( char * name );

    void registerAnims ();
    void makeBrain     ();

	const char * getAttack      ( int & damage, float & prepareTime );
	const char * getDamageReact ( float & time );
};

#endif