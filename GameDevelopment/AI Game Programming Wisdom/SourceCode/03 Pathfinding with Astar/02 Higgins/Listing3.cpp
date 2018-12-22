/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// This class definition is:
template<class TSTORAGE, class TGOAL, class TMAP>
class AStarMachine : public AStarBase, public TSTORAGE


// To create the forest A* machine, we combine the storage, 
// goal and map classes, and use it like:

// typedef for clarity in code example.
typedef AStarForestStorage ASTARSForest;
typedef AStarForestGoal ASTARGForest;
AStarMachine<ASTARSForest, ASTARGForest, Map> theMachine;

// set the source and destination
theMachine.SetSource(theSourcePoint);
theMachine.SetDestination(theDestinationPoint);

// run it!
theMachine.RunAStar();

