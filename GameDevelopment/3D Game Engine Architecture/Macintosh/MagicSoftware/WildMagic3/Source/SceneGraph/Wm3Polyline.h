// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3POLYLINE_H
#define WM3POLYLINE_H

#include "Wm3Geometry.h"

namespace Wm3
{

class WM3_ITEM Polyline : public Geometry
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction and destruction
    Polyline (Vector3fArrayPtr spkVertices, bool bClosed, bool bContiguous);
    virtual ~Polyline ();

    // member access
    void SetActiveQuantity (int iActiveQuantity);
    int GetActiveQuantity () const;

    void SetClosed (bool bClosed);
    bool GetClosed () const;

    void SetContiguous (bool bContiguous);
    bool GetContiguous () const;

protected:
    Polyline ();
    void SetGeometryType ();

    // Allow application to specify fewer than the maximum number of vertices
    // to draw.
    int m_iActiveQuantity;

    // polyline is open or closed, contiguous or disjoint segments
    bool m_bClosed, m_bContiguous;
};

WM3_REGISTER_STREAM(Polyline);
typedef Pointer<Polyline> PolylinePtr;
#include "Wm3Polyline.inl"

}

#endif
