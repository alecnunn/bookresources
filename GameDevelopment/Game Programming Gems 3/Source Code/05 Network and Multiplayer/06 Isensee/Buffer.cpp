///////////////////////////////////////////////////////////////////////////////
//
//  Buffer.cpp
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

#include "Buffer.h"

namespace SecureSock
{

///////////////////////////////////////////////////////////////////////////////
//
// Get a non-const pointer to buffer data. Requires that any std string
// implementation that does reference counting reset the ref count on this
// buffer object to zero, since calling this function implies that we are
// about to change the data.

unsigned char* GetData( Buffer& srcBuf )
{
    // Most refcounting implementations create a separate copy of
    // the string when erase is called, even if the string is not changed.
    // Your mileage my vary...
    srcBuf.erase( srcBuf.length() );

    // Direct access to data
    return const_cast< unsigned char* >( srcBuf.data() );
}

} // end namespace SecureSock

///////////////////////////////////////////////////////////////////////////////
