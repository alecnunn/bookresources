/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#include <windows.h>
#include <iostream>
#include <vector>
#include "function.h"

// Code from "Calling the Function" Section
//
__declspec ( naked ) DWORD GetEIP( void )
{
    __asm
    {
        mov eax, dword ptr [esp]
        ret
    }
}

// sample RPC'able function
void NetFoo( bool send, int i )
{
    // FindFunction() should look in g_Functions for highest 'm_Proc'
    // less than 'ip' and return it
    static const Function* sFunction = FindFunction( GetEIP() );
    if ( send )
    {
        // RouteFunction() should pack up the parameters and send the
        // request over the network.
        RouteFunction( sFunction, (BYTE*)&send + 4 );
        return;
    }

    // ... normal execution of NetFoo
    printf( "i = %d\n", i );
}


