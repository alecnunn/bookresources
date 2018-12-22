#include "standard_headers.hpp"
#include "ai_behaviour_hungry.hpp"
#include "entities.hpp"
#include "intelligent_pointer.hpp"

//************************************************************************************************
bool	CAIBehaviourHungry::IsRunnable(void)
{
	CEntity* pfood = PEntity->FindNearestEntity(ENTITY_FOOD);

	if(pfood)
	{
		if(Get2DDistance(pfood->GetPos(), PEntity->GetPos()) < 30)
		{
			return(true);
		}
	}

	return(false);
}


//************************************************************************************************
void	CAIBehaviourHungry::Update(void)
{
	CIntelligentPointer<CEntity> pfood = PEntity->FindNearestEntity(ENTITY_FOOD);
	
	if(!pfood.IsNull())
	{
		int food_index = pfood->GetID();
		if(Action_MoveToPos(pfood->GetPos())==AI_OK)
		{
			if(!pfood.IsNull())
			{
				pfood->Kill();
			}
			Terminate();
		}
	}

}
