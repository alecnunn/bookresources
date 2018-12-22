// Non-Local Includes
#include "..\Global.h"

// Local Includes
#include "FlashingLightBulb.h"

// Constructor
CFlashingLightBulb::CFlashingLightBulb() : CLightBulb()
{
	// Set States
	m_StateOnDim.Set(this,BeginStateOnDim,StateOnDim,EndStateOnDim);
}

//=====================================================================================================================
// Input Event Functions

// Switch Off Event
void CFlashingLightBulb::SwitchOffEvent()
{
	// Check State
	if (IsState(m_StateOn) || IsState(m_StateOnDim))
		// Goto Off State
		GotoState(m_StateOff);
}

//=====================================================================================================================
// State Functions

// Begin State On
void CFlashingLightBulb::BeginStateOn()
{
	CLightBulb::BeginStateOn();
	// Set Interval
	m_uTimer=10;
}

// State On
void CFlashingLightBulb::StateOn()
{
	// Update Interval
	--m_uTimer;
	if (m_uTimer==0)
		// Goto On Dim
		GotoState(m_StateOnDim);
}

// Begin State On Dim
void CFlashingLightBulb::BeginStateOnDim()
{
	cout << "State:  On Dim" << endl;
	// Set Interval
	m_uTimer=10;
}

// State On Dim
void CFlashingLightBulb::StateOnDim()
{
	// Update Interval
	--m_uTimer;
	if (m_uTimer==0)
		// Goto On
		GotoState(m_StateOn);
}

// End State On Dim
void CFlashingLightBulb::EndStateOnDim()
{
}

