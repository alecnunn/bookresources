// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SCREENPOLYGON_H
#define WM3SCREENPOLYGON_H

#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM ScreenPolygon : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  The polygon must be convex and vertices
    // stored in counterclockwise order.  ScreenPolygon assumes responsibility
    // for deleting the input array.  Consequently, the array cannot be a
    // shared object.
    ScreenPolygon (int iVQuantity, Vector2f* akVertex, bool bForeground);
    virtual ~ScreenPolygon ();

    void SetVertex (int i, const Vector2f& rkV);
    const Vector2f& GetVertex (int i) const;
    bool IsForeground () const;

protected:
    ScreenPolygon ();

    Vector2f* m_akVertex;
    bool m_bForeground;
};

WM3_REGISTER_STREAM(ScreenPolygon);
typedef Pointer<ScreenPolygon> ScreenPolygonPtr;
#include "Wm3ScreenPolygon.inl"

}

#endif
