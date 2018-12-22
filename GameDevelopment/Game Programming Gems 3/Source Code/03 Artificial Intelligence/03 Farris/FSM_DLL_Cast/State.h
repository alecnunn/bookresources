#ifndef _fsm_state_h
#define _fsm_state_h

// System Includes
#include <assert.h>

// Local Includes
#include "Main.h"

//==================================================================================================
// CState

// CState Class
class FSM_API CState
{
public:
	// Destructor
	virtual ~CState() {}

	// State Functions
	virtual void ExecuteBeginState(void *pInstance)=0;
	virtual void ExecuteState(void *pInstance)=0;
	virtual void ExecuteEndState(void *pInstance)=0;
};

//==================================================================================================
// CStateTemplate

// CStateTemplate Class
template <class T>
class FSM_API CStateTemplate : public CState
{
protected:
	typedef void (T::*PFNSTATE)(void);
	PFNSTATE m_pfnBeginState;						// State Function Pointer
	PFNSTATE m_pfnState;							// State Function Pointer
	PFNSTATE m_pfnEndState;							// State Function Pointer

public:
	// Constructors
	CStateTemplate() : m_pfnBeginState(0),m_pfnState(0),m_pfnEndState(0) {}
	CStateTemplate(PFNSTATE pfnBeginState,PFNSTATE pfnState,PFNSTATE pfnEndState)  : m_pfnBeginState(0),m_pfnState(0),m_pfnEndState(0)
	{
		// Set Function Pointers
		Set(pfnBeginState,pfnState,pfnEndState);
	}

	// Initialize Functions
	void Set(PFNSTATE pfnBeginState,PFNSTATE pfnState,PFNSTATE pfnEndState)
	{
		// Set Function Pointers
		assert(pfnBeginState);
		m_pfnBeginState=pfnBeginState;
		assert(pfnBeginState);
		m_pfnState=pfnState;
		assert(pfnBeginState);
		m_pfnEndState=pfnEndState;
	}

	// State Functions
	virtual void ExecuteBeginState(void *pInstance) 
	{
		// Begin State
		assert(pInstance && m_pfnBeginState);
		(static_cast<T*>(pInstance)->*m_pfnBeginState)();
	}
	virtual void ExecuteState(void *pInstance) 
	{
		// State
		assert(pInstance && m_pfnState);
		(static_cast<T*>(pInstance)->*m_pfnState)();
	}
	virtual void ExecuteEndState(void *pInstance)
	{
		// End State
		assert(pInstance && m_pfnEndState);
		(static_cast<T*>(pInstance)->*m_pfnEndState)();
	}
};

#endif
