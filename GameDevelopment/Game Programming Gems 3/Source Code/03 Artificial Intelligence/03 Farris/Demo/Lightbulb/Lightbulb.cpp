// Non-Local Includes
#include "..\Global.h"

// Local Includes
#include "LightBulb.h"

// Constructor
CLightBulb::CLightBulb() : CFSM()
{
	// Set States
	m_StateOn.Set(this,BeginStateOn,StateOn,EndStateOn);
	m_StateOff.Set(this,BeginStateOff,StateOff,EndStateOff);
}

//=====================================================================================================================
// Input Event Functions

// Switch On Event
void CLightBulb::SwitchOnEvent()
{
	// Check State
	if (IsState(m_StateOff))
		// Goto On State
		GotoState(m_StateOn);
}

// Switch Off Event
void CLightBulb::SwitchOffEvent()
{
	// Check State
	if (IsState(m_StateOn))
		// Goto Off State
		GotoState(m_StateOff);
}

//=====================================================================================================================
// State Functions

// State Initial
void CLightBulb::StateInitial()
{
	// Goto Off
	cout << "State:  Initial" << endl;
	GotoState(m_StateOff);
}

// Begin State On
void CLightBulb::BeginStateOn()
{
	cout << "State:  On" << endl;
}

// State On
void CLightBulb::StateOn()
{
}

// End State On
void CLightBulb::EndStateOn()
{
}

// Begin State Off
void CLightBulb::BeginStateOff()
{
	cout << "State:  Off" << endl;
}

// State Off
void CLightBulb::StateOff()
{
}

// End State Off
void CLightBulb::EndStateOff()
{
}
