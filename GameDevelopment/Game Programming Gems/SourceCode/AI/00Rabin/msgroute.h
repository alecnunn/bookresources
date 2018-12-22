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
// Filename: msgroute.h
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

#ifndef _MSGROUTE_H
#define _MSGROUTE_H

#include "game_object.h"
#include "msg.h"



void SendMsg( MsgName name, unsigned int sender, unsigned int receiver );
void SendDelayedMsg( MsgName name, float delay, unsigned int sender, unsigned int receiver );
void SendDelayedMessages( void );
void RouteMessage( MsgObject* msg );
void InitDelayedMessages( void );

#endif
