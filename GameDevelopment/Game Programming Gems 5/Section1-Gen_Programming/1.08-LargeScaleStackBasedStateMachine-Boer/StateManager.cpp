/***********************************************************\
Copyright (C) James Boer, 2004. 
All rights reserved worldwide.

This software is provided "as is" without express or implied
warranties. You may freely copy and compile this source into
applications you distribute provided that the copyright text
below is included in the resulting source code, for example:
"Portions Copyright (C) James Boer, 2004"
\***********************************************************/


#pragma warning(disable:4786)
#pragma warning(disable:4251)

#include "StateManager.h"

#include <assert.h>



using namespace std;

const char* g_szInvalidState = "invalid state";


#define STATE_DEBUG_OUT printf

//----------------------------------------------------------------------------
// StateManager
//----------------------------------------------------------------------------

StateManager::StateManager()
{
	Clear();
}

StateManager::~StateManager()
{
	Term();
}

void StateManager::Clear()
{
	m_CurrentState.clear();
	m_State.clear();
	while(!m_CmdQueue.empty())
		m_CmdQueue.pop();
	m_bFirstUpdate = false;
	m_bInitialized = false;
	m_fCurrentTime = 0.0f;
}

void StateManager::Init()
{
	m_bFirstUpdate = true;
	m_bInitialized = true;
}

void StateManager::Term()
{
	if(!m_bInitialized)
		return;

    // Make sure we properly exit the currently active state
    for(int i = m_CurrentState.size() - 1; i >= 0; i--)
        GetStateClass(m_CurrentState[i].c_str())->OnExit(g_szInvalidState);

	Clear();
}


bool StateManager::RegisterState(const char* szStateName, IBaseState* pState)
{
	if(!m_bInitialized)
		return false;

	// Attempt to find the given string ID
	GameStateClassMap::iterator itr = m_State.find(szStateName);
	
	// If the string ID already exists in the state map, return an error
	if(itr != m_State.end())
		return false;

	// Assign the object pointer to the ID location in the map
	m_State[szStateName] = pState;

	// If this is the first state registered, set it as the default state
	if(m_State.size() == 1)
		m_CurrentState.push_back(szStateName);

	return true;
}


bool StateManager::IsStateChangePending() const
{
	return (m_CmdQueue.empty()) ? false : true;
}

const char* StateManager::GetState() const
{
	if (m_CurrentState.size() == 0)
		return 0;
	return m_CurrentState[m_CurrentState.size() - 1].c_str();
}

int StateManager::GetStateStackSize() const
{
	return m_CurrentState.size();
}


IBaseState* StateManager::GetStateClass(const char* szState)
{
	// Make sure the string is not too long or a null string
	assert(szState);
	assert(strlen(szState) < MAX_STATE_STRING_LENGTH);
	assert(!m_State.empty());
	return m_State[szState];
}

IBaseState* StateManager::GetCurrentStateClass()
{
	assert(!m_CurrentState.empty());
	return GetStateClass(m_CurrentState[m_CurrentState.size() - 1].c_str());
}

void StateManager::ChangeState(const char* szState, float fDelay, bool bFlush)
{
	// Make sure the string is not too long or a null string
	assert(strlen(szState) < MAX_STATE_STRING_LENGTH);
	assert(szState);
	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "change state" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_CHANGE;
	strcpy(cmd.szState, szState);
	cmd.fDelay = fDelay + m_fCurrentTime;
	m_CmdQueue.push(cmd);
}

void StateManager::PushState(const char* szState, float fDelay, bool bFlush)
{
	// Make sure the string is not too long or a null string
	assert(strlen(szState) < MAX_STATE_STRING_LENGTH);
	assert(szState);
	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "push state" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_PUSH;
	strcpy(cmd.szState, szState);
	cmd.fDelay = fDelay + m_fCurrentTime;
	m_CmdQueue.push(cmd);
}

void StateManager::PopState(int iStatesToPop, float fDelay, bool bFlush)
{
	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "pop state" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_POP;
	cmd.szState[0] = 0;
	cmd.fDelay = fDelay + m_fCurrentTime;
	while(iStatesToPop > 0)
	{
		m_CmdQueue.push(cmd);
		// Only the first state command can have a non-zero value
		cmd.fDelay = 0.0f;
		iStatesToPop--;
	}
}

void StateManager::PopAllStates(float fDelay, bool bFlush)
{
	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "pop all states" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_POP_ALL;
	cmd.szState[0] = 0;
	cmd.fDelay = fDelay + m_fCurrentTime;
	m_CmdQueue.push(cmd);
}

void StateManager::Update(float dt)
{
	// Update the total run time
	m_fCurrentTime += dt;

	// Check for the first update and make the appropriate calls
	if(m_bFirstUpdate)
	{
		STATE_DEBUG_OUT("StateManager - entering state: %s", GetState());
		m_bFirstUpdate = false;
		GetCurrentStateClass()->OnEnter(g_szInvalidState);
	}
	// Empty our comment queue, consisting of commands to either
	// push new states onto the stack, pop states off the stack, or
	// to switch the states on the top of the stack.  In each case
	// we transmit the new state to the old one, and vice-versa.
	GSCommand cmd;
	while(!m_CmdQueue.empty() && (m_CmdQueue.front().fDelay <= m_fCurrentTime))
	{
		cmd = m_CmdQueue.front();
		if(cmd.eCmd == GS_CMD_PUSH)
		{
			const char* szPrev = GetState();
			if(strcmp(szPrev, cmd.szState) != 0)
			{
				STATE_DEBUG_OUT("StateManager - overriding state: %s", GetState());
				GetCurrentStateClass()->OnOverride(cmd.szState);
				m_CurrentState.push_back(cmd.szState);
				STATE_DEBUG_OUT("StateManager - entering state: %s", GetState());
				GetCurrentStateClass()->OnEnter(szPrev);
			}
			else
			{
				STATE_DEBUG_OUT("StateManager - cannot push duplicate state: %s", cmd.szState);
			}
		}
		else if(cmd.eCmd == GS_CMD_POP)
		{
			STATE_DEBUG_OUT("StateManager - exiting state: %s", GetState());
			const char* szNext = g_szInvalidState;
			const char* szPrev = GetState();
			// If we only have one state, we shouldn't be popping it off the
			// current state stack.  We should be switching it instead.  This
			// code was hit because someone tried to pop off too many states.
			if(m_CurrentState.size() > 1)
			{
				GetCurrentStateClass()->OnExit(m_CurrentState[m_CurrentState.size() - 2].c_str());
				m_CurrentState.pop_back();
				STATE_DEBUG_OUT("StateManager - resuming state: %s", GetState());
				GetCurrentStateClass()->OnResume(szPrev);
			}
			else
			{
				STATE_DEBUG_OUT("StateManager - cannot pop the last state: %s", GetState());
			}
		}
		else if(cmd.eCmd == GS_CMD_POP_ALL)
		{
			STATE_DEBUG_OUT("StateManager - exiting state: %s", GetState());
			while(m_CurrentState.size() > 1)
			{
				const char* szNext = g_szInvalidState;
				const char* szPrev = GetState();
				GetCurrentStateClass()->OnExit(m_CurrentState[m_CurrentState.size() - 2].c_str());
				m_CurrentState.pop_back();
				STATE_DEBUG_OUT("StateManager - resuming state: %s", GetState());
				GetCurrentStateClass()->OnResume(szPrev);
			}
		}
		else
		{
			const char* szPrev = GetState();
			if(strcmp(szPrev, cmd.szState) != 0)
			{
				STATE_DEBUG_OUT("StateManager - exiting state: %s", GetState());
				GetCurrentStateClass()->OnExit(cmd.szState);
				m_CurrentState.pop_back();
				m_CurrentState.push_back(cmd.szState);
				STATE_DEBUG_OUT("StateManager - entering state: %s", GetState());
				GetCurrentStateClass()->OnEnter(szPrev);
			}
			else
			{
				STATE_DEBUG_OUT("StateManager - cannot switch to duplicate state: %s", cmd.szState);
			}
		}
		m_CmdQueue.pop();
	}
	// After all state transitions are finished, do the current state stack updates
	for(int i = m_CurrentState.size() - 1; i >= 0; i--)
		GetStateClass(m_CurrentState[i].c_str())->Update();
}




