///////////////////////////////////////////////////////////////////////////////
//
//  Common.h
//
//  Standard header file for all TrueRand objects
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

#if !defined( TRUERAND_COMMON_H )
#define TRUERAND_COMMON_H

#pragma warning( disable: 4514 )    // ignored unreference inline function
#pragma warning( disable: 4710 )

#include <cassert>

#if defined(_XBOX)
    #include <xtl.h>
#elif defined(WIN32)
    #include <windows.h>
#else
    #define TEXT // Unicode macro
#endif

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <string>
#pragma warning( pop )

///////////////////////////////////////////////////////////////////////////////
//
// Defines

// avoid compiler warnings about unused variables or function returns
#define USED( expr ) static_cast<void>( expr );

// handle UNICODE properly with C++ strings
#if defined(UNICODE)
    typedef std::wstring stdstring;
#else
    typedef std::string stdstring;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Helper class used to wrap HANDLEs

template< class T, int tInvalid = NULL >
class HandleBase
{
public:

    explicit HandleBase( T h = T(tInvalid) )
    :
        mHandle( h )
    {
    }

    ~HandleBase()
    {
        Close();
    }

    HandleBase& operator = ( int iNull )
    {
        Close();
        assert( iNull == NULL );
        mHandle = NULL;
        return *this;
    }

    HandleBase& operator = ( T h )
    {
        Close();
        mHandle = h;
        return *this;
    }

    operator T() const
    {
        return mHandle;
    }

private:
    
    void Close()
    {
        if( mHandle != T(tInvalid) )
        {
            #if defined(WIN32)
                BOOL bSuccess = CloseHandle( mHandle ); // Win32 call
            #else
                BOOL bSuccess = FALSE; // Platform-specific close call
            #endif
            assert( bSuccess != 0 );
            USED( bSuccess );
            mHandle = NULL;
        }
    }

    // Disabled
    HandleBase( const HandleBase& );
    HandleBase& operator=( const HandleBase& );

private:

    T mHandle;
};

#if defined(WIN32)
    typedef HandleBase< HANDLE >                             Handle;
    typedef HandleBase< HANDLE, int(INVALID_HANDLE_VALUE) >  FileHandle;
#endif

#if defined(WIN32)
namespace Win32
{

///////////////////////////////////////////////////////////////////////////////
//
// Helper class used to wrap Win32 FindFirstFile HANDLE

class FindHandle
{
public:

    explicit FindHandle( HANDLE h = NULL )
    :
        mFind( h )
    {
    }

    ~FindHandle()
    {
        if( mFind != NULL )
        {
            BOOL bSuccess = FindClose( mFind ); // Win32 call
            assert( bSuccess != 0 );
            USED( bSuccess );
            mFind = NULL;
        }
    }

    operator HANDLE() const
    {
        return mFind;
    }

private:

    // Disabled
    FindHandle( const FindHandle& );
    FindHandle& operator=( const FindHandle& );

private:

    HANDLE mFind;
};

} // end namespace Win32

#endif // WIN32

///////////////////////////////////////////////////////////////////////////////
//
// Helper class used to wrap Win32 HeapAlloc/HeapFree HANDLE or
// malloc/free

class AutoHeapPtr
{
public:

    AutoHeapPtr()
    :
        #if defined(WIN32)
            mHeap( GetProcessHeap() ), // Win32 call
        #endif
        mpData( NULL )
    {
    }

    ~AutoHeapPtr()
    {
        Free();
    }

    unsigned char* Alloc( unsigned long nBytes )
    {
        Free();

        #if defined(WIN32)
            // Win32 call
            mpData = static_cast< BYTE* >( HeapAlloc( mHeap, 0, nBytes ) );
        #else
            mpData = static_cast< unsigned char* >( malloc( nBytes ) );
        #endif

        return mpData;
    }

    void Free()
    {
        if( mpData != NULL )
        {
            #if defined(WIN32)
                BOOL bSuccess = HeapFree( mHeap, 0, mpData ); // Win32 call
            #else
                free( mpData );
                bool bSuccess = true;
            #endif
            assert( bSuccess != 0 );
            USED( bSuccess );
            mpData = NULL;
        }
    }

private:

    // Disabled
    AutoHeapPtr( const AutoHeapPtr& );
    AutoHeapPtr& operator=( const AutoHeapPtr& );

private:

    #if defined(WIN32)
        HANDLE mHeap;
        BYTE* mpData;
    #else
        unsigned char* mpData;
    #endif
};

#endif // TRUERAND_BUFFER_H

///////////////////////////////////////////////////////////////////////////////
