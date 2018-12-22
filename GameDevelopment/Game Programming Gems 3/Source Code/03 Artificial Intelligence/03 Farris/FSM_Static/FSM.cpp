// Local Includes
#include "FSM.h"

// Constructor
CFSM::CFSM()
{
	// Initialize States
	m_StateInitial.Set(this,BeginStateInitial,StateInitial,EndStateInitial);
	// Initialize State Machine
	m_pCurrentState=static_cast<CState*>(&m_StateInitial);
	m_pNewState=0;
}

//======================================================================================================
// Global Functions

// Update
void CFSM::Update()
{
	// Check New State
	if (m_pNewState)
	{
		// Execute End State
		m_pCurrentState->ExecuteEndState();
		// Set New State
		m_pCurrentState=m_pNewState;
		m_pNewState=0;
		// Execute Begin State
		m_pCurrentState->ExecuteBeginState();
	}
	// Execute State
	m_pCurrentState->ExecuteState();
}

//======================================================================================================
// State Functions

// Is State
bool CFSM::IsState(CState &State)
{
	return (m_pCurrentState==&State);
}

// Goto State
bool CFSM::GotoState(CState &NewState)
{
	// Set New State
	m_pNewState=&NewState;
	return true;
}

