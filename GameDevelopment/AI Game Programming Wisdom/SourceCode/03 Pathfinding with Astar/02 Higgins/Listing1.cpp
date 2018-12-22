/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// Excerpt from AStarMachine<TGoal, TStorage, TMap>'s run method.

// Infinite loop. The goal will tell us when we are done.
for(;;)
{    
    // used for time-slicing
    this->mRevolutions++; 

    // get the best choice so far
    this->mCurrentNode = this->RemoveCheapestOpenNode();

    // if == true, then its likely that we are not at the
    // goal and we have no more nodes to search through
    if(this->mGoal.GetIsPathFinished(this->mCurrentNode))
        break;

    // for all 8 neighbor tiles, examine them by checking
    // their TileOpen status, and putting them on the
    // appropriate A* lists. (code not shown)
	
    // add this node to closed list now
    this->AddNodeToClosedList(this->mCurrentNode);

    // Should we pause? (used in time-slicing)
    if(this->mGoal.ShouldPause(this->mCurrentRevolutions))
        break;

    // if == true, this means we have exceeded our max
    // pathfinding revolutions and should give up.
    if(this->mGoal.ShouldGiveUp(this->mRevolutions))
        break;

    // used for time-slicing
    this->mCurrentRevolutions++; 
}
