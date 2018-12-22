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

enum StateMachineEvent { EVENT_Update,
						 EVENT_Enter,
						 EVENT_Exit
};


//State Machine Language Macros (put these keywords in the file USERTYPE.DAT in the same directory as MSDEV.EXE)
#define BeginStateMachine		if( state < 0 ) { if(0) {
#define EndStateMachine			return( true ); } } else { assert(0); return( false ); }  return( false );
#define State(a)				return( true ); } } else if( a == state ) { if(0) {
#define OnEvent(a)				return( true ); } else if( a == event ) {
#define OnEnter					OnEvent( EVENT_Enter )
#define OnUpdate				OnEvent( EVENT_Update )
#define OnExit					OnEvent( EVENT_Exit )


class StateMachine
{
public:

	StateMachine( void );
	~StateMachine( void ) {}

	void Initialize( void );
	void Update( void );
	void SetState( unsigned int newState );

private:

	unsigned int m_currentState;
	unsigned int m_nextState;
	bool m_stateChange;

	void Process( StateMachineEvent event );
	virtual bool States( StateMachineEvent event, int state ) = 0;

};


#endif	// __STATEMCH_H__