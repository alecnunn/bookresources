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
// Filename: fsm.cpp
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
// This file contains a few functions to make the fsm
// (finite state machine) work. 
//
//////////////////////////////////////////////////////////////

#include "fsm.h"
#include "game_object.h"
#include "game_object_db.h"
#include "msgroute.h"
#include "fsm_drone.h"
#include <string.h>
#include <stdlib.h>


void FSMInitialize( unsigned int unique_id, FSM_Type type )
{
	GameObject* go = GODBGetGO( unique_id );

	go->force_state_change = false;
	go->state = 0;
	go->next_state = 0;
	go->state_machine_id = type;

	{	//Initialize the state machine by sending the first Initialize msg
		MsgObject msg;
		msg.name = MSG_RESERVED_Enter;
		msg.sender_id = go->unique_id;
		msg.receiver_id = go->unique_id;
		RouteMessage( &msg );
	}

}


void TranslateStateName( unsigned int state_machine_id, unsigned int state, char* name )
{

	switch( state_machine_id )
	{
		case FSM_Drone:
			DroneTranslateStateName( state, name );
			break;

		default:
			strcpy( name, "Unknown State Name" );
			break;
	}

}



float RandomBetween( float low, float high )
{
	if( low == high ) {
		return( low );
	}
	else {
		int randnum;
		int first, second, negitivemag, diff;
		float mag, largeresult, result;

		if( low < 10.0f && high < 10.0f &&
			low > -10.0f && high > -10.0f )
		{
			mag = 1000.0f;
		}
		else if( low < 100.0f && high < 100.0f &&
				 low > -100.0f && high > -100.0f )
		{
			mag = 100.0f;
		}
		else if( low < 1000.0f && high < 1000.0f &&
				 low > -1000.0f && high > -1000.0f )
		{
			mag = 10.0f;
		}
		else {
			mag = 1.0f;
		}

		if( low <= high ) {
			first = (unsigned int)(low*mag);
			second = (unsigned int)(high*mag);
		}
		else {
			first = (unsigned int)(high*mag);
			second = (unsigned int)(low*mag);
		}

		negitivemag = 0;
		if( first < 0.0f ) {
			negitivemag = first;
			first = 0;
			second = second - negitivemag;
		}

		diff = second - first;
		
		{
			randnum = rand();
			randnum = randnum%diff;
			largeresult = (float)(randnum + first + negitivemag);
			result = largeresult / mag;
			return( result );
		}

	}

}
