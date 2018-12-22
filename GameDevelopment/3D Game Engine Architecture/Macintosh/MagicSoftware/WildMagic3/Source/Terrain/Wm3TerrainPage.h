// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TERRAINPAGE_H
#define WM3TERRAINPAGE_H

#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM TerrainPage : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // size = 2^p + 1, p <= 7 (size = 3, 5, 9, 17, 33, 65, 129)
    TerrainPage (int iSize, unsigned short* ausHeight,
        const Vector2f& rkOrigin, float fMinElevation, float fMaxElevation,
        float fSpacing, float fUVBias = 0.0f);

    virtual ~TerrainPage ();

    // height field access
    int GetSize () const;
    const unsigned short* GetHeights () const;
    const Vector2f& GetOrigin () const;
    float GetMinElevation () const;
    float GetMaxElevation () const;
    float GetSpacing () const;

    // Height field measurements.  If the location is not in the page, the
    // return value is INFINITY.
    float GetHeight (float fX, float fY) const;

    // texture coordinates for the page
    Vector2fArrayPtr GetUVs () const;
    float& UVBias ();

protected:
    // streaming support
    TerrainPage ();
    void InitializeDerivedData ();

    // tessellation
    float GetX (int iX) const;
    float GetY (int iY) const;
    float GetHeight (int iIndex) const;
    float GetTextureCoordinate (int iIndex) const;

    // height field
    int m_iSize, m_iSizeM1;
    unsigned short* m_ausHeight;
    Vector2f m_kOrigin;
    float m_fMinElevation, m_fMaxElevation, m_fSpacing;
    float m_fInvSpacing, m_fTextureSpacing, m_fMultiplier;

    // (2^p+1) by (2^p_1) array of texture coordinates, row-major order
    Vector2fArrayPtr m_spkUVs;
    float m_fUVBias;
};

WM3_REGISTER_STREAM(TerrainPage);
typedef Pointer<TerrainPage> TerrainPagePtr;
#include "Wm3TerrainPage.inl"

}

#endif
