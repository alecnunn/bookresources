/* Copyright (C) William van der Sterren, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2001"
 */

// Left code portion from Figure 1
void SoloAI::Think() {
	CombatSituation* sit = GetSituation();
	sit->Observe();


	for each available action {
		if( action.IsApplicable(sit) )
		{
			determine value of action in sit;
			if better than best action, 
				make best action
		}
	}

	Execute best action();
}

// Right code portion from Figure 1
void SquadMemberAI::Think() {
	CombatSituation* sit = GetSituation();
	Sit->Observe();
	sit->ProcessTeamIntentions();
	sit->ProcessTeamObservations();
	for each available action {
		if( action.IsApplicable(sit) )
		{
			determine value of action in sit;
			if better than best action, 
				make best action
		}
	}
	nearbySqdMembers->AnnounceActn();
	Execute best action();
}
