// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef POLYHEDRONDISTANCE_H
#define POLYHEDRONDISTANCE_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class PolyhedronDistance : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    PolyhedronDistance ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    TriMesh* CreateTetra (float fSize, bool bBlack);
    Polyline* CreateLine ();
    TriMesh* CreatePlane ();
    void UpdateLine ();
    void InitialState ();
    bool Transform (unsigned char ucKey);

    // representation of bodies
    Tuple<3>* m_akFace;
    TriMesh* m_apkTetra[4];
    Vector3f* m_apkVertex[2];
    Vector3f m_akTV[2];
    Matrix3f m_akTM[2];

    // display variables
    float m_fSeparation;
    float m_fEdgeLength;
    float m_fSmall;

    // Offsets during calculation with LCPPolyDist to ensure that all of the
    // vertices are in the first octant.
    float m_fOffset;
 
    // the scene graph
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    PolylinePtr m_aspkLine[2];
};

WM3_REGISTER_INITIALIZE(PolyhedronDistance);

#endif
