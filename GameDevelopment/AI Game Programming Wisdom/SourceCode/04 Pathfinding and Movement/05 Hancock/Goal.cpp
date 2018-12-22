
#include "Goal.h"

GoalQueue::GoalQueue()
{
}

GoalQueue::~GoalQueue()
{
	ResetSubgoals();
}

typeGoalStatus GoalQueue::UpdateSubgoals( float secs_elapsed )
{
	if (mGoals.empty()) return statusSucceeded; //return success when no goals left
	
	mGoals.front()->Update( secs_elapsed );
	
	//if the goal is in progress, simply return this status
	//if the goal has failed, clear the entire subgoal list
	//if the goal has succeeded, remove it, return 
	typeGoalStatus status = mGoals.front()->GoalStatus();
	
	switch (status){
	case statusInProgress:
		break;
	case statusFailed:
		ResetSubgoals();
		return statusFailed; //return failure
		break;
	case statusSucceeded:
		delete mGoals.front();
		mGoals.pop_front();
		break;
	}
	
	//returns statusInProgress if we didn't fail and there are still goals on the list
	return statusInProgress; 
}

void GoalQueue::NewSubgoal( Goal* pGoal )
{
	mGoals.push_back( pGoal );
}

void GoalQueue::InsertSubgoal( Goal* pGoal )
{
	mGoals.push_back( pGoal );
}

void GoalQueue::ResetSubgoals() 
{ 
	while (!mGoals.empty()){
		Goal *pGoal = mGoals.back();
		delete pGoal;
		mGoals.pop_back();
	}
}

