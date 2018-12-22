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
// Filename: fsm.h
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

#ifndef _FSM_H
#define _FSM_H

#include "msg.h"

typedef enum { FSM_NULL, 
			   FSM_Drone,
} FSM_Type;


void FSMInitialize( unsigned int unique_id, FSM_Type type );
void TranslateStateName( unsigned int unique_id, unsigned int state, char* name );
float RandomBetween( float low, float high );

#endif
