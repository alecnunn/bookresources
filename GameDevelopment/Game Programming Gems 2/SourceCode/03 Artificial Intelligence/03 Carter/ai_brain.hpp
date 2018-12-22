#pragma once

#ifndef __BRAIN_HPP__
#define __BRAIN_HPP__

#include "ai_behaviour_base.hpp"
#include "microthreads.h"

class CEntity;

using namespace MicroThread;

//-------------------------------------------------------------------
/*class CAIBrainEntry
{
protected:
	int Priority;
	CAIBehaviour* PBehaviour;
	std::string Name;

public:
	CAIBrainEntry()	{};
	CAIBrainEntry(int priority, CAIBehaviour& behaviour)
		: Priority(priority), PBehaviour(&behaviour)	{}
	//~CAIBrainEntry(void)	{ delete PBehaviour; }

	int				GetPriority() const		{ return(Priority); }
	CAIBehaviour& 	GetBehaviour() const { return(*PBehaviour); }
};	 */

//-------------------------------------------------------------------
class CAIBrain
{
protected: //Data
	std::vector<CAIBehaviour*>	BehaviourList;
	CAIBehaviour*				PActiveBehaviour;
	CAIBehaviour*				PPendingBehaviour;
	MicroThreadInfo*			PMicroThread;
	bool						TerminateThread;
	CEntity*					PEntity;
	bool						ActiveBehaviourRunning;
	bool						ThreadRunning;
	
protected: //Methods
	CAIBehaviour*		FindNextBehaviourToUse(int priority);
	void				InitThread();
	void				UninitThread();
	void				RunThread();
	static void			ThreadFunc(void* pcontext);
	void				ThreadUpdate();
	void				TerminateActiveBehaviour();
		
	
public:
	CAIBrain(CEntity& entity);
	//CAIBrain(void);
	virtual ~CAIBrain(void);

	void AddBehaviour(int priority, CAIBehaviour* behaviour, const std::string& name);
	void Update(void);
	void SuspendThread();
	std::string GetCurrentBehaviourName(void) const;
};


#endif //__BRAIN_HPP__