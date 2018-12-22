/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once

//////////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////////

extern HDC			hDC;														// Private GDI Device Context
extern HGLRC		hRC;														// Permanent Rendering Context
extern HWND			hWnd;														// Holds Our Window Handle
extern HINSTANCE	hInstance;													// Holds The Instance Of The Application
																			
extern bool	keys[256];															// Array Used For The Keyboard Routine (0 init)
extern bool	active;																// Window Active Flag Set To TRUE By Default
extern bool	fullscreen;															// Fullscreen Flag Set To Fullscreen Mode By Default
extern HWND hDLGWnd;		// Holds FPS Window Handle

extern bool	normals;		//display normals
extern bool	skel;			//display skeletal structure
extern bool rotate;

extern Clock t;
