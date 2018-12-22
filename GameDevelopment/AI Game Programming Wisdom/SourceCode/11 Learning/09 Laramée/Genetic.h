
#ifndef GENETIC_H
#define GENETIC_H

#include "Chromosome.h"
#include "Globals.h"
#include "Simulation.h"

/****************************************************************
 * STRUCT Individual
 * A helper for the genetic algorithm, this struct contains an
 * individual strand of DNA and information about its performance 
 * in the field
 ***************************************************************/

struct Individual
{
	// Genetic material
	Chromosome DNA;

	// Performance score
	double Performance;

	// And some stats compiled during simulation; we use them to
	// present a more complete picture of the results
	int StatsKnightsKilled;
	int StatsSheepEaten;
	int StatsDamageTaken;
	int StatsTimeAlive;
	int StatsTimeCaptive;
};


/****************************************************************
 * CLASS Genetic
 * The genetic algorithm controller itself
 ***************************************************************/

class Genetic
{
	// The population we are working on
	Individual Population[ GA_POPULATION_SIZE ];

	// The simulation process we are submitting individuals to
	Simulation theSim;

public:
	// Construction
	Genetic() {}

	// The big mothership method for the whole evolutionary process
	void RunEvolution();

private:
	// Making up new individuals from scratch
	void CreateInitialPopulation();
	void MakeRandomIndividual( int which );

	// Testing the whole population against the test cases
	void RunGeneration();

	// Creating a new generation from the best individuals of
	// the current one
	void BegetNextGeneration();

	// Presenting the results to the user
	void ReportGenerationResults( int i );

	// The genetic operators
	void Crossover( Chromosome & c1, Chromosome & c2 );
	void Mutation( Chromosome & c );
};


#endif