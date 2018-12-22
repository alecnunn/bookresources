
#include "GoThroughDoor.h"
#include "Goal_GotoNode.h"
#include "Goal_FollowLink.h"
#include "NodeMapPrims.h"
#include "Goal_HitDoorSwitch.h"
#include "Door.h"

Goal_GoThroughDoor::Goal_GoThroughDoor(AI* pAI, const PathLink *pathlink)
										: Goal( pAI ), GoalQueue(), link(pathlink), active(false)
{
	LAssert(link->flags & PathLink::flagLinkDoor); //make sure the link was labeled with a door
	door = link->GetDoor();
	LAssert(door);
}

Goal_GoThroughDoor::~Goal_GoThroughDoor()
{
}

bool Goal_GoThroughDoor::ReplanSubgoals(){

	Door::DoorStatus doorStatus = door->GetDoorStatus();

	const PathNode *bestswitchnode = NULL;
	const Switch *bestswitch = NULL;
	//Now find the switch that is in the same noDoor region as the door entrance node
	for (int i=0; i < door->NumControllers(); i++){
		const CPathNode *sn = g_NodeMap.GetSwitchLocation(door->GetController(i));
		if (sn && sn->NoDoorsBetween(link->Start())){
			bestswitchnode = sn;
			bestswitch = door->GetController(i);
		}
	}
	if (bestswitchnode){ //otherwise, we're going to assume the door is nudgeable
		NewSubgoal(new Goal_HitDoorSwitch(mpAI, bestswitchnode, door)); //nudge the switch
	}
	NewSubgoal(new Goal_GotoNode(mpAI, link->Start()));
	NewSubgoal(new Goal_FollowLink(mpAI, link));
	
	return true;
}

// Update the goal
void Goal_GoThroughDoor::Update( float secs_elapsed )
{
	//if we haven't been activated yet, then we need to plan (replan) our subgoals
	if (!active){
		ReplanSubgoals();
		active = true;
		return;
	}
	
   mGoalStatus = UpdateSubgoals( secs_elapsed ); //update subgoals and set our status
}
