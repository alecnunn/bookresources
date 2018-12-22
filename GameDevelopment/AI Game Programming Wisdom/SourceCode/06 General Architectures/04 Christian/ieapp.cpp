// ieapp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "gametime.h"
#include "dog.h"
#include "cat.h"
#include "world2d.h"


int main(int argc, char* argv[])
{
	printf("\n\n*** Welcome to text town. ***\n\n");

    GameTime gameTime;

	World2D world ( 100, 100, 1.0f  );

    // seed randomness
    srand ( (unsigned)time( NULL ) );

    // Dog 1
    Dog dog1        ( "Butch" );
	dog1.setWorld   ( &world, "Butch" );
    dog1.setPos     ( 50, 50 );
    dog1.initEnergy ( 2.0f, 10.0f );
    dog1.setSpeeds  ( 0.5f, 8.0f );

    // Cat 1
    Cat cat1        ( "Socks" );
    cat1.setWorld   ( &world, "Socks" );
    cat1.setPos     ( 45, 50 );
    cat1.initEnergy ( 1.0f, 15.0f );
    cat1.setSpeeds  ( 1.0f, 4.0f );

    // Dog 2
    Dog dog2        ( "Speck" );
	dog2.setWorld   ( &world, "Speck" );
    dog2.setPos     ( 25, 25 );
    dog2.initEnergy ( 1.5f, 13.0f );
    dog2.setSpeeds  ( 0.8f, 9.0f );

    // Cat 2
    Cat cat2        ( "Fluff" );
    cat2.setWorld   ( &world, "Fluff" );
    cat2.setPos     ( 60, 70 );
    cat2.initEnergy ( 1.2f, 16.0f );
    cat2.setSpeeds  ( 0.5f, 3.5f );

    while ( 1 )
    {
        gameTime.start();

        dog1.update();
        cat1.update();
        dog2.update();
        cat2.update();

        gameTime.end();
    }

	return 0;
}

