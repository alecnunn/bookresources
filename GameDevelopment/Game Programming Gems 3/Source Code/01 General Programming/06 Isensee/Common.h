///////////////////////////////////////////////////////////////////////////////
//
//  Common.h
//
//  Standard header file for all Custom STL Allocator code
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

#if !defined( CUSTOM_STL_ALLOCATOR_COMMON_H )
#define CUSTOM_STL_ALLOCATOR_COMMON_H

// Disable common warnings that appear when compiling at W4
#pragma warning( disable: 4100 4018 4146 4284 4512 4663 4702 4710 4711 4786 )

///////////////////////////////////////////////////////////////////////////////
//
// Includes

#include <cassert>

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <string>
#pragma warning( pop )

///////////////////////////////////////////////////////////////////////////////
//
// Types

typedef __int64 longlong;
typedef unsigned __int64 ulonglong;

///////////////////////////////////////////////////////////////////////////////
//
// Defines

#define NOMINMAX // disable windows.h min and max #defines

// avoid compiler warnings about unused variables or function returns
#define USED( expr ) static_cast<void>( expr );

#ifdef _DEBUG

    #define VERIFY( expr ) assert( expr )

#else

    // ErrorMsg function platform dependent; see Main.cpp
    void ErrorMsg( const char*, const char*, int );
    #define VERIFY( expr ) (void)( (expr) || (ErrorMsg( #expr, __FILE__, __LINE__ ), 0 ) )

#endif

#endif // CUSTOM_STL_ALLOCATOR_COMMON_H

///////////////////////////////////////////////////////////////////////////////
