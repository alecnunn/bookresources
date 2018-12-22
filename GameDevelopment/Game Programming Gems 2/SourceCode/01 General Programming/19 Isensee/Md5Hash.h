///////////////////////////////////////////////////////////////////////////////
//
//  Md5Hash.h
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

#if !defined( TRUERAND_MD5HASH_H )
#define TRUERAND_MD5HASH_H

#include "Common.h"
#include "Buffer.h"

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// MD5 hash algorithm

class Md5Hash
{
public:

    inline Md5Hash() { Init(); }
    inline ~Md5Hash() {}

    void Init();
    Buffer GetHash( const unsigned char*, unsigned int nBytes );
    int GetSize() const;

private:

    void ProcessBlock( const unsigned long* );
    void ProcessFinalBlock( const unsigned char*, int );

    // MD5 functions
    static inline unsigned long F( unsigned long, unsigned long, unsigned long );
    static inline unsigned long G( unsigned long, unsigned long, unsigned long );
    static inline unsigned long H( unsigned long, unsigned long, unsigned long );
    static inline unsigned long I( unsigned long, unsigned long, unsigned long );

private:

    enum
    {
        kHashBits   = 128,
        kHashBytes  = kHashBits / 8,
        kHashDwords = kHashBytes / sizeof( unsigned long )
    };

private:

    unsigned long mDigest[ kHashDwords ];
    
};

///////////////////////////////////////////////////////////////////////////////
//
// The four basic functions F(), G(), H() and I()

inline unsigned long Md5Hash::F( unsigned long x, unsigned long y, unsigned long z )
{
   return( z ^ ( x & ( y ^ z ) ) );
}

inline unsigned long Md5Hash::G( unsigned long x, unsigned long y, unsigned long z )
{
   return( y ^ ( z & ( x ^ y ) ) );
}

inline unsigned long Md5Hash::H( unsigned long x, unsigned long y, unsigned long z )
{
   return( x ^ y ^ z );
}

inline unsigned long Md5Hash::I( unsigned long x, unsigned long y, unsigned long z )
{
   return( y ^ ( x | ~z ) );
}

} // end namespace TrueRand

#endif // TRUERAND_MD5HASH_H

///////////////////////////////////////////////////////////////////////////////
