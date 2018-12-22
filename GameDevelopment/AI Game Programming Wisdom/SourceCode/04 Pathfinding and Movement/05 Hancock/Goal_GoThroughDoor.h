

class Goal_GoThroughDoor : public Goal, public GoalStack
{
public:
	Goal_GoThroughDoor( AI* pAI, const PathLink *pathlink);

	virtual ~Goal_GoThroughDoor();

	// Update the goal
	virtual void Update( float secs_elapsed );

	virtual bool ReplanSubgoals();

protected:
	const PathLink *link;
	const Door *door;
	bool active; //has this goal been active?
};

