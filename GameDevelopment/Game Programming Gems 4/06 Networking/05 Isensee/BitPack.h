///////////////////////////////////////////////////////////////////////////////
//
//  BitPack.h
//
//  Bit packing technique for minimizing network traffic.
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
#if !defined( BITPACK_BITPACK_H )
#define BITPACK_BITPACK_H

#include <cassert>
#include <limits>
#include "Platform.h"
#include "StaticAssert.h"
#include "Util.h"

namespace BitPack
{

//-----------------------------------------------------------------------------
//
// Pack
//
// This is the workhorse packing function. It packs nSrcBits of any unsigned
// integer T into the destination bit buffer starting at the given bit
// position. The function assumes that the destination bit buffer has been
// zeroed out before packing is initiated.

template< typename T >
void Pack( const T& tSrc, int nSrcBits, char* pDestBitBuffer, int nDestStartBit )
{
    static_assert( IsUnsignedInt< T >::GetResult );
    assert( pDestBitBuffer != NULL );
    assert( nSrcBits > 0 );
    assert( nSrcBits <= sizeof( T ) * CHAR_BIT );

    // All packing is done left-to-right
    
    // Determine starting position in bit buffer
    int nDestCurrBit = nDestStartBit;
    const int nDestCurrByte = nDestCurrBit / CHAR_BIT;
    int nDestNextByteBoundary = (nDestCurrByte+1) * CHAR_BIT;
    
    // Determine bit offset between T and bit buffer
    const int nTBits = sizeof( T ) * CHAR_BIT;
    const int nOffsetBits = nDestStartBit + nSrcBits - nTBits;
    int nOffset = nOffsetBits;
    if( nOffset < 0 )
        nOffset = ( nOffset % CHAR_BIT ) + CHAR_BIT;

    // Bit shifting is based on the offset
    const int nRightShiftBits = nOffset % CHAR_BIT;
    const int nLeftShiftBits = CHAR_BIT - nRightShiftBits;
    
    // Determine starting position in T
    int nSrcCurrBit = nTBits - nSrcBits;
    const int nSrcCurrByte = nSrcCurrBit / CHAR_BIT;
    int nSrcNextByteBoundary = (nSrcCurrByte+1) * CHAR_BIT;

    assert( nDestCurrBit >= 0 );
    assert( nSrcCurrBit >= 0 );
    
    // Set pointers to starting position in bit buffer
    unsigned char* pDestByte = (unsigned char*)( pDestBitBuffer );
    pDestByte += nDestCurrByte;
    
    // Convert T to network order
    T nSrc = HostToNetworkOrder( tSrc );
    
    // Set pointers to starting position in T
    const unsigned char* pSrcByte = (unsigned char*)( &nSrc );
    pSrcByte += nSrcCurrByte;
    
    while( nDestCurrBit < nDestStartBit + nSrcBits )
    {
        // Determine where the next byte crossover point is located.
        // Only one of these values is ever true at a time.
        bool bMatchingBoundary = nDestNextByteBoundary - nOffsetBits ==
                                 nSrcNextByteBoundary;
        bool bIsSrcNextBoundary = nDestNextByteBoundary - nOffsetBits >
                                  nSrcNextByteBoundary;
        bool bIsDestNextBoundary = nDestNextByteBoundary - nOffsetBits <
                                   nSrcNextByteBoundary;
        
        // nNextByteBoundary is src-based
        int nNextByteBoundary = bIsDestNextBoundary ?
                                nDestNextByteBoundary - nOffsetBits :
                                nSrcNextByteBoundary;

        // Generate source mask
        int nMaskBits = nSrcNextByteBoundary - nSrcCurrBit;
        assert( nMaskBits > 0 );
        assert( nMaskBits <= CHAR_BIT );
        unsigned char cMask = 0xFF;
        cMask >>= ( CHAR_BIT - nMaskBits );
        unsigned char cSrcBits = *pSrcByte & cMask;
        
        // Shift source appropriately depending on next boundary.
        // No shift if matching byte boundarys.
        if( bIsDestNextBoundary )
            cSrcBits >>= nRightShiftBits;
        if( bIsSrcNextBoundary )
            cSrcBits <<= nLeftShiftBits;

        // Update the destination bit buffer
        *pDestByte |= cSrcBits;
            
        // Update bit ptrs
        int nBitsUpdated = nNextByteBoundary - nSrcCurrBit;
        assert( nBitsUpdated > 0 );
        assert( nBitsUpdated <= CHAR_BIT );
        nSrcCurrBit += nBitsUpdated;
        nDestCurrBit += nBitsUpdated;

        if( bIsDestNextBoundary || bMatchingBoundary )
        {
            // Move into the next dest byte
            ++pDestByte;
            assert( nDestCurrBit % CHAR_BIT == 0 );
            nDestNextByteBoundary += CHAR_BIT;
        }
        if( bIsSrcNextBoundary || bMatchingBoundary )
        {
            // Move into the next src byte
            ++pSrcByte;
            assert( nSrcCurrBit % CHAR_BIT == 0 );
            nSrcNextByteBoundary += CHAR_BIT;
        }                
    }
}

//-----------------------------------------------------------------------------
//
// Unpack
//
// This is the workhorse unpacking function. It unpacks nDestBits of the
// source bit buffer starting at the source start bit into any unsigned
// integer T.

template< typename T >
void Unpack( T* ptDest, int nDestBits, const char* pSrcBitBuffer, int nSrcStartBit )
{
    static_assert( IsUnsignedInt< T >::GetResult );
    assert( ptDest != NULL );
    assert( nDestBits > 0 );
    assert( nDestBits <= sizeof( T ) * CHAR_BIT );
    assert( pSrcBitBuffer != NULL );

    // Determine starting position in bit buffer    
    int nSrcCurrBit = nSrcStartBit;
    const int nSrcCurrByte = nSrcCurrBit / CHAR_BIT;
    int nSrcNextByteBoundary = (nSrcCurrByte+1) * CHAR_BIT;
    
    // Determine bit offset between T and bit buffer
    const int nTBits = sizeof( T ) * CHAR_BIT;
    const int nOffsetBits = nSrcStartBit + nDestBits - nTBits;
    int nOffset = nOffsetBits;
    if( nOffset < 0 )
        nOffset = ( nOffset % CHAR_BIT ) + CHAR_BIT;
    
    // Bit shifting is based on the offset
    const int nLeftShiftBits = nOffset % CHAR_BIT;
    const int nRightShiftBits = CHAR_BIT - nLeftShiftBits;

    // Determine starting position in T    
    int nDestCurrBit = nTBits - nDestBits;
    const int nDestCurrByte = nDestCurrBit / CHAR_BIT;
    int nDestNextByteBoundary = (nDestCurrByte+1) * CHAR_BIT;

    assert( nSrcCurrBit >= 0 );
    assert( nDestCurrBit >= 0 );
    
    // Set pointers to starting position in bit buffer
    const unsigned char* pSrcByte = (unsigned char*)( pSrcBitBuffer );
    pSrcByte += nSrcCurrByte;

    // Set pointers to starting position in empty temporary T
    T nDest( T(0) );
    unsigned char* pDestByte = (unsigned char*)( &nDest );
    pDestByte += nDestCurrByte;
    
    while( nSrcCurrBit < nSrcStartBit + nDestBits )
    {
        // Determine where the next byte crossover point is located
        // Only one of these values is ever true at a time.
        bool bMatchingBoundary = nSrcNextByteBoundary ==
                                 nDestNextByteBoundary + nOffsetBits;
        bool bIsDestNextBoundary = nSrcNextByteBoundary >
                                   nDestNextByteBoundary + nOffsetBits;
        bool bIsSrcNextBoundary = nSrcNextByteBoundary <
                                  nDestNextByteBoundary + nOffsetBits;
        
        // nNextByteBoundary is src-based
        int nNextByteBoundary = bIsSrcNextBoundary ?
                                nSrcNextByteBoundary :
                                nDestNextByteBoundary + nOffsetBits;

        // Generate source mask
        int nMaskBits = nSrcNextByteBoundary - nSrcCurrBit;
        assert( nMaskBits > 0 );
        assert( nMaskBits <= CHAR_BIT );
        unsigned char cMask = 0xFF;
        cMask >>= ( CHAR_BIT - nMaskBits );
        unsigned char cSrcBits = *pSrcByte & cMask;
        
        // Shift source appropriately depending on next boundary.
        // No shift if matching byte boundarys.
        if( bIsSrcNextBoundary )
            cSrcBits <<= nLeftShiftBits;
        if( bIsDestNextBoundary )
            cSrcBits >>= nRightShiftBits;

        // Update the destination        
        *pDestByte |= cSrcBits;
            
        // Update bit ptrs
        int nBitsUpdated = nNextByteBoundary - nSrcCurrBit;
        assert( nBitsUpdated > 0 );
        assert( nBitsUpdated <= CHAR_BIT );
        nSrcCurrBit += nBitsUpdated;
        nDestCurrBit += nBitsUpdated;

        if( bIsSrcNextBoundary || bMatchingBoundary )
        {
            // Move into the next src byte
            ++pSrcByte;
            assert( nSrcCurrBit % CHAR_BIT == 0 );
            nSrcNextByteBoundary += CHAR_BIT;
        }
        if( bIsDestNextBoundary || bMatchingBoundary )
        {
            // Move into the next dest byte
            ++pDestByte;
            assert( nDestCurrBit % CHAR_BIT == 0 );
            nDestNextByteBoundary += CHAR_BIT;
        }                
    }

    // Convert T to host order
    *ptDest = NetworkToHostOrder( nDest );
}

//-----------------------------------------------------------------------------
//
// Packable class
//
// For any object, define how you pack yourself.
// Abstract base class; contains no data.
// Overhead: 1 pointer for virtual function table.

class Packable
{
public:

    virtual ~Packable() = 0
    {
    }
    
    // char* and int parameters match standard socket interface
    virtual void Pack( char* pPackBuffer, int* pnStartBit ) const = 0;
    virtual void Unpack( const char* pUnpackBuffer, int* pnStartBit ) = 0;
    virtual int GetBits() const = 0;
    
    // Override if minimum value is not zero
    virtual int64 GetMin() const
    {
        return int64( 0 );
    }
    
    // Override if minimum value is not based on number of bits
    virtual int64 GetMax() const
    {
        assert( GetBits() > 0 );
        int64 nRange = 1;
        for( int i = 0; i < GetBits(); ++i )
            nRange *= 2;
        nRange -= 1;        
        return GetMin() + nRange;
    }

};

//-----------------------------------------------------------------------------
//
// PackableT class
//
// Holds the value itself. Defines operators that allow the callers to treat
// the class as if it were the value itself.
// Overhead: sizeof(T)

template< typename T >
class PackableT : public Packable
{
protected:

    T mn;

public:

    PackableT( const T& t )
    :
        mn( t )
    {
    }

    // conversion operator allows PackableT to appear as normal T
    operator T() const
    {
        return mn;
    }
    
    // address operator allows caller to take address of underlying value
    const T* operator &() const
    {
        return &mn;
    }

    T* operator &()
    {
        return &mn;
    }

};

//-----------------------------------------------------------------------------
//
// PkUint class
//
// How unsigned integers pack themselves. Caller specifies number of bits
// used by the unsigned value. Bits default to the maximum number of bits in T.

template< typename T, int Bits = sizeof(T) * CHAR_BIT >
class PkUint : public PackableT< T >
{
public:

    PkUint()
    :
        PackableT< T >( T( 0 ) )
    {
        static_assert( IsUnsignedInt< T >::GetResult );
        static_assert( Bits > 0 );
        static_assert( Bits <= ( sizeof( T ) * CHAR_BIT ) );
    }
    
    PkUint( const T& t )
    :
        PackableT< T >( t )
    {
        static_assert( IsUnsignedInt< T >::GetResult );
        static_assert( Bits > 0 );
        static_assert( Bits <= ( sizeof( T ) * CHAR_BIT ) );
        assert( t >= T( GetMin() ) );
        assert( t <= T( GetMax() ) );
    }
    
    PkUint& operator=( const T& t )
    {
        assert( t >= T( GetMin() ) );
        assert( t <= T( GetMax() ) );
        mn = t;
        return *this;
    }
    
    template< typename U >
    PkUint& operator=( const U& u )
    {
        static_assert( IsUnsignedInt< U >::GetResult ||
                       IsSignedInt< U >::GetResult );
        assert( u >= GetMin() );
        assert( u <= GetMax() );
        mn = T( u );
        return *this;
    }

    void Pack( char* pBitBuffer, int* pnStartBit ) const
    {
        assert( pBitBuffer != NULL );
        assert( pnStartBit != NULL );
        assert( mn >= T( GetMin() ) );
        assert( mn <= T( GetMax() ) );
        
        BitPack::Pack( mn, GetBits(), pBitBuffer, *pnStartBit );

        *pnStartBit += GetBits();
    }
    
    void Unpack( const char* pBitBuffer, int* pnStartBit )
    {
        assert( pBitBuffer != NULL );
        assert( pnStartBit != NULL );
        
        BitPack::Unpack( &mn, GetBits(), pBitBuffer, *pnStartBit );
        
        *pnStartBit += GetBits();
    }

    int GetBits() const
    {
        return Bits;
    }

};

//-----------------------------------------------------------------------------
//
// PkUintRange class
//
// How unsigned integers ranges pack themselves. Caller specifies range of valid
// unsigned values (inclusive).

template< typename T, uint64 Min, uint64 Max >
class PkUintRange : public PackableT< T >
{
public:
    
    PkUintRange()
    :
        PackableT< T >( T( Min ) )
    {
        static_assert( Min < Max );
        static_assert( IsUnsignedInt< T >::GetResult );
        assert( Max <= uint64( std::numeric_limits< T >::max() ) );
    }
    
    PkUintRange( const T& t )
    :
        PackableT< T >( t )
    {
        static_assert( Min < Max );
        static_assert( IsUnsignedInt< T >::GetResult );
        assert( Max <= uint64( std::numeric_limits< T >::max() ) );
        assert( uint64( t ) >= Min );
        assert( uint64( t ) <= Max );
        assert( t >= T( GetMin() ) );
        assert( t <= T( GetMax() ) );
    }
    
    PkUintRange& operator=( const T& t )
    {
        assert( uint64( t ) >= Min );
        assert( uint64( t ) <= Max );
        assert( t >= T( GetMin() ) );
        assert( t <= T( GetMax() ) );
        mn = t;
        return *this;
    }
    
    template< typename U >
    PkUintRange& operator=( const U& u )
    {
        static_assert( IsUnsignedInt< U >::GetResult ||
                       IsSignedInt< U >::GetResult );
        assert( uint64( u ) >= Min );
        assert( uint64( u ) <= Max );
        assert( T( u ) >= T( GetMin() ) );
        assert( T( u ) <= T( GetMax() ) );
        mn = T( u );
        return *this;
    }
    
    void Pack( char* pBitBuffer, int* pnStartBit ) const
    {
        assert( uint64( mn ) >= Min );
        assert( uint64( mn ) <= Max );
        assert( mn >= T( GetMin() ) );
        assert( mn <= T( GetMax() ) );

        assert( pBitBuffer != NULL );
        assert( pnStartBit != NULL );

        // Offset by Min
        uint64 t = mn - GetMin();
        
        BitPack::Pack( t, GetBits(), pBitBuffer, *pnStartBit );

        *pnStartBit += GetBits();
    }
    
    void Unpack( const char* pBitBuffer, int* pnStartBit )
    {
        assert( pBitBuffer != NULL );
        assert( pnStartBit != NULL );
        
        uint64 t;
        BitPack::Unpack( &t, GetBits(), pBitBuffer, *pnStartBit );
        
        // Offset by Min
        mn = T( t + GetMin() );

        assert( uint64( mn ) >= Min );
        assert( uint64( mn ) <= Max );
        assert( mn >= T( GetMin() ) );
        assert( mn <= T( GetMax() ) );
        
        *pnStartBit += GetBits();
    }

    int GetBits() const
    {
        return BitsRequired< Max - Min >::GetValue;
    }

    int64 GetMin() const
    {
        return Min;
    }
    
    int64 GetMax() const
    {
        return Max;
    }

};

//-----------------------------------------------------------------------------
//
// PkInt class
//
// How signed integers pack themselves. Caller specifies range of valid
// values (inclusive).

template< typename T, int64 Min, int64 Max >
class PkInt : public PackableT< T >
{
public:
    
    PkInt()
    :
        PackableT< T >( T( 0 ) )
    {
        static_assert( Min < Max );
        static_assert( IsSignedInt< T >::GetResult );
        assert( Min >= int64( std::numeric_limits< T >::min() ) );
        assert( Max <= int64( std::numeric_limits< T >::max() ) );
    }
    
    PkInt( const T& t )
    :
        PackableT< T >( t )
    {
        static_assert( Min < Max );
        static_assert( IsSignedInt< T >::GetResult );
        assert( Min >= int64( std::numeric_limits< T >::min() ) );
        assert( Max <= int64( std::numeric_limits< T >::max() ) );
        assert( int64( t ) >= Min );
        assert( int64( t ) <= Max );
        assert( t >= T( GetMin() ) );
        assert( t <= T( GetMax() ) );
    }
    
    PkInt& operator=( const T& t )
    {
        assert( int64( t ) >= Min );
        assert( int64( t ) <= Max );
        assert( t >= T( GetMin() ) );
        assert( t <= T( GetMax() ) );
        mn = t;
        return *this;
    }
    
    template< typename U >
    PkInt& operator=( const U& u )
    {
        static_assert( IsUnsignedInt< U >::GetResult ||
                       IsSignedInt< U >::GetResult );
        assert( int64( u ) >= Min );
        assert( int64( u ) <= Max );
        assert( T( u ) >= T( GetMin() ) );
        assert( T( u ) <= T( GetMax() ) );
        mn = T( u );
        return *this;
    }
    
    void Pack( char* pBitBuffer, int* pnStartBit ) const
    {
        assert( int64( mn ) >= Min );
        assert( int64( mn ) <= Max );
        assert( mn >= T( GetMin() ) );
        assert( mn <= T( GetMax() ) );

        assert( pBitBuffer != NULL );
        assert( pnStartBit != NULL );

        // Convert to unsigned; offset by Min
        uint64 t = mn - GetMin();
        
        BitPack::Pack( t, GetBits(), pBitBuffer, *pnStartBit );

        *pnStartBit += GetBits();
    }
    
    void Unpack( const char* pBitBuffer, int* pnStartBit )
    {
        assert( pBitBuffer != NULL );
        assert( pnStartBit != NULL );
        
        uint64 t;
        BitPack::Unpack( &t, GetBits(), pBitBuffer, *pnStartBit );
        
        // Convert to signed; offset by Min
        mn = T( t + GetMin() );

        assert( int64( mn ) >= Min );
        assert( int64( mn ) <= Max );
        assert( mn >= T( GetMin() ) );
        assert( mn <= T( GetMax() ) );
        
        *pnStartBit += GetBits();
    }

    int GetBits() const
    {
        return BitsRequired< Max - Min >::GetValue;
    }

    int64 GetMin() const
    {
        return Min;
    }
    
    int64 GetMax() const
    {
        return Max;
    }

};

//-----------------------------------------------------------------------------
//
// BitPackCodec class
//
// Allows derived structs/classes to easily pack and unpack all packable
// values within the struct/class. Caller invokes Register in caller's ctor
// for each packable value.
//
// N is the maximum number of items being packed.

template< int N >
class BitPackCodec
{
private:

    Packable* mPackList[ N ]; // list of Packable ptrs
    int mnSize;               // actual number of elements in the list
    
public:    

    BitPackCodec()
    : 
        mnSize( 0 )
    {
#ifdef _DEBUG
        for( int i = 0; i < N; ++i )
            mPackList[ i ] = NULL;
#endif
    }
        
    // Add the packable item to the list
    void Register( Packable& n )
    {
        assert( mnSize < N );
        mPackList[ mnSize++ ] = &n;
    }

    char* Pack( char* pPackBuffer )
    {
        assert( pPackBuffer != NULL );
        assert( mnSize > 0 );
        
        // The bit buffer is assumed to be large enough
        assert( !IsBadWritePtr( pPackBuffer, GetPackedBytes() ) );

        // Zero init the buffer
        memset( pPackBuffer, 0, GetPackedBytes() );

        int nCurrBit = 0;
        for( int i = 0; i < mnSize; ++i )
        {
            assert( mPackList[ i ] != NULL );
            mPackList[ i ]->Pack( pPackBuffer, &nCurrBit );
        }

        // Return the result so the it can be used directly
        return pPackBuffer;
    }
    
    void Unpack( const char* pUnpackBuffer )
    {
        assert( pUnpackBuffer != NULL );
        assert( mnSize > 0 );

        // The bit buffer is assumed to be large enough
        assert( !IsBadReadPtr( pUnpackBuffer, GetPackedBytes() ) );
        
        int nCurrBit = 0;
        for( int i = 0; i < mnSize; ++i )
        {
            assert( mPackList[ i ] != NULL );
            mPackList[ i ]->Unpack( pUnpackBuffer, &nCurrBit );
        }
    }

    // Minimum number of bytes required for pack buffer    
    int GetPackedBytes() const
    {
        // As long as mnSize is small, this is a fairly fast operation.
        // If mnSize is large, this function could be optimized by 
        // having another member variable that stores the total number
        // of Packable bits, computed when objects are Register()'d.
        int nBits = 0;
        for( int i = 0; i < mnSize; ++i )
            nBits += mPackList[ i ]->GetBits();
    
        return ( nBits + CHAR_BIT - 1 ) / CHAR_BIT;
    }
    
};

} // end namespace BitPack

#endif // BITPACK_BITPACK_H
