/****************************************************************
 * CLASS Troll
 * The hero of our grand adventure; this is the object we will be
 * evolving with genetic algorithms
 ***************************************************************/

#ifndef TROLL_H
#define TROLL_H

#include "Globals.h"
#include "Entities.h"
#include "Chromosome.h"

/****************************************************************
 
	 NOTE: Although we will often treat the Troll in unique fashion,
	 we still derive it from the Entity class to benefit from its
	 services

*****************************************************************/

class Troll : public Entity
{
	// The troll's genetic material...
	Chromosome * DNA;

	// How healthy is our friend the troll?  Globals.h defines a
	// few constants describing health levels
	int HitPoints;

	// Some flags used during the simulation
	bool IsCaptured;
	bool IsDead;


public:
	// Stats compiled during the simulation; they are used to 
	// compute the individual's performance and assess its fitness
	int StatsTimeSinceLastMeal;
	int StatsKnightsKilled;
	int StatsSheepEaten;
	int StatsDamageTaken;
	int StatsTimeAlive;
	int StatsTimeCaptive;

	// Construction
	Troll( Chromosome & c, int x, int y ) : Entity( x, y, -1 ), HitPoints( FULL_HEALTH ), 
		IsCaptured( false ), IsDead( false ), DNA( &c ), StatsTimeSinceLastMeal( 0 ),
		StatsKnightsKilled( 0 ), StatsSheepEaten( 0 ), StatsDamageTaken( 0 ),
		StatsTimeAlive( 0 ), StatsTimeCaptive( 0 ) {}
	Entity * Clone() { return 0; } // Cloning of the troll is illegal

	bool Dead() { return IsDead; }

	// The Entity virtual classes
	bool Update();
	int ExportedClass() { return ENTITY_TROLL; }

	// Messages sent to the troll by other entities
	void SendCaptureMessage();
	void SendDamageMessage( int ouch );
	bool SendFightMessage();

	// Functions needed by the simulation
	void Reset();
	double GetEvaluation();

private:
	// A whole mess of methods used to decide what the troll
	// will do in a given turn.  First, a self-explanatory one
	int PickStrategy();

	// These methods explore the environment for conditions
	// suitable to each of the troll's goals.  The scores
	// they return are multiplied by the priorities set in the
	// troll's genes, and the highest total determines which
	// goal the troll will follow this time
	double FleeingStrategy();
	double EatingStrategy();
	double HealingStrategy();
	double KillingStrategy();
	double ExploringStrategy();

	// These methods implement the goals selected by PickStrategy
	void ImplementFleeingStrategy();
	void ImplementEatingStrategy();
	void ImplementHealingStrategy();
	void ImplementKillingStrategy();
	void ImplementExploringStrategy();


public:
	// Debugging help
	void Dump();
};

#endif