#include "Goal.h"

class PathNode;
class AI;

class Goal_FollowLink : public Goal, public GoalQueue
{
public:

	enum LinkType { Normal, Jump, JumpOverGap, Shimmy, MaxLinkTypes };
	Goal_FollowLink( AI* pAI, const PathLink *pathlink);

	virtual ~Goal_FollowLink();

	virtual bool Success();

	// Update the goal
	virtual void Update( float secs_elapsed );

	void DoLedgeShimmy();
	void DoJumpOverGap();

	float ExpectedLinkSpeed() const;

protected:
	bool active;
	bool jumped;
	float stuckTimer;
	bool TooFarBelowLink(const CVector& curPos) const;
	const PathLink *link;

	LinkType linkType;
};

#endif // _Goal_FollowLink_H_