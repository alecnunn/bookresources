// main 
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "atomgaud.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


const double ATOM_SQR2 = 1.4142135623730950488016887242097;
const double ATOM_SQR2DIV2 = ATOM_SQR2 / 2.0;
const double ATOM_SQR2DIV4 = ATOM_SQR2 / 4.0;

const double ATOM_SQR3 = 1.7320508075688772935274463415059;
const double ATOM_SQR3DIV2 = ATOM_SQR3 / 2.0;
const double ATOM_SQR3DIV3 = ATOM_SQR3 / 3.0; // 0.57735026918962576450914878050196
const double ATOM_SQR3DIV4 = ATOM_SQR3 / 4.0;
const double ATOM_SQR3DIV6 = ATOM_SQR3 / 6.0;

const double ATOM_PI = 3.1415926535897932384626433832795;
const double ATOM_2PI = ATOM_PI * 2.0;
const double ATOM_E = 2.71828183; 

const UINT32 ATOM_POW2[] =
{
1,2,4,8,16,32,64,128,
256,512,1024,2048,4096,8192,16384,32768,
65536,131072,262144,524288,1048576,2097152,4194304,8388608,
16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,
};

