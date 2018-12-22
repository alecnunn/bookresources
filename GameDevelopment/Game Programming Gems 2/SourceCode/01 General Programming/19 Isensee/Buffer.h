///////////////////////////////////////////////////////////////////////////////
//
//  Buffer.h
//
//  Dynamic byte buffer, based on std::basic_string< unsigned char >
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

#if !defined( TRUERAND_BUFFER_H )
#define TRUERAND_BUFFER_H

#include "Common.h"

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Byte buffer is a string of bytes.  We get the following for free using
// basic_string<>:
//
//      Buffer size:        length()
//      Appending:          append() or +=
//      Direct access:      c_str()
//      Preallocation:      reserve()
//      Clear:              erase()
//
// And, since basic_string<> is typically reference counted on copy, copying
// buffers is cheap.

typedef std::basic_string< unsigned char > Buffer;

///////////////////////////////////////////////////////////////////////////////
//
// Append data T into the Buffer object

template <class T>
void Append( Buffer& srcBuf, const T& objAppend, size_t nSize )
{
    srcBuf.append( reinterpret_cast< const unsigned char* >( &objAppend ), nSize );
}

template <class T>
void Append( Buffer& srcBuf, const T& objAppend )
{
    Append( srcBuf, objAppend, sizeof(objAppend) );
}

} // end namespace TrueRand

#endif // TRUERAND_BUFFER_H

///////////////////////////////////////////////////////////////////////////////
