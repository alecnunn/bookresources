/****************************************************************
 *
 * Dempster-Shafer Theory
 * Using belief in decision-making
 *
 * This code demonstrates how to combine sources of "evidence"
 * into something your AI can base its decisions on.
 *
 ***************************************************************/

// The world states we are trying to decide between
enum { eventPitcherOK = 1, eventPitcherNervous = 2, eventPitcherTired = 4, eventAllEvents = 8 };

/***************************************************************
 *
 * STRUCT PieceOfEvidence
 *
 * A simple struct containing the evidence provided by a single
 * source for a single set of possibilities.  For example, a 
 * source may say: "30% of my evidence points towards event A
 * or event B being true".
 * 
 ***************************************************************/

struct PieceOfEvidence
{
	// The data.  We store the definition of the set of events 
	// explicitly to make manipulation and computation easier
	int beliefSet;
	double beliefValue;

	// Construction
	PieceOfEvidence();
	void Clear();

	// Set intersection operation to determine whether two pieces of
	// evidence corroborate each other to some degree
  static int Intersection( PieceOfEvidence & a, PieceOfEvidence & b )
  {
		return( a.beliefSet & b.beliefSet );
  }
	static int Intersection( PieceOfEvidence & a, int set )
	{
		return( a.beliefSet & set );
	}

	// Evidence combines just like probability: Bel( a AND b ) = Bel( a ) * Bel( b )
  static double CombinedEvidence( PieceOfEvidence & a, PieceOfEvidence & b )
	{
		return( a.beliefValue * b.beliefValue );
	}

};


/****************************************************************
 *
 * CLASS BodyOfEvidence
 *
 * This class contains all of the evidence provided by a single
 * source, or a combination of the evidence provided by multiple
 * sources.  For example, a source may say:
 *
 * "My evidence shows that there is a 30% chance that A is true,
 * a 40% chance that either A or B is true, and a 30% chance that
 * C is true."
 *
 ***************************************************************/

class BodyOfEvidence
{
	// The data.  For simplicity, we use static storage for the 
	// power set of possible events, because our pitcher can only
	// be in 3 different states and thus we only require 2^3 = 8
	// slots in the array.  If you have a huge event set, you may
	// need a more involved data structure
	PieceOfEvidence mass[ eventAllEvents ];

public:

	// Construction
	BodyOfEvidence();
	void Clear();
	void Copy( BodyOfEvidence & source );

	// Build and store a single PieceOfEvidence
	void AcceptEvidence( int set, double belief );

	// Combine the currently held beliefs and evidence with the
	// evidence provided by a new source
	bool Combine( BodyOfEvidence & source );

	// Compute the credibility and plausibility of an event set
	double Plausibility( int set );
	double Credibility( int set );

	// Helper function
	void Print();
};



