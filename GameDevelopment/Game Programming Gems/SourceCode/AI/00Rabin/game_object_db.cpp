/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
//////////////////////////////////////////////////////////////
//
// Filename: game_object_db.cpp
//
// Author: Steve Rabin
// E-mail: stevera@noa.nintendo.com
// From the book "Game Programming Gems"
// From the article "Designing a General Robust AI Engine"
//
// Brief Disclaimer:
// This code is free to use for commercial use and is in the
// public domain. You may distribute, copy, modify, or use as
// is as long as this information is present. This code makes
// no guarantees written or implied and is provided solely as
// an example. Have a nice day!
//
// Purpose:
// This file contains the code to deal with the game object
// database.
//
//////////////////////////////////////////////////////////////

#include "game_object_db.h"
#include "game_object.h"
#include "malloc.h"
#include "hud.h"
#include <string.h>


typedef struct
{
	GameObject* head;

} GODB;

// INTERNAL LOCAL VARIABLES
GODB masterGODB;
GameObject* GODBiterator;
unsigned int nextGUID = 1;

// INTERNAL HELPER FUNCTIONS
void GODBPushBack( GameObject* go );
void GODBDestroyMarkedForDeletion( void );




//Every frame this function gets called to
//update all of the game object logic
void GODBUpdate( void )
{
	GameObject* curGO = masterGODB.head;
	while( curGO != 0 )
	{
		GOUpdate( curGO );
		curGO = curGO->goNext;
	}

	GODBDestroyMarkedForDeletion();

}

//Draw all game objects
void GODBDraw( void )
{
	GameObject* curGO = masterGODB.head;
	while( curGO != 0 )
	{
		GODraw( curGO );
		curGO = curGO->goNext;
	}

}


//Initializes the Game Object Database
void GODBInit( void )
{
	masterGODB.head = 0;
	GODBiterator = 0;

}

//Creates and inserts a new game object into the database
//Returns a pointer to the newly created object
GameObject* GODBCreateAndReturnGO( char* name )
{
	GameObject* newGO = (GameObject*) malloc( sizeof( GameObject ) );
	GOInitialize( newGO, nextGUID );
	nextGUID++;

	GODBPushBack( newGO );
	strcpy( newGO->szName, name );

	return( newGO );

}

//Helper function for inserting a game object into the database
void GODBPushBack( GameObject* go )
{
	GameObject* curGO = masterGODB.head;

	if( curGO != 0 )
	{
		while( curGO->goNext != 0 ) {
			curGO = curGO->goNext;
		}

		curGO->goNext = go;
	}
	else {
		masterGODB.head = go;
	}


}


//Allows others to get the head of the database
GameObject* GODBGetHead( void )
{
	return( masterGODB.head );

}

//Returns an object pointer from the database using a game object unique id (guid)
GameObject* GODBGetGO( unsigned int unique_id )
{
	if( unique_id != 0 )
	{
		GameObject* curGO = masterGODB.head;

		while( curGO != 0 )
		{
			if( curGO->unique_id == unique_id ) {
				return( curGO );
			}

			curGO = curGO->goNext;
		}
	}

	return( 0 );
}

//Returns an object pointer from the database using a string name
GameObject* GODBGetGOFromName( char* name )
{
	GameObject* curGO = masterGODB.head;

	while( curGO != 0 )
	{
		if( strcmp( curGO->szName, name ) == 0 ) {
			return( curGO );
		}

		curGO = curGO->goNext;
	}

	return( 0 );

}

//Removes all game objects from the database that have been marked for deletion
//(When this is called, it is assumed that its a safe time to delete objects)
void GODBDestroyMarkedForDeletion( void )
{
	GameObject* curGO = masterGODB.head;
	GameObject* lastGO = 0;

	while( curGO != 0 )
	{
		if( curGO->bMarkedForDeletion ) {
			if( lastGO != 0 ) {
				lastGO->goNext = curGO->goNext;
				free( curGO );
				curGO = lastGO->goNext;
			}
			else {
				masterGODB.head = curGO->goNext;
				free( curGO );
				curGO = masterGODB.head;
			}
			
		}
		else if( curGO != 0 ) {
			lastGO = curGO;
			curGO = curGO->goNext;
		}
	}

}




void GODBOutputStateInfoToHUD( void )
{
	char output[256];
	char state_name[256];
	GameObject* curGO = masterGODB.head;
	while( curGO != 0 )
	{
		TranslateStateName( curGO->state_machine_id, curGO->state, state_name );
		sprintf( output, "Name: %s    State: %s\n", curGO->szName, state_name  );
		HUDPrintToScreen( output );
		curGO = curGO->goNext;
	}
}







