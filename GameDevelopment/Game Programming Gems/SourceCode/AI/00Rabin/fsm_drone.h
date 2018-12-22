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
// Filename: fsm_drone.h
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

#ifndef _FSM_DRONE_H__
#define _FSM_DRONE_H__

#include "game_object.h"

bool DroneProcessStateMachine( GameObject* go, unsigned int state, MsgObject* msg );
void DroneTranslateStateName( unsigned int state, char* name );


#endif