// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TERRAIN_H
#define WM3TERRAIN_H

#include "Wm3Node.h"
#include "Wm3TerrainPage.h"

namespace Wm3
{

class Camera;

class WM3_ITEM Terrain : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    Terrain (const char* acHeightPrefix, const char* acImagePrefix,
        Camera* pkCamera, float fUVBias = 0.0f,
        ColorRGBA* pkBorderColor = 0);

    virtual ~Terrain ();

    // member access
    int GetRowQuantity () const;
    int GetColQuantity () const;
    unsigned short GetSize () const;
    float GetMinElevation () const;
    float GetMaxElevation () const;
    float GetSpacing () const;
    float& UVBias ();
    ColorRGBA& BorderColor ();
    void SetCamera (Camera* pkCamera);

    // page management
    TerrainPage* GetPage (int iRow, int iCol);
    TerrainPage* GetCurrentPage (float fX, float fY) const;
    float GetHeight (float fX, float fY) const;

    // Allow a page to be replaced.  The code unstitches the old page, loads
    // the new page and stitches it, then returns the old page in case the
    // application wants to cache it for quick reuse.
    TerrainPagePtr ReplacePage (int iRow, int iCol,
        const char* acHeightPrefix, const char* acImagePrefix,
        const char* acHeightName, const char* acImageName);

    TerrainPagePtr ReplacePage (int iRow, int iCol, TerrainPage* pkNewPage);

    // update of active set of terrain pages
    void OnCameraMotion ();

protected:
    // streaming support
    Terrain ();

    void LoadHeader (const char* acHeightPrefix);
    void LoadData (const char* acHeightPrefix, const char* acImagePrefix,
        const char* acHeightName, const char* acImageName,
        unsigned short*& rausHeight, Image*& rpkImage);
    void LoadPage (int iRow, int iCol, const char* acHeightPrefix,
        const char* acImagePrefix, const char* acHeightName,
        const char* acImageName);

    // page information
    int m_iRows, m_iCols;
    unsigned short m_usSize;
    float m_fMinElevation, m_fMaxElevation, m_fSpacing;
    TerrainPagePtr** m_aaspkPage;

    // current page containing the camera
    int m_iCameraRow, m_iCameraCol;
    Pointer<Camera> m_spkCamera;

    // texture parameters
    float m_fUVBias;
    ColorRGBA m_kBorderColor;
};

WM3_REGISTER_STREAM(Terrain);
typedef Pointer<Terrain> TerrainPtr;
#include "Wm3Terrain.inl"

}

#endif
