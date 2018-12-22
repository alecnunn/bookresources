// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3WindowApplication2.h"
using namespace Wm3;

//----------------------------------------------------------------------------
WindowApplication2::WindowApplication2 (const char* acWindowTitle,
    int iXPosition, int iYPosition, int iWidth, int iHeight,
    const ColorRGB& rkBackgroundColor)
    :
    // rows are required to be multiple of 4 bytes, so iWidth is adjusted
    WindowApplication(acWindowTitle,iXPosition,iYPosition,
        iWidth-(iWidth % 4),iHeight,rkBackgroundColor)
{
    m_iScrWidth = 0;
    m_iScrHeight = 0;
    m_akScreen = 0;
    m_bClampToWindow = true;
    m_akFlipScreen = 0;
}
//----------------------------------------------------------------------------
WindowApplication2::~WindowApplication2 ()
{
}
//----------------------------------------------------------------------------
bool WindowApplication2::OnInitialize ()
{
    if ( !WindowApplication::OnInitialize() )
        return false;

    // the RGB screen pixels
    m_iScrWidth = GetWidth();
    m_iScrHeight = GetHeight();
    m_akScreen = new Color[m_iScrWidth*m_iScrHeight];
    ClearScreen();
    return true;
}
//----------------------------------------------------------------------------
void WindowApplication2::OnTerminate ()
{
    delete[] m_akScreen;
    delete[] m_akFlipScreen;
    WindowApplication::OnTerminate();
}
//----------------------------------------------------------------------------
void WindowApplication2::OnDisplay ()
{
    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        if ( !m_akFlipScreen )
        {
            m_pkRenderer->Draw((const unsigned char*)m_akScreen);
        }
        else
        {
            // flip the screen
            Color* akSPtr = m_akScreen;
            Color* akFPtr = m_akFlipScreen + m_iScrWidth*(m_iScrHeight-1);
            int iQuantity = m_iScrWidth*sizeof(Color);
            for (int i = 0; i < m_iScrHeight; i++)
            {
                memcpy(akFPtr,akSPtr,iQuantity);
                akSPtr += m_iScrWidth;
                akFPtr -= m_iScrWidth;
            }

            m_pkRenderer->Draw((const unsigned char*)m_akFlipScreen);
        }

        // Screen overlays should use m_pkRenderer and not access the
        // m_akScreen array directly.
        ScreenOverlay();

        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();
}
//----------------------------------------------------------------------------
void WindowApplication2::OnResize (int iWidth, int iHeight)
{
    iWidth = iWidth - (iWidth % 4);
    WindowApplication::OnResize(iWidth,iHeight);
    if ( iWidth*iHeight <= 0 )
        return;

    if ( iWidth != m_iScrWidth || iHeight != m_iScrHeight )
    {
        delete[] m_akScreen;
        m_iScrWidth = iWidth;
        m_iScrHeight = iHeight;
        m_akScreen = new Color[m_iScrWidth*m_iScrHeight];
        ClearScreen();

        if ( m_akFlipScreen )
        {
            delete[] m_akFlipScreen;
            m_akFlipScreen = new Color[m_iScrWidth*m_iScrHeight];
        }
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::ScreenOverlay ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
void WindowApplication2::ClearScreen ()
{
    for (int i = 0; i < m_iWidth*m_iHeight; i++)
    {
        // This can lead to slow float-to-int conversions.
        //m_akScreen[i].r = (unsigned char)(255.0f*ms_kBackgroundColor.r);
        //m_akScreen[i].b = (unsigned char)(255.0f*ms_kBackgroundColor.g);
        //m_akScreen[i].b = (unsigned char)(255.0f*ms_kBackgroundColor.b);

        // fast float-to-int conversions
        int iValue;
        WM3_SCALED_FLOAT_TO_INT(m_kBackgroundColor.R(),8,iValue);
        m_akScreen[i].r = (unsigned char)iValue;
        WM3_SCALED_FLOAT_TO_INT(m_kBackgroundColor.G(),8,iValue);
        m_akScreen[i].g = (unsigned char)iValue;
        WM3_SCALED_FLOAT_TO_INT(m_kBackgroundColor.B(),8,iValue);
        m_akScreen[i].b = (unsigned char)iValue;
    }
}
//----------------------------------------------------------------------------
bool& WindowApplication2::ClampToWindow ()
{
    return m_bClampToWindow;
}
//----------------------------------------------------------------------------
void WindowApplication2::SetPixel (int iX, int iY, Color kColor)
{
    if ( m_bClampToWindow )
    {
        if ( 0 <= iX && iX < m_iWidth && 0 <= iY && iY < m_iHeight )
            m_akScreen[Index(iX,iY)] = kColor;
    }
    else
    {
        m_akScreen[Index(iX,iY)] = kColor;
    }
}
//----------------------------------------------------------------------------
WindowApplication2::Color WindowApplication2::GetPixel (int iX, int iY)
{
    if ( m_bClampToWindow )
    {
        if ( 0 <= iX && iX < m_iWidth && 0 <= iY && iY < m_iHeight )
            return m_akScreen[Index(iX,iY)];
        else
            return Color(0,0,0);
    }
    else
    {
        return m_akScreen[Index(iX,iY)];
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DrawLine (int iX0, int iY0, int iX1, int iY1,
    Color kColor)
{
    int iX = iX0, iY = iY0;

    // direction of line
    int iDx = iX1-iX0, iDy = iY1-iY0;

    // increment or decrement depending on direction of line
    int iSx = (iDx > 0 ? 1 : (iDx < 0 ? -1 : 0));
    int iSy = (iDy > 0 ? 1 : (iDy < 0 ? -1 : 0));

    // decision parameters for voxel selection
    if ( iDx < 0 ) iDx = -iDx;
    if ( iDy < 0 ) iDy = -iDy;
    int iAx = 2*iDx, iAy = 2*iDy;
    int iDecX, iDecY;

    // determine largest direction component, single-step related variable
    int iMax = iDx, iVar = 0;
    if ( iDy > iMax ) { iVar = 1; }

    // traverse Bresenham line
    switch ( iVar )
    {
    case 0:  // single-step in x-direction
        iDecY = iAy - iDx;
        for (/**/; /**/; iX += iSx, iDecY += iAy)
        {
            // process pixel
            SetPixel(iX,iY,kColor);

            // take Bresenham step
            if ( iX == iX1 )  break;
            if ( iDecY >= 0 ) { iDecY -= iAx; iY += iSy; }
        }
        break;
    case 1:  // single-step in y-direction
        iDecX = iAx - iDy;
        for (/**/; /**/; iY += iSy, iDecX += iAx)
        {
            // process pixel
            SetPixel(iX,iY,kColor);

            // take Bresenham step
            if ( iY == iY1 ) break;
            if ( iDecX >= 0 ) { iDecX -= iAy; iX += iSx; }
        }
        break;
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DrawRectangle (int iXMin, int iYMin, int iXMax,
    int iYMax, Color kColor, bool bSolid)
{
    if ( iXMin >= m_iWidth || iXMax < 0 || iYMin >= m_iHeight || iYMax < 0 )
    {
        // rectangle not visible
        return;
    }

    // clamp to window border
    if ( iXMin < 0 )
        iXMin = 0;
    if ( iXMax >= m_iWidth )
        iXMax = m_iWidth-1;
    if ( iYMin < 0 )
        iYMin = 0;
    if ( iYMax >= m_iHeight )
        iYMax = m_iHeight-1;

    int iX, iY;

    if ( bSolid )
    {
        for (iY = iYMin; iY <= iYMax; iY++)
        {
            for (iX = iXMin; iX <= iXMax; iX++)
                m_akScreen[Index(iX,iY)] = kColor;
        }
    }
    else
    {
        for (iX = iXMin; iX <= iXMax; iX++)
        {
            m_akScreen[Index(iX,iYMin)] = kColor;
            m_akScreen[Index(iX,iYMax)] = kColor;
        }
        for (iY = iYMin+1; iY <= iYMax-1; iY++)
        {
            m_akScreen[Index(iXMin,iY)] = kColor;
            m_akScreen[Index(iXMax,iY)] = kColor;
        }
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::DrawCircle (int iXCenter, int iYCenter, int iRadius,
    Color kColor, bool bSolid)
{
    int iX, iY, iDec;

    if ( bSolid )
    {
        int iXValue, iYMin, iYMax, i;
        for (iX = 0, iY = iRadius, iDec = 3-2*iRadius; iX <= iY; iX++)
        {
            iXValue = iXCenter + iX;
            iYMin = iYCenter - iY;
            iYMax = iYCenter + iY;
            for (i = iYMin; i <= iYMax; i++)
                m_akScreen[Index(iXValue,i)] = kColor;

            iXValue = iXCenter - iX;
            for (i = iYMin; i <= iYMax; i++)
                m_akScreen[Index(iXValue,i)] = kColor;

            iXValue = iXCenter + iY;
            iYMin = iYCenter - iX;
            iYMax = iYCenter + iX;
            for (i = iYMin; i <= iYMax; i++)
                m_akScreen[Index(iXValue,i)] = kColor;

            iXValue = iXCenter - iY;
            for (i = iYMin; i <= iYMax; i++)
                m_akScreen[Index(iXValue,i)] = kColor;

            if ( iDec >= 0 )
                iDec += -4*(iY--)+4;
            iDec += 4*iX+6;
        }
    }
    else
    {
        for (iX = 0, iY = iRadius, iDec = 3-2*iRadius; iX <= iY; iX++)
        {
            m_akScreen[Index(iXCenter+iX,iYCenter+iY)] = kColor;
            m_akScreen[Index(iXCenter+iX,iYCenter-iY)] = kColor;
            m_akScreen[Index(iXCenter-iX,iYCenter+iY)] = kColor;
            m_akScreen[Index(iXCenter-iX,iYCenter-iY)] = kColor;
            m_akScreen[Index(iXCenter+iY,iYCenter+iX)] = kColor;
            m_akScreen[Index(iXCenter+iY,iYCenter-iX)] = kColor;
            m_akScreen[Index(iXCenter-iY,iYCenter+iX)] = kColor;
            m_akScreen[Index(iXCenter-iY,iYCenter-iX)] = kColor;

            if ( iDec >= 0 )
                iDec += -4*(iY--)+4;
            iDec += 4*iX+6;
        }
    }
}
//----------------------------------------------------------------------------
void WindowApplication2::Fill (int iX, int iY, Color kFColor, Color kBColor)
{
    // Allocate the maximum amount of space needed.  If you prefer less, you
    // need to modify this data structure to allow for dynamic reallocation
    // when it is needed.  An empty stack has iTop == -1.
    int iXMax = m_iWidth, iYMax = m_iHeight;
    int iQuantity = iXMax*iYMax;
    int* aiXStack = new int[iQuantity];
    int* aiYStack = new int[iQuantity];

    // Push seed point onto stack if it has the background color.  All points
    // pushed onto stack have background color iBColor.
    int iTop = 0;
    aiXStack[iTop] = iX;
    aiYStack[iTop] = iY;

    while ( iTop >= 0 )  // stack is not empty
    {
        // Read top of stack.  Do not pop since we need to return to this
        // top value later to restart the fill in a different direction.
        iX = aiXStack[iTop];
        iY = aiYStack[iTop];

        // fill the pixel
        SetPixel(iX,iY,kFColor);

        int iXp1 = iX+1;
        if ( iXp1 < iXMax && GetPixel(iXp1,iY) == kBColor )
        {
            // push pixel with background color
            iTop++;
            aiXStack[iTop] = iXp1;
            aiYStack[iTop] = iY;
            continue;
        }

        int iXm1 = iX-1;
        if ( 0 <= iXm1 && GetPixel(iXm1,iY) == kBColor )
        {
            // push pixel with background color
            iTop++;
            aiXStack[iTop] = iXm1;
            aiYStack[iTop] = iY;
            continue;
        }

        int iYp1 = iY+1;
        if ( iYp1 < iYMax && GetPixel(iX,iYp1) == kBColor )
        {
            // push pixel with background color
            iTop++;
            aiXStack[iTop] = iX;
            aiYStack[iTop] = iYp1;
            continue;
        }

        int iYm1 = iY-1;
        if ( 0 <= iYm1 && GetPixel(iX,iYm1) == kBColor )
        {
            // push pixel with background color
            iTop++;
            aiXStack[iTop] = iX;
            aiYStack[iTop] = iYm1;
            continue;
        }

        // done in all directions, pop and return to search other directions
        iTop--;
    }

    delete[] aiXStack;
    delete[] aiYStack;
}
//----------------------------------------------------------------------------
void WindowApplication2::DoFlip (bool bDoFlip)
{
    if ( m_akFlipScreen )
    {
        if ( !bDoFlip )
        {
            delete[] m_akFlipScreen;
            m_akFlipScreen = 0;
        }
    }
    else
    {
        if ( bDoFlip )
            m_akFlipScreen = new Color[m_iScrWidth*m_iScrHeight];
    }
}
//----------------------------------------------------------------------------
