/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

PathManager::DoPriorityPath(Unit* inUnit)
{
	// find the pathfinder for this unit.
	Pathfinder* thePath = this->FindPathfinder(inUnit);

	// If we have a node, finish the path right away!
	if(thePath != NULL)
	{
		// Tell it not to pause.
		thePath->SetPauseCount(-1);

		// let's artificially cap the path so that they 
		// don't bring down the CPU.
		thePath->AddToMaxRevolutions (250);
									
		// go-pher-it..
		thePath->RunAStar();	

		// Process the completed path.
		this->ProcessFinishedPath(thePath);

		// Remove this unit's path 
		// which includes erasing it from our queue
		this->RemovePath(inUnit);
	}
}
