#pragma once

#ifndef __AI_BEHAVIOUR_HUNGRY_HPP__
#define __AI_BEHAVIOUR_HUNGRY_HPP__

#include "ai_behaviour_base.hpp"

class CAIBehaviourHungry : public CAIBehaviour
{
public:
	bool	IsRunnable(void);
	void	Update(void);

};

#endif //__AI_BEHAVIOUR_HUNGRY_HPP__