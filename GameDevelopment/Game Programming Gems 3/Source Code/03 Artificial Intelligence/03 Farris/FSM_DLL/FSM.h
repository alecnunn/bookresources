#ifndef _fsm_fsm_h
#define _fsm_fsm_h

// Local Includes
#include "Main.h"
#include "State.h"

// Forward Declarations
class CFSM;

// Template Instantiation
#pragma warning( disable : 4231 )  
extern template class FSM_API CStateTemplate<CFSM>;
  
// FSM Class
class FSM_API CFSM
{
protected:
	CState *m_pCurrentState;						// Current State
	CState *m_pNewState;							// New State
	CStateTemplate<CFSM> m_StateInitial;			// Initial State

public:
	// Constructor
	CFSM();

	// Destructor
	virtual ~CFSM() {}

	// Global Functions
	virtual void Update();

	// State Functions
	bool IsState(CState &State);
	bool GotoState(CState &NewState);

	virtual void BeginStateInitial() {}
	virtual void StateInitial() {}
	virtual void EndStateInitial() {}
};

#endif
