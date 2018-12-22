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
// Filename: msg.h
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
// This file contains the definition of a message along with
// an enumerated type of all messages within the game.
//
//////////////////////////////////////////////////////////////

#ifndef _MSG_H
#define _MSG_H

typedef enum { MSG_NULL,
			   MSG_RESERVED_Enter,	//Don't send this message - the system sends this when a state is first entered - Use OnInitialize to listen for it
			   MSG_RESERVED_Exit,	//Don't send this message - the system sends this when a state is exited - Use OnExit to listen for it
			   MSG_RESERVED_Update,	//Don't send this message - the system sends this when a game tick goes by - Use OnUpdate to listen for it
			   MSG_Timeout,
			   MSG_ChangeState,
} MsgName;


typedef struct
{
	MsgName name;				//name of message (an enumerated type works well)
	unsigned int sender_id;		//unique id of sender
	unsigned int receiver_id;	//unique id of receiver
	float delivery_time;		//deliver message at this time

	//Note that the sender_id and receiver_id are not pointers to game objects.
	//Since messages can be delayed, the sender or receiver may get removed
	//from the game and a pointer would become dangerously invalid.

	//You can add right here any data you want to be passed
	//along with every message - sometimes it's helpful to let
	//messages convey more info by using extra data.
	//For example, a damaged message could carry with it the amount of damage

} MsgObject;

#endif
