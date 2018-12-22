///////////////////////////////////////////////////////////////////////////////
//
//  Buffer.h
//
//  Dynamic byte buffer, based on std::basic_string< unsigned char >
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

#if !defined( SECURE_SOCK_BUFFER_H )
#define SECURE_SOCK_BUFFER_H

#include "Common.h"

namespace SecureSock
{

///////////////////////////////////////////////////////////////////////////////
//
// Byte buffer is a string of bytes.  We get the following for free using
// basic_string<>:
//
//      Buffer size:        length()
//      Appending:          append() or +=
//      Direct access:      c_str() or data()
//      Preallocation:      reserve()
//      Clear:              erase()

typedef std::basic_string< unsigned char > Buffer;

///////////////////////////////////////////////////////////////////////////////
//
// Assign data T into the Buffer object

template <class T>
void Assign( Buffer& srcBuf, const T& objAssign, size_t nSize )
{
    srcBuf.assign( reinterpret_cast< const unsigned char* >( &objAssign ), nSize );
}

template <class T>
void Assign( Buffer& srcBuf, const T& objAssign )
{
    Assign( srcBuf, objAssign, sizeof(objAssign) );
}

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

///////////////////////////////////////////////////////////////////////////////
//
// Get a non-const pointer to buffer data

unsigned char* GetData( Buffer& srcBuf );

} // end namespace SecureSock

#endif // SECURE_SOCK_BUFFER_H

///////////////////////////////////////////////////////////////////////////////
