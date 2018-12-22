#ifndef _lightbulb_flashinglightbulb_h
#define _lightbulb_flashinglightbulb_h

// Local Includes
#include "Lightbulb.h"

// Flashing LightBulb Class
class CFlashingLightBulb : public CLightBulb
{
protected:
	CStateTemplate<CFlashingLightBulb> m_StateOnDim;			// State On Dim
	unsigned int m_uTimer;

public:
	// Constructor
	CFlashingLightBulb();

	// Input Functions
	virtual void SwitchOffEvent();

	// State Functions
	virtual void BeginStateOn();
	virtual void StateOn();
	virtual void BeginStateOnDim();
	virtual void StateOnDim();
	virtual void EndStateOnDim();
};

#endif