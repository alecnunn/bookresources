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
inline int RevolutionSurface::GetCurveSamples () const
{
    return m_iCurveSamples;
}
//----------------------------------------------------------------------------
inline int RevolutionSurface::GetRadialSamples () const
{
    return m_iRadialSamples;
}
//----------------------------------------------------------------------------
inline void RevolutionSurface::SetCurve (Curve2f* pkCurve)
{
    m_pkCurve = pkCurve;
}
//----------------------------------------------------------------------------
inline const Curve2f* RevolutionSurface::GetCurve () const
{
    return m_pkCurve;
}
//----------------------------------------------------------------------------
inline int RevolutionSurface::GetTopology () const
{
    return m_eTopology;
}
//----------------------------------------------------------------------------
inline bool& RevolutionSurface::SampleByArcLength ()
{
    return m_bSampleByArcLength;
}
//----------------------------------------------------------------------------
inline bool RevolutionSurface::GetSampleByArcLength () const
{
    return m_bSampleByArcLength;
}
//----------------------------------------------------------------------------
