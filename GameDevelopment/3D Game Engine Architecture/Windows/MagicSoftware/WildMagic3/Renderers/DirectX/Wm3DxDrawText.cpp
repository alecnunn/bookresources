// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DxRenderer.h"
using namespace Wm3;

// Enable this #define if you have the DX9.0a SDK installed.  Disable the
// #define if you have the DX9.0b or DX9.0c SDK installed.  The function
// prototypes changed for D3DXCreateFont and the DrawText member of the
// ID3DXFont interface.
//
// #define WM3_USE_DX90A

//----------------------------------------------------------------------------
int DxRenderer::LoadFont (const char* acFace, int iSize, bool bBold,
    bool bItalic)
{
    // Find the first unused font location
    int iLoc;
    for (iLoc = 0; iLoc < m_kFontArray.GetQuantity(); iLoc++)
    {
        if ( !m_kFontArray[iLoc] )
            break;
    }

    IFontPtr pqFont;
    DWORD dwWeight = ( bBold ? FW_BOLD : FW_REGULAR );

#ifdef WM3_USE_DX90A
    HFONT hFont = CreateFont(
        iSize,                          // height of font
        0,                              // average character width
        0,                              // angle of escapement
        0,                              // base-line orientation angle
        dwWeight,                       // font weight
        (DWORD)bItalic,                 // italic attribute option
        FALSE,                          // underline attribute option
        FALSE,                          // strikeout attribute option
        DEFAULT_CHARSET,                // character set identifier
        OUT_DEFAULT_PRECIS,             // output precision
        CLIP_DEFAULT_PRECIS,            // clipping precision
        ANTIALIASED_QUALITY,            // output quality
        VARIABLE_PITCH,                 // pitch and family
        acFace);                        // type-face name

    ms_hResult = D3DXCreateFont(m_pqDevice,hFont,&pqFont);
#else
    ms_hResult = D3DXCreateFont(
        m_pqDevice,                     // pDevice
        iSize,                          // Height
        0,                              // Width
        dwWeight,                       // Weight
        0,                              // MipLevels
        (DWORD)bItalic,                 // Italic
        DEFAULT_CHARSET,                // CharSet
        OUT_DEFAULT_PRECIS,             // OutputPrecision
        ANTIALIASED_QUALITY,            // Quality
        VARIABLE_PITCH,                 // PitchAndFamily
        acFace,                         // pFaceName
        &pqFont);                       // ppFont
#endif

    assert( SUCCEEDED(ms_hResult) );

    // Put into the font array
    if ( iLoc == m_kFontArray.GetQuantity() )
        m_kFontArray.Append(pqFont);
    else
        m_kFontArray[iLoc] = pqFont;

    return iLoc;
}
//----------------------------------------------------------------------------
void DxRenderer::UnloadFont (int iFontID)
{
    // You may not unload the default font (id = 0).
    if ( 1 <= iFontID && iFontID < m_kFontArray.GetQuantity() )
    {
        m_kFontArray[iFontID].Release();
    }
}
//----------------------------------------------------------------------------
bool DxRenderer::SelectFont (int iFontID)
{
    if ( 0 <= iFontID && iFontID < m_kFontArray.GetQuantity() )
    {
        if ( m_kFontArray[iFontID] )
        {
            m_iFontID = iFontID;
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void DxRenderer::Draw (int iX, int iY, const ColorRGBA& rkColor,
    const char* acText)
{
    assert( acText );
    if ( !acText )
        return;

    RECT kText;
    kText.bottom = iY;
    kText.top = iY;
    kText.left = iX;
    kText.right = iX;

    D3DCOLOR kColor = D3DCOLOR_COLORVALUE(rkColor.R(),rkColor.G(),rkColor.B(),
        rkColor.A());

#ifdef WM3_USE_DX90A
    m_kFontArray[m_iFontID]->Begin();
    ms_hResult = m_kFontArray[m_iFontID]->DrawText(
        acText,                             // pString
        -1,                                 // Count
        &kText,                             // pRect
        DT_LEFT | DT_BOTTOM | DT_CALCRECT,  // Format
        kColor);                            // Color
    assert( SUCCEEDED(ms_hResult) );
#else
    ms_hResult = m_kFontArray[m_iFontID]->DrawText(
        0,                                  // pSprite
        acText,                             // pString
        -1,                                 // Count
        &kText,                             // pRect
        DT_LEFT | DT_BOTTOM | DT_CALCRECT,  // Format
        kColor);                            // Color
    assert( SUCCEEDED(ms_hResult) );
#endif

#ifdef WM3_USE_DX90A
    ms_hResult = m_kFontArray[m_iFontID]->DrawText(
        acText,
        -1,
        &kText,
        DT_LEFT | DT_BOTTOM,
        kColor);
    assert( SUCCEEDED(ms_hResult) );
    m_kFontArray[m_iFontID]->End();
#else
    ms_hResult = m_kFontArray[m_iFontID]->DrawText(
        0,
        acText,
        -1,
        &kText,
        DT_LEFT | DT_BOTTOM,
        kColor);
    assert( SUCCEEDED(ms_hResult) );
#endif
}
//----------------------------------------------------------------------------
