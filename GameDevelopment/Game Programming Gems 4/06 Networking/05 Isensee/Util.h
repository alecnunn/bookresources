///////////////////////////////////////////////////////////////////////////////
//
//  Util.h
//
//  Utility functions.
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
///////////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined( BITPACK_UTIL_H )
#define BITPACK_UTIL_H

#include <algorithm>

namespace BitPack
{

//-----------------------------------------------------------------------------
//
// Macros

#define test( expr ) (void)( ( expr ) || ( ErrorMsg( #expr, __FILE__, __LINE__ ), 0 ) )

#if defined( _DEBUG )
    #define verify( expr ) assert( expr )
#else
    #define verify( expr ) expr
#endif

//-----------------------------------------------------------------------------
//
// Utility functions

inline bool IsPlatformLittleEndian()
{
    // Decent compilers will optimize this function out of existence so
    // there's no runtime overhead in release builds.
    const int n = 1;
    return( *((char*)( &n ) ) ? true : false );
}

template< typename T >
T ReverseBytes( T n )
{
    // Take advantage of STL reverse algorithm to move bytes around
    unsigned char* p = (unsigned char*)( &n );
    std::reverse( p, p + sizeof( T ) );
    return n;
}

template< typename T >
T HostToNetworkOrder( T n )
{
    // Network order is big endian; host order is platform specific
    if( IsPlatformLittleEndian() )
        return ReverseBytes( n );
    return n;        
}

template< typename T >
T NetworkToHostOrder( T n )
{
    // Network order is big endian; host order is platform specific
    if( IsPlatformLittleEndian() )
        return ReverseBytes( n );
    return n;        
}

template< typename T, typename U, typename V >
T Confine( const T& t, const U& Min, const V& Max )
{
    assert( Min <= Max );
    if( t < T( Min ) )
        return T( Min );
    if( t > T( Max ) )
        return T( Max );
    return t;
}

//-----------------------------------------------------------------------------
//
// Calculate number of bits required to store the given value.
// Calculation is done at compile time.
// Usage: int i = BitsRequired< 1234 >::GetValue;

template< int64 Value >
struct BitsRequired
{
    // Recursive definition
    enum { GetValue = BitsRequired< Value / 2 >::GetValue + 1 };
};

// Specialization for base case
template <>
struct BitsRequired< 0 >
{
    enum { GetValue = 0 };
};

//-----------------------------------------------------------------------------
//
// Determine if the given type is an unsigned integer.
// Usage: IsUnsignedInt< unsigned short >::GetResult

template< typename T >
struct IsUnsignedInt
{
    enum { GetResult = false };
};

// Specializations for unsigned types
template <> struct IsUnsignedInt< unsigned char >  { enum { GetResult = true }; };
template <> struct IsUnsignedInt< unsigned short > { enum { GetResult = true }; };
template <> struct IsUnsignedInt< unsigned long >  { enum { GetResult = true }; };
template <> struct IsUnsignedInt< unsigned int >   { enum { GetResult = true }; };
template <> struct IsUnsignedInt< uint64 >         { enum { GetResult = true }; };

//-----------------------------------------------------------------------------
//
// Determine if the given type is a signed integer.
// Usage: IsSignedInt< short >::GetResult

template< typename T >
struct IsSignedInt
{
    enum { GetResult = false };
};

// Specializations for signed types
template <> struct IsSignedInt< char >  { enum { GetResult = true }; };
template <> struct IsSignedInt< short > { enum { GetResult = true }; };
template <> struct IsSignedInt< long >  { enum { GetResult = true }; };
template <> struct IsSignedInt< int >   { enum { GetResult = true }; };
template <> struct IsSignedInt< int64 > { enum { GetResult = true }; };

} // end namespace BitPack

#endif // BITPACK_UTIL_H