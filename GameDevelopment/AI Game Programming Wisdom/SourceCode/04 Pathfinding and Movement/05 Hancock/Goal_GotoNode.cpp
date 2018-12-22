

Goal_GotoNode::Goal_GotoNode( AI* pAI, const PathNode *destination, 
											  bool bForceDirectPath /*= false*/)
											  : Goal( pAI ), GoalQueue(), node(destination),
											  active(false), directPath(bForceDirectPath)
												
{
}

Goal_GotoNode::~Goal_GotoNode()
{
}

bool Goal_GotoNode::ReplanSubgoals(){
	ResetSubgoals();

	stuckTimer = 0.0f;

	if (directPath){ //don't plan a path if we've been told to make a beeline for the goal
		return true;
	}
	
	//find the closest node to our current position
	const PathNode *nodeStart = g_NodeMap.FindClosestVisibleNode(mpAI->WorldPos());


	if (!nodeStart){
		SetStatus( statusFailed );
		return false; //we couldn't find a node near us, abandon all hope!
	}
	
	float pathLength = mpAI->GeneratePathingGoals(nodeStart, node, *this);
	if (pathLength < 0){
		SetStatus( statusFailed );
		return false;
	}

	return true;
}


bool Goal_GotoNode::Success() {	
   return (mpAI->WorldPos().distance(node->pos) < node->radius);
}


// Update the goal
void Goal_GotoNode::Update( float secs_elapsed )
{	
	typeGoalStatus status = UpdateSubgoals( secs_elapsed );
	
	if (status==statusFailed) {
		SetStatus( statusFailed );	
		return; 
	}
	
	if (!NoSubgoals()) return;
	
	if (Success()) {
		SetStatus( statusSucceeded );
		return; 
	}

	if (!active){ //if we weren't previously active, we should now generate a plan to goto the node
		ReplanSubgoals();
		active = true;
		return;
	}
	
	mpAI->Crouch(node->flags & PathNode::flagNodeCrouch);
	
	mpAI->Servo(node->pos);
	
	if (mpAI->IsStuck())
		stuckTimer += secs_elapsed;
	
	//check if we are stuck or not
	if (stuckTimer > 1.0) {
		SetStatus( statusFailed ); /*ReplanSubgoals();*/
		return; 
	}
}
