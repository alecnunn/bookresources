

class PathNode;
class TAI;

class Goal_GotoNode : public Goal, public GoalQueue
{
public:
	Goal_GotoNode( AI* pAI, const PathNode *destination, 
				      bool bForceDirectPath = false );
	virtual ~Goal_GotoNode();

	// Update the goal
	virtual void Update( float secs_elapsed );

	virtual bool Success();

	virtual bool ReplanSubgoals(); 

	const PathNode* GetDestinationNode() { return node; }

protected:
	bool active;
	float stuckTimer;
	const PathNode *node;
	bool directPath;
};
