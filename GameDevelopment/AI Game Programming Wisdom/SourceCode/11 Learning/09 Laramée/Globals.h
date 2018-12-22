/***************************************************************
 * Some global constants...
 **************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

// the types of entities found in the world
enum 
{ 
	ENTITY_NULL, 
	ENTITY_SHEEP, 
	ENTITY_KNIGHT, 
	ENTITY_TOWER, 
	ENTITY_TRAP, 
	ENTITY_HAVEN, 
	ENTITY_TROLL 
};

// parameters of the game world
enum
{ 
	WORLD_GRID_SIZE = 30,
	FULL_HEALTH = 50,
	REASONABLY_HEALTHY = 35,
	HEAVILY_DAMAGED = 20,
	CRITICALLY_DAMAGED = 10,
	MAX_ENTITIES = 100,
};

// The genes on a chromosome
enum 
{ 
	EATING_PRIORITY, 
	HEALING_PRIORITY,
	KILLING_PRIORITY,
	FLEEING_PRIORITY,
	EXPLORING_PRIORITY,
	ALL_PRIORITIES
};


// The parameters of the Genetic Algorithm
enum
{
	GA_GENERATIONS = 100,
	GA_POPULATION_SIZE = 300,
	MUTATION_RATE = 3
};

#endif
