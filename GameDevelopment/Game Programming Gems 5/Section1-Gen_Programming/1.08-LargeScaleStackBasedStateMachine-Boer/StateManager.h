/***********************************************************\
Copyright (C) James Boer, 2004. 
All rights reserved worldwide.

This software is provided "as is" without express or implied
warranties. You may freely copy and compile this source into
applications you distribute provided that the copyright text
below is included in the resulting source code, for example:
"Portions Copyright (C) James Boer, 2004"
\***********************************************************/

#ifndef STATE_MANAGER_H__
#define STATE_MANAGER_H__
#ifdef _MSC_VER
#pragma once
#endif


#include <string>
#include <vector>
#include <map>
#include <queue>

#pragma warning(disable:4786)
#pragma warning(disable:4251)

const int MAX_STATE_STRING_LENGTH = 64;

// Base state class
//----------------------------------------------------------------------------
// IBaseState
//----------------------------------------------------------------------------
class IBaseState
{
public:
	virtual ~IBaseState() {}

	virtual void OnEnter(const char* szPrevious) = 0;
	virtual void Update() = 0;
	virtual void OnExit(const char* szNext) = 0;

	virtual void OnOverride(const char* szNext) = 0;
	virtual void OnResume(const char* szPrevious) = 0;
};


//----------------------------------------------------------------------------
// StateManager
//----------------------------------------------------------------------------
class StateManager
{
public:
	StateManager();
	~StateManager();

	void Init();
	void Term();

	// Register a state object and associate it with a string identifier
	bool RegisterState(const char* szStateName, IBaseState* pState);

	// Checks if the current state will change
	// on the next update cycle
	bool IsStateChangePending() const;

	// Returns the current state
	const char* GetState() const;

	// Get the state object based on the string ID
	IBaseState* GetStateClass(const char* szState);

	// Get the state object on top of the current state stack
	IBaseState* GetCurrentStateClass();

	// Returns the size of the state stack
	int GetStateStackSize() const;

	// Passing bFlush = true will override any previous state changing
	// commands that may be pending.  Otherwise, state commands
	// will queue and be executed in the order of the calls made.

	// Changes the current state on the next update cycle.  
	void ChangeState(const char* szState, float fDelay = 0.0f, bool bFlush = false);

	// Pushes a new state on top of the existing one on the next update cycle.
	void PushState(const char* szState, float fDelay = 0.0f, bool bFlush = false);

	// Pops off the current state or states to reveal a stored state 
	// underneath.  You may not pop off the last state
	void PopState(int iStatesToPop = 1, float fDelay = 0.0f, bool bFlush = false);

	// Pops all but the last state.
	void PopAllStates(float fDelay = 0.0f, bool bFlush = false);
	//----------------------

	// Updates the state machine internal mechanism.  This function is called 
	// once by the main update loop and should not be called by anyone else.
	void Update(float dt);



private:

	// private enums, structs, and typedefs
	enum GS_CMD_TYPE
	{
		GS_CMD_PUSH,
		GS_CMD_CHANGE,
		GS_CMD_POP,
		GS_CMD_POP_ALL
	};

	struct GSCommand
	{
		GS_CMD_TYPE		eCmd;
		char			szState[MAX_STATE_STRING_LENGTH];
		float			fDelay;
	};

	typedef std::vector<std::string> GameStateVector;
	typedef std::map<std::string, IBaseState*> GameStateClassMap;
	typedef std::queue<GSCommand> GSCmdQueue;


	// Private functions
	void Clear();

	// Private class data
	GameStateVector         m_CurrentState;
	GameStateClassMap	    m_State;
	GSCmdQueue              m_CmdQueue;
	bool					m_bFirstUpdate;
	bool					m_bInitialized;
	float					m_fCurrentTime;

};



#endif // STATE_MANAGER_H__
