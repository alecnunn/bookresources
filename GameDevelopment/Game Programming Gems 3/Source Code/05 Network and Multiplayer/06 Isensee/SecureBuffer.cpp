///////////////////////////////////////////////////////////////////////////////
//
//  SecureBuffer.cpp
//
//  Functions for encapsulating a buffer with an encrypted and authenticated
//  protocol similar to IPSec/ESP.
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

#include "SecureBuffer.h"
#include "SecurityAssociation.h"
#include "SADatabase.h"

namespace SecureSock
{

// Statics
SADatabase* SecureBuffer::mpSADatabase = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

SecureBuffer::SecureBuffer( SpiType nSPI )
:
    mBuf(),
    mSPIndex( nSPI )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Create from existing encrypted buffer. This function is designed to work
// with socket recv().

SecureBuffer::SecureBuffer( const void* pCipherData, unsigned long nBytes )
{
    mBuf.assign( reinterpret_cast< const unsigned char* >( pCipherData ), nBytes );
    mSPIndex = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy ctor

SecureBuffer::SecureBuffer( const SecureBuffer& buf )
:
    mBuf( buf.mBuf ),
    mSPIndex( buf.mSPIndex )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

SecureBuffer::~SecureBuffer()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a secure buffer given the payload

bool SecureBuffer::Create( const Buffer& payload )
{
    // Clear any existing buffer
    mBuf.erase();

    // Pre-allocate for speed
    mBuf.reserve( GetMaxSecureSize( payload ) );

    // Append the Security Parameters Index
    Append( mBuf, mSPIndex );

    // Append the sequence number
    Append( mBuf, GetSA().GetSequenceNum() );

    // Append the encryption Initialization Vector
    Buffer iv = GenerateIV();
    mBuf.append( iv, 0, GetSA().GetIVSize() );

    // Append the encrypted portion of the buffer
    Buffer crypt = GenerateCrypt( payload, iv );
    if( crypt.empty() )
        return false;
    mBuf.append( crypt );

    // Append the Integrity Check Value
    Buffer icv = GenerateICV( mBuf );
    if( icv.empty() )
        return false;
    mBuf.append( icv );

    // Secure buffer was successfully created; increment the seq number
    GetSA().IncrSequenceNum();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a secure buffer given the payload

bool SecureBuffer::Create( const void* pData, unsigned long nBytes )
{
    return Create( Buffer( reinterpret_cast<const unsigned char*>(pData), nBytes ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy a secure buffer

void SecureBuffer::Set( const Buffer& secureBuffer )
{
    mBuf = secureBuffer;
    mSPIndex = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Validate a secure buffer by examining the authentication data

bool SecureBuffer::IsAuthentic() const
{
    // Check Security Parameters Index (SPI)
    SpiType nSPIndex = 0;
    if( !IsValidSPIndex( nSPIndex ) )
        return false;

    // Update internal index to match
    mSPIndex = nSPIndex;

    // Check packet size
    if( !IsValidSize() )
        return false;

    // Validate the Integrity Check Value (ICV)
    if( !IsValidICV() )
        return false;

    // Check sequence number
    SeqType nSequenceNum;
    if( !IsValidSequenceNum( nSequenceNum ) )
        return false;

    // If we reach this point, the packet has passed all integrity
    // checks. (Though we may later determine that the encrypted data 
    // or padding is bogus).
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the payload from a secure buffer. Returns false if the payload
// is invalid or cannot be retrieved. Assumes the buffer has already been
// authenticated.

bool SecureBuffer::GetPayload( Buffer& payload ) const
{
    assert( IsAuthentic() );

    // Extract the IV from the buffer. It's located after the SPI and
    // sequence number.
    unsigned long nPos = sizeof( SpiType ) + sizeof( SeqType );
    unsigned long nSize = GetSA().GetIVSize();
    if( nPos + nSize > mBuf.length() )
        return false;
    Buffer iv = mBuf.substr( nPos, nSize );

    // Fill in the remaining portion of the IV with zero bytes
    iv.resize( GetSA().GetCipherKey().GetIVSize() );

    // Set the IV in a local copy of the cipher key
    Key keyCipher = GetSA().GetCipherKey();
    keyCipher.SetIV( iv );

    // Extract the encrypted portion of the packet. It's located after the IV,
    // and does not include the trailing ICV.
    nPos = sizeof( SpiType ) + sizeof( SeqType ) + nSize;
    if( nPos + GetSA().GetICVSize() > mBuf.length() )
        return false;
    nSize = mBuf.length() - nPos - GetSA().GetICVSize();
    Buffer crypt = mBuf.substr( nPos, nSize );

    // Decrypt the payload
    if( !Cipher( keyCipher ).Decrypt( crypt, true ) )
        return false;

    // Validate any padding bytes
    unsigned long nPadBytes;
    if( !IsValidPadding( crypt, nPadBytes ) )
        return false;

    // Strip the padding and the padding size byte
    payload = crypt;
    assert( payload.length() >= nPadBytes + sizeof(unsigned char) );
    payload.resize( payload.length() - nPadBytes - sizeof(unsigned char) );

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract the payload from a secure buffer. This function is designed to
// work with socket recv.

bool SecureBuffer::GetPayload( char* pPayload, int* pnBytes )
{
    assert( pPayload != NULL );
    assert( pnBytes != NULL );
    Buffer buf;
    if( !GetPayload( buf ) )
        return false;

    *pnBytes = buf.length();
    buf.copy( reinterpret_cast< unsigned char* >( pPayload ), *pnBytes );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Marks a given buffer as authenticated and accepted. Automatically adjusts
// the replay window, so that IsAuthentic() will no longer validate the
// buffer correctly. Call this function only after calling IsAuthentic() and
// GetPayload(), but prior to calling IsAuthentic() on the next packet.

void SecureBuffer::SetAccepted()
{
    SeqType nSequenceNum;
    bool bSuccess = IsValidSequenceNum( nSequenceNum );
    assert( bSuccess );
    USED( bSuccess );

    // Update the replay window
    GetSA().UpdateReplayWindow( nSequenceNum );
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns a pointer to the beginning of the secure data. This function is
// designed to work with socket send().

const char* SecureBuffer::GetDataPtr() const
{
    return( reinterpret_cast< const char* >( mBuf.data() ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the size of the secure buffer in bytes

unsigned long SecureBuffer::GetSize() const
{
    return mBuf.length();
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the global Security Association Database

void SecureBuffer::SetSADatabase( SADatabase* pSAD ) // static
{
    assert( pSAD != NULL );
    mpSADatabase = pSAD;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the SecurityAssociation for this buffer

const SecurityAssociation& SecureBuffer::GetSA() const
{
    assert( mpSADatabase != NULL );
    return mpSADatabase->GetSA( mSPIndex );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the SecurityAssociation for this buffer

SecurityAssociation& SecureBuffer::GetSA()
{
    assert( mpSADatabase != NULL );
    return mpSADatabase->GetSA( mSPIndex );
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate an Initialization Vector

Buffer SecureBuffer::GenerateIV() const
{
    // Put random numbers in the region defined by the SA
    Buffer iv;
    Key::GenRandom( GetSA().GetIVSize(), iv );

    // Resize to the length defined by the cipher, zero-filling if necessary
    iv.resize( GetSA().GetCipherKey().GetIVSize() );
    return iv;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate the encrypted version of the given payload

Buffer SecureBuffer::GenerateCrypt( const Buffer& payload, const Buffer& iv ) const
{
    Buffer crypt( payload );

    // Append padding, if any
    Buffer pad = GeneratePadding( payload );
    crypt.append( pad );

    // Append pad length
    Append( crypt, pad.length(), 1 );

    // Set the IV in a local copy of the cipher key
    Key keyCipher = GetSA().GetCipherKey();
    keyCipher.SetIV( iv );

    // Peform the encryption
    if( !Cipher( keyCipher ).Encrypt( crypt, true ) )
        return Buffer();

    return crypt;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate the padding bytes based on the given payload

Buffer SecureBuffer::GeneratePadding( const Buffer& payload ) const
{
    // The size of the payload, the padding and the pad length (1 byte)
    // must be evenly divisible by nBlockBytes
    unsigned long nBlockBytes = GetSA().GetCipherKey().GetBlockLength() / 
                                CHAR_BIT;
    unsigned long nPadBytes = nBlockBytes - ( ( payload.length() + 1 ) % nBlockBytes );

    // Add some random padding to hide the true size of the payload
    Buffer bufRand;
    Key::GenRandom( 1, bufRand );
    unsigned char nRand = *bufRand.data();
    unsigned long nRandBlocks = 0;
    if( GetSA().GetMaxPadBlocks() > 0 )
        nRandBlocks = nRand % GetSA().GetMaxPadBlocks();
    nPadBytes += nBlockBytes * nRandBlocks;

    // Create the padding buffer
    Buffer pad;
    pad.resize( nPadBytes );

    // RFC 2406 says padding bytes are initialized with a series of 
    // one-byte integer values
    unsigned char* pData = GetData( pad );
    for( unsigned long i = 1; i <= nPadBytes; ++i, ++pData )
        *pData = unsigned char( i );

    return pad;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate the Integrity Check Value of the given buffer

Buffer SecureBuffer::GenerateICV( const Buffer& auth ) const
{
    // Create the ICV using HMAC algorithm
    Hash hash;
    if( !hash.Create( CALG_HMAC, GetSA().GetAuthKey() ) )
        return Buffer();

    if( !hash.SetHMACAlgorithm( GetSA().GetAuthHashAlg() ) )
        return Buffer();

    if( !hash.HashData( auth ) )
        return Buffer();

    Buffer icv = hash.GetHashValue();

    // Truncate the HMAC
    icv.resize( GetSA().GetICVSize() );
    return icv;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine the max size of the secure buffer based on the given payload

unsigned long SecureBuffer::GetMaxSecureSize( const Buffer& payload ) const
{
    unsigned long nSize = 0;
    nSize += sizeof( SpiType );         // SPI
    nSize += sizeof( SeqType );         // sequence number
    nSize += GetSA().GetIVSize();       // IV
    nSize += payload.length();          // payload
    nSize += GetMaxPadSize( payload );  // padding
    nSize += sizeof( unsigned char );   // pad length
    nSize += GetSA().GetICVSize();      // ICV
    return nSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine the maximum number of bytes used for padding based on the
// payload size and encryption key

unsigned long SecureBuffer::GetMaxPadSize( const Buffer& payload ) const
{
    // Determine the number of bytes required for the given cipher
    unsigned long nBlockBytes = GetSA().GetCipherKey().GetBlockLength() / 
                                CHAR_BIT;

    // The size of the payload, the padding and the pad length (1 byte)
    // must be evenly divisible by nBlockBytes
    unsigned long nPadBytes = nBlockBytes - ( ( payload.length() + 1 ) % nBlockBytes );

    // To conceal the actual length of the payload, additional padding
    // may be added on a random basis.
    nPadBytes += GetSA().GetMaxPadBlocks() * nBlockBytes;

    assert( ( payload.length() + 1 + nPadBytes ) % nBlockBytes  == 0 );
    assert( nPadBytes < 255 );
    return nPadBytes;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if the buffer is correctly sized

bool SecureBuffer::IsValidSize() const
{
    // Assume no padding for quick check
    if( mBuf.length() < sizeof( SpiType ) +        // SPI
                        sizeof( SeqType ) +        // sequence number
                        GetSA().GetIVSize() +      // IV
                        sizeof( unsigned char ) +  // pad size
                        GetSA().GetICVSize() )     // ICV
        return false;

    // Buffer meets minimum requires; other checks performed later
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if the SPI of the buffer is valid

bool SecureBuffer::IsValidSPIndex( SpiType& nSPIndex ) const
{
    // If buffer is too small to contain the index, then obviously invalid
    if( mBuf.length() < sizeof( SpiType ) )
        return false;

    // The Security Parameters Index is the first item in the buffer
    nSPIndex = *reinterpret_cast< const SpiType* >( mBuf.data() );

    assert( mpSADatabase != NULL );
    return mpSADatabase->Find( nSPIndex );
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if the sequence number is valid

bool SecureBuffer::IsValidSequenceNum( SeqType& nSequenceNum ) const
{
    // The sequence number is located immediately following the SPI
    nSequenceNum = *reinterpret_cast< const SeqType* >
                   ( mBuf.data() + sizeof( mSPIndex ) );

    return GetSA().IsValidSequenceNum( nSequenceNum );
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if the ICV is valid

bool SecureBuffer::IsValidICV() const
{
    // The ICV is located in the last ICVSize bytes of the packet
    unsigned long nSize = GetSA().GetICVSize();
    unsigned long nPos = mBuf.length() - nSize;
    Buffer icv = mBuf.substr( nPos, nSize );

    // The remainder of the packet is authenticated against the ICV
    nSize = nPos;
    Buffer auth = mBuf.substr( 0, nSize );

    // Validate using HMAC algorithm
    Hash hash;
    if( !hash.Create( CALG_HMAC, GetSA().GetAuthKey() ) )
        return false;

    if( !hash.SetHMACAlgorithm( GetSA().GetAuthHashAlg() ) )
        return false;

    if( !hash.HashData( auth ) )
        return false;

    // Truncate hash to ICV size
    Buffer icvCompare = hash.GetHashValue();
    icvCompare.resize( GetSA().GetICVSize() );

    // Compare the ICV we computed against the ICV that was sent
    return( icv == icvCompare );
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if the given padding is valid

bool SecureBuffer::IsValidPadding( const Buffer& payload, 
                                   unsigned long& nPadBytes ) const
{
    // The payload must minimally contain the padding size
    if( payload.length() == 0 )
        return false;

    // The padding size is in the last byte of the payload
    nPadBytes = *( payload.data() + payload.length() - 1 );

    // Check for valid padding size
    if( nPadBytes + sizeof(unsigned char) > payload.length() )
        return false;

    // Extract the padding
    unsigned long nPos = payload.length() - nPadBytes - sizeof(unsigned char);
    Buffer pad = payload.substr( nPos, nPadBytes );

    // Validate the padding bytes. They should contain a series of
    // one-byte integer values, starting at 1
    const unsigned char* pData = pad.data();
    for( unsigned long i = 1; i <= nPadBytes; ++i, ++pData )
    {
        if( *pData != unsigned char(i) )
            return false;
    }

    // Padding is good
    return true;
}

} // end namespace SecureSock

///////////////////////////////////////////////////////////////////////////////
