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
// Filename: game_object.cpp
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
// This file contains the code to initialize and update
// game objects.
//
//////////////////////////////////////////////////////////////

#include "game_object.h"
#include "msgroute.h"


void GOInitialize( GameObject* go, unsigned int unique_id )
{
	go->unique_id = unique_id;

	//State Machine variables
	go->state = 0;
	go->next_state = 0;
	go->state_machine_id = (FSM_Type)0;
	go->force_state_change = false;

	go->bMarkedForDeletion = false;
	go->goNext = 0;

}


void GOUpdate( GameObject* go )
{
	MsgObject msg;
	msg.name = MSG_RESERVED_Update;
	msg.receiver_id = go->unique_id;
	msg.sender_id = go->unique_id;

	RouteMessage( &msg );

}


void GODraw( GameObject* go )
{
	//Put drawing code here

}