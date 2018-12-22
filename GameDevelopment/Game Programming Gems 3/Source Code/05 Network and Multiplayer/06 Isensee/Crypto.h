///////////////////////////////////////////////////////////////////////////////
//
//  Crypto.h
//
//  Wrapper for Win32 HCRYPTPROV and HCRYPTHASH. Replace this with your own
//  crypto functionality (e.g. Crypto++ library) if you wish.
//
//  Copyright © 2002 Pete Isensee (PKIsensee@msn.com).
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

#if !defined( SECURE_SOCK_CRYPTO_H )
#define SECURE_SOCK_CRYPTO_H

#include "Common.h"
#include "Buffer.h"

#include "windows.h"  // Win32 standard header
#include "wincrypt.h" // Win32 crypto header

namespace SecureSock
{

// Common typedefs
typedef ALG_ID AlgType;

class Key; // forward declaration

///////////////////////////////////////////////////////////////////////////////
//
// Wrap HCRYPTPROV. This is a Win32-specific way of segregating encryption
// algorithms and other crypto information.

class CryptContext
{
public:

    CryptContext();
    ~CryptContext();

    bool Acquire( const char* sContainer, const char* sProvider, 
                  unsigned long nProviderType,
                  unsigned long nFlags=0 );
    void Release();
    bool IsValid() const;
    bool GenRandom( unsigned long nBytes, Buffer& ) const;

private:

    // disabled
    CryptContext( const CryptContext& );
    CryptContext& operator=( const CryptContext& );

    HCRYPTPROV GetHandle() const;
    friend class Key;
    friend class Hash;

private:

    HCRYPTPROV mProv;
    
};

///////////////////////////////////////////////////////////////////////////////
//
// Wrap HCRYPTKEY. Includes key data, IV, algorithm type, algorithm mode,
// block length.

class Key
{
public:

    Key();
    Key( AlgType );
    ~Key();
    Key( const Key& );
    Key& operator=( const Key& );

    static void SetContext( CryptContext* );
    static bool GenRandom( unsigned long nBytes, Buffer& randBuffer );

    void Swap( Key& );
    bool Create( AlgType nAlgId, unsigned long nKeyLength=0, 
                 unsigned long nFlags=0 );
    bool Create( AlgType nAlgId, const Buffer& keyBuffer );
    bool Import( const Buffer& keyBlob, const Key& keyPublic, 
                 unsigned long nFlags=0 );
    Buffer Export( unsigned long nBlobType, const Key& keyExp ) const;
    void Destroy();
    bool IsValid() const;

    bool SetMode( unsigned long nMode );
    bool SetIV( const Buffer& IV );

    AlgType GetAlgorithm() const;
    unsigned long GetKeyLength() const;
    unsigned long GetBlockLength() const;
    unsigned long GetMode() const;
    unsigned long GetIVSize() const;
    Buffer GetIV() const;
    Buffer GetKeyData() const;

private:

    HCRYPTPROV GetHandle() const;
    friend class Cipher;
    friend class Hash;

    bool CreateExpOne();
    static void SetExpOneKey( Buffer& KeyExpOneBlob );
    static Buffer CreateSimpleBlob( const Key& keyExpOne, AlgType nAlgId, 
                                    const Buffer& keyBuffer );

private:

    HCRYPTKEY mKey;
    mutable Buffer mKeyData; // cached
    static CryptContext* mpContext;

};

///////////////////////////////////////////////////////////////////////////////
//
// Encryption/Decryption object. Stores the key, which also defines the
// crypto algorithm and other critical information.

class Cipher
{
public:

    Cipher( const Key& );
    ~Cipher();

    bool Encrypt( Buffer&, bool bFinal=false ) const;
    bool Decrypt( Buffer&, bool bFinal=false ) const;

private:

    // Disabled
    Cipher();
    Cipher( const Cipher& );
    Cipher& operator=( const Cipher& );

private:

    Key mKey;
};

///////////////////////////////////////////////////////////////////////////////
//
// Wrap HCRYPTHASH. Includes the hash algorithm and intermediate hash data.
// Also stores the key data for HMAC hashing.

class Hash
{
public:

    Hash();
    ~Hash();

    static void SetContext( CryptContext* );
    static bool GenRandom( unsigned long nBytes, Buffer& randBuffer );

    bool Create( AlgType );
    bool Create( AlgType, const Key& );
    void Destroy();
    bool IsValid() const;

    bool SetHMACAlgorithm( AlgType );

    AlgType GetAlgorithm() const;
    unsigned long GetHashSize() const;

    bool HashData( const Buffer& ) const;
    Buffer GetHashValue() const;

private:

    // disabled
    Hash( const Hash& );
    Hash& operator=( const Hash& );

private:

    HCRYPTHASH mHash;
    static CryptContext* mpContext;

};


} // end namespace SecureSock

#endif // SECURE_SOCK_CRYPTO_H

///////////////////////////////////////////////////////////////////////////////
