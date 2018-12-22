#ifndef _lightbulb_realisticlightbulb_h
#define _lightbulb_realisticlightbulb_h

// Local Includes
#include "Lightbulb.h"

// Realistic LightBulb Class
class CRealisticLightBulb : public CLightBulb
{
protected:
	CStateTemplate<CRealisticLightBulb> m_StateBurnedOut;
	unsigned int m_uLifeSpan;							

public:
	// Constructor
	CRealisticLightBulb();

	// State Functions
	virtual void StateOn();
	virtual void BeginStateBurnedOut();
	virtual void StateBurnedOut();
	virtual void EndStateBurnedOut();
};

#endif