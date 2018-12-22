///////////////////////////////////////////////////////////////////////////////
//
//  SecurityAssociation.h
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

#if !defined( SECURE_SOCK_ASSOCIATION_H )
#define SECURE_SOCK_ASSOCIATION_H

#include "Common.h"
#include "Crypto.h"
#include "SecureBuffer.h"

namespace SecureSock
{

///////////////////////////////////////////////////////////////////////////////
//
// SecurityAssociation

class SecurityAssociation
{
public:

    SecurityAssociation();
    SecurityAssociation( const Key& keyAuth, const Key& keyCipher,
                         AlgType nHashAldId );
    SecurityAssociation( const SecurityAssociation& );
    SecurityAssociation& operator=( const SecurityAssociation& );
    ~SecurityAssociation();

    void Swap( SecurityAssociation& );

    void SetAuthKey( const Key& );
    void SetCipherKey( const Key& );
    void SetAuthHashAlg( AlgType nAlgId );
    void SetIVSize( unsigned char nIVSize );
    void SetICVSize( unsigned char nICVSize );
    void SetMaxPadBlocks( unsigned char nMaxPadBlocks );

    const Key& GetAuthKey() const;
    const Key& GetCipherKey() const;
    AlgType GetAuthHashAlg() const;
    SeqType GetSequenceNum() const;
    unsigned char GetIVSize() const;
    unsigned char GetICVSize() const;
    unsigned char GetMaxPadBlocks() const;

    void IncrSequenceNum();
    bool IsValidSequenceNum( SeqType nSequenceNum ) const;
    void UpdateReplayWindow( SeqType nSequenceNum );

private:

    typedef ulonglong ReplayWinType; // Any unsigned integer type

    Key mKeyAuth;                   // Key used for HMAC auth
    Key mKeyCipher;                 // Key used to encrypt payload
    AlgType mAuthHashAlg;           // Which HMAC algorithm to use
    SeqType mSequenceNum;           // Sequence number to use on next packet
    SeqType mLastSequenceNum;       // Last validate sequence number received
    ReplayWinType mReplayWindow;    // Replay window bits
    unsigned char mIVSize;          // IV size in bytes
    unsigned char mICVSize;         // ICV size in bytes
    unsigned char mMaxPadBlocks;    // Maximum additional padding blocks

};

} // end namespace SecureSock

#endif // SECURE_SOCK_ASSOCIATION_H

///////////////////////////////////////////////////////////////////////////////
