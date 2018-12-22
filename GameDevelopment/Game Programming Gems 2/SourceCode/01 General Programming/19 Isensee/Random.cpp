///////////////////////////////////////////////////////////////////////////////
//
//  Random.cpp
//
//  Examples and testing code for GenRand and associated objects
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

#pragma warning( disable: 4127 4514 4702 4710 4786 )
#include "GenRand.h"
#include <string>
#include <iostream>

#if !defined(WIN32)
    #define INVALID_HANDLE_VALUE NULL
    #define GENERIC_WRITE 0
    #include <ctime>
    #include <cstdio>
#endif

namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
//
// Wrap file access

class File
{
public:

    File() : mFile( INVALID_HANDLE_VALUE ) {}

    ~File()
    {
        #if !defined(WIN32)
            if( mFile != INVALID_HANDLE_VALUE )
                fclose( mFile );
        #endif
    }

    bool Create( const stdstring& sFile, unsigned long dwAccess )
    {
        #if defined(WIN32)
            mFile = CreateFile( sFile.c_str(), dwAccess, 0, NULL, 
                                CREATE_ALWAYS, 0, NULL );
        #else
            USED( dwAccess );
            mFile = fopen( sFile.c_str(), "wb" );
        #endif

        return( mFile != INVALID_HANDLE_VALUE );
    }

    bool Write( const void* pBuffer, unsigned long dwBytes, 
                unsigned long& dwWritten ) const
    {
        assert( mFile != INVALID_HANDLE_VALUE );

        #if defined(WIN32)
            BOOL bSuccess = WriteFile( mFile, pBuffer, dwBytes, &dwWritten, NULL );
        #else
            dwWritten = fwrite( pBuffer, 1, dwBytes, mFile );
            bool bSuccess = ( dwBytes == dwWritten );
        #endif

        assert( bSuccess != 0 );
        return( bSuccess != 0 );
    }

    bool Read( void* pBuffer, unsigned long dwBytes, unsigned long& dwRead ) const
    {
        assert( mFile != INVALID_HANDLE_VALUE );

        #if defined(WIN32)
            BOOL bSuccess = ReadFile( mFile, pBuffer, dwBytes, &dwRead, NULL );
        #else
            dwRead = fread( pBuffer, dwBytes, 1, mFile );
            bool bSuccess = ( dwRead == dwBytes );
        #endif

        assert( bSuccess != 0 );
        return( bSuccess != 0 );
    }

    #if defined(WIN32)
    HANDLE GetHandle() const
    {
        return mFile;
    }
    #endif

    bool SetPos( unsigned long dwPos ) const
    {
        assert( mFile != INVALID_HANDLE_VALUE );

        #if defined(WIN32)
            DWORD dwNewPos = SetFilePointer( mFile, dwPos, NULL, FILE_BEGIN );
        #else
            int nSuccess = fseek( mFile, dwPos, SEEK_SET );
            unsigned long dwNewPos = nSuccess ? dwPos : 0;
        #endif
        assert( dwNewPos == dwPos );
        return( dwNewPos == dwPos );
    }

    unsigned long GetSize() const
    {
        assert( mFile != INVALID_HANDLE_VALUE );

        #if defined(WIN32)
            DWORD dwFileSize = GetFileSize( mFile, NULL );
        #else
            unsigned long dwFileSize = unsigned long( ftell( mFile ) );
        #endif

        assert( dwFileSize != static_cast<unsigned long>(-1) );
        return dwFileSize;
    }

private:

    // disabled
    File( const File& );
    File& operator=( const File& );

private:

    #if defined(WIN32)
        FileHandle mFile;
    #else
        FILE* mFile;
    #endif
    
};

///////////////////////////////////////////////////////////////////////////////
//
// Create a file of pseudo random bytes

bool CreatePseudoRandFile( const stdstring& sFile, int nBytes )
{
    std::cout << "generating pseudo random bytes with std::rand()" << std::endl;

    File fileRand;
    if( !fileRand.Create( sFile, GENERIC_WRITE ) )
        return false;

#if defined(WIN32)
    srand( GetTickCount() ); // Win32 call
#else
    srand( time(NULL) );
#endif
    for( ;; )
    {
        // RAND_MAX typically has less precision than a full unsigned int, so
        // we only write out two bytes of the result
        int nRand = rand();

        unsigned long nBytesWritten = 0;
        if( !fileRand.Write( &nRand, 2, nBytesWritten ) )
            return false;

        nBytes -= nBytesWritten;
        if( nBytes <= 0 )
            return true;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Create a file of truly random bytes

bool CreateTrueRandFile( const stdstring& sFile, int nBytes, bool bUseBuffer )
{
    std::cout << "generating truly random bytes with ";
    std::cout << ( bUseBuffer ? "GetRand()" : "GetRandInt()" ) << std::endl;

    // Win32 call
    File fileRand;
    if( !fileRand.Create( sFile, GENERIC_WRITE ) )
        return false;

    TrueRand::GenRand randGen;
    for( ;; )
    {
        unsigned long nBytesWritten = 0;
        bool bSuccess = false;
        if( bUseBuffer )
        {
            TrueRand::Buffer bufRand = randGen.GetRand();
            bSuccess = fileRand.Write( bufRand.c_str(), bufRand.length(),
                                       nBytesWritten );
        }
        else
        {
            unsigned int nRand = randGen.GetRandInt();
            bSuccess = fileRand.Write( &nRand, sizeof(nRand), nBytesWritten );
        }

        if( !bSuccess )
            return false;

        nBytes -= nBytesWritten;
        if( nBytes <= 0 )
            return true;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Perform standard randomness tests on the given file

std::string TestRandFile( const stdstring& sFile )
{
#if defined(WIN32) && !defined(_XBOX)

    // Create a temporary output file
    File fileOut;
    if( !fileOut.Create( TEXT( "ent.out" ), GENERIC_WRITE | GENERIC_READ ) )
        return std::string();

    // Prepare to call CreateProcess()
    STARTUPINFO si;
    memset( &si, 0, sizeof(si) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;

    // Redirect output to our file and redirect error messages to stderr
    si.hStdOutput = fileOut.GetHandle();      
    si.hStdError = GetStdHandle( STD_ERROR_HANDLE ); // Win32 call

    PROCESS_INFORMATION pi;
    memset( &pi, 0, sizeof(pi) );

    // Invoke cmd "ent <sFile>"
    stdstring sCmdLine( TEXT( "ent " ) );
    sCmdLine += sFile;

    // Win32 call
    BOOL bSuccess = CreateProcess( NULL, const_cast< LPTSTR >( sCmdLine.c_str() ),
                                   NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi );

    // Wrap handles so they're properly closed
    Handle hThread( pi.hThread );
    Handle hProcess( pi.hProcess );

    assert( bSuccess != 0 );
    if( bSuccess == 0 )
        return std::string();

    // Wait for the test program to complete
    WaitForSingleObject( hProcess, INFINITE ); // Win32 call

    // Put the output in the string
    if( !fileOut.SetPos( 0 ) )
        return std::string();

    DWORD nFileSize = fileOut.GetSize();
    std::string sResult;        
    sResult.resize( nFileSize );

    DWORD nBytesRead;
    if( !fileOut.Read( const_cast< LPSTR >( sResult.c_str() ), sResult.size(),
                       nBytesRead ) )
        return std::string();

    return sResult;

#else
    
    // Platform-specific code for spawning "ent <sFile>" and returning
    // the result
    USED( sFile );
    return std::string();

#endif
}

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////////////
//
// Main

int __cdecl main()
{
    //-------------------------------------------------------------------------
    //
    // Create a truly random seed for a pseudo-random number generator

    {
        unsigned int nSeed = TrueRand::GenRand().GetRandInt();
        srand( nSeed );
        int nPseudoRand = rand();
        std::cout << "seed: " << nSeed << " pseudo rand: " << nPseudoRand << std::endl;
    }

    //-------------------------------------------------------------------------
    //
    // Generate a session key for secure communication with a server

    {
        TrueRand::GenRand randGen;
        TrueRand::Buffer bufSessionKey = randGen.GetRand();

        std::cout << "session key: ";
        for( TrueRand::Buffer::size_type i = 0; i < bufSessionKey.length(); ++i )
            printf( "%x", bufSessionKey[ i ] ); // MS support of ios::hex lame
        std::cout << std::endl << std::endl;

        // Encrypt session key with public key and send result to server
        //
        // ...

        // Encrypt client data using session key as symmetric key and send to server
        //
        // ...
    }
    
    // Increase this value for more accurate (though slower) testing
    const int nBytes = 1000;
    stdstring sFile;
    std::string sResults;

    //-------------------------------------------------------------------------
    //
    // Evaluate randomness of std::rand()

    sFile = TEXT( "rand.txt" );
    CreatePseudoRandFile( sFile, nBytes );

    // Test the file of random values using standard randomness tests
    sResults = TestRandFile( sFile );

    // Show results
    std::cout << sResults << std::endl;

    //-------------------------------------------------------------------------
    //
    // Validate the randomness of TrueRand::GenRand().GetRand()

    sFile = TEXT( "randbuf.txt" );
    CreateTrueRandFile( sFile, nBytes, true );

    // Test the file of random values using standard randomness tests
    sResults = TestRandFile( sFile );

    // Show results
    std::cout << sResults << std::endl;

    //-------------------------------------------------------------------------
    //
    // Validate the randomness of TrueRand::GenRand().GetRandInt()

    sFile = TEXT( "randint.txt" );
    CreateTrueRandFile( sFile, nBytes, false );

    // Test the file of random values using standard randomness tests
    sResults = TestRandFile( sFile );

    // Show results
    std::cout << sResults << std::endl;

	return 0;
}
