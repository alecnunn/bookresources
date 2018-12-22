#include "standard_headers.hpp"
#include "ai_brain.hpp"
#include "entities.hpp"

//*****************************************************************************
CAIBrain::CAIBrain(CEntity& pentity)
 : PEntity(&pentity), PActiveBehaviour(NULL), PPendingBehaviour(NULL), TerminateThread(false), ActiveBehaviourRunning(false), ThreadRunning(true)
{
	InitThread();
}

//*****************************************************************************
CAIBrain::~CAIBrain()
{
	UninitThread();
}

//*****************************************************************************
void CAIBrain::AddBehaviour(int priority, CAIBehaviour* behaviour, const std::string& name)
{
	behaviour->Init(*this, *PEntity, priority, name);
	BehaviourList.push_back(behaviour);
}

//*****************************************************************************
CAIBehaviour* CAIBrain::FindNextBehaviourToUse(int priority)
{
	int best_priority = priority;
	CAIBehaviour* pbest_entry = NULL;
	
	//Go through all the registered behaviours, and find the highest priority one that is runnable.
	for(std::vector<CAIBehaviour*>::iterator it_entry=BehaviourList.begin(); it_entry!=BehaviourList.end(); ++it_entry)
	{
		CAIBehaviour& behaviour = (**it_entry);
		if(behaviour.GetPriority()>best_priority)
		{
			if(behaviour.IsRunnable())
			{
				pbest_entry = *it_entry;
			}
		}
	}

	return(pbest_entry);
}

//*****************************************************************************
void CAIBrain::Update(void)
{
	if(BehaviourList.size())
	{
		//See if we need to interrupt the current behaviour
		if(PActiveBehaviour!=NULL && !PActiveBehaviour->IsTerminated())
		{
			CAIBehaviour* pentry = FindNextBehaviourToUse(PActiveBehaviour->GetPriority());
			if(pentry)
			{
				PPendingBehaviour = pentry;
			}
		}

		//See if the current behaviour has finished, and get a new one
		if(PPendingBehaviour==NULL && (PActiveBehaviour==NULL || PActiveBehaviour->IsTerminated()))
		{
			PPendingBehaviour = FindNextBehaviourToUse(-1);
		}

		//If we are waiting to plug in a behaviour
		if(PPendingBehaviour)
		{
			//If a behaviour is running, ask it to stop
			if(PActiveBehaviour)
			{
				TerminateActiveBehaviour();
			}

			PPendingBehaviour->ClearTerminateFlag();
			PActiveBehaviour = PPendingBehaviour;
			PPendingBehaviour = NULL;
			PActiveBehaviour->OnActivate();
		}


		if(PActiveBehaviour)
		{
			RunThread();

			//If the active state group has finished, turn it off and clean it up.
			if(!ActiveBehaviourRunning && PActiveBehaviour->IsTerminated())
			{
				PActiveBehaviour->Cleanup();
				PActiveBehaviour = NULL;
			}

		}
	}
}

//*****************************************************************************
void CAIBrain::InitThread()
{
	PMicroThread = CreateMicroThread(ThreadFunc, (void*) this);
	TerminateThread = false;
	ThreadRunning = true;
}

//*****************************************************************************
void CAIBrain::UninitThread()
{
	TerminateActiveBehaviour();
	TerminateThread = true;
	
	while(ActiveBehaviourRunning)
	{
		RunThread();
	}
	DeleteMicroThread(PMicroThread);
}

//*****************************************************************************
void CAIBrain::RunThread()
{
	SwitchToMicroThread(PMicroThread);
}

//*****************************************************************************
void CAIBrain::SuspendThread()
{
	MicroThreadSleep();
}

//*****************************************************************************
void CAIBrain::TerminateActiveBehaviour()
{
	if(PActiveBehaviour)
	{
		PActiveBehaviour->Terminate();
		while(ActiveBehaviourRunning)
		{
			RunThread();
		}
		PActiveBehaviour->Cleanup();
		PActiveBehaviour->ClearTerminateFlag();
		PActiveBehaviour = NULL;
	}		
}

//*****************************************************************************
std::string CAIBrain::GetCurrentBehaviourName(void) const
{
	if(PActiveBehaviour)
	{
		return(PActiveBehaviour->GetName());
	}
	return("");
}

//*****************************************************************************
//*****************************************************************************
void CAIBrain::ThreadUpdate()
{
	//GAME_ASSERT(PActiveBehaviour!=NULL);
	
	
}

//*****************************************************************************
void CAIBrain::ThreadFunc(void* pcontext)
{
	CAIBrain* pthis = (CAIBrain*)(pcontext);
	
	while(!pthis->TerminateThread)
	{
		if(pthis->PActiveBehaviour)
		{
			pthis->ActiveBehaviourRunning = true;
			pthis->PActiveBehaviour->Update();
			pthis->ActiveBehaviourRunning = false;
		}
		
		pthis->SuspendThread();
	}
}


