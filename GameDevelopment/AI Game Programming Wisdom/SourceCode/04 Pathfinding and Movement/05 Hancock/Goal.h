
enum typeGoalStatus { statusSucceeded = 0, statusFailed, statusInProgress };

class Goal
{
public:
	Goal( AI* pAI );
	virtual ~Goal();
	
	// Update the goal
	virtual void Update( float secs_elapsed ) = 0;
	
	typeGoalStatus GoalStatus() { return mGoalStatus; }
	
	virtual void SetStatus(typeGoalStatus GoalStatus) {mGoalStatus = GoalStatus;}
	
	AI* GetAI() { return mpAI; }
	
protected:
	Goal(AI *pAI) : mpAI(pAI), mGoalStatus(statusInProgress){}
	Goal(){mGoalStatus = statusInProgress;}	//for virtual constructor purposes
	AI*           mpAI;		     // The AI we are controlling
	typeGoalStatus mGoalStatus;  // What is are current status (default is statusInProgress)
};


class GoalQueue
{
public:
	GoalQueue();
	virtual ~GoalQueue();
	
	typeGoalStatus UpdateSubgoals( float secs_elapsed );
	bool NoSubgoals() const { return mGoals.empty(); }
	
	void InsertSubgoal (Goal *pGoal); //adds a goal to the front of the queue
	void NewSubgoal( Goal* pGoal ); //adds a goal to the back of the queue
	
	void ResetSubgoals(); //deletes all current subgoals
	
	virtual bool ReplanSubgoals() { return false; }
	
	Goal*   ActiveGoal() { return mGoals.empty() ? NULL : mGoals.front(); }
	
protected:
	std::list<Goal *> mGoals;
};



