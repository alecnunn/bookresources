// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef MAPTEXTURETOQUAD_H
#define MAPTEXTURETOQUAD_H

#include "Wm3WindowApplication2.h"
using namespace Wm3;

// If this is defined, use a HmQuadToSqrf mapping.  If this is not defined,
// use a BiQuadToSqrf mapping.
#define USE_HM_QUAD_TO_SQR

class MapTextureToQuad : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    MapTextureToQuad ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();

    // allows user to drag vertices of convex quadrilateral
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY);

protected:
    // the image to perspectively draw onto the convex quadrilateral.
    Image* m_pkImage;

    // The four vertices of the convex quadrilateral in counterclockwise
    // order:  Q00 = V[0], Q10 = V[1], Q11 = V[2], Q01 = V[3].
    void CreateMapping ();
    Vector2f m_akVertex[4];
#ifdef USE_HM_QUAD_TO_SQR
    HmQuadToSqrf* m_pkMap;
#else
    BiQuadToSqrf* m_pkMap;
#endif

    // for dragging the quadrilateral vertices
    void SelectVertex (const Vector2f& rkPos);
    void UpdateQuadrilateral (const Vector2f& rkPos);
    bool m_bMouseDown;
    int m_iSelected;
};

WM3_REGISTER_INITIALIZE(MapTextureToQuad);

#endif
