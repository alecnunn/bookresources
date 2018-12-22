


class Goal_HitSwitch : public Goal, public GoalQueue //for hitting a switch
{
public:
	Goal_HitSwitch( AI* pAI, const PathNode *switchNode_);

	virtual ~Goal_HitSwitch();

	virtual bool Success();

	//replan subgoals
	virtual bool ReplanSubgoals();

	// Update the goal
	virtual void Update( float secs_elapsed );

protected:
	const Switch *pSwitch;
	bool active;
	const PathNode *switchNode;
};
