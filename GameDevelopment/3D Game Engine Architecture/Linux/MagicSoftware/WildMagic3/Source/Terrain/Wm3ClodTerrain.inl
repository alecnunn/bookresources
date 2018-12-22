// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
inline int ClodTerrain::GetRowQuantity () const
{
    return m_iRows;
}
//----------------------------------------------------------------------------
inline int ClodTerrain::GetColQuantity () const
{
    return m_iCols;
}
//----------------------------------------------------------------------------
inline unsigned short ClodTerrain::GetSize () const
{
    return m_usSize;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetMinElevation () const
{
    return m_fMinElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetMaxElevation () const
{
    return m_fMaxElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetSpacing () const
{
    return m_fSpacing;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetPixelTolerance () const
{
    return m_fPixelTolerance;
}
//----------------------------------------------------------------------------
inline void ClodTerrain::SetRenderer (Renderer* pkRenderer)
{
    m_pkRenderer = pkRenderer;
}
//----------------------------------------------------------------------------
inline bool& ClodTerrain::CloseAssumption ()
{
    return m_bCloseAssumption;
}
//----------------------------------------------------------------------------
inline float& ClodTerrain::UVBias ()
{
    return m_fUVBias;
}
//----------------------------------------------------------------------------
inline ColorRGBA& ClodTerrain::BorderColor ()
{
    return m_kBorderColor;
}
//----------------------------------------------------------------------------
