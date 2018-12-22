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
inline ParametricSurfacef*& RectangleSurface::Surface ()
{
    return m_pkSurface;
}
//----------------------------------------------------------------------------
inline const ParametricSurfacef* RectangleSurface::GetSurface () const
{
    return m_pkSurface;
}
//----------------------------------------------------------------------------
inline int RectangleSurface::GetUSamples () const
{
    return m_iUSamples;
}
//----------------------------------------------------------------------------
inline int RectangleSurface::GetVSamples () const
{
    return m_iVSamples;
}
//----------------------------------------------------------------------------
