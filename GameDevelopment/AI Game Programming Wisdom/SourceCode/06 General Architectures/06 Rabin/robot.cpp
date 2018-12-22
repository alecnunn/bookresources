/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#include "robot.h"
#include <string.h>


//Add new states here
enum States { STATE_Initialize,
			  STATE_Wander
};


//Note: The macro keywords can be highlighted by placing them in the file 
//USERTYPE.DAT in the same directory as MSDEV.EXE

bool Robot::States( StateMachineEvent event, MSG_Object * msg, int state )
{
BeginStateMachine

	OnMsg( MSG_ChangeState ) //Default msg response (if not handled inside current state)
		m_timer--;

	///////////////////////////////////////////////////////////////
	State( STATE_Initialize )
		OnEnter
			unsigned int paper = 8;
			paper++;
			m_timer = 0;
			char test[256];
			strcpy( test, statename );
			SendDelayedMsg( 1.0f, MSG_Timeout, m_Owner->GetID() );
			SendDelayedMsgToMe( 10.0f, MSG_ChangeState, SCOPE_TO_THIS_STATE );

		OnMsg( MSG_Timeout )
			m_timer--;

		OnUpdate
			m_timer++;
			if( m_timer > 10 ) {
				SetState( STATE_Wander );
			}
			
		OnExit
			unsigned int joke = 5;
			joke++;

	
	///////////////////////////////////////////////////////////////
	State( STATE_Wander )

		OnMsg( MSG_ChangeState )
			SetState( STATE_Initialize );
	
		OnUpdate
			m_timer = 0;


EndStateMachine
}
