#include "standard_headers.hpp"
#include "ai_behaviour_base.hpp" 
#include "entities.hpp"
#include "ai_brain.hpp"

EAIReturn CAIBehaviour::Action_MoveToPos(const C2DCoordF& pos)
{
	bool reached_destination = false;
	
	while(!Terminated && !reached_destination)
	{
		if(PEntity->MoveTowardsPos(pos))
		{
			reached_destination = true;
		}
		
		PBrain->SuspendThread();
	}
	
	if(Terminated)
	{
		return(AI_TERMINATE);
	}
	
	return(AI_OK);
}

EAIReturn CAIBehaviour::Action_Sleep(signed int amount)
{
	bool reached_destination = false;
	unsigned int amount_to_get_to = PEntity->GetTiredness()-amount;
	
	while(!Terminated && PEntity->GetTiredness()>amount_to_get_to)
	{
		PEntity->Sleep(4);
		if(PEntity->GetTiredness()<=amount_to_get_to)
		{
			PBrain->SuspendThread();
			break;
		}
		PBrain->SuspendThread();
	}
	
	if(Terminated)
	{
		return(AI_TERMINATE);
	}
	
	return(AI_OK);
}

