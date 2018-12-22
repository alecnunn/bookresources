/****************************************************************
 * Implementation of Class Genetic
 ***************************************************************/

#include "Genetic.h"
#include "Simulation.h"
#include "Troll.h"
#include <stdlib.h>
#include <iostream.h>
#include <iomanip.h>


/*****************************************************************
 * Populating the world with trolls...
 ****************************************************************/

void Genetic::MakeRandomIndividual( int which )
{
	Population[ which ].DNA.Priorities[ EATING_PRIORITY ] = (double) ( rand() % 10000 ) / 10000.0;
	Population[ which ].DNA.Priorities[ KILLING_PRIORITY ] = (double) ( rand() % 10000 ) / 10000.0;
	Population[ which ].DNA.Priorities[ FLEEING_PRIORITY ] = (double) ( rand() % 10000 ) / 10000.0;
	Population[ which ].DNA.Priorities[ EXPLORING_PRIORITY ] = (double) ( rand() % 10000 ) / 10000.0;
	Population[ which ].DNA.Priorities[ HEALING_PRIORITY ] = (double) ( rand() % 10000 ) / 10000.0;
}

void Genetic::CreateInitialPopulation()
{
	for( int i = 0; i < GA_POPULATION_SIZE; i++ )
	{
		MakeRandomIndividual( i );
	}
}

/*****************************************************************
 * The evolutionary process
 ****************************************************************/

// The mothership method for the whole process
void Genetic::RunEvolution()
{
	// Set up the gene pool
	CreateInitialPopulation();
	theSim.BuildTestCases();

	// And iterate the testing process over N generations
	for( int i = 0; i < GA_GENERATIONS; i++ )
	{
		RunGeneration();
		ReportGenerationResults( i );
		if( i < GA_GENERATIONS - 1 )
			BegetNextGeneration();
	}
}

// Helper function to classify individuals based on performance
int RankIndividuals( const void * a, const void * b )
{
	Individual * ia = ( Individual * ) a;
	Individual * ib = ( Individual * ) b;

	if( ia->Performance > ib->Performance )
		return -1;
	else
		return 1;
}

// Test an entire population of trolls
void Genetic::RunGeneration()
{
	for( int i = 0; i < GA_POPULATION_SIZE; i++ )
	{
		// Place the troll in the game world
		Troll theTroll( Population[ i ].DNA, rand() % WORLD_GRID_SIZE, rand() % WORLD_GRID_SIZE );
		Entity::AttachTroll( theTroll );

		// Run the simulation with this troll and extract the results
		Population[ i ].Performance = theSim.RunSim( theTroll );
		Population[ i ].StatsSheepEaten = theTroll.StatsSheepEaten;
		Population[ i ].StatsKnightsKilled = theTroll.StatsKnightsKilled;
		Population[ i ].StatsDamageTaken = theTroll.StatsDamageTaken;
		Population[ i ].StatsTimeAlive = theTroll.StatsTimeAlive;
		Population[ i ].StatsTimeCaptive = theTroll.StatsTimeCaptive;
	}

	// And classify the population
	qsort( Population, GA_POPULATION_SIZE, sizeof( Individual ), RankIndividuals );
}


// How do we generate a new population according to the results of
// their parents?
// WARNING: THIS FUNCTION WILL NOT WORK IF POPULATION SIZE IS LESS 
// THAN 90 INDIVIDUALS.  For smaller populations, you will have to
// change the parenting rules.
void Genetic::BegetNextGeneration()
{
	// First, we keep the top 20 individuals from the last 
	// generation, without change.

	// Then, we make 42 new individuals by mating the top
	// 7 performers, giving the higher-ranked individuals more
	// chances to reproduce.  The following chunk of code only
	// selects the parents; mating occurs later.
	int currentIndex = 20;
	for( int higher = 0; higher < 6; higher++ )
	{
		for( int lower = higher + 1; lower < 7; lower++ )
		{
			Population[ currentIndex++ ] = Population[ higher ];
			Population[ currentIndex++ ] = Population[ lower ];
		}
	}

	// Then, we mate 14 pairs of randomly-selected individuals
	// from the top third of the parent population.  Again, we
	// only select the parents here; mating for both this group
	// and the preceding one takes place later
	for( int i = 0; i < 14; i++ )
	{
		Population[ currentIndex++ ] = Population[ rand() % 10 ];
		Population[ currentIndex++ ] = Population[ rand() % 25 + 10 ];
	}

	// This is the actual mating process for the last two groups
	for( i = 20; i < 90; i += 2 )
	{
		// First, apply crossover to each pair of parents
		Crossover( Population[ i ].DNA, Population[ i + 1 ].DNA );

		// And then mutate the children, maybe
		Mutation( Population[ i ].DNA );
		Mutation( Population[ i + 1 ].DNA );
	}

	// Finally, complete the population with a handful of brand
	// new individuals to introduce new variety into the gene pool
	for( i = 90; i < GA_POPULATION_SIZE; i++ )
	{
		MakeRandomIndividual( i );
	}
}


/****************************************************************
 * The Genetic Operators
 ***************************************************************/


// We crossover using the Disruption method (each gene can
// crossover independantly from the others).  Given the small
// number of genes in our scenario, this sounds like a reasonable
// approach.
void Genetic::Crossover( Chromosome & c1, Chromosome & c2 )
{
	for( int i = 0; i < ALL_PRIORITIES; i++ )
	{
		if( rand() % 2 == 0 )
		{
			double tmp = c1.Priorities[ i ];
			c1.Priorities[ i ] = c2.Priorities[ i ];
			c2.Priorities[ i ] = tmp;
		}
	}
}

// We mutate by replacing one gene by a random number, once
// in a while
void Genetic::Mutation( Chromosome & c )
{
	for( int i = 0; i < ALL_PRIORITIES; i++ )
	{
		if( rand() % 100 < MUTATION_RATE )
		{
			c.Priorities[ i ] = (double) (rand() % 10000) / 10000.0;
		}
	}
}


/****************************************************************
 * REPORTING RESULTS
 ***************************************************************/

void Genetic::ReportGenerationResults( int which )
{
	cout << "RESULTS FOR GENERATION " << which << endl;
	cout << "-------------------------" << endl << endl;

	cout << "   EAT   KILL   HEAL   FLEE   EXPL    SHEEP  KNIGHT  ALIVE  CAPTV  DAMAG    PERF" << endl;
	for( int i = 0; i < GA_POPULATION_SIZE; i++ )
	{
		cout << setw( 6 ) << setprecision( 5 ) << Population[ i ].DNA.Priorities[ EATING_PRIORITY ] << " "
				 << setw( 6 ) << setprecision( 5 ) << Population[ i ].DNA.Priorities[ KILLING_PRIORITY ] << " "
				 << setw( 6 ) << setprecision( 5 ) << Population[ i ].DNA.Priorities[ HEALING_PRIORITY ] << " "
				 << setw( 6 ) << setprecision( 5 ) << Population[ i ].DNA.Priorities[ FLEEING_PRIORITY ] << " "
				 << setw( 6 ) << setprecision( 5 ) << Population[ i ].DNA.Priorities[ EXPLORING_PRIORITY ] << " "
				 << setw( 8 ) << Population[ i ].StatsSheepEaten << " "
				 << setw( 7 ) << Population[ i ].StatsKnightsKilled << " "
				 << setw( 6 ) << Population[ i ].StatsTimeAlive << " "
				 << setw( 6 ) << Population[ i ].StatsTimeCaptive << " "
				 << setw( 6 ) << Population[ i ].StatsDamageTaken << " "
				 << setw( 8 ) << setprecision( 6 ) << Population[ i ].Performance
				 << endl;
	}

	cout << endl << endl;
}