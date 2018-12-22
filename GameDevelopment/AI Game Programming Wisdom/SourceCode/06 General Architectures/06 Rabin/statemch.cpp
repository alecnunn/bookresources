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
#include "msgroute.h"
#include "global.h"


StateMachine::StateMachine( GameObject * object )
{
	m_Owner = object;
	m_currentState = 0;
	m_stateChange = false;
	m_nextState = false;
	m_timeOnEnter = 0.0f;
	m_ccMessagesToGameObject = 0;
}

	
void StateMachine::Initialize( void )
{
	Process( EVENT_Enter, 0 );
}


void StateMachine::Update( void )
{
	Process( EVENT_Update, 0 );
}


void StateMachine::Process( StateMachineEvent event, MSG_Object * msg )
{
	
	if( event == EVENT_Message && msg && GetCCReceiver() > 0 )
	{	// CC this message
		SendMsg( msg->GetMsgName(), GetCCReceiver() );
	}

	if( States( event, msg, m_currentState ) == false )
	{	// Current state didn't handle msg, try Global state (-1)
		States( event, msg, -1 );
	}

	// Check for a state change
	int safetyCount = 50;
	while( m_stateChange && (--safetyCount >= 0) )
	{
		assert( safetyCount > 0 && "StateMachine::Process - States are flip-flopping in an infinite loop." );

		m_stateChange = false;

		// Let the last state clean-up
		States( EVENT_Exit, 0, m_currentState );

		// Set the new state
		m_currentState = m_nextState;

		// Remember the time we entered this state
		m_timeOnEnter = g_time.GetCurTime();

		// Let the new state initialize
		States( EVENT_Enter, 0, m_currentState );
	}

}


void StateMachine::SetState( unsigned int newState )
{
	m_stateChange = true;
	m_nextState = newState;

}


void StateMachine::SendMsg( MSG_Name name, objectID receiver )
{
	g_msgroute.SendMsg( 0, name, receiver, m_Owner->GetID(), -1 );

}


void StateMachine::SendDelayedMsg( float delay, MSG_Name name, objectID receiver )
{
	g_msgroute.SendMsg( delay, name, receiver, m_Owner->GetID(), -1 );
	
}


void StateMachine::SendDelayedMsgToMe( float delay, MSG_Name name, MSG_Scope scope )
{
	if( scope == SCOPE_TO_THIS_STATE ) {
		g_msgroute.SendMsg( delay, name, m_Owner->GetID(), m_Owner->GetID(), m_currentState );
	}
	else {
		g_msgroute.SendMsg( delay, name, m_Owner->GetID(), m_Owner->GetID(), -1 );
	}

}



