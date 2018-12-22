/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
//////////////////////////////////////////////////////////////
//
// Filename: fsm_drone.cpp
//
// Author: Steve Rabin
// E-mail: stevera@noa.nintendo.com
// From the book "Game Programming Gems"
// From the article "Designing a General Robust AI Engine"
//
// Brief Disclaimer:
// This code is free to use for commercial use and is in the
// public domain. You may distribute, copy, modify, or use as
// is as long as this information is present. This code makes
// no guarantees written or implied and is provided solely as
// an example. Have a nice day!
//
// Purpose:
// This file is an example of a state machine that multiple
// game objects can reference. Note that no state information
// is saved within this file. State information lies solely
// inside each game object. It would be a mistake to save any
// state info within this file.
//
//////////////////////////////////////////////////////////////


#include "fsm_drone.h"
#include "custom_time.h"
#include "fsmmacros.h"
#include "msgroute.h"
#include "game_object_db.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>


typedef enum { STATE_Global,	//STATE_Global must always be the first one
			   STATE_Idle,
			   STATE_Wander,
			   STATE_Pursue,
			   STATE_Evade,
			   STATE_Cower,
			   STATE_Hide,
			   STATE_Dead,
} FSM_States;



bool DroneProcessStateMachine( GameObject* go, unsigned int state, MsgObject* msg )
{

//This is the state machine language.
//To see the keywords, look in the file "fsmmacros.h".
//You can get MS Visual Studio to highlight these words by listing them in
//a text file called "usertype.dat" put in the same directory where Msdev.exe
//is stored (like C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin).
//You'll find the "usertype.dat file in the same directory this file is in.
//Just copy it to the correct directory.

BeginStateMachine


	//----------------------------------------------------------------
	//This first section (before STATE declarations) is known as the
	//Global section. These responses apply over all states, regardless
	//of the current state. Importantly, these responses are only
	//active after the current state has been checked. When the state
	//machine first starts up, its not in any state. Therefore, the code
	//below is critical for setting the starting state.
	
	OnEnter
		//When the state machine becomes initialized, it will execute this code	
		SetState( STATE_Idle );	//This sets the initial state that it starts up in

	OnMsg( MSG_ChangeState )
		//If a ChangeState message doesn't get handled by the current state,
		//it would be handled right here. You can execute any code or even
		//change state right here. Therefore, you can have global triggers
		//that can change from any state to any other state.


	//----------------------------------------------------------------
	//This is a state definition
	State( STATE_Idle )
	
		OnEnter	//When the state is entered, it executes the following code
			SendMsg( MSG_ChangeState, go->unique_id, go->unique_id );

		OnMsg( MSG_ChangeState ) //When a MSG_ChangeState is received, it executes the following code
			SetState( STATE_Wander );

		OnExit
			//You can execute code right here on exiting this state - but I didn't put any

	//----------------------------------------------------------------
	State( STATE_Wander )

		OnEnter
			//Sends a delayed message (a timer) between 1 and 10 seconds in the future to ourselves
			SendDelayedMsg( MSG_Timeout, RandomBetween( 1.0f, 10.0f ), go->unique_id, go->unique_id );

		OnUpdate
			//You can execute code right here every game tick - but I didn't put any

		OnMsg( MSG_Timeout )
			if( rand()%100 < 50 ) {
				SetState( STATE_Pursue );
			}
			else {
				SetState( STATE_Evade );
			}


	//----------------------------------------------------------------
	State( STATE_Pursue )

		OnEnter
			SendDelayedMsg( MSG_Timeout, RandomBetween( 1.0f, 10.0f ), go->unique_id, go->unique_id );

		OnMsg( MSG_Timeout )
			if( rand()%100 < 30 ) {
				SetState( STATE_Wander );
			}
			else {
				SetState( STATE_Evade );
			}


	//----------------------------------------------------------------
	State( STATE_Evade )

		OnEnter
			SendDelayedMsg( MSG_Timeout, RandomBetween( 1.0f, 10.0f ), go->unique_id, go->unique_id );

		OnMsg( MSG_Timeout )
			if( rand()%100 < 70 ) {
				SetState( STATE_Wander );
			}
			else {
				SetState( STATE_Cower );
			}


	//----------------------------------------------------------------
	State( STATE_Cower )

		OnEnter
			SendDelayedMsg( MSG_Timeout, RandomBetween( 1.0f, 10.0f ), go->unique_id, go->unique_id );

		OnMsg( MSG_Timeout )
			if( rand()%100 < 10 ) {
				SetState( STATE_Dead );
			}
			else {
				SetState( STATE_Hide );
			}


	//----------------------------------------------------------------
	State( STATE_Hide )

		OnEnter
			SendDelayedMsg( MSG_Timeout, RandomBetween( 1.0f, 10.0f ), go->unique_id, go->unique_id );

		OnMsg( MSG_Timeout )
			if( rand()%100 < 30 ) {
				SetState( STATE_Evade );
			}
			else {
				SetState( STATE_Wander );
			}


	//----------------------------------------------------------------
	State( STATE_Dead )

		//We're dead - don't do anything
		//A state can exist without responding to any messages or
		//without actually doing anything



EndStateMachine

}


void DroneTranslateStateName( unsigned int state, char* name )
{
	switch( state )
	{
		case STATE_Global:
			strcpy( name, "Global" );
			break;

		case STATE_Idle:
			strcpy( name, "Idle" );
			break;
		
		case STATE_Wander:
			strcpy( name, "Wander" );
			break;
		
		case STATE_Pursue:
			strcpy( name, "Pursue" );
			break;
		
		case STATE_Evade:
			strcpy( name, "Evade" );
			break;
		
		case STATE_Cower:
			strcpy( name, "Cower" );
			break;
		
		case STATE_Hide:
			strcpy( name, "Hide" );
			break;
		
		case STATE_Dead:
			strcpy( name, "Dead As A Doornail" );
			break;
		
		default:
			strcpy( name, "Can't translate state name" );
	}

}
