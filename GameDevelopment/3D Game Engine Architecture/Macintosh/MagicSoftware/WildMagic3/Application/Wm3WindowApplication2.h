// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3WINDOWAPPLICATION2_H
#define WM3WINDOWAPPLICATION2_H

#include "Wm3WindowApplication.h"

namespace Wm3
{

class WM3_ITEM WindowApplication2 : public WindowApplication
{
public:
    WindowApplication2 (const char* acWindowTitle, int iXPosition,
        int iYPosition, int iWidth, int iHeight,
        const ColorRGB& rkBackgroundColor);

    virtual ~WindowApplication2 ();

    // event callbacks
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnResize (int iWidth, int iHeight);
    virtual void OnDisplay ();

    // Allows you to do additional drawing after the screen polygon is drawn.
    virtual void ScreenOverlay ();

    void ClearScreen ();

    class Color
    {
    public:
        Color (unsigned char ucR=0, unsigned char ucG=0, unsigned char ucB=0)
        {
            r = ucR;
            g = ucG;
            b = ucB;
        }

        bool operator== (Color kColor) const
        {
            return b == kColor.b && g == kColor.g && b == kColor.b;
        }

        bool operator!= (Color kColor) const
        {
            return b != kColor.b || g != kColor.g || b != kColor.b;
        }

        unsigned char b, g, r;
    };

    void SetPixel (int iX, int iY, Color kColor);
    Color GetPixel (int iX, int iY);
    void DrawLine (int iX0, int iY0, int iX1, int iY1, Color kColor);
    void DrawRectangle (int iXMin, int iYMin, int iXMax, int iYMax,
        Color kColor, bool bSolid = false);
    void DrawCircle (int iXCenter, int iYCenter, int iRadius, Color kColor,
        bool bSolid = false);
    void Fill (int iX, int iY, Color kFColor, Color kBColor);

    bool& ClampToWindow ();

    // For right-handed drawing.  You still draw to the left-handed screen,
    // but immediately before drawing the screen is copied into another buffer
    // with the rows reversed.  You need only call DoFlip(true) once for an
    // application.  The default is 'false'.
    void DoFlip (bool bDoFlip);


protected:
    int Index (int iX, int iY)
    {
        // left-handed screen coordinates
        return iX + m_iWidth*iY;
    }

    int m_iScrWidth, m_iScrHeight;
    Color* m_akScreen;
    bool m_bClampToWindow;

    // For right-handed drawing.  The array m_akScreen is copied to
    // m_akFlipScreen so that the rows are reversed.
    Color* m_akFlipScreen;
};

}

#endif
