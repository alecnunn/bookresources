#ifndef _lightbulb_lightbulb_h
#define _lightbulb_lightbulb_h

// Non-Local Includes
#include "..\FSM\FSM.h"

// Lightbulb Class
class CLightBulb : public CFSM
{
protected:
	CStateTemplate<CLightBulb> m_StateOn;					// On State
	CStateTemplate<CLightBulb> m_StateOff;					// Off State

public:
	// Constructor
	CLightBulb();

	// Input Event Functions
	virtual void SwitchOnEvent();
	virtual void SwitchOffEvent();

	// State Functions
	virtual void StateInitial();
	virtual void BeginStateOn();
	virtual void StateOn();
	virtual void EndStateOn();
	virtual void BeginStateOff();
	virtual void StateOff();
	virtual void EndStateOff();
};

#endif
