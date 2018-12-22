/*****************************************************************
 * TROLL IMPLEMENTATION
 ****************************************************************/

#include "Troll.h"
#include "Chromosome.h"
#include <iostream.h>
#include <stdlib.h>


/*****************************************************************
 * THE MESSAGES
 ****************************************************************/

// Take some damage, either from a fight or from incoming fire
// from a Tower.  HitPoints stores the current health of the 
// troll; StatsDamageTaken stores the cumulative damage taken
// since the beginning of the simulation
void Troll::SendDamageMessage( int ouch )
{
	HitPoints -= ouch;
	StatsDamageTaken += ouch;

	if ( HitPoints <= 0 )
		IsDead = true;
}

// Capture message sent by traps
void Troll::SendCaptureMessage()
{
	IsCaptured = true;
}

// Fight message, sent by knights.  Returns true if the
// troll wins the duel
bool Troll::SendFightMessage()
{
	// Not a very elaborate fight system: we just roll the damage
  // caused by the knight in the whole fight at once, randomly

	// Fighting while captive ain't a smart idea
	if( IsCaptured )
	{
		SendDamageMessage( rand() % 15 + 5 );
	}
	else
	{
		SendDamageMessage( rand() % 10 );
	}

	if( IsDead )
	{
	  // Troll is killed
		return false;
	}
	else
	{
		// Knight is killed
		StatsKnightsKilled++;
		return true;
	}
}


/*****************************************************************
 * TURN UPDATE
 ****************************************************************/

bool Troll::Update()
{
	// We should never get here with a dead troll, but just in case...
	if( IsDead )
		return true;

  // If the troll hasn't eaten in a while, it sustains damage
	if( StatsTimeSinceLastMeal++ > 25 )
		SendDamageMessage( 1 );
	
	StatsTimeAlive++;

	// A captured troll will attempt to escape.  If he fails, he 
	// can't do a thing.
	if( IsCaptured && ( rand() % 10 < 7 ) )
	{
		StatsTimeCaptive++;
		return true;
	}

	IsCaptured = false;

	// Which goal do we follow?
	switch( PickStrategy() )
	{
		case EATING_PRIORITY: ImplementEatingStrategy(); break;
		case KILLING_PRIORITY: ImplementKillingStrategy(); break;
		case EXPLORING_PRIORITY: ImplementExploringStrategy(); break;
		case FLEEING_PRIORITY: ImplementFleeingStrategy(); break;
		case HEALING_PRIORITY: ImplementHealingStrategy(); break;
		default: break;
	}

	return true;
}


/****************************************************************
 * STRATEGY SELECTION
 ***************************************************************/

int Troll::PickStrategy()
{
	// Look at the state of the world around the troll and pick a
  // strategy that makes sense.  The biases encoded in the troll's
	// DNA play a crucial role here

	double bestSoFar = DNA->Priorities[ EATING_PRIORITY ] * EatingStrategy();
	int choice = EATING_PRIORITY;

	double newCandidate = DNA->Priorities[ KILLING_PRIORITY ] * KillingStrategy();

	if ( newCandidate > bestSoFar )
	{
		bestSoFar = newCandidate;
		choice = KILLING_PRIORITY;
	}

	newCandidate = DNA->Priorities[ HEALING_PRIORITY ] * HealingStrategy();
	if ( newCandidate > bestSoFar )
	{
		bestSoFar = newCandidate;
		choice = HEALING_PRIORITY;
	}

	newCandidate = DNA->Priorities[ FLEEING_PRIORITY ] * FleeingStrategy();
	if ( newCandidate > bestSoFar )
	{
		bestSoFar = newCandidate;
		choice = FLEEING_PRIORITY;
	}

	newCandidate = DNA->Priorities[ EXPLORING_PRIORITY ] * ExploringStrategy();
	if ( newCandidate > bestSoFar )
	{
		bestSoFar = newCandidate;		// Unnecessary, but kept there in case I add other goals later
		choice = EXPLORING_PRIORITY;
	}

	return choice;
}


// Trolls like to kill isolated adventurers, but stay away from
// packs of them.  Plus, a fight sounds a lot better when healthy!
double Troll::KillingStrategy()
{
	double score;
	int knights = ptrGrid->HowManyCloseToTroll( ENTITY_KNIGHT, 6 );
	if ( knights == 0 )
		score = 0.0;
	else if ( knights <= 2 )
		score = 0.8;
	else if ( knights <= 4 )
		score = 0.4;
	else
		score = 0.25;

	score -= 0.02 * ( FULL_HEALTH - HitPoints );
	return score;
}


// Eating is more interesting if there are sheep nearby and if
// the troll is hungry
double Troll::EatingStrategy()
{
	double score = 0.02 * StatsTimeSinceLastMeal;
	score += 0.1 * ptrGrid->HowManyCloseToTroll( ENTITY_SHEEP, 5 );
	return score;
}


// Exploring is always a decent option, except when heavily wounded
double Troll::ExploringStrategy()
{
	return 0.5 - 0.01 * ( FULL_HEALTH - HitPoints );
}


// If there are too many enemies around, and/or the troll is
// wounded, he should get away if he can.  Especially if there is
// a safe haven nearby!
double Troll::FleeingStrategy()
{
	double score;

	if ( HitPoints > REASONABLY_HEALTHY )
		score = 0.0;
	else if ( HitPoints > HEAVILY_DAMAGED )
		score = 0.3;
	else if ( HitPoints > CRITICALLY_DAMAGED )
		score = 0.6;

	score += 0.1 * ptrGrid->HowManyCloseToTroll( ENTITY_KNIGHT, 4 );
	score += 0.05 * ptrGrid->HowManyCloseToTroll( ENTITY_TOWER, 3 );
	if( ptrGrid->HowManyCloseToTroll( ENTITY_HAVEN, 3 ) > 0 )
		score += 0.2;

	return 0.2;
}


// Is it time to just do nothing and heal?  Well, if we are within
// range of a firing tower, we're wasting our time because the
// damage will just keep piling on.  And if we're very close to a
// haven without actually being in it, we probably should go there
// first because we'll heal faster.
double Troll::HealingStrategy()
{
	double score = 0.02 * ( FULL_HEALTH - HitPoints );
	if( ptrGrid->HowManyCloseToTroll( ENTITY_TOWER, 3 ) > 0 )
		score -= 0.2;
	if( ptrGrid->HowManyCloseToTroll( ENTITY_KNIGHT, 3 ) > 0 )
		score -= 0.1;
	if( ptrGrid->HowManyCloseToTroll( ENTITY_HAVEN, 2 ) > 0 )
		score -= 0.1;
	if( ptrGrid->HowManyCloseToTroll( ENTITY_HAVEN, 0 ) > 0 )
		score += 0.5;

	return score;
}


/*****************************************************************
 * STRATEGY IMPLEMENTATIONS
 * These methods are pretty dumb, but we're talking about a troll 
 * here, not the Dalai Lama.  Plus, this sample code is cluttered
 * enough as it is; no need to make it even worse! ;-)
 ****************************************************************/

void Troll::ImplementEatingStrategy()
{
	// Look for a sheep nearby
	int target = ptrGrid->FindClosestFromTroll( ENTITY_SHEEP );
	if ( target == -1 )
	{
		// No more sheep on the grid; wander instead
		ImplementExploringStrategy();	
	}

	// Can we reach it at once?  If so, let's do it
	if ( ptrGrid->DistanceFromTroll( target ) <= 4 )
	{
	  MoveEntity( ptrGrid->GetEntityX( target ) - GetX(), ptrGrid->GetEntityY( target ) - GetY() );
		StatsSheepEaten += ptrGrid->HowManyCloseToTroll( ENTITY_SHEEP, 0 );
	}

	// Otherwise, march towards the sheep in question
	else
	{
		int dx, dy;
		if( ptrGrid->GetEntityX( target ) > GetX() )
			dx = 2;
		else if ( ptrGrid->GetEntityX( target ) == GetX() )
			dx = 0;
		else
			dx = -2;
		if( ptrGrid->GetEntityY( target ) > GetY() )
			dy = 2;
		else if ( ptrGrid->GetEntityY( target ) == GetY() )
			dy = 0;
		else
			dy = -2;
		MoveEntity( dx, dy );
	}
}


void Troll::ImplementKillingStrategy()
{
	// Look for a suitable target
	int target = ptrGrid->FindClosestFromTroll( ENTITY_KNIGHT );
	if ( target == -1 )
	{
		ImplementExploringStrategy();	
	}

	// Can we attack it immediately?  If so, let's move into its square
	if ( ptrGrid->DistanceFromTroll( target ) <= 4 )
	{
		// the knight will initiate combat next turn, so we don't have to.
	  MoveEntity( ptrGrid->GetEntityX( target ) - GetX(), ptrGrid->GetEntityY( target ) - GetY() );
	}

	// Otherwise, march towards the target
	else
	{
		int dx, dy;
		if( ptrGrid->GetEntityX( target ) > GetX() )
			dx = 2;
		else if ( ptrGrid->GetEntityX( target ) == GetX() )
			dx = 0;
		else
			dx = -2;
		if( ptrGrid->GetEntityY( target ) > GetY() )
			dy = 2;
		else if ( ptrGrid->GetEntityY( target ) == GetY() )
			dy = 0;
		else
			dy = -2;
		MoveEntity( dx, dy );
	}
}


void Troll::ImplementHealingStrategy()
{
	// Doing nothing restores some health
	HitPoints += ( rand() % 3 );

	// Safe havens are filled with decaying flesh, carnivorous
	// plants, venomous mushrooms and other delicacies that speed
	// up troll healing.
	if( ptrGrid->HowManyCloseToTroll( ENTITY_HAVEN, 0 ) > 0 )
	{
		HitPoints += 2;
	}
	
	// Can't get healthier than perfect health
	if( HitPoints > FULL_HEALTH )
		HitPoints = FULL_HEALTH;
}


void Troll::ImplementFleeingStrategy()
{
	// Can we retreat to a safe haven?  If so, do it
	int haven = ptrGrid->FindClosestFromTroll( ENTITY_HAVEN );
	if( ptrGrid->DistanceFromTroll( haven ) <= 4 )
	{
	  MoveEntity( ptrGrid->GetEntityX( haven ) - GetX(), ptrGrid->GetEntityY( haven ) - GetY() );		
	}

	// Otherwise, run away from the closest knight
	else
	{
		int target = ptrGrid->FindClosestFromTroll( ENTITY_KNIGHT );
		int dx, dy;

		if( ptrGrid->GetEntityX( target ) > GetX() )
			dx = -2;
		else
			dx = 2;
		if( ptrGrid->GetEntityY( target ) > GetY() )
			dy = -2;
		else
			dy = 2;

		MoveEntity( dx, dy );
	}
}


// The troll's exploring "strategy" consists of 
// wandering about aimlessly.
void Troll::ImplementExploringStrategy()
{
	MoveEntity( rand() % 5 - 2, rand() % 5 - 2 );
}


/*****************************************************************
 * SIMULATION
 *****************************************************************/

void Troll::Reset()
{
	HitPoints = FULL_HEALTH;
	IsCaptured = false;
	IsDead = false;
	StatsTimeSinceLastMeal = 0;
	StatsKnightsKilled = 0;
	StatsSheepEaten = 0;
	StatsDamageTaken = 0;
	StatsTimeAlive = 0;
	StatsTimeCaptive = 0;
}


double Troll::GetEvaluation()
{
	double score = 8.0 * StatsKnightsKilled + 	10.0 * StatsSheepEaten +	1.5 * StatsTimeAlive -
					1.0 * StatsTimeCaptive - 2.50 * StatsDamageTaken;
	return score;
}




void Troll::Dump()
{
	cout << "TROLL DUMPED at x,y = " << GetX() << "," << GetY() << endl;
}