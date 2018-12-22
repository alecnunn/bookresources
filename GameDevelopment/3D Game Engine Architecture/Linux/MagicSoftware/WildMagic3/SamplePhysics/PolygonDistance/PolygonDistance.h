// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef POLYGONDISTANCE_H
#define POLYGONDISTANCE_H

#include "Wm3WindowApplication2.h"
using namespace Wm3;

class PolygonDistance : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    PolygonDistance ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void InitialConfiguration ();
    void NextConfiguration ();
    void PolarRepresentation (int iNum, const Vector2f* akVertices,
        Vector2f& rkCentroid, Vector2f* akPolar);
    void CartesianRepresentation (int iNum, Vector2f* akVertices,
        const Vector2f& rkCentroid, const Vector2f* akPolar);
    void PolyRotate (int iNum, int iSign, Vector2f* akPolar);
    void DoEdgeNorm (int iNum, const Vector2f* akVertices,
        const Vector2f& rkClosest, Vector2f* akEnd);
    void DrawLineSegment (int iThick, Color kColor, const Vector2f& rkEnd1,
        const Vector2f& rkEnd2);
    void DrawPerps (const Vector2f* akEP);
    void DrawPoints (int iThick, Color kColor, const Vector2f& rkPoint);

    enum { PD_NUM_POLYS = 3 };

    class Polygon
    {
    public:
        int m_iNumVertices;
        int m_iSign;
        Vector2f* m_akVertices;
        Vector2f m_akCentroid;
        Vector2f* m_akPolar;
        Tuple2* m_akFaces;
    };

    Polygon* m_kPolygon;
    bool m_bDoPerps;
};

WM3_REGISTER_INITIALIZE(PolygonDistance);

#endif


