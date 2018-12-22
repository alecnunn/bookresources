///////////////////////////////////////////////////////////////////////////////
//
//  Crypto.h
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

#if !defined( TRUERAND_CRYPTO_H )
#define TRUERAND_CRYPTO_H

#include "Common.h"
#include "Buffer.h"

#if defined(WIN32) && !defined(_XBOX)
    #include <wincrypt.h> // Windows header
#else
    typedef void* HCRYPTPROV;
    typedef void* HCRYPTHASH;

    const unsigned long CALG_MD5 = 0;
    const unsigned long PROV_RSA_FULL = 0;
    const unsigned long CRYPT_VERIFYCONTEXT = 0;
#endif

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Wrap HCRYPTPROV

class Crypto
{
public:

    inline Crypto() : mProv( NULL ) {}
    inline ~Crypto() { Release(); }

    bool Acquire( const char* sContainer, const char* sProvider, 
                  unsigned long nProviderType,
                  unsigned long nFlags=0 );
    void Release();
    bool IsValid() const;
    bool GenRandom( unsigned long nBytes, Buffer& randBuffer ) const;
    Buffer GetHash( unsigned long nAlgId, const Buffer& inputBuffer ) const;

private:

    // disabled
    Crypto( const Crypto& );
    Crypto& operator=( const Crypto& );

private:

    HCRYPTPROV mProv;
    
};

///////////////////////////////////////////////////////////////////////////////
//
// Wrap HCRYPTHASH

class Hash
{
public:

    inline Hash()
    : 
        mHash( NULL )
    #if defined(_XBOX) || !defined(WIN32)
        ,mHashBuffer()
    #endif
    {
    }

    inline ~Hash() { Destroy(); }

    bool Create( HCRYPTPROV hProv, unsigned long nAlgId );
    void Destroy();
    bool HashData( const Buffer& inputBuffer ) const;
    Buffer GetHashValue() const;

private:

    // disabled
    Hash( const Hash& );
    Hash& operator=( const Hash& );

private:

    HCRYPTHASH mHash;

    #if defined(_XBOX) || !defined(WIN32)
        mutable Buffer mHashBuffer;
    #endif
};


} // end namespace TrueRand

#endif // TRUERAND_CRYPTO_H

///////////////////////////////////////////////////////////////////////////////
