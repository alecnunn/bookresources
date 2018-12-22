// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef SORTEDCUBE_H
#define SORTEDCUBE_H

#include "Wm3Node.h"
#include "Wm3Camera.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class SortedCube : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    SortedCube (Camera* pkCamera, Image* pkXpImage, Image* pkXmImage,
        Image* pkYpImage, Image* pkYmImage, Image* pkZpImage,
        Image* pkZmImage);

    virtual ~SortedCube ();

    void SetCamera (Camera* pkCamera);

protected:
    SortedCube ();

    void SortFaces ();
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    CameraPtr m_spkCamera;

    // 0 = face xp
    // 1 = face xm
    // 2 = face yp
    // 3 = face ym
    // 4 = face zp
    // 5 = face zm
    TriMeshPtr m_aspkFace[6];

    // bit 0 = face xp
    // bit 1 = face xm
    // bit 2 = face yp
    // bit 3 = face ym
    // bit 4 = face zp
    // bit 5 = face zm
    int m_iBackFacing;
};

WM3_REGISTER_STREAM(SortedCube);
typedef Pointer<SortedCube> SortedCubePtr;

}

#endif
