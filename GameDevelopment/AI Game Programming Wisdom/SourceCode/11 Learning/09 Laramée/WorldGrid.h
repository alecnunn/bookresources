/***************************************************************
 * Class WORLDGRID
 * A representation of the world as a wrapping grid of squares
 * that can be occupied by Entities.  Could easily be implemented
 * as a Singleton (cf: Design Patterns by Gamma et al.)
 **************************************************************/

#ifndef WORLDGRID_H
#define WORLDGRID_H

#include "Globals.h"
#include <stdlib.h>


class WorldGrid
{
	// We store the troll's position 
	// explicitly because many entities' behavior depends on it
	int TrollX, TrollY;

	// A registry of world entities by location
	struct WGRegistryEntry
	{
		int type;
		int x;
		int y;
	};

	WGRegistryEntry Registry[ MAX_ENTITIES ];

public:
	// Construction and access
	WorldGrid();
	WorldGrid( int x, int y );
	void Copy( WorldGrid * src );
	void Register( int id, int type, int x, int y );
	void UnRegister( int id ) { Registry[ id ].type = ENTITY_NULL; }
	int GetEntityX( int id ) { return Registry[ id ].x; }
	int GetEntityY( int id ) { return Registry[ id ].y; }
	int GetTrollX() { return TrollX; }
	int GetTrollY() { return TrollY; }

	// Keep track of movement
	void MoveTroll( int newx, int newy );
	void MoveEntity( int id, int newx, int newy );

	// Check whether the troll is close enough to trigger some activity
	// in the object associated with registry entry "id"
	bool IsTrollWithinRange( int id );

	// Manhattan distance relationships between troll and entities
	int DistanceFromTroll( int id ) 
		{ return( abs( Registry[ id ].x - TrollX ) + abs( Registry[ id ].y - TrollY ) ); }

	// Look for the object of class "type" closest to the troll
	int FindClosestFromTroll( int type );

	// Look for objects of a certain class within a certain distance of the troll
	int HowManyCloseToTroll( int type, int maxdist );

	// Debugging help
	void Dump();
};


#endif