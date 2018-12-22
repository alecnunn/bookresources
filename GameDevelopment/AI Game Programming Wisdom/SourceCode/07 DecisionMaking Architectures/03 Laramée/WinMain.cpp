/****************************************************************
 *
 * MAIN PROGRAM
 *
 * Here, we assume that the various sources of evidence have
 * examined data before coming up with their beliefs.  How to do
 * that should be fairly straightforward.
 *
 ***************************************************************/

#include "Evidence.h"
#include <iostream.h>

int main( int argc, char ** argv )
{
	// The evidence provided by the catcher
	// Of the pitcher's last 10 pitches, 8 have hit the catcher's
	// target or come reasonably close, while 2 have not.  
	BodyOfEvidence catcher;
	catcher.AcceptEvidence( eventPitcherOK, 0.8 );
	catcher.AcceptEvidence( eventPitcherNervous | eventPitcherTired, 0.2 );

	// The evidence provided by the pitch counts
	// The pitcher normally tires after 100 pitches, and he has now
	// reached 115; of the last 20 pitches, 75% were thus beyong his
	// normal limit and suggest he is probably getting tired...
	BodyOfEvidence pitchCount;
	pitchCount.AcceptEvidence( eventPitcherOK | eventPitcherNervous, 0.25 );
	pitchCount.AcceptEvidence( eventPitcherTired, 0.75 );

	// The evidence provided by the radar gun examines the speeds of the
	// last 10 pitches thrown:
	// 2 were clocked at over 92 mph (which suggests that the pitcher is fine)
	// 1 was clocked at between 87 and 92 (which suggests that he is either 
	//			tired or being cautious)
	// 4 were clocked at less than 87 (which suggests that his arm is tired) 
	// and 3 were changeups or breaking balls (in which case the radar gun 
	//			evidence is inconclusive).
	BodyOfEvidence radarGun;
	radarGun.AcceptEvidence( eventPitcherOK, 0.2 );
	radarGun.AcceptEvidence( eventPitcherTired | eventPitcherNervous, 0.1 );
	radarGun.AcceptEvidence( eventPitcherTired, 0.4 );
	radarGun.AcceptEvidence( eventPitcherOK | eventPitcherNervous | eventPitcherTired, 0.3 );

	// The evidence provided by the state of the game: of the last
	// 10 pitches thrown:
	// 1 was a strike or resulted in an out (a good sign)
	// 2 were balls with runners on base (he may be distracted or tired)
	// 1 was grounded for a base hit (the hitters are catching up)
	// 6 were balls with nobody on (he can't hit the plate)
	BodyOfEvidence stats;
	stats.AcceptEvidence( eventPitcherOK, 0.1 );
	stats.AcceptEvidence( eventPitcherTired | eventPitcherNervous, 0.2 );
	stats.AcceptEvidence( eventPitcherTired, 0.1 + 0.6 );


	// Now, we combine the evidence
	BodyOfEvidence manager;
	manager.Copy( catcher );
	manager.Combine( pitchCount );
	manager.Combine( radarGun );
	manager.Combine( stats );

	// Let's look at the evidence, shall we?
	cout << "MANAGER'S EVIDENCE" << endl;
	cout << "------------------" << endl << endl;
	manager.Print();
	cout << endl;
	
	// And now, let's make a decision
	if( manager.Credibility( eventPitcherTired ) > 0.75 )
		cout << "Time to go to the bullpen!" << endl;
	else if ( manager.Plausibility( eventPitcherNervous ) > 0.60 )
		cout << "Time to visit the mound" << endl;
	else
		cout << "The pitcher is OK, no need to do anything" << endl;

	return 0;
}
