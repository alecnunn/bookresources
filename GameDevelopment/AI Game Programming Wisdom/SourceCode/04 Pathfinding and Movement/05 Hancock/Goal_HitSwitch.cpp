

Goal_HitSwitch::Goal_HitSwitch(AI* pAI, const PathNode *switchNode_) : Goal( pAI ), GoalQueue()
 switchNode(switchNode_), timer(0.0f), active(false)
{
	Assert(switchNode->flags & CPathNode::flagNodeSwitch);
	const CEntity *pEntity = switchNode->GetEntity();
	pSwitch = pEntity->Switch();
	Assert(pSwitch);
}

bool Goal_HitSwitch::ReplanSubgoals(){
	ResetSubgoals();
	NewSubgoal(new Goal_GotoNode(mpAI, switchNode)); //goto the switch
	NewSubgoal(new Goal_TurnToEntity(mpAI, pSwitch)); //turn towards the switch
	return true;
}

Goal_HitSwitch::~Goal_HitSwitch()
{
}

// Update the goal
void Goal_HitSwitch::Update( float secs_elapsed )
{	
	if (!active){
		ReplanSubgoals();
		active = true;
	}

	typeGoalStatus status = UpdateSubgoals( secs_elapsed );
	if (status==statusFailed) {
		mGoalStatus = statusFailed;	
		return; 
	}

	if (!NoSubgoals())
		return;

	mpAI->Nudge(pSwitch->ID()); //send an activation message to the switch

	mGoalStatus = statusSucceeded;
}
