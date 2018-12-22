/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "frame.h"

#define _HEAPNUM 1	// Arbitrary. We'll use the upper heap (1).

static void *_pObject1;	// Where our CopCar object will be loaded.
static void *_pObject2;	// Where our RobberCar object will be loaded.

void* LoadMyObject(char*);

// Loads the CopCar and RobberCar objects from disk into _HEAPNUM.
// Returns 0 if successful, or 1 if not successful.
int LoadCarObjects( void ) {
	Frame_t Frame;

	// Get a frame handle:
	Frame = GetFrame( _HEAPNUM );

	// Attempt to load the CopCar object:
	_pObject1 = LoadMyObject( "CopCar" );
	if( _pObject1 == 0 ) {
		// Object couldn't be loaded. Release memory:
		ReleaseFrame( Frame );
		return 1;
	}

	// Attempt to load the RobberCar object:
	_pObject2 = LoadMyObject( "RobberCar" );
	if( _pObject2 == 0 ) {
		// Object couldn't be loaded. Release memory:
		ReleaseFrame( Frame );
		return 1;
	}

	// Objects loaded ok. Keep the memory around:
	return 0;
}

// Allocates memory from _HEAPNUM and loads the specified object
// from disk into the allocated memory. Returns a pointer to the
// object if successful, or 0 if not successful.
void *LoadMyObject( const char *pszObjectName ) {
	int nObjectSize;
	void *pObject;

	nObjectSize = GetObjectSize( pszObjectName );
	if( nObjectSize == 0 ) {
		// Trouble getting object size:
		return 0;
	}

	pObject = AllocFrameMemory( nObjectSize, _HEAPNUM );
	if( pObject == 0 ) {
		// Insufficient memory:
		return 0;
	}

	if( LoadFromDisk( pszObjectName, pObject ) ) {
		// Trouble loading object from disk:
		return 0;
	}

	// Object loaded successfully:
	return pObject;
}
