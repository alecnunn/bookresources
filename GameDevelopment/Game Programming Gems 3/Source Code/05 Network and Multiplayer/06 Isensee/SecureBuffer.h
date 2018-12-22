///////////////////////////////////////////////////////////////////////////////
//
//  SecureBuffer.h
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

#if !defined( SECURE_SOCK_SECURE_BUFFER_H )
#define SECURE_SOCK_SECURE_BUFFER_H

#include "Common.h"
#include "Buffer.h"
#include "Crypto.h"

namespace SecureSock
{

// Common typedefs
typedef unsigned short SpiType; // SPI
typedef unsigned long  SeqType; // Sequence Number

// Forward declarations
class SecurityAssociation;
class SADatabase;

///////////////////////////////////////////////////////////////////////////////
//
// SecureBuffer

class SecureBuffer
{
public:

    SecureBuffer( SpiType nSPI=0 );
    SecureBuffer( const void*, unsigned long nBytes );
    SecureBuffer( const SecureBuffer& buf );
    SecureBuffer& operator=( const SecureBuffer& );
    ~SecureBuffer();

    bool Create( const Buffer& );
    bool Create( const void*, unsigned long nBytes );
    void Set( const Buffer& );
    bool IsAuthentic() const;
    bool GetPayload( Buffer& payload ) const;
    bool GetPayload( char*, int* );
    void SetAccepted();

    const char* GetDataPtr() const;
    unsigned long GetSize() const;

    static void SetSADatabase( SADatabase* );

private:

    const SecurityAssociation& GetSA() const;
    SecurityAssociation& GetSA();

    Buffer GenerateIV() const;
    Buffer GenerateCrypt( const Buffer& payload, const Buffer& iv ) const;
    Buffer GeneratePadding( const Buffer& payload ) const;
    Buffer GenerateICV( const Buffer& auth ) const;

    unsigned long GetMaxSecureSize( const Buffer& payload ) const;
    unsigned long GetMaxPadSize( const Buffer& payload ) const;

    bool IsValidSize() const;
    bool IsValidSPIndex( SpiType& nSPIndex ) const;
    bool IsValidSequenceNum( SeqType& nSequenceNum ) const;
    bool IsValidICV() const;
    bool IsValidPadding( const Buffer& payload,
                         unsigned long& nPadBytes ) const;

private:

    Buffer mBuf;
    mutable SpiType mSPIndex; // Security Parameters Index

    static SADatabase* mpSADatabase;

};

} // end namespace SecureSock

#endif // SECURE_SOCK_SECURE_BUFFER_H

///////////////////////////////////////////////////////////////////////////////
