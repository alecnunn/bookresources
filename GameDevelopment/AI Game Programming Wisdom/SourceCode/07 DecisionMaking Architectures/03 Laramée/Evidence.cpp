#include "Evidence.h"
#include <iostream.h>


/*****************************************************************
 * PieceOfEvidence Implementation
 * All very straightforward
 ****************************************************************/

PieceOfEvidence::PieceOfEvidence()
{
	Clear();
}

void PieceOfEvidence::Clear()
{
	beliefSet = 0;
	beliefValue = 0.0;
}


/***************************************************************
 * Implementation of BodyOfEvidence
 **************************************************************/

BodyOfEvidence::BodyOfEvidence()
{
	Clear();
}


void BodyOfEvidence::Clear()
{
	for( int i = 0; i < eventAllEvents; i++ )
	{
		mass[ i ].Clear();
		mass[ i ].beliefSet = i;
	}
}


void BodyOfEvidence::AcceptEvidence( int set, double belief )
{
	mass[ set ].beliefSet = set;
	mass[ set ].beliefValue = belief;
}

char * eventNames[] =
{
	"Null event",
	"Pitcher OK",
	"Pitcher Nervous",
	"Pitcher OK or Nervous",
	"Pitcher Tired",
	"Pitcher Tired or OK",
	"Pitcher Tired or Nervous",
	"No opinion"
};

void BodyOfEvidence::Print()
{
	for( int i = 0; i < eventAllEvents; i++ )
	{
		cout << "EVENT " << eventNames[ i ] 
			   << " has credibility: " << Credibility( mass[ i ].beliefSet )
				 << " and plausibility: " << Plausibility( mass[ i ].beliefSet )
				 << endl;
	}
}


void BodyOfEvidence::Copy( BodyOfEvidence & source )
{
	for( int i = 0; i < eventAllEvents; i++ )
	{
		mass[ i ].beliefSet = source.mass[ i ].beliefSet;
		mass[ i ].beliefValue = source.mass[ i ].beliefValue;
	}
}


/**************************************************************
 *
 * BodyOfEvidence::Combine
 *
 * The real meat of the class.  This method implements Dempster's
 * rule for combination of evidence.
 *
 * Return value: true if the two sources of evidence can be 
 * combined, false otherwise.  Dempster's rule fails when the two
 * sources of evidence are completely contradictory, i.e., if
 * Source #1 is absolutely convinced that A is true and Source #2
 * is equally convinced that B is true, there is no way to 
 * reconcile them.
 *
 *************************************************************/

bool BodyOfEvidence::Combine( BodyOfEvidence & source )
{
	// Temporary storage required by the application of Dempster's
	// rule; see the article for a discussion of the denominator
  PieceOfEvidence temp[ eventAllEvents ];
	double denominator = 1.0;

	// Compare the sets of events in both sources for compatibility
	for( int i = 0; i < eventAllEvents; i++ )
  {
    for( int j = 0; j < eventAllEvents; j++ )
    {
			int intersection = PieceOfEvidence::Intersection( mass[ i ], source.mass[ j ] );

			// If the sources corroborate each other, store the combined evidence; otherwise,
			// increase the denominator.  This is direct implementation of Dempster's rule
      if ( intersection )
        temp[ intersection ].beliefValue += PieceOfEvidence::CombinedEvidence( mass[ i ], source.mass[ j ] );
      else
        denominator -= PieceOfEvidence::CombinedEvidence( mass[ i ], source.mass[ j ] );
    }
  }

	// If the denominator underflows, the sources contradict each other;
	// there is nothing more we can do
	if ( denominator < 0.0001 )
		return false;

	// Otherwise, apply the correction factor to each element of the new
	// body of evidence
	for( i = 0; i < eventAllEvents; i++ )
	{
		mass[ i ].beliefValue = temp[ i ].beliefValue / denominator;
	}

	return true;
}


/***************************************************************
 * 
 * BodyOfEvidence::Plausibility
 *
 * Plausibility of X is equal to the sum of the evidence  
 * supporting all event sets with which X is not 
 * in complete contradiction.  It represents an upper bound on
 * the probability that one or more of the events in X
 * is true.
 *
 **************************************************************/

double BodyOfEvidence::Plausibility( int set )
{
	double val = 0.0;
	
	for( int i = 0; i < eventAllEvents; i++ )
	{
		if( PieceOfEvidence::Intersection( mass[ i ], set ) != 0 )
			val += mass[ i ].beliefValue;
	}

  return val;
}


/***************************************************************
 * 
 * BodyOfEvidence::Credibility
 *
 * Credibility of X is equal to the sum of the evidence
 * supporting all event sets completely contained in X.  It 
 * represents a lower bound on the probability that some event
 * in X is true.
 *
 **************************************************************/

double BodyOfEvidence::Credibility( int set )
{
	double val = 0.0;
	
	for( int i = 0; i < eventAllEvents; i++ )
	{
		if( PieceOfEvidence::Intersection( mass[ i ], set ) == mass[ i ].beliefSet )
			val += mass[ i ].beliefValue;
	}

  return val;
}
