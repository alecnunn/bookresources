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
// Filename: game_object.h
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

#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "fsm.h"
#include "vector.h"
#include <cstdio>


typedef struct GameObject_Str
{
   unsigned int unique_id;		//This game object is referenced by a unique id

   //State machine info
   FSM_Type state_machine_id;	//the state machine that this object running
   unsigned int state;			//the current state of the state machine
   unsigned int next_state;		//the next state when a state change was requested
   bool force_state_change;		//whether a state change has been requested

   //Put other game object info in here
   char szName[256];
   bool bMarkedForDeletion;
   struct GameObject_Str* goNext;


} GameObject;



void GOInitialize( GameObject* go, unsigned int unique_id );
void GOUpdate( GameObject* go );
void GODraw( GameObject* go );


#endif