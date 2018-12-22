///////////////////////////////////////////////////////////////////////////////
//
//  Platform.cpp
//
//  Platform-dependent code.
//
//  Copyright © 2003 Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined( BITPACK_PLATFORM_H )
#define BITPACK_PLATFORM_H

#pragma warning( disable: 4702 )

//-----------------------------------------------------------------------------
//
// Includes

#include <cstdio>
#if defined( WIN32 )
    #include "windows.h"
    #include "wincrypt.h"
    #include "winsock.h"
#else
    #include <cstdlib>
    #include <cstring>
#endif

#undef min
#undef max

//-----------------------------------------------------------------------------
//
// Types

#if defined( WIN32 )
    typedef          __int64 int64;
    typedef unsigned __int64 uint64;
#else
    typedef long long          int64;
    typedef unsigned long long uint64;
    typedef void*              HCRYPTPROV;
    struct SOCKET {};
#endif

//-----------------------------------------------------------------------------
//
// Macros/#defines

#if defined( WIN32 )
    // ...
#else
    #define MB_OK               0
    #define CRYPT_VERIFYCONTEXT 0
    #define AF_INET             0
    #define SOCK_DGRAM          0
    #define IPPROTO_UDP         0
    #define NULL                0
    #define CDECL
#endif

//-----------------------------------------------------------------------------
//
// Functions

#if defined( WIN32 )

#else // !WIN32

//-----------------------------------------------------------------------------
//
// Fake implementations of Win32 functions that are good enough for our porpoises

unsigned int GetTickCount()
{
    return 0;
}

void MessageBox( int, const char* pMsg, const char*, int )
{
    puts( pMsg );
}

void DebugBreak()
{
}

bool IsBadWritePtr( const void* p, unsigned int )
{
    return( p == NULL );
}

bool IsBadReadPtr( const void* p, unsigned int )
{
    return( p == NULL );
}

int CryptAcquireContextA( void** ppContext, void*, void*, int, int )
{
    *ppContext = "";
    return 1;
}

int CryptGenRandom( void*, unsigned long nBytes, unsigned char* pDest )
{
    memset( pDest, rand(), nBytes );
    return 1;
}

SOCKET socket( int, int, int )
{
    return SOCKET();
}

int send( SOCKET, const char*, int, int )
{
    return 0;
}

int closesocket( SOCKET )
{
    return 0;
}

#endif

#endif // BITPACK_PLATFORM_H
