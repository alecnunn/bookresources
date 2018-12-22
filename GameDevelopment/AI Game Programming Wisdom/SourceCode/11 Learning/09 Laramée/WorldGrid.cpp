/****************************************************************
 * WorldGrid Implementation
 ***************************************************************/

#include "WorldGrid.h"
#include "Globals.h"
#include <iostream.h>
#include <math.h>


// Construction
WorldGrid::WorldGrid()
{
	for( int i = 0; i < MAX_ENTITIES; i++ )
	{
		Registry[ i ].type = ENTITY_NULL;
	}
}

WorldGrid::WorldGrid( int x, int y )
{
	TrollX = x;
	TrollY = y;
	for( int i = 0; i < MAX_ENTITIES; i++ )
	{
		Registry[ i ].type = ENTITY_NULL;
	}
}

void WorldGrid::Copy( WorldGrid * src )
{
	for( int i = 0; i < MAX_ENTITIES; i++ )
	{
		Registry[ i ].type = src->Registry[ i ].type;
		Registry[ i ].x = src->Registry[ i ].x;
		Registry[ i ].y = src->Registry[ i ].y;
	}
}


// Check whether the troll is close enough to trigger activity
bool WorldGrid::IsTrollWithinRange( int id )
{
	int manhattan = DistanceFromTroll( id );
	switch( Registry[ id ].type )
	{
		case ENTITY_SHEEP:
			return ( manhattan <= 2 ) ? true : false; break;
		case ENTITY_TOWER:
			return ( manhattan <= 4 ) ? true : false; break;
		case ENTITY_KNIGHT:
			return ( manhattan <= 3 ) ? true : false; break;
		case ENTITY_HAVEN:
		case ENTITY_TRAP:
			return ( manhattan == 0 ) ? true : false; break;
		default:
			return false; break;
	}
}


// Movement on the game world grid
void WorldGrid::MoveTroll( int newx, int newy )
{
	TrollX = newx;
	TrollY = newy;
}

void WorldGrid::MoveEntity( int registryIndex, int newX, int newY )
{
	Registry[ registryIndex ].x = newX;
	Registry[ registryIndex ].y = newY;
}


// Add an entity to the registry
void WorldGrid::Register( int ID, int type, int x, int y )
{
	Registry[ ID ].type = type;
	Registry[ ID ].x = x;
	Registry[ ID ].y = y;
}


// Print the contents of the registry
void WorldGrid::Dump()
{
	cout << "DUMP OF WORLDGRID:" << endl;
	for( int i = 0; i < MAX_ENTITIES; i++ )
	{
		if( Registry[ i ].type != ENTITY_NULL )
		{
			cout << "ENTITY #" << i << " of type " << Registry[ i ].type << " at "
				   << Registry[ i ].x << "," << Registry[ i ].y << endl;
		}
	}
}


// Look for the entity of class "type" closest to the troll
int WorldGrid::FindClosestFromTroll( int type )
{
	int choice = -1;
	int bestSoFar = 1000;

	for( int i = 0; i < MAX_ENTITIES; i++ )
	{
		if ( Registry[ i ].type == type && DistanceFromTroll( i ) < bestSoFar )
		{
			choice = i;
			bestSoFar = DistanceFromTroll( i );
		}
	}

	return choice;
}


// Count objects of a certain kind close to the troll
int WorldGrid::HowManyCloseToTroll( int type, int maxdist )
{
	int howMany = 0;
	for( int i = 0; i < MAX_ENTITIES; i++ )
	{
		if ( Registry[ i ].type == type && DistanceFromTroll( i ) <= maxdist )
			howMany++;
	}
	return howMany;
}