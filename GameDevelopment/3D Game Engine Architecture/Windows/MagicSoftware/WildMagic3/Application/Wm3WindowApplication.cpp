// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3WindowApplication.h"
using namespace Wm3;

//----------------------------------------------------------------------------
WindowApplication::WindowApplication (const char* acWindowTitle,
    int iXPosition, int iYPosition, int iWidth, int iHeight,
    const ColorRGB& rkBackgroundColor)
    :
    m_acWindowTitle(acWindowTitle),
    m_iXPosition(iXPosition),
    m_iYPosition(iYPosition),
    m_iWidth(iWidth),
    m_iHeight(iHeight),
    m_kBackgroundColor(rkBackgroundColor)
{
    m_iWindowID = 0;
    m_pkRenderer = 0;
}
//----------------------------------------------------------------------------
WindowApplication::~WindowApplication ()
{
}
//----------------------------------------------------------------------------
int WindowApplication::Run (int iQuantity, char** apcArgument)
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;
    return pkTheApp->Main(iQuantity,apcArgument);
}
//----------------------------------------------------------------------------
bool WindowApplication::OnInitialize ()
{
    m_pkRenderer->SetBackgroundColor(m_kBackgroundColor);
    return true;
}
//----------------------------------------------------------------------------
void WindowApplication::OnTerminate ()
{
    delete m_pkRenderer;
    m_pkRenderer = 0;
}
//----------------------------------------------------------------------------
void WindowApplication::OnMove (int iX, int iY)
{
    m_iXPosition = iX;
    m_iYPosition = iY;
}
//----------------------------------------------------------------------------
void WindowApplication::OnResize (int iWidth, int iHeight)
{
    if ( iWidth > 0 && iHeight > 0 )
    {
        if ( m_pkRenderer )
            m_pkRenderer->Resize(iWidth,iHeight);

        m_iWidth = iWidth;
        m_iHeight = iHeight;
    }
}
//----------------------------------------------------------------------------
bool WindowApplication::OnKeyDown (unsigned char ucKey, int, int)
{
    // quit application if the ESCAPE key is pressed
    if ( ucKey == KEY_ESCAPE )
    {
        RequestTermination();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
