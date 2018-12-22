// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TRIANGLEKEY_H
#define WM3TRIANGLEKEY_H

#include "Wm3System.h"

namespace Wm3
{

class WM3_ITEM TriangleKey
{
public:
    TriangleKey (int iV0 = -1, int iV1 = -1, int iV2 = -1);
    bool operator< (const TriangleKey& rkKey) const;
    int V[3];
};

#include "Wm3TriangleKey.inl"

}

#endif
