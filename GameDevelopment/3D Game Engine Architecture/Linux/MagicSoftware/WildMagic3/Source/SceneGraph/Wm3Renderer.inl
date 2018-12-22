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
inline Camera* Renderer::GetCamera () const
{
    return m_pkCamera;
}
//----------------------------------------------------------------------------
inline int Renderer::GetWidth () const
{
    return m_iWidth;
}
//----------------------------------------------------------------------------
inline int Renderer::GetHeight () const
{
    return m_iHeight;
}
//----------------------------------------------------------------------------
inline void Renderer::ToggleFullscreen ()
{
    m_bFullscreen = !m_bFullscreen;
}
//----------------------------------------------------------------------------
inline void Renderer::SetBackgroundColor (const ColorRGB& rkColor)
{
    m_kBackgroundColor = rkColor;
}
//----------------------------------------------------------------------------
inline const ColorRGB& Renderer::GetBackgroundColor () const
{
    return m_kBackgroundColor;
}
//----------------------------------------------------------------------------
inline bool Renderer::BeginScene ()
{
    // stub for derived classes
    return true;
}
//----------------------------------------------------------------------------
inline void Renderer::EndScene ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline int Renderer::GetMaxLights () const
{
    return m_iMaxLights;
}
//----------------------------------------------------------------------------
inline int Renderer::GetMaxTextures () const
{
    return m_iMaxTextures;
}
//----------------------------------------------------------------------------
inline int Renderer::GetMaxStencilIndices () const
{
    return m_iMaxStencilIndices;
}
//----------------------------------------------------------------------------
