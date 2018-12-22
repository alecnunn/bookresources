///////////////////////////////////////////////////////////////////////////////
//
//  Main.cpp
//
//  Examples and testing code for SecureSock and associated objects
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
#include "windows.h"
#include "SADatabase.h"
#include "SecurityAssociation.h"
#include "SecureBuffer.h"
#include <vector>
#include <algorithm>

extern void TestCrypto();

///////////////////////////////////////////////////////////////////////////////
//
// VERIFY message

void ErrorMsg( const char* pMsg, const char* pFile, int nLine )
{
    char msg[2048];
    sprintf( msg, "%s\nFile:%s Line:%d", pMsg, pFile, nLine );
    MessageBox( NULL, msg, "Error", MB_OK );
    DebugBreak();
}

namespace // anonymous
{

using namespace SecureSock;

///////////////////////////////////////////////////////////////////////////////
//
// Test SecurityAssociation

void TestSecurityAssociation()
{
    SecurityAssociation sa;
    SecurityAssociation sa2( sa );
    sa = sa2;

    SecurityAssociation sa3( Key(CALG_DES), Key(CALG_RC2), CALG_SHA1 );
    VERIFY( sa3.GetAuthKey().GetAlgorithm() == CALG_DES );
    VERIFY( sa3.GetCipherKey().GetAlgorithm() == CALG_RC2 );
    VERIFY( sa3.GetAuthHashAlg() == CALG_SHA1 );

    Key keyAuth;
    VERIFY( keyAuth.Create( CALG_DES ) );
    sa.SetAuthKey( keyAuth );
    VERIFY( sa.GetAuthKey().GetAlgorithm() == CALG_DES );
    
    Key keyCipher;
    VERIFY( keyCipher.Create( CALG_DES ) );
    sa.SetCipherKey( keyCipher );
    VERIFY( sa.GetCipherKey().GetAlgorithm() == CALG_DES );

    sa.SetAuthHashAlg( CALG_SHA1 );
    VERIFY( sa.GetAuthHashAlg() == CALG_SHA1 );

    sa.SetIVSize( 6 );
    VERIFY( sa.GetIVSize() == 6 );

    sa.SetICVSize( 10 );
    VERIFY( sa.GetICVSize() == 10 );

    sa.SetMaxPadBlocks( 2 );
    VERIFY( sa.GetMaxPadBlocks() == 2 );

    VERIFY( sa.GetSequenceNum() == 1 );
    sa.IncrSequenceNum();
    VERIFY( sa.GetSequenceNum() == 2 );

    sa.UpdateReplayWindow( 100 );
    sa.UpdateReplayWindow( 60 );
    sa.UpdateReplayWindow( 80 );
    sa.UpdateReplayWindow( 140 );

    VERIFY( !sa.IsValidSequenceNum( 0 ) );
    VERIFY( !sa.IsValidSequenceNum( 1 ) );
    VERIFY( !sa.IsValidSequenceNum( 60 ) );
    VERIFY( !sa.IsValidSequenceNum( 80 ) );
    VERIFY( !sa.IsValidSequenceNum( 140 ) );
    VERIFY( sa.IsValidSequenceNum( 81 ) );
    VERIFY( sa.IsValidSequenceNum( 101 ) );
    VERIFY( sa.IsValidSequenceNum( 139 ) );
    VERIFY( sa.IsValidSequenceNum( 141 ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test SADatabase

void TestSADatabase()
{
    SADatabase sad;

    unsigned long i;
    for( i=0; i < 10; ++i )
    {
        SpiType nSPI = sad.GenNewSPI();
        VERIFY( nSPI != 0 );
        VERIFY( !sad.Find( nSPI ) );
    }

    SpiType nSPI = sad.GenNewSPI();
    SecurityAssociation sa;
    sa.SetAuthHashAlg( CALG_SHA1 );
    sad.Insert( nSPI, sa );
    VERIFY( sad.Find( nSPI ) );
    VERIFY( sad.GetSA( nSPI ).GetAuthHashAlg() == CALG_SHA1 );
    VERIFY( sad.GetSA( nSPI ).GetAuthHashAlg() == CALG_SHA1 );

    for( i=0; i < 10; ++i )
    {
        SpiType nSPI = sad.GenNewSPI();
        VERIFY( nSPI != 0 );
        VERIFY( !sad.Find( nSPI ) );
    }

    SpiType nSPI2 = sad.GenNewSPI();
    VERIFY( !sad.Find( nSPI2 ) );
    SecurityAssociation sa2;
    sad.Insert( nSPI2, sa2 );
    VERIFY( sad.Find( nSPI ) );
    VERIFY( sad.Find( nSPI2 ) );
    VERIFY( !sad.GetSA( nSPI2 ).GetAuthKey().IsValid() );
    VERIFY( !sad.GetSA( nSPI2 ).GetAuthKey().IsValid() );
    VERIFY( sad.GetSA( nSPI ).GetAuthHashAlg() == CALG_SHA1 );
    VERIFY( sad.GetSA( nSPI ).GetAuthHashAlg() == CALG_SHA1 );
    sad.Delete( nSPI2 );
    VERIFY( !sad.Find( nSPI2 ) );
    VERIFY( !sad.Find( nSPI2 ) );
    VERIFY( sad.Find( nSPI ) );
    VERIFY( sad.Find( nSPI ) );
    VERIFY( sad.GetSA( nSPI ).GetAuthHashAlg() == CALG_SHA1 );
    VERIFY( sad.GetSA( nSPI ).GetAuthHashAlg() == CALG_SHA1 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Gem Example code

void TestExampleCode()
{
    SADatabase sad;

    Key keyAuth( CALG_DES );
    Key keyCipher( CALG_DES );

    SecurityAssociation sa( keyAuth, keyCipher, CALG_MD5 );

    SpiType nSPI = sad.GenNewSPI();
    sad.Insert( nSPI, sa );

    SecureBuffer::SetSADatabase( &sad );
    SecureBuffer sb( nSPI );
    Buffer buf;
    VERIFY( sb.Create( "payload", 7 ) );

    // Send the secure packet (fake call)
    SOCKET sock = INVALID_SOCKET;
    send( sock, sb.GetDataPtr(), sb.GetSize(), 0 );

    // Receive the secure packet (fake call)
    char* pData = new char [ 1024 ];
    recv( sock, pData, 1024, 0 );
    delete [] pData;
    pData = const_cast< char* >( sb.GetDataPtr() );
    int n = sb.GetSize();

    // Validate the packet
    SecureBuffer sbReceive( pData, n );
    VERIFY( sbReceive.IsAuthentic() );

    // Extract the original payload
    VERIFY( sbReceive.GetPayload( pData, &n ) );
    Buffer payload( reinterpret_cast< unsigned char *>( pData ), n );
    Buffer verify;
    Assign( verify, "payload", 7 );
    VERIFY( payload == verify );

    // Adjust the replay window
    sbReceive.SetAccepted();
}

///////////////////////////////////////////////////////////////////////////////
//
// Minimal test pass for SecureBuffer

void TestSecureBufferMin()
{
    struct KeyInfo
    {
        AlgType nCipher;
        AlgType nAuth;
    };

    KeyInfo keyInfo[4] =
    {
        { CALG_DES, CALG_MD5  },
        { CALG_RC2, CALG_MD5  },
        { CALG_DES, CALG_SHA1 },
        { CALG_RC2, CALG_SHA1 },
    };

    SADatabase sad;
    SecureBuffer::SetSADatabase( &sad );

    // Test various algorithms
    for( int i = 0; i < 4; ++ i )
    {
        Key keyAuth;
        VERIFY( keyAuth.Create( keyInfo[i].nCipher ) );
        Key keyCipher;
        VERIFY( keyCipher.Create( keyInfo[i].nCipher ) );

        // Test all size parameters
        for( unsigned char ivSize = 2; ivSize <= 8; ++ivSize )
        {
            for( unsigned char icvSize = 8; icvSize <= 12; ++icvSize )
            {
                for( unsigned char pad = 0; pad <= 4; ++pad )
                {
                    SecurityAssociation sa;
                    sa.SetAuthKey( keyAuth );
                    sa.SetCipherKey( keyCipher );
                    sa.SetAuthHashAlg( keyInfo[i].nAuth );
                    sa.SetIVSize( ivSize );
                    sa.SetICVSize( icvSize );
                    sa.SetMaxPadBlocks( pad );

                    SpiType nSPI = sad.GenNewSPI();
                    sad.Insert( nSPI, sa );
                    SecureBuffer sb( nSPI );

                    // Test payloads of random sizes and random data
                    const unsigned long nMaxPayload = 32;
                    srand( GetTickCount() );
                    for( unsigned long j=0; j < nMaxPayload * 4; ++j )
                    {
                        Buffer buf;
                        unsigned long nBytes = rand() % nMaxPayload;
                        VERIFY( Key::GenRandom( nBytes+1, buf ) );
    
                        VERIFY( sb.Create( buf ) );
                        VERIFY( sb.IsAuthentic() );
                        Buffer payload;
                        VERIFY( sb.GetPayload( payload ) );
                        VERIFY( payload == buf );

                        SecureBuffer sb2( sb );
                        VERIFY( sb2.IsAuthentic() );
                        VERIFY( sb2.GetPayload( payload ) );
                        VERIFY( payload == buf );

                        sb.SetAccepted();
                    }
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Receive lots of packets in strange order

void TestSecureBufferReplayWindow()
{
    Key keyAuth;
    VERIFY( keyAuth.Create( CALG_DES ) );
    Key keyCipher;
    VERIFY( keyCipher.Create( CALG_DES ) );

    SecurityAssociation sa;
    sa.SetAuthKey( keyAuth );
    sa.SetCipherKey( keyCipher );
    sa.SetAuthHashAlg( CALG_MD5 );

    SADatabase sad;
    sad.Insert( 1, sa );
    SecureBuffer::SetSADatabase( &sad );

    // Create lots of small secure buffers and put them in a list
    const unsigned long MAX = 500;
    Buffer buf;
    Assign( buf, "1", 1 );
    std::vector< std::pair< unsigned long, SecureBuffer* > > sbList;
    unsigned long i;
    for( i=0; i < MAX; ++i )
    {
        SecureBuffer* pBuf = new SecureBuffer( 1 );
        VERIFY( pBuf->Create( buf ) );
        std::pair< unsigned long, SecureBuffer* > pr( i+1, pBuf );
        sbList.push_back( pr );
    }

    // Shuffle the list into a random order
    srand( GetTickCount() );
    std::random_shuffle( sbList.begin(), sbList.end() );

    // Now validate in an unusual order
    unsigned long nLast = 0;
    for( i=0; i < MAX; ++i )
    {
        std::pair< unsigned long, SecureBuffer* > pr = sbList[i];
        SecureBuffer* pBuf = pr.second;
        bool bIsAuthentic = pBuf->IsAuthentic();
        if( bIsAuthentic )
        {
            VERIFY( pr.first != nLast );
            if( pr.first < nLast )
                VERIFY( ( nLast - pr.first ) < 64 );
            Buffer payload;
            VERIFY( pBuf->GetPayload( payload ) );
            VERIFY( payload == buf );
            pBuf->SetAccepted();
            nLast = pr.first;
        }
    }

    // Cleanup
    for( i=0; i < MAX; ++i )
    {
        std::pair< unsigned long, SecureBuffer* > pr = sbList[i];
        delete pr.second;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Try to alter the message

void TestSecureBufferAuthentication()
{
    Key keyAuth;
    VERIFY( keyAuth.Create( CALG_DES ) );
    Key keyCipher;
    VERIFY( keyCipher.Create( CALG_DES ) );

    SecurityAssociation sa;
    sa.SetAuthKey( keyAuth );
    sa.SetCipherKey( keyCipher );
    sa.SetAuthHashAlg( CALG_MD5 );

    SADatabase sad;
    sad.Insert( 1, sa );
    SecureBuffer::SetSADatabase( &sad );
    SecureBuffer sb( 1 );

    unsigned long i;
    for( i=1; i < 9; ++i )
    {
        // Generate a random payload
        Buffer buf;
        VERIFY( Key::GenRandom( i, buf ) );
        VERIFY( sb.Create( buf ) );
        
        // Munge every single bit of the data
        unsigned char* pData = const_cast< unsigned char* >
                             ( reinterpret_cast< const unsigned char* >
                             ( sb.GetDataPtr() ) );
        for( unsigned long j=0; j < sb.GetSize(); ++j, ++pData )
        {
            unsigned char b = *pData;
            for( unsigned long k=0; k < CHAR_BIT; ++k )
            {
                unsigned char bit = unsigned char( 1 << k );
                unsigned char data = unsigned char( b ^ bit );
                *pData = data;
                VERIFY( !sb.IsAuthentic() );
            }
            *pData = b;
        }
    }

    // Generate buffers too small to contain SPIs
    for( i=0; i < sizeof(SpiType); ++i )
    {
        Buffer buf;
        if( i > 0 )
            VERIFY( Key::GenRandom( i, buf ) );
        sb.Set( buf );
        VERIFY( !sb.IsAuthentic() );
    }

    // Generate buffers with valid SPIs and invalid lengths
    for( i=0; i < 40; ++i )
    {
        Buffer buf;
        VERIFY( Key::GenRandom( i+sizeof(SpiType), buf ) );
        unsigned char* pData = GetData( buf );

        // SPI = 1
        *pData = 1;
        ++pData;
        *pData = 0;

        sb.Set( buf );
        VERIFY( !sb.IsAuthentic() );
    }

    // Generate random bogus buffers with valid SPIs
    srand( GetTickCount() );
    for( i=0; i < 100; ++i )
    {
        Buffer buf;
        unsigned long nBytes = rand() % 128;
        nBytes += sizeof( SpiType );
        VERIFY( Key::GenRandom( nBytes, buf ) );
        unsigned char* pData = GetData( buf );

        // SPI = 1
        *pData = 1;
        ++pData;
        *pData = 0;

        sb.Set( buf );
        VERIFY( !sb.IsAuthentic() );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Test SecureBuffer and friends

void TestSecureBuffer()
{
    TestSecureBufferMin();
    TestSecureBufferReplayWindow();
    TestSecureBufferAuthentication();
}

///////////////////////////////////////////////////////////////////////////////
//
// Performance test

void PerfTest()
{
    SADatabase sad;

    Key keyAuth;
    keyAuth.Create( CALG_DES );
    Key keyCipher;
    keyCipher.Create( CALG_DES );

    SecurityAssociation sa;
    sa.SetAuthKey( keyAuth );
    sa.SetCipherKey( keyCipher );
    sa.SetAuthHashAlg( CALG_MD5 );

    SpiType nSPI = sad.GenNewSPI();
    sad.Insert( nSPI, sa );
    SecureBuffer::SetSADatabase( &sad );
    SecureBuffer sb( nSPI );

    longlong j;
    longlong i;
    longlong nStart=0;
    longlong nEnd=0;
    for( j=32; j < 4096; j *= 2 )
    {
        longlong nCreate=0;
        longlong nAuth=0;
        longlong nDecrypt=0;
        for( i=0; i < 10000; ++i )
        {
            Buffer buf;
            Key::GenRandom( (unsigned long)j, buf );

            // Create
            QueryPerformanceCounter( (LARGE_INTEGER*)(&nStart) );
            sb.Create( buf );
            QueryPerformanceCounter( (LARGE_INTEGER*)(&nEnd) );
            nCreate += nEnd - nStart;

            // IsAuthentic
            QueryPerformanceCounter( (LARGE_INTEGER*)(&nStart) );
            sb.IsAuthentic();
            QueryPerformanceCounter( (LARGE_INTEGER*)(&nEnd) );
            nAuth += nEnd - nStart;
            // End timing

            // Start timing
            QueryPerformanceCounter( (LARGE_INTEGER*)(&nStart) );
            sb.GetPayload( buf );
            QueryPerformanceCounter( (LARGE_INTEGER*)(&nEnd) );
            nDecrypt += nEnd - nStart;
            // End timing

            sb.SetAccepted();
        }
        longlong nFreq;
        QueryPerformanceFrequency( (LARGE_INTEGER*)(&nFreq) );

        double fFreq       = double( nFreq );
        double fCreateSec  = double( nCreate  ) / fFreq;
        double fAuthSec    = double( nAuth    ) / fFreq;
        double fDecryptSec = double( nDecrypt ) / fFreq;

        double fTotalK     = (double)i * (double)j / 1024.0;

        double fCreateSecondsPerK  = fCreateSec  / fTotalK;
        double fAuthSecondsPerK    = fAuthSec    / fTotalK;
        double fDecryptSecondsPerK = fDecryptSec / fTotalK;
        
        double fCreateMillisecondsPerK  = fCreateSecondsPerK  * 1000.0;
        double fAuthMillisecondsPerK    = fAuthSecondsPerK    * 1000.0;
        double fDecryptMillisecondsPerK = fDecryptSecondsPerK * 1000.0;
        
        double fCreateMicrosecondsPerK  = fCreateMillisecondsPerK  * 1000.0;
        double fAuthMicrosecondsPerK    = fAuthMillisecondsPerK    * 1000.0;
        double fDecryptMicrosecondsPerK = fDecryptMillisecondsPerK * 1000.0;
        
        printf( "Buf size: %I64d\n"
                "Runs: %I64d\n"
                "Create : %f S   %f mS/K   %f uS/K\n"
                "Auth   : %f S   %f mS/K   %f uS/K\n"
                "Decrypt: %f S   %f mS/K   %f uS/K\n\n",
                j, i,
                fCreateSecondsPerK, fCreateMillisecondsPerK, fCreateMicrosecondsPerK, 
                fAuthSecondsPerK, fAuthMillisecondsPerK, fAuthMicrosecondsPerK,
                fDecryptSecondsPerK, fDecryptMillisecondsPerK, fDecryptMicrosecondsPerK );
    }
}

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////////////
//
// Main

int __cdecl main()
{
    TestCrypto();

    CryptContext context;
    VERIFY( context.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
    Key::SetContext( &context );
    Hash::SetContext( &context );

    PerfTest();
    TestSecurityAssociation();
    TestSADatabase();
    TestSecureBuffer();
    TestExampleCode();

	return 0;
}
