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
// Filename: game_object_db.h
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
// This file contains the code to deal with the game object
// database.
//
//////////////////////////////////////////////////////////////

#ifndef _GAME_OBJECT_DB_H__
#define _GAME_OBJECT_DB_H__

#include "game_object.h"

void GODBInit( void );
void GODBUpdate( void );
void GODBDraw( void );
void GODBOutputStateInfoToHUD( void );
GameObject* GODBCreateAndReturnGO( char* name );
GameObject* GODBGetHead( void );
GameObject* GODBGetGO( unsigned int unique_id );
GameObject* GODBGetGOFromName( char* name );


#endif