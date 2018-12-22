#include "standard_headers.hpp"
#include "ai_behaviour_sleep.hpp"
#include "entities.hpp"
#include "intelligent_pointer.hpp"

//************************************************************************************************
bool	CAIBehaviourSleep::IsRunnable(void)
{
	return(PEntity->GetTiredness()>20000);
}


//************************************************************************************************
void	CAIBehaviourSleep::Update(void)
{
	if(PEntity->GetTiredness()>20000)
	{
		if(Action_Sleep(PEntity->GetTiredness())==AI_OK)
		{
			Terminate();
		}
	}
}
