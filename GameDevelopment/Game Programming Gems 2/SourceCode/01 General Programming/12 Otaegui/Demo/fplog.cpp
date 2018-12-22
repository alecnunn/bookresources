/************************************************************
 * fplog.cpp - Funcion de log de errores V2.06.002
 *
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/               

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>
#include <malloc.h>

#include "mainptah7.h"

// Funcion de log de errores
void
fplog( char * fmt, ... )
{
	FILE *fp;
	va_list args;
	
	if ( fp = fopen( "debug.log", "at" ) )
	{
		va_start( args, fmt );
		// Al archivo
		vfprintf( fp, fmt, args );

		va_end( args );
		
		fclose( fp );
	}
}