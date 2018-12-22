/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __STATEMCH_H__
#define __STATEMCH_H__

#include <assert.h>
#include "msg.h"
#include "gameobject.h"
#include "debuglog.h"
#include "time.h"
#include "global.h"


//State Machine Language Macros (put these keywords in the file USERTYPE.DAT in the same directory as MSDEV.EXE)
#define BeginStateMachine		if( state < 0 ) { char statename[64] = "STATE_Global"; if(0) {
#define EndStateMachine			return( true ); } } else { assert( 0 && "Invalid State" ); return( false ); } return( false );
#define State(a)				return( true ); } } else if( a == state ) { char statename[64] = #a; if(0) { 
#define OnMsg(a)				return( true ); } else if( EVENT_Message == event && msg && a == msg->GetMsgName() ) { g_debuglog.LogStateMachineEvent( m_Owner->GetID(), msg, statename, #a, true );
#define OnEvent(a)				return( true ); } else if( a == event ) { g_debuglog.LogStateMachineEvent( m_Owner->GetID(), msg, statename, #a, true );
#define OnUpdate				OnEvent( EVENT_Update )
#define OnEnter					OnEvent( EVENT_Enter )
#define OnExit					OnEvent( EVENT_Exit )



enum StateMachineEvent { EVENT_INVALID,
						 EVENT_Update,
						 EVENT_Message,
						 EVENT_Enter,
						 EVENT_Exit
};

typedef enum { NO_MSG_SCOPING,
			   SCOPE_TO_THIS_STATE
} MSG_Scope;


class StateMachine
{
public:

	StateMachine( GameObject * object );
	~StateMachine( void ) {}

	void Initialize( void );
	void Update( void );
	void SetState( unsigned int newState );

	void SendMsg( MSG_Name name, objectID receiver );
	void SendDelayedMsg( float delay, MSG_Name name, objectID receiver );
	void SendDelayedMsgToMe( float delay, MSG_Name name, MSG_Scope scope );

	int GetState( void )						{ return( m_currentState ); }
	float GetTimeInState( void )				{ return( g_time.GetCurTime() - m_timeOnEnter ); }				

	void SetCCReceiver( objectID id )			{ m_ccMessagesToGameObject = id; }
	void ClearCCReceiver( void )				{ m_ccMessagesToGameObject = 0; }
	objectID GetCCReceiver( void )				{ return( m_ccMessagesToGameObject ); }

	void Process( StateMachineEvent event, MSG_Object * msg );


protected:

	GameObject * m_Owner;


private:

	unsigned int m_currentState;
	unsigned int m_nextState;
	bool m_stateChange;
	float m_timeOnEnter;
	objectID m_ccMessagesToGameObject;

	virtual bool States( StateMachineEvent event, MSG_Object * msg, int state ) = 0;

};


#endif	// __STATEMCH_H__