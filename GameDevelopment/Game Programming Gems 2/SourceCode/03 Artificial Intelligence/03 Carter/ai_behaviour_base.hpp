#pragma once

#ifndef __AI_BEHAVIOUR_BASE_HPP__
#define __AI_BEHAVIOUR_BASE_HPP__

class CAIBrain;
class CEntity;

class CAITerminateException
{
};

enum EAIReturn
{
	AI_OK,
	AI_FAIL,
	AI_TERMINATE,
};

class CAIBehaviour
{
protected:
	CAIBrain*	PBrain;
	CEntity*	PEntity;
	bool		Terminated;	  
	int			Priority;
	std::string	Name;

protected:
	EAIReturn Action_MoveToPos(const C2DCoordF& pos);
	EAIReturn Action_Sleep(signed int amount);
	

public:
	void	Init(CAIBrain& brain, CEntity& creature, int priority, const std::string& name)
	{
		PBrain = &brain;
		PEntity = &creature;
		Terminated = false;
		Name = name;
		Priority = priority;
	}
	
	virtual void	Update(void) {};
	virtual void	Cleanup(void)	{};
	virtual bool	IsRunnable(void)	{ return(true); }
	virtual void	OnActivate(void) {};
	
	void			Terminate(void)				{ Terminated = true; };
	void			ClearTerminateFlag(void)	{ Terminated = false; };
	bool			IsTerminated(void) const	{ return(Terminated); }
	int				GetPriority(void) const		{ return(Priority); };
	const std::string& GetName(void) const		{ return(Name); };
};

#endif //__AI_BEHAVIOUR_BASE_HPP__