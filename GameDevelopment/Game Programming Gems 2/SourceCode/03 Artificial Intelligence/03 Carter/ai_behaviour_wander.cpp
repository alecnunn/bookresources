#include "standard_headers.hpp"
#include "ai_behaviour_wander.hpp"
#include "entities.hpp"
#include "ai_brain.hpp"



void	 CAIBehaviourWander::Update(void)
{
	C2DCoordF pos = PEntity->FindWanderPos();

	Action_MoveToPos(pos);
}
