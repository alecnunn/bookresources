///////////////////////////////////////////////////////////////////////////////
//
//  Crypto.cpp
//
//  Wrapper for Win32 cryptographic algorithms, hashing and key management.
//  Can easily be replaced with public domain code (e.g. Crypto++ library)
//  as needed to support other platforms.
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

#include "Crypto.h"

namespace SecureSock
{

// Win32 export structure for symmetric key data
struct SimpleBlob
{
    BLOBHEADER hdr;
    ALG_ID     algid;

    // PKCS #1, type 2 encryption block
    BYTE       key[1];  // length = (pubkey.bitlen/8)
};

// Win32 export structure for private key data
struct PrivateKeyBlob
{
    BLOBHEADER hdr;
    RSAPUBKEY  rsapubkey;
    BYTE       modulus[1];

    // Actual data is sized based on rsapubkey data
    // BYTE modulus         [rsapubkey.bitlen/8];
    // BYTE prime1          [rsapubkey.bitlen/16];
    // BYTE prime2          [rsapubkey.bitlen/16];
    // BYTE exponent1       [rsapubkey.bitlen/16];
    // BYTE exponent2       [rsapubkey.bitlen/16];
    // BYTE coefficient     [rsapubkey.bitlen/16];
    // BYTE privateExponent [rsapubkey.bitlen/8];
};

///////////////////////////////////////////////////////////////////////////////
//
// Static data

CryptContext* Key::mpContext  = NULL;
CryptContext* Hash::mpContext = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

CryptContext::CryptContext() 
: 
    mProv( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

CryptContext::~CryptContext()
{
    Release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Acquire the HCRYPTPROV context. Must be called before any other class
// functions will succeed.

bool CryptContext::Acquire( const char* sContainer, const char* sProvider,
                            unsigned long nProviderType, unsigned long nFlags )
{
    Release();

    BOOL bSuccess = CryptAcquireContext( &mProv, sContainer, sProvider,
                                         nProviderType, nFlags );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Destroy the current HCRYPTPROV context

void CryptContext::Release()
{
    if( mProv != NULL )
    {
        BOOL bSuccess = CryptReleaseContext( mProv, 0 );
        assert( bSuccess != 0 );
        USED( bSuccess );
        mProv = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// TRUE if we have a non-NULL HCRYPTPROV context

bool CryptContext::IsValid() const
{
    return( mProv != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a cryptographically random value using the current HCRYPTPROV
// context. Returns false if the current context doesn't support this function.

bool CryptContext::GenRandom( unsigned long nBytes, Buffer& randBuffer ) const
{
    assert( mProv != NULL );
    assert( nBytes > 0 );
    randBuffer.resize( nBytes );

    BYTE* pData = GetData( randBuffer );
    BOOL bSuccess = CryptGenRandom( mProv, nBytes, pData );
    assert( bSuccess );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Direct access to HCRYPTPROV handle. Only available to friends.

HCRYPTPROV CryptContext::GetHandle() const // private
{
    return mProv;
}

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

Key::Key()
: 
    mKey( NULL ),
    mKeyData()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

Key::Key( AlgType nAlgId )
: 
    mKey( NULL ),
    mKeyData()
{
    bool bSuccess = Create( nAlgId );    
    assert( bSuccess );
    USED( bSuccess );
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

Key::~Key()
{ 
    Destroy();
}

///////////////////////////////////////////////////////////////////////////////
//
// Key copy ctor

Key::Key( const Key& key )
:
    mKey( NULL ),
    mKeyData( key.mKeyData )
{
    if( key.mKey != NULL )
    {
        BOOL bSuccess = CryptDuplicateKey( key.mKey, NULL, 0, &mKey );
        assert( bSuccess );
        USED( bSuccess );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Key copy operator

Key& Key::operator=( const Key& key )
{
    Key temp( key );
    Swap( temp );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the global crypto context for key data

void Key::SetContext( CryptContext* pContext ) // static
{
    assert( pContext != NULL );
    mpContext = pContext;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a cryptographically random value. 

bool Key::GenRandom( unsigned long nBytes, Buffer& randBuffer ) // static
{
    assert( mpContext != NULL );
    return mpContext->GenRandom( nBytes, randBuffer );
}

///////////////////////////////////////////////////////////////////////////////
//
// Swap (see Exceptional C++, Herb Sutter, Item 12 )

void Key::Swap( Key& key )
{
    std::swap( mKey, key.mKey );
    mKeyData.swap( key.mKeyData );
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a cipher key HCRYPTKEY of the specified algorithm type (e.g. CALG_DES).
// The key is generated automatically using a cryptographically strong random
// number generator. The flags define features of the key and enciphering 
// method. If the key length is zero, a default key length is used. For instance,
// a DES key is 64 bits (56 bits of which are actually used by the algorithm).

bool Key::Create( AlgType nAlgId, unsigned long nKeyLength, 
                  unsigned long nFlags )
{
    assert( mpContext != NULL );
    Destroy();

    // Key length (in bits) is specified in high word of flags
    nFlags |= ( nKeyLength << ( sizeof(WORD) * CHAR_BIT ) );

    BOOL bSuccess = CryptGenKey( mpContext->GetHandle(), nAlgId, nFlags, &mKey );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a cipher key HCRYPTKEY of the specified algorithm type (e.g. CALG_DES).
// The key is initialized with the keyBuffer, which also defines the length
// of the key.

bool Key::Create( AlgType nAlgId, const Buffer& keyBuffer )
{
    // The CryptoAPI does not directly allow new keys to be created 
    // based on existing bits. However, KnowledgeBase article 
    // Q228786 describes a method for directly setting keys.
    // That method is used here:

    // Create an "exponent of one" key
    Key keyExpOne;
    if( !keyExpOne.CreateExpOne() )
        return false;

    // Validate incoming symmetric key size by creating a temp key 
    // using the given algorithm and key buffer size
    Key keyTemp;
    if( !keyTemp.Create( nAlgId, keyBuffer.length() * CHAR_BIT ) )
        return false;

    // Create a simple key blob with the actual unencrypted key data using
    // the "exponent of one" as a pseudo encryption key
    Buffer keyBlob = CreateSimpleBlob( keyExpOne, nAlgId, keyBuffer );

    // Import the blob to create the HCRYPTKEY that we want
    if( !Import( keyBlob, keyExpOne, CRYPT_EXPORTABLE ) )
        return false;

    // Cache the buffer
    mKeyData = keyBuffer;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a new key by importing an existing "key blob" buffer. See CryptoAPI
// for additional details about key blobs.

bool Key::Import( const Buffer& keyBlob, const Key& keyPublic, 
                  unsigned long nFlags )
{
    assert( mpContext != NULL );
    Destroy();

    BOOL bSuccess = CryptImportKey( mpContext->GetHandle(), 
                                    keyBlob.data(), 
                                    keyBlob.length(),
                                    keyPublic.GetHandle(), nFlags, &mKey );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy the key information to a "key blob" buffer. See CryptoAPI for
// additional details about key blobs.

Buffer Key::Export( unsigned long nBlobType, const Key& keyExp ) const
{
    assert( mKey != NULL );

    // Determine the size of the key blob
    DWORD nDataBytes = 0;
    BOOL bSuccess = CryptExportKey( mKey, keyExp.mKey, nBlobType, 0, 
                                    NULL, &nDataBytes );
    if( !bSuccess )
        return Buffer();

    // Make the buffer big enough to hold the key blob
    Buffer KeyBlob;
    KeyBlob.resize( nDataBytes );
    BYTE* pData = GetData( KeyBlob );

    // Grab the blob
    bSuccess = CryptExportKey( mKey, keyExp.mKey, nBlobType, 0, 
                               pData, &nDataBytes );
    if( !bSuccess )
        return Buffer();
    return KeyBlob;
}

///////////////////////////////////////////////////////////////////////////////
//
// Destroy the HCRYPTKEY

void Key::Destroy()
{
    if( mKey != NULL )
    {
        BOOL bSuccess = CryptDestroyKey( mKey );
        assert( bSuccess != 0 );
        USED( bSuccess );
        mKey = NULL;

        // Remove any cached key data
        mKeyData.erase();
    }
    assert( mKeyData.empty() );
}

///////////////////////////////////////////////////////////////////////////////
//
// True if key has been created

bool Key::IsValid() const
{
    return( mKey != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the cipher mode (CRYPT_MODE_CBC, _CFB, _ECB or _OFB)

bool Key::SetMode( unsigned long nMode )
{
    assert( mKey != NULL );
    BYTE* pData = reinterpret_cast< BYTE* >( &nMode );

    BOOL bSuccess = CryptSetKeyParam( mKey, KP_MODE, pData, 0 );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the initialization vector

bool Key::SetIV( const Buffer& IV )
{
    assert( mKey != NULL );

    const BYTE* pIV = IV.data();
    assert( pIV != NULL );

    // Validate IV length
    unsigned long nBytes = IV.length();
    assert( nBytes == GetBlockLength() / 8 );
    USED( nBytes );

    BOOL bSuccess = CryptSetKeyParam( mKey, KP_IV, 
                                      const_cast< BYTE* >( pIV ), 0 );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get key algorithm

AlgType Key::GetAlgorithm() const
{
    assert( mKey != NULL );

    ALG_ID nAlgorithm;
    DWORD nDataBytes = sizeof( nAlgorithm );
    BYTE* pData = reinterpret_cast< BYTE* >( &nAlgorithm );

    BOOL bSuccess = CryptGetKeyParam( mKey, KP_ALGID, pData, &nDataBytes, 0 );
    assert( bSuccess );
    assert( nDataBytes == sizeof( nAlgorithm ) );
    USED( bSuccess );
    return nAlgorithm;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the key length in bits

unsigned long Key::GetKeyLength() const
{
    assert( mKey != NULL );
    DWORD nKeyBits;
    DWORD nDataBytes = sizeof( nKeyBits );
    BYTE* pData = reinterpret_cast< BYTE* >( &nKeyBits );

    BOOL bSuccess = CryptGetKeyParam( mKey, KP_KEYLEN, pData, &nDataBytes, 0 );
    assert( bSuccess );
    assert( nDataBytes == sizeof( nKeyBits ) );
    USED( bSuccess );
    return nKeyBits;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the block length of the cipher in bits

unsigned long Key::GetBlockLength() const
{
    assert( mKey != NULL );
    DWORD nBlockBits;
    DWORD nDataBytes = sizeof( nBlockBits );
    BYTE* pData = reinterpret_cast< BYTE* >( &nBlockBits );

    BOOL bSuccess = CryptGetKeyParam( mKey, KP_BLOCKLEN, pData, &nDataBytes, 0 );
    assert( bSuccess );
    assert( nDataBytes == sizeof( nBlockBits ) );
    USED( bSuccess );

    // Stream ciphers give a zero block length. For consistency, we return
    // a block length of 8 (1 byte) for stream ciphers.
    if( nBlockBits == 0 )
        nBlockBits = CHAR_BIT;

    return nBlockBits;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the cipher mode (CRYPT_MODE_CBC, _CFB, _ECB or _OFB)

unsigned long Key::GetMode() const
{
    assert( mKey != NULL );
    DWORD nMode;
    DWORD nDataBytes = sizeof( nMode );
    BYTE* pData = reinterpret_cast< BYTE* >( &nMode );

    BOOL bSuccess = CryptGetKeyParam( mKey, KP_MODE, pData, &nDataBytes, 0 );
    assert( bSuccess );
    assert( nDataBytes == sizeof( nMode ) );
    USED( bSuccess );
    return nMode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the initialization vector size in bytes

unsigned long Key::GetIVSize() const
{
    assert( mKey != NULL );

    // Determine size
    DWORD nDataBytes = 0;
    BOOL bSuccess = CryptGetKeyParam( mKey, KP_IV, NULL, &nDataBytes, 0 );
    if( !bSuccess )
        return 0;

    return nDataBytes;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the initialization vector

Buffer Key::GetIV() const
{
    assert( mKey != NULL );

    DWORD nDataBytes = GetIVSize();
    if( nDataBytes == 0 )
        return Buffer();

    // Make the buffer big enough to hold the IV
    Buffer ivBuffer;
    ivBuffer.resize( nDataBytes );
    BYTE* pData = GetData( ivBuffer );

    // Extract the IV
    BOOL bSuccess = CryptGetKeyParam( mKey, KP_IV, pData, &nDataBytes, 0 );
    if( !bSuccess )
        return Buffer();
    return ivBuffer;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the true key data (only works for symmetric keys, not for 
// public/private key pairs).

Buffer Key::GetKeyData() const
{
    // If we already have the key, just return it
    if( !mKeyData.empty() )
        return mKeyData;

    // The CryptoAPI does not directly expose key bits. However, 
    // KnowledgeBase article Q228786 describes a method for extracting
    // the actual bits. That method is used here. This method is painfully
    // slow, so the actual key data is cached once it has been obtained.

    // Create an "exponent of one" key
    Key keyExpOne;
    if( !keyExpOne.CreateExpOne() )
        return Buffer();

    // Extract the key blob. The key blob is encrypted with the 
    // "exponent of one" key, which means it's not encrypted at all!
    Buffer keyBlob = Export( SIMPLEBLOB, keyExpOne );
    if( keyBlob.empty() )
        return Buffer();

    // Validate the blob
    const SimpleBlob* pKeyBlob = reinterpret_cast< const SimpleBlob* >
                                 ( keyBlob.data() );
    assert( pKeyBlob->hdr.bType    == SIMPLEBLOB );
    assert( pKeyBlob->hdr.bVersion == 2 );
    assert( pKeyBlob->hdr.reserved == 0 );
    assert( pKeyBlob->hdr.aiKeyAlg == GetAlgorithm() );
    assert( pKeyBlob->algid        == keyExpOne.GetAlgorithm() );

    // Fill the key data buffer
    unsigned long nKeyBytes = GetKeyLength() / 8;
    mKeyData.resize( nKeyBytes );
    unsigned char* pData = GetData( mKeyData );

    // Extract the data (in reverse order)
    const unsigned char* pKey = pKeyBlob->key;
    pKey += ( nKeyBytes - 1 );
    for( unsigned long n = 0; n < nKeyBytes; ++n, --pKey, ++pData )
        *pData = *pKey;

    return mKeyData;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the HCRYPTKEY data; only Cipher class has access

HCRYPTKEY Key::GetHandle() const // private
{
    return mKey;
}

///////////////////////////////////////////////////////////////////////////////
//
// Create an "exponent of one" key. Data encrypted with an "exponent of one"
// key remains unencrypted. Keys of this type are useful when directly setting
// or extracting key data from an HCRYPTKEY, since the CryptoAPI process of
// importing/exporting always requires that the key blob be "encrypted."

bool Key::CreateExpOne() // private
{
    Destroy();

    // Create a generic private key
    Key keyPrivate;
    if( !keyPrivate.Create( AT_KEYEXCHANGE, 0, CRYPT_EXPORTABLE ) )
        return false;

    // Extract the key data into a "key blob" buffer
    Buffer KeyExpOneBlob = keyPrivate.Export( PRIVATEKEYBLOB, Key() );
    if( KeyExpOneBlob.empty() )
        return false;

    // Adjust the key blob data to represent an "exponent of one" key
    SetExpOneKey( KeyExpOneBlob );

    // Create a true HCRYPTKEY from the "exponent of one" key blob data
    return Import( KeyExpOneBlob, Key() );
}

///////////////////////////////////////////////////////////////////////////////
//
// Adjust private key blob data to be an "exponent of one" key. See KB
// article Q228786 for details. Description of private key blob layout in
// CryptoAPI SDK.

void Key::SetExpOneKey( Buffer& KeyExpOneBlob ) // private static
{
    // Directly access the bits
    unsigned char* pData = GetData( KeyExpOneBlob );
    PrivateKeyBlob* pKeyBlob = reinterpret_cast< PrivateKeyBlob* >( pData );

     // Extract the length of the key
    unsigned long nKeyBytes = pKeyBlob->rsapubkey.bitlen / 8;

    // Skip to rsapubkey.pubexp
    pData += ( sizeof(BLOBHEADER) + sizeof(RSAPUBKEY) - sizeof(DWORD) );

    // Convert pubexp to 1
    unsigned long nOne = 1;
    memcpy( pData, &nOne, sizeof( nOne ) );
    pData += sizeof( nOne );

    // Skip modulus, prime1, prime2
    pData += nKeyBytes;
    pData += nKeyBytes / 2;
    pData += nKeyBytes / 2;

    // Convert exponent1 to 1
    *pData = 1;
    ++pData;
    unsigned long n;
    for( n = 1; n < (nKeyBytes / 2); ++n, ++pData )
        *pData = 0;

    // Convert exponent2 to 1
    *pData = 1;
    ++pData;
    for( n = 1; n < (nKeyBytes / 2); ++n, ++pData )
        *pData = 0;

    // Skip coefficient
    pData += nKeyBytes / 2;

    // Convert privateExponent to 1
    *pData = 1;
    ++pData;
    for( n = 1; n < nKeyBytes; ++n, ++pData )
        *pData = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a simple key blob buffer based on the exponent of one key and the
// given symmetric key buffer

Buffer Key::CreateSimpleBlob( const Key& keyExpOne, AlgType nAlgId, 
                              const Buffer& keyBuffer ) // private static
{
    // Extract the private key's public key length
    DWORD dwPubKeyBytes = keyExpOne.GetKeyLength() / 8;

    // Create a simple key blob
    //
    // The simple key blob has the following structure:
    //
    // BLOBHEADER blobHeader;
    // ALG_ID     algid;
    // BYTE       key[ pubkey.bitlen/8 ];
 
    Buffer keyBlob;
    keyBlob.resize( sizeof( BLOBHEADER ) + sizeof( ALG_ID ) + dwPubKeyBytes );

    // Fill out the blob header
    SimpleBlob* pKeyBlob = reinterpret_cast< SimpleBlob* >( GetData( keyBlob ) );
    pKeyBlob->hdr.bType    = SIMPLEBLOB;
    pKeyBlob->hdr.bVersion = 2;
    pKeyBlob->hdr.reserved = 0;
    pKeyBlob->hdr.aiKeyAlg = nAlgId;
    pKeyBlob->algid        = keyExpOne.GetAlgorithm();

    // Key data is in the format of a PKCS #1, type 2 encryption block.
    // In our case, this data block contains the key material in reverse
    // order, followed by a single zero byte, followed by random data,
    // followed by byte 2, followed by byte 0.

    unsigned char* pKey = pKeyBlob->key;

    // Key material (in reverse order)
    const unsigned char* pKeyBuffer = keyBuffer.data();
    unsigned long nKeyBufferLen = keyBuffer.length();
    unsigned long n;
    for( n=0; n < nKeyBufferLen; ++n, ++pKey )
        *pKey = pKeyBuffer[ nKeyBufferLen-n-1 ];

    // Zero byte
    *pKey = 0;
    ++pKey;

    // Random bytes
    assert( mpContext != NULL );
    unsigned long nRandom = dwPubKeyBytes - nKeyBufferLen - 1 - 2;
    CryptGenRandom( mpContext->GetHandle(), nRandom, pKey );

    // None of the random bytes may be zero
    for( n=0; n < nRandom; ++n, ++pKey )
    {
        if( *pKey == 0 )
            *pKey = 1;
    }

    // Two byte
    *pKey = 2;
    ++pKey;

    // Zero byte
    *pKey = 0;

    return keyBlob;
}

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

Cipher::Cipher( const Key& key )
: 
    mKey( key )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

Cipher::~Cipher()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Encrypt the incoming plain text using the stored key and algorithm.
// Results are returned via the input buffer, which may be resized by the
// function.

bool Cipher::Encrypt( Buffer& buffer, bool bFinal ) const
{
    assert( mKey.GetHandle() != NULL );
    DWORD nPlainBytes = buffer.length();

    // If this is not the final block, length must be a multiple of the
    // block size.
    if( !bFinal )
        assert( nPlainBytes % ( mKey.GetBlockLength() / 8 ) == 0 );

    // Determine size of resulting cipher text
    DWORD nCipherBytes = nPlainBytes;
    BOOL bSuccess = CryptEncrypt( mKey.GetHandle(), NULL, bFinal, 0, NULL,
                                  &nCipherBytes, nCipherBytes );
    if( !bSuccess )
        return false;

    // Adjust buffer to hold encrypted data
    assert( nCipherBytes >= nPlainBytes );
    buffer.resize( nCipherBytes );
    BYTE* pData = GetData( buffer );
    assert( pData != NULL );

    // Encryption pass
    bSuccess = CryptEncrypt( mKey.GetHandle(), NULL, bFinal, 0, pData,
                             &nPlainBytes, nCipherBytes );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Decrypt the incoming cipher text using the stored key and algorithm.
// Results are returned via the input buffer.

bool Cipher::Decrypt( Buffer& buffer, bool bFinal ) const
{
    assert( mKey.GetHandle() != NULL );
    DWORD nCipherBytes = buffer.length();

    // If this is not the final block, length must be a multiple of the
    // block size.
    if( !bFinal )
        assert( nCipherBytes % ( mKey.GetBlockLength() / 8 ) == 0 );

    BYTE* pData = GetData( buffer );
    assert( pData != NULL );

    // Decryption pass
    DWORD nPlainBytes = nCipherBytes;
    BOOL bSuccess = CryptDecrypt( mKey.GetHandle(), NULL, bFinal, 0, pData,
                                 &nPlainBytes );
    if( !bSuccess )
        return false;

    // Adjust size of output buffer (plain text)
    assert( nPlainBytes <= nCipherBytes );
    buffer.resize( nPlainBytes );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

Hash::Hash()
: 
    mHash( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

Hash::~Hash()
{ 
    Destroy();
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the global crypto context for hash data

void Hash::SetContext( CryptContext* pContext ) // static
{
    assert( pContext != NULL );
    mpContext = pContext;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a cryptographically random value. 

bool Hash::GenRandom( unsigned long nBytes, Buffer& randBuffer ) // static
{
    assert( mpContext != NULL );
    return mpContext->GenRandom( nBytes, randBuffer );
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a HCRYPTHASH of the specified algorithm type (e.g. CALG_MD5).

bool Hash::Create( AlgType nAlgId )
{
    return Create( nAlgId, Key() );
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a HCRYPTHASH of the specified algorithm type (e.g. CALG_MD5).
// Use key value for keyed hash (e.g. CALG_HMAC).

bool Hash::Create( AlgType nAlgId, const Key& key )
{
    assert( mpContext != NULL );
    Destroy();

    assert( !key.IsValid() || key.GetMode() == CRYPT_MODE_CBC );

    BOOL bSuccess = CryptCreateHash( mpContext->GetHandle(), nAlgId, 
                                     key.GetHandle(), 0, &mHash );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Destroy the HCRYPTHASH

void Hash::Destroy()
{
    if( mHash != NULL )
    {
        BOOL bSuccess = CryptDestroyHash( mHash );
        assert( bSuccess != 0 );
        USED( bSuccess );
        mHash = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// True if hash has been created

bool Hash::IsValid() const
{
    return( mHash != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set HMAC algorithm

bool Hash::SetHMACAlgorithm( AlgType nAlgId )
{
    assert( mHash != NULL );
    assert( GetAlgorithm() == CALG_HMAC );

    HMAC_INFO hmacData = { nAlgId };
    BYTE* pData = const_cast< BYTE* >( reinterpret_cast< const BYTE* >( &hmacData ) );

    BOOL bSuccess = CryptSetHashParam( mHash, HP_HMAC_INFO, pData, 0 );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the hashing algorithm. If the algorithm is CALG_HMAC, the actual hashing
// algorithm is stored in hash param HP_HMAC_INFO.

AlgType Hash::GetAlgorithm() const
{
    assert( mHash != NULL );

    ALG_ID nAlgorithm;
    DWORD nDataBytes = sizeof( nAlgorithm );
    BYTE* pData = reinterpret_cast< BYTE* >( &nAlgorithm );

    BOOL bSuccess = CryptGetHashParam( mHash, HP_ALGID, pData, &nDataBytes, 0 );
    if( !bSuccess )
        return 0;

    assert( nDataBytes == sizeof( nAlgorithm ) );
    return nAlgorithm;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the hash length in bytes (e.g. MD5 gives 16). Returns zero on error.

unsigned long Hash::GetHashSize() const
{
    assert( mHash != NULL );

    DWORD nBytes;
    DWORD nDataBytes = sizeof( nBytes );
    BYTE* pData = reinterpret_cast< BYTE* >( &nBytes );

    BOOL bSuccess = CryptGetHashParam( mHash, HP_HASHSIZE, pData, &nDataBytes, 0 );
    if( !bSuccess )
        return 0;

    assert( nDataBytes == sizeof( nBytes ) );
    return nBytes;
}

///////////////////////////////////////////////////////////////////////////////
//
// Hash the input buffer. If this is successful, the hash output is available
// using GetHashValue(). This may be called multiple times on chunks of data
// before calling GetHashValue().

bool Hash::HashData( const Buffer& inputBuffer ) const
{
    assert( mHash != NULL );
    const BYTE* pData = inputBuffer.data();
    assert( pData != NULL );
    unsigned long nBytes = inputBuffer.length();

    BOOL bSuccess = CryptHashData( mHash, pData, nBytes, 0 );
    return( bSuccess != 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the output from a successful call (or calls) to HashData()

Buffer Hash::GetHashValue() const
{
    DWORD nBytes = GetHashSize();
    if( nBytes == 0 )
        return Buffer();

    // Make the buffer big enough to hold the hash
    Buffer hashBuffer;
    hashBuffer.resize( nBytes );
    BYTE* pData = GetData( hashBuffer );

    // Extract the hash value
    BOOL bSuccess = CryptGetHashParam( mHash, HP_HASHVAL, pData, &nBytes, 0 );
    if( !bSuccess )
        return Buffer();
    return hashBuffer;
}

} // end namespace SecureSock

///////////////////////////////////////////////////////////////////////////////
