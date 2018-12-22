/* Copyright (C) William van der Sterren, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2001"
 */

// Algorithms for picking a next position and evaluating each position.

// SquadMemberAI::PickNextPosition
for( spot=nearbyspots.begin(); position!=nearbyspots.end(); ++spot ) {
	if( IsClaimedByFellowSquadMember(*spot) )
		continue;
	value = GetManeuverValueForNextPosition(*spot);

	if( value > highestvalue ) {
		mostsuitablespot = *spot; 
		highestvalue= value;
	}
}

float SquadMemberAI::GetManeuverValueForNextPosition(spot)
{
  return   m_BunchPenalty       * ProjDistToBuddiesAsSeenFromThreat(spot)
         + m_BlockedPenalty     * BlockedLineOfFireBySqdMembers(spot)
         + m_BlockingPenalty    * BlockingLineOfFireOfSqdMembers(spot)
         + m_NeedForCover       * DistanceToNearestFreeCoverSpot(spot)
         + m_NeedForContact     * NumberOfLinesOfSightToBuddies(spot)
         + m_NeedCohesion       * ProperDistanceToBuddies(spot)
         + m_NeedForIntel       * NumberOfLinesOfSightToThreats(spot)
         + m_NeedToAvoidCorpses * DistanceToDeadBuddies(spot)   
         + m_NeedForClosingIn   * MinimumDistanceToThreats(spot)
         // below are non-reactive
         + m_NeedForStrongSpot  * AverageTacticalValueOfSpot(spot)
         + m_FormationPenalty   * DistanceFromFormationSlot(spot);
}


