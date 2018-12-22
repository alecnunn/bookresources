///////////////////////////////////////////////////////////////////////////////
//
//  Md5Hash.cpp
//
//  Implementation of public domain MD5 hashing algorithm. This implementation
//  will work on any 32-bit platform.
//
//  Specs at http://www.faqs.org/rfcs/rfc1321.html
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

#include "Md5Hash.h"

///////////////////////////////////////////////////////////////////////////////
//
// Using #define guarantees inline code

#define Subround( f, a, b, c, d, k, s, t )                                    \
{                                                                             \
    a += ( k + t + f( b, c, d ) );                                            \
    a = RotateLeft( a, s );                                                   \
    a += b;                                                                   \
}

///////////////////////////////////////////////////////////////////////////////
//
// unsigned 64-bit type

typedef unsigned __int64 ulonglong;

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Local constants

const unsigned char kOneBit = 0x80;

const unsigned int kBlockBits   = 512;
const unsigned int kBlockBytes  = kBlockBits / 8;
const unsigned int kBlockDwords = kBlockBytes / sizeof( unsigned long );

///////////////////////////////////////////////////////////////////////////////
//
// Local functions

inline unsigned long RotateLeft( unsigned long x, int iBits )
{
    // _lrotl is non-standard rotation function to left rotate x by iBits
    return( _lrotl( x, iBits ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Initialize the hash

void Md5Hash::Init()
{
    // Standard MD5 initialization values
    mDigest[ 0 ] = 0x67452301;
    mDigest[ 1 ] = 0xEFCDAB89;
    mDigest[ 2 ] = 0x98BADCFE;
    mDigest[ 3 ] = 0x10325476;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the digest of the input message

Buffer Md5Hash::GetHash( const unsigned char* pBuffer, unsigned int nBytes )
{
    // Determine number of blocks in message, including partial blocks
    int iNumBlocks = 0;
    if( nBytes > 0 )
        iNumBlocks = ( nBytes / kBlockBytes ) + 1;

    // Process each block except the last
    const unsigned long* pBuf = reinterpret_cast< const unsigned long* >( pBuffer );
    int i;
    int iBlockOffset = 0;
    for( i = 0; i < iNumBlocks - 1; ++i, iBlockOffset += kBlockDwords )
        ProcessBlock( pBuf + iBlockOffset );

    // Process the final block
    ProcessFinalBlock( pBuffer + ( i * kBlockBytes ), nBytes );

    const unsigned char* pDigest = reinterpret_cast< const unsigned char* >
                                                            ( mDigest );
    return Buffer( pDigest, kHashBytes );
}

///////////////////////////////////////////////////////////////////////////////
//
// Size of the hash value in bytes

int Md5Hash::GetSize() const
{
    return kHashBytes;
}

///////////////////////////////////////////////////////////////////////////////
//
// Process a 512-bit block

void Md5Hash::ProcessBlock( const unsigned long* pBlock ) // private
{
    assert( pBlock != NULL );

    unsigned long A = mDigest[ 0 ];
    unsigned long B = mDigest[ 1 ];
    unsigned long C = mDigest[ 2 ];
    unsigned long D = mDigest[ 3 ];

    // Note: the constants for the MD5 hash were chosen as follows:
    // In step i, c is the integer part of 2^32 * abs( sin ( i ) ), where i is
    // in radians.

    // Round 0
    Subround( F, A, B, C, D, pBlock[  0 ],  7, 0xD76AA478 );
    Subround( F, D, A, B, C, pBlock[  1 ], 12, 0xE8C7B756 );
    Subround( F, C, D, A, B, pBlock[  2 ], 17, 0x242070DB );
    Subround( F, B, C, D, A, pBlock[  3 ], 22, 0xC1BDCEEE );
    Subround( F, A, B, C, D, pBlock[  4 ],  7, 0xF57C0FAF );
    Subround( F, D, A, B, C, pBlock[  5 ], 12, 0x4787C62A );
    Subround( F, C, D, A, B, pBlock[  6 ], 17, 0xA8304613 );
    Subround( F, B, C, D, A, pBlock[  7 ], 22, 0xFD469501 );
    Subround( F, A, B, C, D, pBlock[  8 ],  7, 0x698098D8 );
    Subround( F, D, A, B, C, pBlock[  9 ], 12, 0x8B44F7AF );
    Subround( F, C, D, A, B, pBlock[ 10 ], 17, 0xFFFF5BB1 );
    Subround( F, B, C, D, A, pBlock[ 11 ], 22, 0x895CD7BE );
    Subround( F, A, B, C, D, pBlock[ 12 ],  7, 0x6B901122 );
    Subround( F, D, A, B, C, pBlock[ 13 ], 12, 0xFD987193 );
    Subround( F, C, D, A, B, pBlock[ 14 ], 17, 0xA679438E );
    Subround( F, B, C, D, A, pBlock[ 15 ], 22, 0x49B40821 );

    // Round 1
    Subround( G, A, B, C, D, pBlock[  1 ],  5, 0xF61E2562 );
    Subround( G, D, A, B, C, pBlock[  6 ],  9, 0xC040B340 );
    Subround( G, C, D, A, B, pBlock[ 11 ], 14, 0x265E5A51 );
    Subround( G, B, C, D, A, pBlock[  0 ], 20, 0xE9B6C7AA );
    Subround( G, A, B, C, D, pBlock[  5 ],  5, 0xD62F105D );
    Subround( G, D, A, B, C, pBlock[ 10 ],  9, 0x02441453 );
    Subround( G, C, D, A, B, pBlock[ 15 ], 14, 0xD8A1E681 );
    Subround( G, B, C, D, A, pBlock[  4 ], 20, 0xE7D3FBC8 );
    Subround( G, A, B, C, D, pBlock[  9 ],  5, 0x21E1CDE6 );
    Subround( G, D, A, B, C, pBlock[ 14 ],  9, 0xC33707D6 );
    Subround( G, C, D, A, B, pBlock[  3 ], 14, 0xF4D50D87 );
    Subround( G, B, C, D, A, pBlock[  8 ], 20, 0x455A14ED );
    Subround( G, A, B, C, D, pBlock[ 13 ],  5, 0xA9E3E905 );
    Subround( G, D, A, B, C, pBlock[  2 ],  9, 0xFCEFA3F8 );
    Subround( G, C, D, A, B, pBlock[  7 ], 14, 0x676F02D9 );
    Subround( G, B, C, D, A, pBlock[ 12 ], 20, 0x8D2A4C8A );

    // Round 2
    Subround( H, A, B, C, D, pBlock[  5 ],  4, 0xFFFA3942 );
    Subround( H, D, A, B, C, pBlock[  8 ], 11, 0x8771F681 );
    Subround( H, C, D, A, B, pBlock[ 11 ], 16, 0x6D9D6122 );
    Subround( H, B, C, D, A, pBlock[ 14 ], 23, 0xFDE5380C );
    Subround( H, A, B, C, D, pBlock[  1 ],  4, 0xA4BEEA44 );
    Subround( H, D, A, B, C, pBlock[  4 ], 11, 0x4BDECFA9 );
    Subround( H, C, D, A, B, pBlock[  7 ], 16, 0xF6BB4B60 );
    Subround( H, B, C, D, A, pBlock[ 10 ], 23, 0xBEBFBC70 );
    Subround( H, A, B, C, D, pBlock[ 13 ],  4, 0x289B7EC6 );
    Subround( H, D, A, B, C, pBlock[  0 ], 11, 0xEAA127FA );
    Subround( H, C, D, A, B, pBlock[  3 ], 16, 0xD4EF3085 );
    Subround( H, B, C, D, A, pBlock[  6 ], 23, 0x04881D05 );
    Subround( H, A, B, C, D, pBlock[  9 ],  4, 0xD9D4D039 );
    Subround( H, D, A, B, C, pBlock[ 12 ], 11, 0xE6DB99E5 );
    Subround( H, C, D, A, B, pBlock[ 15 ], 16, 0x1FA27CF8 );
    Subround( H, B, C, D, A, pBlock[  2 ], 23, 0xC4AC5665 );

    // Round 3
    Subround( I, A, B, C, D, pBlock[  0 ],  6, 0xF4292244 );
    Subround( I, D, A, B, C, pBlock[  7 ], 10, 0x432AFF97 );
    Subround( I, C, D, A, B, pBlock[ 14 ], 15, 0xAB9423A7 );
    Subround( I, B, C, D, A, pBlock[  5 ], 21, 0xFC93A039 );
    Subround( I, A, B, C, D, pBlock[ 12 ],  6, 0x655B59C3 );
    Subround( I, D, A, B, C, pBlock[  3 ], 10, 0x8F0CCC92 );
    Subround( I, C, D, A, B, pBlock[ 10 ], 15, 0xFFEFF47D );
    Subround( I, B, C, D, A, pBlock[  1 ], 21, 0x85845DD1 );
    Subround( I, A, B, C, D, pBlock[  8 ],  6, 0x6FA87E4F );
    Subround( I, D, A, B, C, pBlock[ 15 ], 10, 0xFE2CE6E0 );
    Subround( I, C, D, A, B, pBlock[  6 ], 15, 0xA3014314 );
    Subround( I, B, C, D, A, pBlock[ 13 ], 21, 0x4E0811A1 );
    Subround( I, A, B, C, D, pBlock[  4 ],  6, 0xF7537E82 );
    Subround( I, D, A, B, C, pBlock[ 11 ], 10, 0xBD3AF235 );
    Subround( I, C, D, A, B, pBlock[  2 ], 15, 0x2AD7D2BB );
    Subround( I, B, C, D, A, pBlock[  9 ], 21, 0xEB86D391 );

    mDigest[ 0 ] += A;
    mDigest[ 1 ] += B;
    mDigest[ 2 ] += C;
    mDigest[ 3 ] += D;
}

///////////////////////////////////////////////////////////////////////////////
//
// Process the final block.  The final block is padded out to a multiple
// of 512 bits.  The padding consists of the following:
//
// 1) A single 1-bit added to the end of the message, followed by zeros
//    up to ( 512 - 64 ) bits.
// 2) The length of the original message, in bits, in the last 64 bits
//    of the block.
//
// If the final message block is too large to be padded out to a 512-bit
// boundary and still include the 64-bit length, then an additional
// 512-bit padding block is added.

void Md5Hash::ProcessFinalBlock( const unsigned char* pBlock, int iBufferLen )
{
    // Determine the actual data size of the final block
    int iDataSizeLastBlock = iBufferLen % kBlockBytes;

    // Copy block data into the final block
    unsigned char pFinal[ kBlockBytes ];
    std::copy( pBlock, pBlock + iDataSizeLastBlock, pFinal );
    unsigned long* pFinal32 = reinterpret_cast< unsigned long* >( pFinal );

    // Zero the padding area
    std::fill( pFinal + iDataSizeLastBlock, pFinal + kBlockBytes, 0 );

    // Put a single 1-bit in the padding area
    if( iDataSizeLastBlock < kBlockBytes )
        pFinal[ iDataSizeLastBlock ] = kOneBit;

    // If the last block is greater than or equal to 56 bytes, then there's
    // no room to include the length as a 64-bit value.  In this case, we
    // actually process two blocks, the padding block and the "length" block
    if( iDataSizeLastBlock >= int( kBlockBytes - sizeof( ulonglong ) ) )
    {
        // Process this next-to-last block
        ProcessBlock( pFinal32 );

        // Now generate and process the real last block
        std::fill( pFinal, pFinal + kBlockBytes, 0 );
    }

    // Put the length of the message (in bits) into the final 64-bits
    ulonglong iBufferLen64 = ulonglong( iBufferLen ) * ulonglong( 8 );
    unsigned char* pBufferLen = reinterpret_cast< unsigned char* >
                                                            ( &iBufferLen64 );
    memcpy( pFinal + kBlockBytes - sizeof( ulonglong ), pBufferLen, 
            sizeof( ulonglong ) );

    // Process this last block
    ProcessBlock( pFinal32 );
}

} // end namespace TrueRand

///////////////////////////////////////////////////////////////////////////////
