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
#include "stdlib.h"


//Add new states here
enum States { STATE_Initialize,
			  STATE_Wander,
			  STATE_Dead
};


//Note: The macro keywords can be highlighted by placing them in the file 
//USERTYPE.DAT in the same directory as MSDEV.EXE

bool Robot::States( StateMachineEvent event, int state )
{
BeginStateMachine

	/////////////////////////////////////////////////////////////////
	State( STATE_Initialize )
		OnEnter
			//Put any C++ initialization code here
			m_timer = 0;

		OnUpdate
			//Put any C++ code here that gets run on every game tick
			m_timer++;
			if( m_timer > 10 ) {
				SetState( STATE_Wander );
			}
			
		OnExit
			//Put any C++ cleanup code here
			m_timer = 0;

	/////////////////////////////////////////////////////////////////
	State( STATE_Wander )
		OnEnter
			//Put any C++ initialization code here
			m_timer = 0;

		OnUpdate
			//Put any C++ code here that gets run on every game tick
			m_timer++;
			if( m_timer > 10 ) {
				SetState( STATE_Initialize );
			}
			else if( rand()%1000 == 0 ) {
				SetState( STATE_Dead );
			}
			
		OnExit
			//Put any C++ cleanup code here
			m_timer = 0;

	/////////////////////////////////////////////////////////////////
	State( STATE_Dead )
		OnEnter
			//Just die on enter...

EndStateMachine
}
