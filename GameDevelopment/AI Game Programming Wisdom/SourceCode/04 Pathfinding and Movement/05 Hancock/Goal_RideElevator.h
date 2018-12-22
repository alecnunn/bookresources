
#include "Goal.h"

class PathNode;
class PathLink;
class AI;
class Elevator;

class Goal_RideElevator : public Goal, public GoalQueue
{
public:
	enum Stage { CallingElevator = 0, EnteringElevator, RidingElevator };
	Goal_RideElevator( AI* pAI, const PathLink *entryLink_, const PathNode *endNode);

	virtual ~Goal_RideElevator();

	// Update the goal
	virtual void Update( float secs_elapsed );

	virtual bool ReplanSubgoals();

protected:
	int stage;
	const Elevator *elevator;

	const PathLink *entryLink;
	const PathNode *end;
	bool active; //has this goal been active?
};

#endif // _TAIGoal_RideElevator_H_