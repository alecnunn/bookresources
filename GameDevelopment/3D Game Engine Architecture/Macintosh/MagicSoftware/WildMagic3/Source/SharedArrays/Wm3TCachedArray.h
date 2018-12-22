// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TCACHEDARRAY_H
#define WM3TCACHEDARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3BindInfo.h"

namespace Wm3
{

class Renderer;

template <class T>
class TCachedArray : public TSharedArray<T>
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    TCachedArray (int iQuantity = 0, T* atArray = 0);
    virtual ~TCachedArray ();

private:
    // prevent these from being called
    TCachedArray (const TCachedArray& rkCArray);
    TCachedArray& operator= (const TCachedArray& rkCArray);

// internal use
public:
    // store renderer-specific information for binding/unbinding arrays
    BindInfoArray BIArray;
};

#include "Wm3TCachedArray.inl"

}

#endif
