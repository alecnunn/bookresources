/************************************************************
 * MainPtah7.h - V2.06.001
 * 
 * (c) Copyright 1996-2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/  

#ifndef _PTAH7_MAIN_H_
#define _PTAH7_MAIN_H_

#include "stack.h"
#include "fplog.h"

#define APPTITLE "Linear Gem"

#define DINPUT_BUFFERSIZE           16

#define INICIO 0
#define CUERPO 1
#define FIN 127

#define PHYSICAL_WIDTH	640
#define PHYSICAL_HEIGHT 480
#define PHYSICAL_BPP	2

// Typedefs

typedef struct
{
	void *func;
	void *data;
} CallbackT;

// Externs

extern BOOL	bActive;        // is application active?
extern BOOL	bSuspended;		// Estado del thread principal
extern HWND	hwnd;
extern HANDLE task_wakeup_event;

extern c_stack gReloadSurfacesStack;

extern int idioma;

// Funciones

BOOL initFail( HWND hwnd, char *msg=NULL );

#endif