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
// Filename: hud.cpp
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
// This file is used to display the on-screen text.
//
//////////////////////////////////////////////////////////////

#include "hud.h"
#include "text.h"

TextBox* textBox = 0;

void HUDInitScreen( void )
{
   textBox = new TextBox(0, 640, 0, 480);
   textBox->SetMode(TEXT_NONE);

}


void HUDClearScreen( void )
{
	if( textBox ) {
		textBox->Clear();
	}

}


void HUDPrintToScreen( char* string )
{
	if( textBox ) {
		textBox->Printf( string );
	}

}


void HUDDrawScreen( void )
{
	if( textBox ) {
		textBox->Draw();
	}

}

