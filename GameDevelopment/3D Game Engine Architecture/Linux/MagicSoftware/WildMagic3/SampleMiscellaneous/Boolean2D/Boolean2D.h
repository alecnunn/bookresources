// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BOOLEAN2D_H
#define BOOLEAN2D_H

#include "Wm3WindowApplication2.h"
#include "Polygon2.h"

class Boolean2D : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    Boolean2D ();

    virtual bool OnInitialize ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    Polygon2* ConstructInvertedEll ();
    Polygon2* ConstructPentagon ();
    Polygon2* ConstructSquare ();
    Polygon2* ConstructSShape ();
    Polygon2* ConstructPolyWithHoles ();

    void DoBoolean ();
    void DrawPolySolid (Polygon2& rkP, Color kColor);
    void DrawRegions (const Polygon2& rkP);

    Polygon2 m_kIntersection, m_kUnion, m_kDiff01, m_kDiff10, m_kXor;
    Polygon2* m_pkPoly0;
    Polygon2* m_pkPoly1;
    Polygon2* m_pkActive;
    int m_iChoice;
    bool m_bPointPolygonTest;
};

WM3_REGISTER_INITIALIZE(Boolean2D);

#endif



