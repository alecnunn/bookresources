// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3POLYPOINT_H
#define WM3POLYPOINT_H

#include "Wm3Geometry.h"
#include "Wm3IntArray.h"

namespace Wm3
{

class WM3_ITEM Polypoint : public Geometry
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction and destruction
    Polypoint (Vector3fArrayPtr spkVertices);
    virtual ~Polypoint ();

    // member access
    void SetActiveQuantity (int iActiveQuantity);
    int GetActiveQuantity () const;

protected:
    Polypoint ();

    // Allow application to specify fewer than the maximum number of vertices
    // to draw.
    int m_iActiveQuantity;
};

WM3_REGISTER_STREAM(Polypoint);
typedef Pointer<Polypoint> PolypointPtr;
#include "Wm3Polypoint.inl"

}

#endif
