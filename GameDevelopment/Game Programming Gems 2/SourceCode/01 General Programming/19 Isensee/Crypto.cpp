///////////////////////////////////////////////////////////////////////////////
//
//  Crypto.cpp
//
//  Wrapper for Win32 HCRYPTPROV and HCRYPTHASH. If platform is not WIN32,
//  will use public-domain MD5 hashing.
//
//  Copyright © 2001 Pete Isensee (PKIsensee@msn.com).
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

#include "Crypto.h"
#include "Md5Hash.h"

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Acquire the HCRYPTPROV context. Must be called before any other class
// functions will succeed.

bool Crypto::Acquire( const char* sContainer, const char* sProvider,
                      unsigned long nProviderType, unsigned long nFlags )
{
    Release();

#if defined(WIN32) && !defined(_XBOX)

    // Win32 call
    BOOL bSuccess = CryptAcquireContext( &mProv, sContainer, sProvider,
                                         nProviderType, nFlags );
    return( bSuccess != 0 );

#else

    USED( sContainer );
    USED( sProvider );
    USED( nProviderType );
    USED( nFlags );
    mProv = HCRYPTPROV(1);
    return true;

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Destroy the current HCRYPTPROV context

void Crypto::Release()
{
    if( mProv != NULL )
    {
#if defined(WIN32) && !defined(_XBOX)
        BOOL bSuccess = CryptReleaseContext( mProv, 0 ); // Win32 call
        assert( bSuccess != 0 );
        USED( bSuccess );
#endif

        mProv = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// TRUE if we have a non-NULL HCRYPTPROV context

bool Crypto::IsValid() const
{
    return( mProv != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a cryptographically random value using the current HCRYPTPROV
// context. Returns false if the current context doesn't support this function.
// Given an Intel HCRYPTPROV context, will return a value from the hardware
// random number generator on the Intel 810 chipset.

bool Crypto::GenRandom( unsigned long nBytes, Buffer& outputBuffer ) const
{
    assert( mProv != NULL );
    assert( nBytes > 0 );
    outputBuffer.resize( nBytes );

#if defined(WIN32) && !defined(_XBOX)

    BYTE* pData = const_cast< BYTE* >( outputBuffer.c_str() );
    BOOL bSuccess = CryptGenRandom( mProv, nBytes, pData ); // Win32 call
    return( bSuccess != 0 );

#else

    return false;

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate the cryptographic hash of the given buffer. Requires that 
// HCRYPTPROV be a standard context, like PROV_RSA_FULL. nAlgId is one of:
// CALG_MD2, CALG_MD4, CALG_MD5, or CALG_SHA

Buffer Crypto::GetHash( unsigned long nAlgId, const Buffer& inputBuffer ) const
{
    // Hash the data
    Hash Hasher;
    if( Hasher.Create( mProv, nAlgId ) )
        if( Hasher.HashData( inputBuffer ) )
            return Hasher.GetHashValue();

    // Hash failed
    return Buffer();
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a HCRYPTHASH of the specified algorithm type (e.g. CALG_MD5)

bool Hash::Create( HCRYPTPROV hProv, unsigned long nAlgId )
{
    Destroy();

#if defined(WIN32) && !defined(_XBOX)

    // Win32 call
    BOOL bSuccess = CryptCreateHash( hProv, nAlgId, NULL, 0, &mHash );
    return( bSuccess != 0 );

#else

    USED( hProv );
    USED( nAlgId );
    assert( nAlgId == CALG_MD5 );
    mHash = HCRYPTHASH(1);
    return true;

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Destroy the HCRYPTHASH

void Hash::Destroy()
{
    if( mHash != NULL )
    {
#if defined(WIN32) && !defined(_XBOX)
        BOOL bSuccess = CryptDestroyHash( mHash ); // Win32 call
        assert( bSuccess != 0 );
        USED( bSuccess );
#endif
        mHash = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Hash the input buffer. If this is successful, the hash output is available
// using GetHashValue()

bool Hash::HashData( const Buffer& inputBuffer ) const
{
    assert( mHash != NULL );
    const unsigned char* pData = static_cast< const unsigned char* >
                                                    ( inputBuffer.c_str() );
    assert( pData != NULL );
    unsigned long nBytes = inputBuffer.length();

#if defined(WIN32) && !defined(_XBOX)

    BOOL bSuccess = CryptHashData( mHash, pData, nBytes, 0 ); // Win32 call
    assert( GetHashValue() == Md5Hash().GetHash( pData, nBytes ) );
    return( bSuccess != 0 );

#else

    mHashBuffer = Md5Hash().GetHash( pData, nBytes );
    return true;

#endif

}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the output from a successful call to HashData()

Buffer Hash::GetHashValue() const
{
    assert( mHash != NULL );

#if defined(WIN32) && !defined(_XBOX)

    DWORD nBytes;
    DWORD nLen = sizeof(nBytes);

    // Determine the size of the hash (e.g. for MD5, nBytes will be 16)
    // Win32 call
    BOOL bSuccess = CryptGetHashParam( mHash, HP_HASHSIZE, 
                                       reinterpret_cast< BYTE* >(&nBytes), 
                                       &nLen, 0 );
    if( bSuccess == 0 )
        return Buffer();

    // Make the buffer big enough to hold the hash
    Buffer hashBuffer;
    hashBuffer.resize( nBytes );
    BYTE* pData = const_cast< BYTE* >( hashBuffer.c_str() );

    // Extract the hash value
    // Win32 call
    bSuccess = CryptGetHashParam( mHash, HP_HASHVAL, pData, &nBytes, 0 );
    if( bSuccess == 0 )
        return Buffer();
    return hashBuffer;

#else

    return mHashBuffer;

#endif
}

} // end namespace TrueRand

///////////////////////////////////////////////////////////////////////////////
