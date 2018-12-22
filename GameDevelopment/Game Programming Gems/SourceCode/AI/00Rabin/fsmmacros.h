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
// Filename: fsmmacros.h
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
// This file contains the macros that make up the state machine
// language. These macros are used in the file "fsm_drone.cpp".
// These macros are designed like a jigsaw puzzle to line up
// with each other. That's the reason there are if(0)'s and such.
// Try expanding the macros in the file "fsm_drone.cpp" to see
// why they work.
//
//////////////////////////////////////////////////////////////


#ifndef _FSM_MACROS_H
#define _FSM_MACROS_H

//This is the state machine language.
//To see the keywords, look in the file "fsmmacros.h".
//You can get MS Visual Studio to highlight these words by listing them in
//a text file called "usertype.dat" put in the same directory where Msdev.exe
//is stored (like C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin).
//You'll find the "usertype.dat file in the same directory this file is in.
//Just copy it to the correct directory.

#define BeginStateMachine   if( STATE_Global == state ) { if(0) {
#define State(a)            return( true ); } } else if( a == state ) { if(0) {
#define OnEnter             return( true ); } else if( MSG_RESERVED_Enter == msg->name ) { 
#define OnExit              return( true ); } else if( MSG_RESERVED_Exit == msg->name ) { 
#define OnUpdate            return( true ); } else if( MSG_RESERVED_Update == msg->name ) { 
#define OnMsg(a)            return( true ); } else if( a == msg->name ) {
#define SetState(a)         go->next_state = a; go->force_state_change = true;
#define EndStateMachine     return( true ); } } else { assert( !"Invalid State" ); \
                            return( false );}  return( false );


#endif