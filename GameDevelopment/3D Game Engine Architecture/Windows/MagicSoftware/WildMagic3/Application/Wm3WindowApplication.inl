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
inline const char* WindowApplication::GetWindowTitle () const
{
    return m_acWindowTitle;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetXPosition () const
{
    return m_iXPosition;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetYPosition () const
{
    return m_iYPosition;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetWidth () const
{
    return m_iWidth;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetHeight () const
{
    return m_iHeight;
}
//----------------------------------------------------------------------------
inline void WindowApplication::SetRenderer (Renderer* pkRenderer)
{
    m_pkRenderer = pkRenderer;
}
//----------------------------------------------------------------------------
inline void WindowApplication::SetWindowID (int iWindowID)
{
    m_iWindowID = iWindowID;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetWindowID () const
{
    return m_iWindowID;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnPrecreate ()
{
    // stub for derived classes
    return true;
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnDisplay ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnIdle ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnKeyUp (unsigned char, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnSpecialKeyDown (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnSpecialKeyUp (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMouseClick (int, int, int, int, unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMotion (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnPassiveMotion (int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
