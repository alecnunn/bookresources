/************************************************************
 * Main.h - V2.06.001
 * 
 * (c) Copyright 1996-2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/  

#ifndef _PTAH_MAIN_H_
#define _PTAH_MAIN_H_

#include "fplog.h"

/////////////////////////////
// GENERALES DEL JUEGO
/////////////////////////////

#define NAME "Linear Gem"
#define TITLE "Linear Gem"

#define PHYSICAL_WIDTH	640
#define PHYSICAL_HEIGHT 480
#define PHYSICAL_BPP	2

#define DINPUT_BUFFERSIZE           16


extern int idioma;

extern BOOL	bActive;  			// Esta activa la aplicacion?
extern BOOL bInicio;			// Esta iniciandose el thread del juego?
extern BOOL bFin;				// Esta finalizandose el thread del juego?
extern BOOL bDriverLoaded;		// Esta cargado el driver de sonido?
extern BOOL	bSuspended;			// Estado del thread principal
extern HWND	hwnd;
extern HINSTANCE g_hInstance;		

#endif