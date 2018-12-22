/***************************************************************
 * Implementation of the Simulation Class
 *************************************************************/

#include "Simulation.h"
#include <stdlib.h>
#include "WorldGrid.h"
#include <iostream.h>

Simulation::Simulation( int x, int y )
{
	for( int i = 0; i < 3; i++ )
	{
		Grids[ i ] = new WorldGrid( x, y );
	}
}

Simulation::Simulation()
{
	for( int i = 0; i < 3; i++ )
	{
		Grids[ i ] = new WorldGrid();
	}
}

/****************************************************************
 * Test case construction
 ****************************************************************/

// We'll test each troll against three different test cases, each
// of which has a unique combination of sheep population, number
// of prowling knights, etc.

void Simulation::BuildTestCases()
{
	int i;
	int x, y;

	// Let's start with a sheep-heavy case
	Entity::AttachGrid( *Grids[ 0 ] );
	for( i = 0; i < 60; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 0 ][ i ] = new Sheep( x, y, i );
	}
	for( i = 60; i < 75; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 0 ][ i ] = new Knight( x, y, i );
	}
	for( i = 75; i < 90; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 0 ][ i ] = new Haven( x, y, i );
	}
	for( i = 90; i < 95; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 0 ][ i ] = new Trap( x, y, i );
	}
	for( i = 95; i < MAX_ENTITIES; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 0 ][ i ] = new Tower( x, y, i );
	}

	// Now, a knight-heavy one
	Entity::AttachGrid( *Grids[ 1 ] );
	for( i = 0; i < 30; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 1 ][ i ] = new Sheep( x, y, i );
	}
	for( i = 30; i < 75; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 1 ][ i ] = new Knight( x, y, i );
	}
	for( i = 75; i < 90; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 1 ][ i ] = new Haven( x, y, i );
	}
	for( i = 90; i < 95; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 1 ][ i ] = new Trap( x, y, i );
	}
	for( i = 95; i < MAX_ENTITIES; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 1 ][ i ] = new Tower( x, y, i );
	}
	
	// And a relatively sparsely populated one to finish
	Entity::AttachGrid( *Grids[ 2 ] );
	for( i = 0; i < 20; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 2 ][ i ] = new Sheep( x, y, i );
	}
	for( i = 20; i < 35; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 2 ][ i ] = new Knight( x, y, i );
	}
	for( i = 35; i < 45; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 2 ][ i ] = new Trap( x, y, i );
	}
	for( i = 45; i < 50; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 2 ][ i ] = new Tower( x, y, i );
	}
	for( i = 50; i < 55; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 2 ][ i ] = new Tower( x, y, i );
	}	
	for( i = 55; i < MAX_ENTITIES; i++ )
	{
		x = rand() % WORLD_GRID_SIZE;
		y = rand() % WORLD_GRID_SIZE;
		EntityTable[ 2 ][ i ] = new Haven( x, y, i );
	}
}


/****************************************************************
 * Testing a Troll
 ***************************************************************/

double Simulation::RunSim( Troll & troll )
{
	double results[ 3 ];

	// Run the three test cases...
	for( int i = 0; i < 3; i++ )
	{
		// Start each test by making a working copy of the world data
		troll.Reset();
		WorkingGrid.Copy( Grids[ i ] );
		for( int j = 0; j < MAX_ENTITIES; j++ )
		{
		  if( EntityTable[ i ][ j ] != NULL )
				WorkingTable[ j ] = EntityTable[ i ][ j ]->Clone();
			else
				WorkingTable[ j ] = NULL;
		}

		Entity::AttachGrid( WorkingGrid );
		Entity::AttachTroll( troll );

		// Run the simulation until the troll dies or time runs out
		int time = 0;
		while( time < 500 )
		{
			// Let each entity (troll included) perform its turn
			for( j = 0; j < MAX_ENTITIES; j++ )
			{
				if ( WorkingTable[ j ] != NULL )
					WorkingTable[ j ]->Update();
			}
			troll.Update();

			// Short-circuit the sim if the troll dies
			if( troll.Dead() )
				break;

			time++;
		}


		// Obtain the results
		results[ i ] = troll.GetEvaluation();

		// And clean up
		for( j = 0; j < MAX_ENTITIES; j++ )
		{
		  if( WorkingTable[ j ] != NULL )
				delete( WorkingTable[ j ] );
		}
	}

	// And report the results...
	return( ( results[ 0 ] + results[ 1 ] + results[ 2 ] ) / 3.0 );
}