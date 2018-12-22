// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#ifndef VERTEX_H
#define VERTEX_H

class Vertex
{
public:
    Vertex ();
    Vertex (float fPx, float fPy, float fPz, float fNx, float fNy,
        float fNz, float fR, float fG, float fB, float fA, float fU0,
        float fV0, float fU1, float fV1, float fU2, float fV2, float fU3,
        float fV3);

    Vertex (const Vertex& rkV);
    Vertex& operator= (const Vertex& rkV);

    float& operator[] (int i) { return m_afValue[i]; }
    float operator[] (int i) const { return m_afValue[i]; }
    bool operator< (const Vertex& rkV) const;
    bool operator== (const Vertex& rkV) const;

    enum { MAXVALUES = 18 };

private:
    // Packed array of position (Vector3), normal (Vector3),
    // color (ColorRGB), alpha (float), uv0 (Vector2), uv1 (Vector2),
    // uv2 (Vector2), uv3 (Vector2).  The total number of float values is 18.
    float m_afValue[MAXVALUES];
};

#endif

