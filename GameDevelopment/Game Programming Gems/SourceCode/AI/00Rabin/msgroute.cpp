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
// Filename: msgroute.cpp
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
// This file contains all of the critical messaging and routing
// code that enables game objects to talk to each other. Delayed
// messages (functionally timers) are also handled, stored, and
// fired from this file.
//
//////////////////////////////////////////////////////////////

#include "msgroute.h"
#include "custom_time.h"
#include "game_object_db.h"
#include "malloc.h"
#include "msg.h"
#include "fsm_drone.h"


typedef struct DelayedMessage_Str
{
	MsgObject msg;
	bool bSent;

	struct DelayedMessage_Str* next;

} DelayedMessage;

typedef struct
{
	DelayedMessage* head;

} MasterDelayedMessage;


// INTERNAL LOCAL VARIABLES
MasterDelayedMessage masterDelayedMessage;


bool RouteMessageHelper( GameObject* go, unsigned int state, MsgObject* msg );
void StoreDelayedMessage( MsgObject* msg );
void PushBackDelayedMessage( DelayedMessage* msg );




void SendMsg( MsgName name, unsigned int sender, unsigned int receiver )
{
   MsgObject msg;
   msg.name = name;						//The name of the message
   msg.sender_id = sender;					//The sender
   msg.receiver_id = receiver;				//The receiver
   msg.delivery_time = GetCurTime();	//Send the message NOW

   RouteMessage( &msg );
}

void SendDelayedMsg( MsgName name, float delay, unsigned int sender, unsigned int receiver )
{
   MsgObject msg;
   msg.name = name;				//The name of the message
   msg.sender_id = sender;			//The sender
   msg.receiver_id = receiver;			//The receiver
   msg.delivery_time = GetCurTime() + delay;	//Send the message at a future time

   RouteMessage( &msg );
}


void RouteMessage( MsgObject* msg )
{
   GameObject* go = GODBGetGO( msg->receiver_id );
   if( !go )
   {  //Receiver doesn't exist anymore - discard the message
      return;
   }

   if( msg->delivery_time > GetCurTime() )
   {  //This message needs to be stored until its time to send it
      StoreDelayedMessage( msg );
      return;
   }

   if( RouteMessageHelper( go, go->state, msg ) == false )
   {  //Current state didn't handle msg, try Global state (0)
      RouteMessageHelper( go, 0, msg );
   }

   // Check for a state change
   while( go->force_state_change )
   {  //Note: circular logic (state changes causing state changes)
      //could cause an infinite loop here - protect against this

      //Create a general msg for initializing and cleaning up the state change
      MsgObject tempmsg;
      tempmsg.receiver_id = go->unique_id;
      tempmsg.sender_id = go->unique_id;

      go->force_state_change = false;

      //Let the last state clean-up
      tempmsg.name = MSG_RESERVED_Exit;
      RouteMessageHelper( go, go->state, &tempmsg );

      //Set the new state
      go->state = go->next_state;

      //Let the new state initialize
      tempmsg.name = MSG_RESERVED_Enter;
      RouteMessageHelper( go, go->state, &tempmsg );
   }
}

bool RouteMessageHelper( GameObject* go, unsigned int state, MsgObject* msg )
{
   //Look up correct state machine for this Game Object
   //and send message to that particular one

   switch( go->state_machine_id )
   {
      case FSM_Drone:
         return( DroneProcessStateMachine( go, state, msg ) );
         break;

      default:
		  return( false );
		  break;
   }
   
}


void InitDelayedMessages( void )
{
	masterDelayedMessage.head = 0;
}


void StoreDelayedMessage( MsgObject* msg )
{
	//Store this message (in some data structure) for later routing

	//A priority queue would be the ideal data structure (but not required)
	//to store the delayed messages - Check out Mark Nelson's article
	//"Priority Queues and the STL" in the January 1996 Dr. Dobb's Journal
	//http://www.dogma.net/markn/articles/pq_stl/priority.htm

	//Note: In main game loop call SendDelayedMessages() every game 
	//      tick to check if its time to send the stored messages

	DelayedMessage* newDelayedMessage = (DelayedMessage*) malloc( sizeof( DelayedMessage ) );

	newDelayedMessage->msg.name = msg->name;
	newDelayedMessage->msg.receiver_id = msg->receiver_id;
	newDelayedMessage->msg.sender_id = msg->sender_id;

	newDelayedMessage->msg.delivery_time = msg->delivery_time;
	newDelayedMessage->next = 0;
	newDelayedMessage->bSent = false;

	PushBackDelayedMessage( newDelayedMessage );

}

void SendDelayedMessages( void )
{  //This function is called every game tick
	DelayedMessage* cur_msg = masterDelayedMessage.head;
	DelayedMessage* last = 0;
	float curTime = GetCurTime();

	while( cur_msg != 0 )
	{
		if( cur_msg->msg.delivery_time <= GetCurTime() )
		{
			cur_msg->bSent = true;
			RouteMessage( &cur_msg->msg );
			if( last != 0 ) {
				last->next = cur_msg->next;
				free( cur_msg );
				cur_msg = last->next;
			}
			else {
				masterDelayedMessage.head = cur_msg->next;
				free( cur_msg );
				cur_msg = masterDelayedMessage.head;
			}
		}
		else if( cur_msg != 0 ) {
			last = cur_msg;
			cur_msg = cur_msg->next;
		}
	}

}

void PushBackDelayedMessage( DelayedMessage* msg )
{
	DelayedMessage* curDelayedMessage = masterDelayedMessage.head;

	if( curDelayedMessage != 0 )
	{
		while( curDelayedMessage->next != 0 )
		{
			if( curDelayedMessage->bSent == false &&
				curDelayedMessage->msg.name == msg->msg.name &&
				curDelayedMessage->msg.receiver_id == msg->msg.receiver_id &&
				curDelayedMessage->msg.sender_id == msg->msg.sender_id )
			{
				//Already on queue - don't add
				return;
			}
			curDelayedMessage = curDelayedMessage->next;
		}

		curDelayedMessage->next = msg;
	}
	else {
		masterDelayedMessage.head = msg;
	}

}


