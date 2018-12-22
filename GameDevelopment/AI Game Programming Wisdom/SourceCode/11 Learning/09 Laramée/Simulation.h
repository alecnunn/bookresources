/****************************************************************
 * CLASS Simulation
 * The object running the process of "testing" a troll against
 * several representative cases
 ****************************************************************/

#ifndef SIMULATION_H
#define SIMULATION_H

#include "WorldGrid.h"
#include "Entities.h"
#include "Troll.h"

class Simulation
{
	// The three test cases
	WorldGrid * Grids[ 3 ];

	// Arrays of pointers to the entities in each grid
	Entity * EntityTable[ 3 ][ MAX_ENTITIES ];

	// Working copies trashed by each troll
	WorldGrid WorkingGrid;
	Entity * WorkingTable[ MAX_ENTITIES ];

public:
	// Construction
	Simulation();
	Simulation( int x, int y );

	// Making test cases for the evolution process
	void BuildTestCases();

	// Running a troll through all test cases
	double RunSim( Troll & troll );
};


#endif