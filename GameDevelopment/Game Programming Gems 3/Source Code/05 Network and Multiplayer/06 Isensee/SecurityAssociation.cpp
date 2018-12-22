///////////////////////////////////////////////////////////////////////////////
//
//  SecurityAssociation.cpp
//
//  The collection of data that defines the security parameters for a pair of
//  endpoints in a communication link.
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

#include "SecurityAssociation.h"

namespace SecureSock
{

enum
{
    // Default hash algorithm used during HMAC if SetHashAuthAlg() not called
    DefaultHash = CALG_MD5,

    // The Initialization Vector (IV) is used to prevent identical plaintext
    // from encrypting to the same ciphertext. Most symmetric ciphers have
    // an IV block size of 8 bytes. Reducing the size increases the likelihood
    // that identical plaintext will encrypt to the same ciphertext. We default
    // to a middle ground that minimizes bandwidth at the expense of some
    // loss of security for identical payloads.

                    // Chance of identical payloads encrypting the same
    IVMin = 2,      // 1 in 65536
    IVMax = 8,      // 1 in 1.8 x 10^19
    IVDefault = 4,  // 1 in 4.3 x 10^9

    // The Integrity Check Value (ICV) is a truncated HMAC. Truncating this
    // digest is a well-known security practice. RFC 2104 recommends
    // that the ICV be not less than half the length of the HMAC and 
    // also not less than 80 bits. An MD5 HMAC is 128 bits, and a SHA 
    // HMAC is 160 bits, so a minimum ICV size is 64. Most IPSec 
    // implementations use 96 bits (12 bytes). We default to 80 bits
    // (10 bytes) to minimize bandwidth.

    ICVMin = 8,         // 64 bits
    ICVMax = 12,        // 96 bits
    ICVDefault = 10,    // 80 bits

    // Additional random padding can be added to hide the true size of
    // the payload. This ensures that payloads of the same size do not
    // always have the same size packet. However, random padding also increases
    // bandwidth requirements. We default to a maximum of 1 additional
    // padding block (the block size is dependent on the encryption cipher).

    MaxPadMin = 0,      // No random padding
    MaxPadMax = 4,      // Significant random padding
    MaxPadDefault = 1,  // Up to one block

};

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

SecurityAssociation::SecurityAssociation()
:
    mKeyAuth(),
    mKeyCipher(),
    mAuthHashAlg( DefaultHash ),
    mSequenceNum( 1 ),
    mLastSequenceNum( 0 ),
    mReplayWindow( 0 ),
    mIVSize( IVDefault ),
    mICVSize( ICVDefault ),
    mMaxPadBlocks( MaxPadDefault )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Create from keys

SecurityAssociation::SecurityAssociation( const Key& keyAuth, const Key& keyCipher,
                                          AlgType nHashAlgId )
:
    mKeyAuth( keyAuth ),
    mKeyCipher( keyCipher ),
    mAuthHashAlg( nHashAlgId ),
    mSequenceNum( 1 ),
    mLastSequenceNum( 0 ),
    mReplayWindow( 0 ),
    mIVSize( IVDefault ),
    mICVSize( ICVDefault ),
    mMaxPadBlocks( MaxPadDefault )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy ctor

SecurityAssociation::SecurityAssociation( const SecurityAssociation& sa )
:
    mKeyAuth         ( sa.mKeyAuth ),
    mKeyCipher       ( sa.mKeyCipher ),
    mAuthHashAlg     ( sa.mAuthHashAlg ),
    mSequenceNum     ( sa.mSequenceNum ),
    mLastSequenceNum ( sa.mLastSequenceNum ),
    mReplayWindow    ( sa.mReplayWindow ),
    mIVSize          ( sa.mIVSize ),
    mICVSize         ( sa.mICVSize ),
    mMaxPadBlocks    ( sa.mMaxPadBlocks )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy operator

SecurityAssociation& SecurityAssociation::operator=( const SecurityAssociation& sa )
{
    SecurityAssociation temp( sa );
    Swap( temp );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

SecurityAssociation::~SecurityAssociation()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Swap (see Exceptional C++, Herb Sutter, Item 12 )

void SecurityAssociation::Swap( SecurityAssociation& sa )
{
    mKeyAuth.Swap( sa.mKeyAuth );
    mKeyCipher.Swap( sa.mKeyCipher );
    std::swap( mAuthHashAlg, sa.mAuthHashAlg );
    std::swap( mSequenceNum, sa.mSequenceNum );
    std::swap( mLastSequenceNum, sa.mLastSequenceNum );
    std::swap( mReplayWindow, sa.mReplayWindow );
    std::swap( mIVSize, sa.mIVSize );
    std::swap( mICVSize, sa.mICVSize );
    std::swap( mMaxPadBlocks, sa.mMaxPadBlocks );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the authentication key

void SecurityAssociation::SetAuthKey( const Key& keyAuth )
{
    assert( keyAuth.IsValid() );
    mKeyAuth = keyAuth;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the cipher key

void SecurityAssociation::SetCipherKey( const Key& keyCipher )
{
    assert( keyCipher.IsValid() );
    mKeyCipher = keyCipher;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the authentication hash algorithm

void SecurityAssociation::SetAuthHashAlg( AlgType nAlgId )
{
    mAuthHashAlg = nAlgId;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the IV size. Increase this number to maximize security for identical
// payloads at the expense of bandwidth. Decrease to minimize bandwidth at
// the expense of exposing identical payloads.

void SecurityAssociation::SetIVSize( unsigned char nIVSize )
{
    assert( nIVSize >= IVMin );
    assert( nIVSize <= IVMax );
    mIVSize = nIVSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the ICV size. Increase this number to maximize authentication at the
// expense of bandwidth. Decrease to minimize bandwidth at the expense of
// reducing authentication guarantees.

void SecurityAssociation::SetICVSize( unsigned char nICVSize )
{
    assert( nICVSize >= ICVMin );
    assert( nICVSize <= ICVMax );
    mICVSize = nICVSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the maximum padding blocks allowed. Increase this number to maximize
// confusion at the expense of bandwidth. Decrease to zero to
// minimize bandwidth at the expense of exposing payload length.

void SecurityAssociation::SetMaxPadBlocks( unsigned char nMaxPadBlocks )
{
    assert( nMaxPadBlocks >= MaxPadMin );
    assert( nMaxPadBlocks <= MaxPadMax );
    mMaxPadBlocks = nMaxPadBlocks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the auth key

const Key& SecurityAssociation::GetAuthKey() const
{
    return mKeyAuth;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the cipher key

const Key& SecurityAssociation::GetCipherKey() const
{
    return mKeyCipher;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the auth hash algorithm

AlgType SecurityAssociation::GetAuthHashAlg() const
{
    return mAuthHashAlg;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the sequence number

SeqType SecurityAssociation::GetSequenceNum() const
{
    return mSequenceNum;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the IV size in bytes

unsigned char SecurityAssociation::GetIVSize() const
{
    return mIVSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the ICV size in bytes

unsigned char SecurityAssociation::GetICVSize() const
{
    return mICVSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the maximum number of padding blocks

unsigned char SecurityAssociation::GetMaxPadBlocks() const
{
    return mMaxPadBlocks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Increment the sequence number

void SecurityAssociation::IncrSequenceNum()
{
    ++mSequenceNum;
}

///////////////////////////////////////////////////////////////////////////////
//
// Validate the given sequence number

bool SecurityAssociation::IsValidSequenceNum( SeqType nSequenceNum ) const
{
    // The following algorithm is based on the Sequence Space Window
    // Code Example presented in RFC 2401.
    //
    // The "right" edge of the window represents the highest validated
    // Sequence Number received. Packets that contain Sequence Numbers lower
    // than the "left" edge of the window are rejected. Packets falling
    // within the window are checked against a list of received packets
    // within the window. Duplicates are rejected. If the received packet 
    // falls within the window or is new, or if the packet is to the right 
    // of the window, the Sequence Number is valid and the packet moves on
    // to the next verification stage.

    // Check for sequence number wrap
    if( nSequenceNum == 0 )
        return false;

    // Nominal case: the new number is larger than the last packet
    if( nSequenceNum > mLastSequenceNum )
        return true;

    const unsigned long nReplayWindowSize = sizeof( mReplayWindow ) * CHAR_BIT;
    SeqType nDiff = mLastSequenceNum - nSequenceNum;

    // Packet is too old or wrapped
    if( nDiff >= nReplayWindowSize )
        return false;

    // Packet is a duplicate
    ReplayWinType nBit = 1;
    if( mReplayWindow & ( nBit << nDiff ) )
        return false;

    // Out of order, but within window
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Update the replay window based on the given (validated) sequence number

void SecurityAssociation::UpdateReplayWindow( SeqType nSequenceNum )
{
    // The following algorithm is based on the Sequence Space Window
    // Code Example presented in RFC 2401.
    //
    // The "right" edge of the window represents the highest validated
    // Sequence Number received. Packets that contain Sequence Numbers lower
    // than the "left" edge of the window are rejected. Packets falling
    // within the window are checked against a list of received packets
    // within the window. Duplicates are rejected. If the received packet 
    // falls within the window or is new, or if the packet is to the right 
    // of the window, the Sequence Number is valid and the packet moves on
    // to the next verification stage.

    assert( IsValidSequenceNum( nSequenceNum ) );
    const unsigned long nReplayWindowSize = sizeof( mReplayWindow ) * CHAR_BIT;

    // Nominal case: the new number is larger than the last packet
    if( nSequenceNum > mLastSequenceNum )
    {
        SeqType nDiff = nSequenceNum - mLastSequenceNum;

        // If the packet is within the window, slide the window
        if( nDiff < nReplayWindowSize )
        {
            mReplayWindow <<= nDiff;
            mReplayWindow |= 1;
        }
        else
        {
            // packet way outside the window; reset the window
            mReplayWindow = 1;
        }

        // Update the "last" sequence number
        mLastSequenceNum = nSequenceNum;
    }
    else
    {
        // New number is smaller than the last packet
        SeqType nDiff = mLastSequenceNum - nSequenceNum;

        // Mark the packet as seen
        ReplayWinType nBit = 1;
        mReplayWindow |= ( nBit << nDiff );
    }
}

} // end namespace SecureSock

///////////////////////////////////////////////////////////////////////////////
