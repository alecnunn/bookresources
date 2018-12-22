/**********************************************************************
 	MODULE: importMRC
	
	DESCRIPTION: This module is useful to put MRC data form file to an 
				 in memory C++ Object.
	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/
// ImportMRC.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MRC.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	}
	
    return TRUE;
}

