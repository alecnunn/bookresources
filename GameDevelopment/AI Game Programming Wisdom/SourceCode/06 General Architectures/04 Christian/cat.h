

#ifndef _Cat_H_
#define _Cat_H_

#include "character.h"

class Cat : public Character
{
public:
    Cat ( char * name );

    void registerAnims ();
    void makeBrain     ();

	const char * getAttack      ( int & damage, float & prepareTime );
	const char * getDamageReact ( float & time);

};

#endif