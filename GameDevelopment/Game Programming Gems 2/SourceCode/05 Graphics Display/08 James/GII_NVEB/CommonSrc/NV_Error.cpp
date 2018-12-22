/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\CommonSrc
File:  NV_Error.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
//	error.cpp

//	Generic error handling routines for the goodness of all...
//	This one's really simple for now :)
//
//	Any memory cleanup/de-allocation should be done before calling the
//		DoError function with fatal == TRUE
//		function.
//
//  Meant to be used with MFC - hence the stdafx and stuff
//
******************************************************************************/

//#include "stdafx.h"

#include	"NV_Error.h"
#include    <stdlib.h>          // for exit()
#include    <stdio.h>
#include    <windows.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void DoError( char * errorString, bool fatal )
{
	// Old MAC code:
//	ParamText( errorString, kEmptyString, kEmptyString, kEmptyString );	
//	StopAlert( kErrorALRTID, kNilFilterProc );
	
	OkMsgBox( "DoError call", errorString );	

	if( fatal )
		exit(0);	//  AfxAbort();		// This not too good, as can do more sophis.
					// exception handling, but will have to do for now.

}

	/*
	Usage:
	case WM_SIZE:
		OkMsgBox("WM_SIZE Message",
			"wParam = %04X-%04X, lParam = %04X-%04X",
			HIWORD (wParam), LOWORD (wParam),
			HIWORD (lParam), LOWORD (lParam));
		break;
	*/
void OkMsgBox( char * szCaption, char * szFormat, ... )
{
	char szBuffer[256];
	char *pArguments;

	pArguments = (char *) &szFormat + sizeof( szFormat );
	vsprintf( szBuffer, szFormat, pArguments );
	MessageBox( NULL, szBuffer, szCaption, MB_OK );
}


#ifdef _DEBUG
	void FDebug ( char * szFormat, ... )
	{	
		static char buffer[256];
		char *pArgs;

		pArgs = (char*) &szFormat + sizeof( szFormat );
		vsprintf( buffer, szFormat, pArgs );

		OutputDebugString ( buffer );

		Sleep( 2 );		// can miss some if calling too fast!
	}

	void NullFunc( char * szFormat, ... ) {}

	#if 0
		#define WMDIAG(str) { OutputDebugString(str); }
	#else
		#define WMDIAG(str) {}
	#endif
#else
	void FDebug( char * szFormat, ... )		{}
	void NullFunc( char * szFormat, ... )	{}

	#define WMDIAG(str) {}
#endif
