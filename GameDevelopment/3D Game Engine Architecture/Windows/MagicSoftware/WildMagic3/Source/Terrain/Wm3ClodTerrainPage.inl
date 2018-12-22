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
inline unsigned short ClodTerrainPage::GetSize () const
{
    return m_usSize;
}
//----------------------------------------------------------------------------
inline const unsigned short* ClodTerrainPage::GetHeights () const
{
    return m_ausHeight;
}
//----------------------------------------------------------------------------
inline const Vector2f& ClodTerrainPage::GetOrigin () const
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetMinElevation () const
{
    return m_fMinElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetMaxElevation () const
{
    return m_fMaxElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetSpacing () const
{
    return m_fSpacing;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetPixelTolerance () const
{
    return m_fPixelTolerance;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetX (unsigned char ucX) const
{
    return m_kOrigin.X() + m_fSpacing*float(ucX);
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetY (unsigned char ucY) const
{
    return m_kOrigin.Y() + m_fSpacing*float(ucY);
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetHeight (unsigned short usIndex) const
{
    return m_fMinElevation + m_fMultiplier*float(m_ausHeight[usIndex]);
}
//----------------------------------------------------------------------------
inline Vector2fArrayPtr ClodTerrainPage::GetUVs () const
{
    return m_spkUVs;
}
//----------------------------------------------------------------------------
inline float& ClodTerrainPage::UVBias ()
{
    return m_fUVBias;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetTextureCoordinate (unsigned char ucIndex)
    const
{
    return m_fUVBias + m_fTextureSpacing*(1.0f-2.0f*m_fUVBias)*ucIndex;
}
//----------------------------------------------------------------------------
