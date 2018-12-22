// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CLODTERRAINPAGE_H
#define WM3CLODTERRAINPAGE_H

#include "Wm3TriMesh.h"

namespace Wm3
{

class Camera;
class ClodTerrainVertex;

class WM3_ITEM ClodTerrainPage : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // size = 2^p + 1, p <= 7 (size = 3, 5, 9, 17, 33, 65, 129)
    ClodTerrainPage (unsigned short usSize, unsigned short* ausHeight,
        const Vector2f& rkOrigin, float fMinElevation, float fMaxElevation,
        float fSpacing, float fUVBias = 0.0f);

    virtual ~ClodTerrainPage ();

    // height field access
    unsigned short GetSize () const;
    const unsigned short* GetHeights () const;
    const Vector2f& GetOrigin () const;
    float GetMinElevation () const;
    float GetMaxElevation () const;
    float GetSpacing () const;

    // pixel tolerance on projected vertex height
    void SetPixelTolerance (const Renderer* pkRenderer, float fTolerance);
    float GetPixelTolerance () const;

    // Height field measurements.  If the location is not in the page, the
    // return value is INFINITY.
    float GetHeight (float fX, float fY) const;

    // texture coordinates for the page
    Vector2fArrayPtr GetUVs () const;
    float& UVBias ();

protected:
    friend class ClodTerrainBlock;

    // streaming support
    ClodTerrainPage ();
    void InitializeDerivedData ();

    // queue handlers
    void AddToQueue (unsigned short usBlock);
    unsigned short RemoveFromQueue ();
    unsigned short ReadFromQueue (unsigned short usIndex);

    // page simplification
    bool IntersectFrustum (const Camera* pkCamera);

    // block simplification
    void SimplifyBlocks (const Camera* pCamera, const Vector3f& rkModelEye,
        const Vector3f& rkModelDir, bool bCloseAssumption);

    // vertex simplification
    void SimplifyVertices (const Vector3f& rkModelEye,
        const Vector3f& rkModelDir, bool bCloseTerrainAssumption);

    // simplification
    friend class ClodTerrain;
    void ResetBlocks ();
    void Simplify (const Renderer* pkRenderer, const Vector3f& rkModelEye,
        const Vector3f& rkModelDir, bool bCloseAssumption);

    // tessellation
    float GetX (unsigned char ucX) const;
    float GetY (unsigned char ucY) const;
    float GetHeight (unsigned short usIndex) const;
    float GetTextureCoordinate (unsigned char ucIndex) const;
    void Render (ClodTerrainBlock& rkBlock);
    void RenderTriangle (unsigned short usT, unsigned short usL,
        unsigned short usR);
    void RenderBlocks ();

    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    // height field
    unsigned short m_usSize, m_usSizeM1;
    unsigned short* m_ausHeight;
    Vector2f m_kOrigin;
    float m_fMinElevation, m_fMaxElevation, m_fSpacing;
    float m_fInvSpacing, m_fTextureSpacing, m_fMultiplier;

    // texture parameters
    float m_fUVBias;

    // simplification
    float m_fPixelTolerance, m_fWorldTolerance;
    bool m_bNeedsTessellation;
    unsigned short* m_ausLookup;
    int m_iConnectLength;

    // (2^p+1) by (2^p+1) array of vertices, row-major order
    ClodTerrainVertex* m_akTVertex;

    // (2^p+1) by (2^p_1) array of texture coordinates, row-major order
    Vector2fArrayPtr m_spkUVs;

    // maximum quantities
    int m_iTotalVQuantity, m_iTotalTQuantity, m_iTotalIQuantity;

    // quadtree of blocks
    unsigned short m_usBlockQuantity;
    ClodTerrainBlock* m_akBlock;

    // circular queue of indices for active blocks
    unsigned short* m_ausQueue;
    unsigned short m_usQueueQuantity;
    unsigned short m_usFront, m_usRear;
    unsigned short m_usNumUnprocessed;
    unsigned short m_usItemsInQueue;

// for internal use (by Terrain)
public:
    // The height fields are functions z = h(x,y) with the xy-plane using
    // right-handed coordinates.  The column index increases with x and the
    // row index increases with y.  For example, looking down the positive
    // z-axis, a 2-by-2 grid of pages is labeled as
    //
    // y
    // ^
    // | col 0   col 1
    // +--------+-------+
    // |   10   |  11   | row 1
    // +--------+-------+
    // |   00   |  01   | row 0
    // +--------+-------+--> x

    // stitch/unstitch (r,c) and (r,c+1)
    void StitchNextCol (ClodTerrainPage* pkNextCol);
    void UnstitchNextCol (ClodTerrainPage* pkNextCol);

    // stitch/unstitch (r,c) and (r+1,c)
    void StitchNextRow (ClodTerrainPage* pkNextRow);
    void UnstitchNextRow (ClodTerrainPage* pkNextRow);
};

WM3_REGISTER_STREAM(ClodTerrainPage);
typedef Pointer<ClodTerrainPage> ClodTerrainPagePtr;
#include "Wm3ClodTerrainPage.inl"

}

#endif
