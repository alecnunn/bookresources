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

// Code from "Calling Conventions" Section
//
DWORD Call_cdecl( const void* args, size_t sz, DWORD func )
{
    DWORD rc;               // here's our return value...
    __asm
    {
        mov   ecx, sz       // get size of buffer
        mov   esi, args     // get buffer
        sub   esp, ecx      // allocate stack space
        mov   edi, esp      // start of destination stack frame
        shr   ecx, 2        // make it dwords
        rep   movsd         // copy params to real stack
        call  [func]        // call the function
        mov   rc,  eax      // save the return value
        add   esp, sz       // restore the stack pointer
    }
    return ( rc );
}

DWORD Call_stdcall( const void* args, size_t sz, DWORD func )
{
    DWORD rc;               // here's our return value...
    __asm
    {
        mov   ecx, sz       // get size of buffer
        mov   esi, args     // get buffer
        sub   esp, ecx      // allocate stack space
        mov   edi, esp      // start of destination stack frame
        shr   ecx, 2        // make it dwords
        rep   movsd         // copy it
        call  [func]        // call the function
        mov   rc,  eax      // save the return value
    }
    return ( rc );
}

__declspec ( naked ) DWORD GetST0( void )
{
    DWORD f;                // temp var
    __asm
    {
        fstp dword ptr [f]      // pop ST0 into f
        mov eax, dword ptr [f]  // copy into eax
        ret                     // done
    }
}


