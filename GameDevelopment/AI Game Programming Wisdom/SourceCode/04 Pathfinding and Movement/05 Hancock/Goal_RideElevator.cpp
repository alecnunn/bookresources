

Goal_RideElevator::Goal_RideElevator(AI* pAI, const PathLink *entryLink_, const PathNode *endNode) :
	Goal( pAI ), GoalQueue(), entryLink(entryLink_), end(endNode), active(false)
{
	elevator = end->GetElevator();
	if (!elevator){
		mGoalStatus = statusFailed;
		return;
	}
	//if entryLink is NULL, then we're inside the elevator to begin with
	//otherwise entryLink is the link that leads us inside the elevator
	if (entryLink)
		stage = CallingElevator;
	else {
		stage = RidingElevator;
	}
}

Goal_RideElevator::~Goal_RideElevator()
{
}

bool Goal_RideElevator::ReplanSubgoals(){

	//while we're not inside the elevator and the elevator isn't moving to our floor or we're blocked by the elevator door, then replanning
	//consists of calling the elevator via switch
	switch (stage){
	case CallingElevator:
		{
			const PathNode *bestswitchnode = NULL;
			const Switch *bestswitch = NULL;

			int startFloor = entryLink->End()->elevatorFloor;

			//find the elevator switch that is reachable without going through a door or elevator
			for (int i=0; i < elevator->NumControllers(); i++){
				CEntitySwitch *pSwitch = elevator->GetController(i);
				if (pSwitch->GetElevatorCallFloor() == startFloor){
					const PathNode *sn = g_NodeMap.GetSwitchLocation(pSwitch);
					if (sn && sn->NoDoorsBetween(entryLink->Start()) && sn->NoElevatorsBetween(entryLink->Start())){
						bestswitchnode = sn;
						bestswitch = pSwitch;
					}
				}
			}
			//if there isn't a call switch, we're going to assume that the elevator is region-triggered
			//to be there when we get close
			if (bestswitchnode){
				NewSubgoal(new Goal_HitElevatorCallSwitch(mpAI, bestswitchnode, elevator));
			}
			//move to the node in front of the elevator
			NewSubgoal(new Goal_GotoNode(mpAI, entryLink->Start()));
		}
		return true;
	case EnteringElevator:
		{
			//else if we're not inside the elevator, but it is at our floor and we're not blocked by the door
			//then we need to get to the node in front of us and then enter the elevator
			NewSubgoal(new Goal_GotoNode(mpAI, entryLink->Start()));
			NewSubgoal(new Goal_FollowLink(mpAI, entryLink));
		}
		return true;
	case RidingElevator:
		{
			const PathNode *bestswitchnode = NULL;

			//send a message to the elevator
			Message_Int2* pMsg = new Message_Int2;
			pMsg->GetData().id = Msg_ElevatorMoveToFloor;
			pMsg->GetData().i1 = mpAI->Character()->ID();
			pMsg->GetData().i2 = end->elevatorFloor;
			gpSystem->Server()->SendMessageToObject( elevator->ID(), pMsg );

		}
		return true;
	default:
		return false;
	}
	return true;
}

// Update the goal
void Goal_RideElevator::Update( float secs_elapsed )
{
	if (GoalStatus()!=statusInProgress) return;

	//if we haven't been activated yet, then we need to plan (replan) our subgoals
	if (!active){
		ReplanSubgoals();
		active = true;
		return;
	}
	
   typeGoalStatus status = UpdateSubgoals( secs_elapsed ); //update the subgoals
	if (status==statusFailed) {
		mGoalStatus = statusFailed;
		return; 
	}

	if (!NoSubgoals()) return; //if we still have some subgoals, don't do anything else

	if (stage == RidingElevator){
		if (elevator->FinishedMoving()){
			if (elevator->GetDestinationFloor() == end->elevatorFloor)
				mGoalStatus = statusSucceeded; //we're done once the elevator has arrived at our floor
			else
				ReplanSubgoals(); //this will send the elevator to go to the destination floor
		}
		else
			mpAI->Stop(); //don't move until the elevator reaches the destination
		return;
	}
	else if (stage == CallingElevator){
		int startFloor = entryLink->End()->elevatorFloor;
		//if the elevator's going to the wrong floor, we failed, so replan
		if (startFloor != elevator->GetDestinationFloor()){
			ReplanSubgoals();
			return;
		}
		//wait for the elevator to arrive at our floor
		if (!elevator->FinishedMoving()){
			mpAI->Stop();
			mpAI->TurnTo(entryLink->End()->pos);
			return;
		}
	}
	stage++; //we presumably finished the last stage since we have no subgoals, so increment the stage
	ReplanSubgoals(); //plan the next stage of the elevator use
}
