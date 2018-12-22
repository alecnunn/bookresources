// Non-Local Includes
#include "..\Global.h"

// Local Includes
#include "RealisticLightBulb.h"

// Constructor
CRealisticLightBulb::CRealisticLightBulb() : CLightBulb()
{
	// Set States
	m_StateBurnedOut.Set(this,BeginStateBurnedOut,StateBurnedOut,EndStateBurnedOut);
	// Set LifeSpan
	m_uLifeSpan=100;
}

//=====================================================================================================================
// State Functions

// State On
void CRealisticLightBulb::StateOn()
{
	// Update Interval
	--m_uLifeSpan;
	if (m_uLifeSpan==0)
		// Goto Burned Out
		GotoState(m_StateBurnedOut);
}

// Begin State Burned Out
void CRealisticLightBulb::BeginStateBurnedOut()
{
	cout << "State:  Burned Out" << endl;
}

// State Burned Out
void CRealisticLightBulb::StateBurnedOut()
{
}

// End State Burned Out
void CRealisticLightBulb::EndStateBurnedOut()
{
}

