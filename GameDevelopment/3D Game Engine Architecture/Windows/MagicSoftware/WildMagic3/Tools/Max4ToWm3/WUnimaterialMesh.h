// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WUNIMATERIALMESH_H
#define WUNIMATERIALMESH_H

#include "Wm3Vector3.h"
#include "Wm3Vector2.h"
#include "Wm3ColorRGB.h"
#include "Wm3MaterialState.h"
#include "Wm3Texture.h"
#include "Wm3TriMesh.h"

class WUnimaterialMesh
{
public:
    WUnimaterialMesh ();
    int& VQuantity ();
    Wm3::Vector3f*& Vertex ();
    Wm3::Vector3f*& Normal ();
    int& CQuantity ();
    Wm3::ColorRGB*& Color ();
    int& TQuantity ();
    Wm3::Vector2f*& Texture ();
    int& FQuantity ();
    int*& Face ();
    int*& CFace ();
    int*& TFace ();
    Wm3::MaterialStatePtr& MState();
    Wm3::TexturePtr& TState();

    // A Max Vertex can have multiple UVW-coordinates.  Wild Magic supports
    // only one UV-coordinate (W is ignored), so any Max Vertex with N
    // UVW-coordinates must be duplicated into N Wild Magic vertices, each
    // having a single UV-coordinate.
    void DuplicateGeometry ();
    Wm3::TriMesh* ToTriMesh ();

    class VertexAttr
    {
    public:
        VertexAttr ();
        bool operator== (const VertexAttr& rkAttr) const;
        bool operator< (const VertexAttr& rkAttr) const;
        int V, C, T;
    };

private:
    int m_iVQuantity;
    Wm3::Vector3f* m_akVertex;
    Wm3::Vector3f* m_akNormal;

    int m_iCQuantity;
    Wm3::ColorRGB* m_akColor;

    int m_iTQuantity;
    Wm3::Vector2f* m_akTexture;

    int m_iFQuantity;
    int* m_aiFace;
    int* m_aiCFace;
    int* m_aiTFace;

    Wm3::MaterialStatePtr m_spkMState;
    Wm3::TexturePtr m_spkTState;
};

#endif

