// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CLODTERRAIN_H
#define WM3CLODTERRAIN_H

#include "Wm3Node.h"
#include "Wm3ClodTerrainPage.h"

namespace Wm3
{

class WM3_ITEM ClodTerrain : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    ClodTerrain (const char* acHeightPrefix, const char* acImagePrefix,
        Renderer* pkRenderer, float fUVBias = 0.0f,
        ColorRGBA* pkBorderColor = 0);

    virtual ~ClodTerrain ();

    // member access
    int GetRowQuantity () const;
    int GetColQuantity () const;
    unsigned short GetSize () const;
    float GetMinElevation () const;
    float GetMaxElevation () const;
    float GetSpacing () const;
    void SetPixelTolerance (float fTolerance);
    float GetPixelTolerance () const;
    void SetRenderer (Renderer* pkRenderer);
    bool& CloseAssumption ();
    float& UVBias ();
    ColorRGBA& BorderColor ();

    // page management
    ClodTerrainPage* GetPage (int iRow, int iCol);
    ClodTerrainPage* GetCurrentPage (float fX, float fY) const;
    float GetHeight (float fX, float fY) const;

    // Allow a page to be replaced.  The code unstitches the old page, loads
    // the new page and stitches it, then returns the old page in case the
    // application wants to cache it for quick reuse.
    ClodTerrainPagePtr ReplacePage (int iRow, int iCol,
        const char* acHeightPrefix, const char* acImagePrefix,
        const char* acHeightName, const char* acImageName);

    ClodTerrainPagePtr ReplacePage (int iRow, int iCol,
        ClodTerrainPage* pkNewPage);

    void Simplify ();

protected:
    ClodTerrain ();

    void LoadHeader (const char* acHeightPrefix);
    void LoadData (const char* acHeightPrefix, const char* acImagePrefix,
        const char* acHeightName, const char* acImageName,
        unsigned short*& rausHeight, Image*& rpkImage);
    void LoadPage (int iRow, int iCol, const char* acHeightPrefix,
        const char* acImagePrefix, const char* acHeightName,
        const char* acImageName);
    void StitchAll ();

    // page information
    int m_iRows, m_iCols;
    unsigned short m_usSize;
    float m_fMinElevation, m_fMaxElevation, m_fSpacing;
    ClodTerrainPagePtr** m_aaspkPage;

    // simplification
    float m_fPixelTolerance;
    Renderer* m_pkRenderer;
    bool m_bCloseAssumption;

    // current page containing the camera
    int m_iCameraRow, m_iCameraCol;

    // texture parameters
    float m_fUVBias;
    ColorRGBA m_kBorderColor;
};

WM3_REGISTER_STREAM(ClodTerrain);
typedef Pointer<ClodTerrain> ClodTerrainPtr;
#include "Wm3ClodTerrain.inl"

}

#endif
