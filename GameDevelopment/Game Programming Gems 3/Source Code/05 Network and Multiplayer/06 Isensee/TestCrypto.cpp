///////////////////////////////////////////////////////////////////////////////
//
//  TestCrypto.cpp
//
//  Testing code for Crypto objects
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

#include "common.h"
#include "crypto.h"

namespace // anonymous
{
using namespace SecureSock;

///////////////////////////////////////////////////////////////////////////////
//
// Test CryptContext

void TestContext()
{
    CryptContext context;
    VERIFY( !context.IsValid() );
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
    VERIFY( context.IsValid() );
    context.Release();
    VERIFY( !context.IsValid() );
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );

    Buffer rand;
    Buffer origr( rand );
    VERIFY( context.GenRandom( 256, rand ) );
    VERIFY( rand.length() == 256 );
    VERIFY( rand != origr );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test Key object

void TestKey()
{
    CryptContext context;
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
    Key::SetContext( &context );
    Hash::SetContext( &context );

    Buffer rand;
    Buffer origr( rand );
    VERIFY( Key::GenRandom( 256, rand ) );
    VERIFY( rand.length() == 256 );
    VERIFY( rand != origr );

    Key key;
    VERIFY( !key.IsValid() );
    VERIFY( key.Create( CALG_DES ) );
    VERIFY( key.IsValid() );
    key.Destroy();
    VERIFY( !key.IsValid() );
    VERIFY( key.Create( CALG_DES ) );

    Key keyCopy( key );
    key = keyCopy;
    VERIFY( keyCopy.IsValid() );
    VERIFY( key.IsValid() );

    VERIFY( key.GetBlockLength() == 64 );
    VERIFY( key.GetAlgorithm() == CALG_DES );

    VERIFY( key.GetMode() == CRYPT_MODE_CBC );
    VERIFY( key.SetMode( CRYPT_MODE_ECB ) );
    VERIFY( key.GetMode() == CRYPT_MODE_ECB );
    VERIFY( key.GetKeyLength() == 64 );

    Key keyAlg( CALG_DES );
    VERIFY( key.IsValid() );
    VERIFY( key.GetAlgorithm() == CALG_DES );

    Buffer iv;
    Assign( iv, "iv123456", 8 );
    VERIFY( key.SetIV( iv ) );
    VERIFY( key.GetIVSize() == 8 );
    VERIFY( key.GetIV() == iv );

    VERIFY( key.Create( CALG_RC2, 40 ) );
    VERIFY( key.GetKeyLength() == 40 );
    VERIFY( key.GetBlockLength() == 64 );

    VERIFY( key.Create( CALG_RC2, 48 ) );
    VERIFY( key.GetKeyLength() == 48 );
    VERIFY( key.GetBlockLength() == 64 );

    VERIFY( key.Create( CALG_RC4, 40 ) );
    VERIFY( key.GetBlockLength() == 8 );
    VERIFY( key.GetKeyLength() == 40 );

    Buffer keyBuffer;
    Assign( keyBuffer, "abcxyz12", 8 );
    VERIFY( key.Create( CALG_DES, keyBuffer ) );
    VERIFY( key.GetKeyData() == keyBuffer );

    Assign( keyBuffer, "xxxyyy", 6 );
    VERIFY( key.Create( CALG_RC2, keyBuffer ) );
    VERIFY( key.GetKeyData() == keyBuffer );

    // Can only get the key buffer if we created it
    VERIFY( key.Create( CALG_DES ) );
    keyBuffer = key.GetKeyData();
    VERIFY( keyBuffer.empty() );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test Cipher object

void TestCipher()
{
    CryptContext context;
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
    Key::SetContext( &context );
    Hash::SetContext( &context );

    struct KeyInfo
    {
        AlgType       nAlgId;
        unsigned long nKeyLength;
    };

    KeyInfo keyInfo[4] =
    {
        { CALG_DES  },
        { CALG_RC2, 40 },
        { CALG_RC2, 48 },
        { CALG_RC4, 40 },
    };

    Buffer ptext;
    Buffer ctext;
    unsigned long nMax = 128 * 1024;
    ptext.reserve( nMax );
    ctext.reserve( nMax );

    for( int i = 0; i < 4; ++i )
    {
        // Encrypt and decrypt large chunks of random data
        Key key;
        VERIFY( key.Create( keyInfo[i].nAlgId, keyInfo[i].nKeyLength ) );
        Cipher cipher(key);
        unsigned long j;
        for( j = 1; j < nMax; j *= 2 )
        {
            VERIFY( context.GenRandom( j, ptext ) );
            ctext = ptext;
            VERIFY( ctext.length() == ptext.length() );
            VERIFY( cipher.Encrypt( ctext, true ) );
            VERIFY( ctext != ptext );
            VERIFY( cipher.Decrypt( ctext, true ) );
            VERIFY( ctext.length() == ptext.length() );
            VERIFY( ctext.compare( 0, ctext.length(), ptext ) == 0 );
        }

        // Encrypt and decrypt small chunks of random data
        srand( GetTickCount() );

        // Generate a random number of plaintext bytes
        unsigned long nBytes = rand() % nMax;
        VERIFY( context.GenRandom( nBytes, ptext ) );
        Buffer psave( ptext );

        // Encrypt/decrypt in 1K chunks
        unsigned long nBlockBytes = key.GetBlockLength() / 8;
        unsigned long nChunkBytes = 1024;
        assert( nChunkBytes % nBlockBytes == 0 );
        USED( nBlockBytes );
        unsigned long nWholeChunks = nBytes / nChunkBytes;
        unsigned long nLastChunk = nBytes % nChunkBytes;
        unsigned long k;

        // Encrypt block
        ctext.erase();
        for( j = 0, k = 0; j < nWholeChunks; ++j, k += nChunkBytes )
        {
            Buffer block;
            block.assign( ptext.data() + k, nChunkBytes );
            bool bIsLast = false;
            if( nLastChunk == 0 && j == nWholeChunks-1 )
                bIsLast = true;
            VERIFY( cipher.Encrypt( block, bIsLast ) );
            ctext += block;
        }
        if( nLastChunk )
        {
            Buffer block;
            block.assign( ptext.data() + k, nLastChunk );
            VERIFY( cipher.Encrypt( block, true ) );
            ctext += block;
        }

        // Decrypt block
        ptext.erase();
        for( j = 0, k = 0; j < nWholeChunks; ++j, k += nChunkBytes )
        {
            Buffer block;
            block.assign( ctext.data() + k, nChunkBytes );
            bool bIsLast = false;
            if( nLastChunk == 0 && j == nWholeChunks-1 )
                bIsLast = true;
            VERIFY( cipher.Decrypt( block, bIsLast ) );
            ptext += block;
        }
        if( nLastChunk )
        {
            Buffer block;
            unsigned long nEncipheredChunk = ctext.length() - k;
            block.assign( ctext.data() + k, nEncipheredChunk );
            VERIFY( cipher.Decrypt( block, true ) );
            ptext += block;
        }

        // Validate
        VERIFY( ptext.length() == psave.length() );
        VERIFY( ptext == psave );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Test Hash object

void TestHash()
{
    CryptContext context;
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
    Key::SetContext( &context );
    Hash::SetContext( &context );

    Buffer rand;
    Buffer origr( rand );
    VERIFY( Hash::GenRandom( 256, rand ) );
    VERIFY( rand.length() == 256 );
    VERIFY( rand != origr );

    Hash hash;
    VERIFY( !hash.IsValid() );
    VERIFY( hash.Create( CALG_MD5 ) );
    VERIFY( hash.IsValid() );
    hash.Destroy();
    VERIFY( !hash.IsValid() );

    struct TestData
    {
        unsigned char* pInput;
        unsigned long nInput;
        unsigned char* pHash;
    };

    unsigned char pEmpty[]        = { 0 };
    unsigned char pA[]            = { 'a' };
    unsigned char pAbc[]          = { 'a', 'b', 'c' };
    unsigned char pMessageDigest[]= { 'm', 'e', 's', 's', 'a', 'g', 'e', ' ', 'd', 'i',
                                      'g', 'e', 's', 't' };
    unsigned char pAlphaLower[]   = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
                                      'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                                      'u', 'v', 'w', 'x', 'y', 'z' };
    unsigned char pAlphaNum[]     = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
                                      'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                                      'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
                                      'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                                      'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                                      '8', '9' };
    unsigned char pNumeric[]      = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

    unsigned char pHashEmpty[]        = { 0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04, 
                                          0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E };
    unsigned char pHashA[]            = { 0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8, 
                                          0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61 };
    unsigned char pHashAbc[]          = { 0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0, 
                                          0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72 };
    unsigned char pHashMessageDigest[]= { 0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D, 
                                          0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0 };
    unsigned char pHashAlphaLower[]   = { 0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00, 
                                          0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B };
    unsigned char pHashAlphaNum[]     = { 0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5, 
                                          0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F };
    unsigned char pHashNumeric[]      = { 0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55, 
                                          0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A };

    // Refer to RFC1321 for testing values
    TestData testData[7] =
    {
        { pEmpty,          0, pHashEmpty },
        { pA,              1, pHashA },
        { pAbc,            3, pHashAbc },
        { pMessageDigest, 14, pHashMessageDigest },
        { pAlphaLower,    26, pHashAlphaLower },
        { pAlphaNum,      62, pHashAlphaNum },
        { pNumeric,       80, pHashNumeric },
    };

    for( int i = 0; i < 7; ++i )
    {
        VERIFY( hash.Create( CALG_MD5 ) );
        VERIFY( hash.GetAlgorithm() == CALG_MD5 );
        VERIFY( hash.GetHashSize() == 16 );

        Buffer input( testData[i].pInput, testData[i].nInput );
        VERIFY( hash.HashData( input ) );

        Buffer hashBuf( testData[i].pHash, 16 );
        VERIFY( hash.GetHashValue() == hashBuf );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Test HMAC

void TestHMAC()
{
    CryptContext context;
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
    Key::SetContext( &context );
    Hash::SetContext( &context );

    Hash hash;
    Key key;
    VERIFY( key.Create( CALG_DES ) );

    VERIFY( hash.Create( CALG_HMAC, key ) );
    VERIFY( hash.SetHMACAlgorithm( CALG_MD5 ) );

    srand( GetTickCount() );
    unsigned long nBytes = rand();
    Buffer buffer;
    VERIFY( context.GenRandom( nBytes, buffer ) );
    VERIFY( hash.HashData( buffer ) );
    Buffer hmac = hash.GetHashValue();
    VERIFY( hmac.length() == 16 );

    hash.Destroy();
    VERIFY( hash.Create( CALG_HMAC, key ) );
    VERIFY( hash.SetHMACAlgorithm( CALG_SHA1 ) );
    VERIFY( hash.HashData( buffer ) );
    hmac = hash.GetHashValue();
    VERIFY( hmac.length() == 20 );

    struct TestData
    {
        unsigned char* pInput;
        unsigned long  nInput;
        unsigned char* pKey;
        unsigned long  nKey;
        unsigned char* pHMAC;
    };

    unsigned char pWhat[]    = { 'w', 'h', 'a', 't', ' ', 'd', 'o', ' ', 'y', 'a', 
                                 ' ', 'w', 'a', 'n', 't', ' ', 'f', 'o', 'r', ' ', 
                                 'n', 'o', 't', 'h', 'i', 'n', 'g', '?' };
    unsigned char pKeyWhat[] = { 'J', 'e', 'f', 'e' };
    unsigned char pMacWhat[] = { 0x75, 0x0C, 0x78, 0x3E, 0x6A, 0xB0, 0xB5, 0x03, 
                                 0xEA, 0xA8, 0x6E, 0x31, 0x0A, 0x5D, 0xB7, 0x38 };

    /*
    unsigned char pHiThere[] =    { 'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e' };
    unsigned char pKeyHiThere[] = { 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 
                                    0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB };
    unsigned char pMacHiThere[] = { 0x92, 0x94, 0x72, 0x7A, 0x36, 0x38, 0xBB, 0x1C,
                                    0x13, 0xF4, 0x8E, 0xF8, 0x15, 0x8B, 0xFC, 0x9D };

    unsigned char pD[]    = { 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                              0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                              0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                              0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                              0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                              0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                              0xDD, 0xDD };
    unsigned char pKeyD[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 
                              0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
    unsigned char pMacD[] = { 0x56, 0xBE, 0x34, 0x52, 0x1D, 0x14, 0x4C, 0x88, 
                              0xDB, 0xB8, 0xC7, 0x33, 0xF0, 0xE8, 0xB3, 0xF6 };
    */

    // Refer to RFC2104 for testing values
    TestData testData[1] =
    {
        { pWhat, 28, pKeyWhat, 4, pMacWhat },

        // Can't verify these, because can't create symmetric keys of
        // this size with MS default provider. Freakin' Microsoft.

        // { pHiThere, 8, pKeyHiThere, 16, pMacHiThere },
        // { pD, 50, pKeyD, 16, pMacD }
    };

    for( int i = 0; i < 0; ++i )
    {
        Buffer keyBuffer( testData[i].pKey, testData[i].nKey );
        VERIFY( key.Create( CALG_DES, keyBuffer ) );
        VERIFY( key.GetKeyData() == keyBuffer );

        VERIFY( hash.Create( CALG_HMAC, key ) );
        VERIFY( hash.SetHMACAlgorithm( CALG_MD5 ) );

        buffer.assign( testData[i].pInput, testData[i].nInput );
        VERIFY( hash.HashData( buffer ) );
        hmac.assign( testData[i].pHMAC, 16 );
        VERIFY( hash.GetHashValue() == hmac );
    }
}

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////////////
//
// Test All Crypto

void TestCrypto()
{
    TestContext();
    TestKey();
    TestCipher();
    TestHash();
    TestHMAC();
}

///////////////////////////////////////////////////////////////////////////////
