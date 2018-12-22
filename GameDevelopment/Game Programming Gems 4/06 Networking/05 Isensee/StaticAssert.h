///////////////////////////////////////////////////////////////////////////////
//
//  StaticAssert.h
//
//  Compile-time assertion function.
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
//  How does this work? Two things:
//
//  1) Template specialization. The templatized StaticAssertFailed
//  struct has a valid definition for the good/true case, but no specialization for
//  the false case. If the false case is triggered, the StaticAssertFailed type is
//  considered incomplete.
//
//  2) The sizeof an incomplete type will generate a compiler error.
//
//  The key is giving an informative error. The code
//  below generates the assertion message:
//
//      "use of undefined type 'StaticAssertFailed<__formal>'
//
//  in Visual Studio 7.1. The VS error message allows the user to trace back to
//  the original source line of the error.
//
//  The typedef provides a unique name, so you can have multiple assertions with
//  the same parameters in the same compilation unit.
//
//  Example usage:
//
//      static_assert( sizeof(int) > sizeof(T) );
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined( BITPACK_STATIC_ASSERT_H )
#define BITPACK_STATIC_ASSERT_H

namespace BitPack
{

// An incomplete type
template< bool >
struct StaticAssertFailed;

// Specialize for the good (true) case so that the type is complete
template<>
struct StaticAssertFailed< true >
{
};

// The template parameter is the sizeof the StaticAssertFailed type
template< int >
struct StaticAssertCheck
{
};

// String concatenation macro to generate unique names
#define MacroStrCat( a, b )   MacroStrCatEx( a, b )
#define MacroStrCatEx( a, b ) a##b

// Match C-style naming of assert() macro
#define static_assert( expr ) \
    typedef StaticAssertCheck< sizeof( StaticAssertFailed< (bool)( expr ) > ) > \
            MacroStrCat( StaticAssert, __LINE__ )

} // end namespace BitPack

#endif // BITPACK_STATIC_ASSERT_H
