#include "Goal_FollowLink.h"


Goal_FollowLink::Goal_FollowLink( AI* pAI, const PathLink *pathlink)
											  : Goal( pAI ), GoalQueue(), link(pathlink),
											  linkType(Normal), active(false), jumped(false),
											  stuckTimer(0.0f)
{	
	//turn towards the jump first if it's a jump link
	if (link->flags & PathLink::flagLinkJumpGap){
		linkType = JumpOverGap;
	}
	else if (link->flags & PathLink::flagLinkDoor)
		linkType = Door;
	else if (link->Start()->flags & PathNode::flagNodeLedgeHang && link->End()->flags & PathNode::flagNodeLedgeHang){
		linkType = Shimmy;
	}
}

Goal_FollowLink::~Goal_FollowLink()
{
}

bool Goal_FollowLink::Success() {
	bool bDone = false;

	closeEnough = link->End()->radius; //make sure we get within the radius of the destination node

	//check if we're within the radius of the node
	const Vector3& curPos = mpAI->WorldPos();
	if (curPos.distance( link->End()->pos ) < closeEnough ) {
		bDone = true;
	}

	//make sure we're actually hanging if the nodemap says we're supposed to do a hang
	//before we declare we're done
	if (bDone){
		//if we're supposed to be ledge-hanging
		if (link->End()->flags & PathNode::flagNodeLedgeHang){
			bDone = mpAI->AmILedgeHanging();
		}
	}

	return bDone;
}

void Goal_FollowLink::DoJumpOverGap() {
	Vector3 forwardDir = mpAI->ForwardDir();
	const Vector3& curPos = mpAI->WorldPos();

	//make sure we're facing the right direction before we run off the cliff
	float hdot = forwardDir.dot(link->End()->pos - curPos);
	if (hdot > 0.8){
		if (GetAtCliff()){
			if (!jumped){
				mpAI->Jump();
				active->jumped = true;
			}
		}
		mpAI->Servo(link->End()->pos, true);
	}
	else
		mpAI->TurnTo(link->End()->pos); //make sure we're
}

// Update the goal
void Goal_FollowLink::Update( float secs_elapsed )
{	
   typeGoalStatus status = UpdateSubgoals( secs_elapsed );

   if (status==statusFailed) {
		mGoalStatus = statusFailed;
		return; 
	}
	
   if (!NoSubgoals()) return;

	if (Success()) {
      return; 
	}

	const Vector3& curPos = mpAI->WorldPos();

	if (!active){

		mpAI->Crouch(link->End()->flags & PathNode::flagNodeCrouch);

		if (link->flags & PathLink::flagLinkJump) //clear any current motion before a jump
			mpAI->DefaultStance();

		return;
	}

	bool outsideDangerousLink = (link->flags & PathLink::flagLinkNearCliff && !link->InsideFatLink(curPos));

	switch(linkType){
		//while following complicated link types, we want to tell the AI to not shutdown the owning state
	case Door:
		GoThroughDoor();
		break;
	case Jump:
		DoJump();
		break;
	case JumpOverGap:
		DoJumpOverGap();
		break;
	case Shimmy:
		mpAI->DoLedgeShimmy(link->Direction());
		break;
	case Normal:
		{
			//just head towards the goal (the character may change its speed based on outsideDangerousLink)
			mpAI->Servo(link->End()->pos, outsideDangerousLink);
		}
		break;
	}
	

   if (mpAI->IsStuck())
		stuckTimer += secs_elapsed;
	//if we're too far from the link or we've been stuck for more than a second, report a failure
	if (TooFarFromLink(curPos) || (active->stuckTimer > 1.0)) {
		mGoalStatus = statusFailed;
		link->ReportLinkFailure();
	}
}
