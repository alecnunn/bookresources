///////////////////////////////////////////////////////////////////////////////
//
//  GetRand.h
//
//  Fill a value T with truly random bits. This function is used for testing
//  purposes.
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
#if !defined( BITPACK_GETRAND_H )
#define BITPACK_GETRAND_H

#include <cassert>
#include "Util.h"
#include "Platform.h"

namespace BitPack
{

HCRYPTPROV g_hProv = NULL;

template< typename T >
T GetRandValue( int nBits )
{
    if( g_hProv == NULL )
    {
        int bSuccess = CryptAcquireContextA( &g_hProv, NULL, NULL, 1,
                                             CRYPT_VERIFYCONTEXT );
        assert( bSuccess );
        (void)bSuccess;
        assert( g_hProv != NULL );                                             
    }
    
    T n;
    int bSuccess = CryptGenRandom( g_hProv, sizeof(T), (unsigned char*)(&n) );
    assert( bSuccess );
    (void)bSuccess;
    
    // Mask off unused bits
    uint64 mask = 0xFFFFFFFFFFFFFFFF;
    mask >>= ( sizeof(uint64) * CHAR_BIT ) - nBits;
    
    return T( n & mask );
}

} // end namespace BitPack

#endif // BITPACK_GETRAND_H
