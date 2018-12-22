
//returns length of path, negative if no path exists
float AI::GeneratePathingGoals(const PathNode *from, const PathNode *to,
							GoalQueue &goals, float maxDistance)
{
	if (!from->PathExists(to, Character())) //make sure that 'to' is reachable from 'from' for our character
		return -1.0f;

	std::vector<const PathLink *> path;
	float pathLength = g_NodeMap.FindPath(Character(), from, to, path);
	if (pathLength > 0) //if we found a valid path
		GenerateGoalsFromPath(path, goals);
	
	return pathLength;
}

void AI::GenerateGoalsFromPath(const std::vector<const PathLink *>& path, GoalQueue& goals) {
	//NOTE: path[0] is final link! (the path is reversed)
	//path[0]->End() is the goal node

	bool replanForDoorOrElevator = false;
	int i = 0; //we start at the end of the path and keep inserting goals at the front of the queue

	const PathNode *currentGotoNode = 0;
	while (i < (int)path.size() && !replanForDoorOrElevator){

		if (path[i]->flags & PathLink::flagLinkDoor){ //if this link has a door on it...
			const Door *door = (path[i]->GetDoor());
			replanForDoorOrElevator = true;
			goals.InsertSubgoal(new Goal_GoThroughDoor(this, path[i]));
		}
		else if (path[i]->flags & PathLink::flagLinkElevator){ //if this link has an elevator
			const Elevator *elevator  = path[i]->End()->GetElevator();
			replanForDoorOrElevator = true;
			const PathNode *lastNode = path[i]->End(); //this is the last node inside the elevator

			//go through multiple elevator floors until we reach the first elevator link
			while ( (i < (int)path.size())  &&  path[i]->flags & PathLink::flagLinkElevator ){
				i++;
			}
			const PathLink *entryLink = NULL; //this is the link used to enter the elevator
			if (i < (int)path.size()) //as long as we aren't past the beginning of the path, set entryLink
				entryLink = path[i];
			goals.InsertSubgoal(new Goal_RideElevator(this, entryLink, lastNode));
		}
		else {
			// if the link can be shortcut and we're not currently shortcutting or we can see the
			//current shortcut goal from the link start then consider generating a shortcut goal
			if (path[i]->CanBeShortcut() &&
				(!currentGotoNode || PathClear(path[i]->Start()->pos, currentGotoNode->pos) ) )
			{
				//if we don't have a node we're currently shortcutting to, add one
				if (!currentGotoNode){
					//all of these goals are subgoals, so they should never trigger a stop
					goals.InsertSubgoal(new Goal_GotoNode(this, path[i]->End(), true )); //the optional last argument forces a direct path
					currentGotoNode = path[i]->End();
				}
			}
			else {
				goals.NewSubgoal(new Goal_FollowLink(this, path[i] ));
				currentGotoNode = 0;
			}
		}
		i++;
	}
}