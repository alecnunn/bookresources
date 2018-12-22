/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#include "statemch.h"



StateMachine::StateMachine( void )
{
	m_currentState = 0;
	m_stateChange = false;
	m_nextState = false;
}


void StateMachine::Initialize( void )
{
	Process( EVENT_Enter );
}


void StateMachine::Update( void )
{
	Process( EVENT_Update );
}


void StateMachine::Process( StateMachineEvent event )
{
	States( event, m_currentState );

	// Check for a state change
	int safetyCount = 10;
	while( m_stateChange && (--safetyCount >= 0) )
	{
		assert( safetyCount > 0 && "StateMachine::Process - States are flip-flopping in an infinite loop." );

		m_stateChange = false;

		// Let the last state clean-up
		States( EVENT_Exit, m_currentState );

		// Set the new state
		m_currentState = m_nextState;

		// Let the new state initialize
		States( EVENT_Enter, m_currentState );
	}

}


void StateMachine::SetState( unsigned int newState )
{
	m_stateChange = true;
	m_nextState = newState;
}
