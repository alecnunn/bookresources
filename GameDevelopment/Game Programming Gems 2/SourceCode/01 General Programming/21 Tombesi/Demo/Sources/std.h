/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#ifndef _STD_H_
#define	_STD_H_

#pragma inline_depth (255)
#pragma inline_recursion (on)

#define WIN32_LEAN_AND_MEAN

#include <windows.h>														// Header File For Windows		
#include <windowsx.h>														// Header File For Windows		
#include <stdio.h>
#include <gl\gl.h>															// Header File For The OpenGL32 Library
#include <gl\glu.h>															// Header File For The GLu32 Library
#include <gl\glaux.h>														// Header File For The Glaux Library

#include <cassert>

#include "Utils.h"
#include "globals.h"

#define Assert(a,b)   assert( a && b )

typedef unsigned long ulong;
typedef unsigned int  uint;


#endif