// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3WINDOWAPPLICATION_H
#define WM3WINDOWAPPLICATION_H

#include "Wm3Application.h"

namespace Wm3
{

class WM3_ITEM WindowApplication : public Application
{
public:
    // abstract base class
    WindowApplication (const char* acWindowTitle, int iXPosition,
        int iYPosition, int iWidth, int iHeight,
        const ColorRGB& rkBackgroundColor);
    virtual ~WindowApplication ();

    // Entry point to be implemented by the application.  The return value
    // is an exit code, if desired.
    virtual int Main (int iQuantity, char** apcArgument);

    // member access
    const char* GetWindowTitle () const;
    int GetXPosition () const;
    int GetYPosition () const;
    int GetWidth () const;
    int GetHeight () const;
    void SetRenderer (Renderer* pkRenderer);
    void SetWindowID (int iWindowID);
    int GetWindowID () const;

    // event callbacks
    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnMove (int iX, int iY);
    virtual void OnResize (int iWidth, int iHeight);
    virtual void OnDisplay ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnKeyUp (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);
    virtual bool OnSpecialKeyUp (int iKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY);
    virtual bool OnPassiveMotion (int iX, int iY);

    // Program termination.  The default key to terminate the program is
    // the ESC key.  The termination is platform-specific, so classes that
    // implement the WindowApplication interfaces must implement this
    // function.  It is not implemented by WindowApplication.
    void RequestTermination ();

    // Font information.  These are platform-specific, so classes that
    // implement the WindowApplication interfaces must implement these
    // functions.  They are not defined by WindowApplication.
    int GetStringWidth (const char* acText) const;
    int GetCharWidth (const char cCharacter) const;
    int GetFontHeight () const;

    // Key identifiers.  These are platform-specific, so classes that
    // implement the WindowApplication interfaces must define these variables.
    // They are not defined by WindowApplication.
    static const int KEY_ESCAPE;
    static const int KEY_LEFT_ARROW;
    static const int KEY_RIGHT_ARROW;
    static const int KEY_UP_ARROW;
    static const int KEY_DOWN_ARROW;
    static const int KEY_HOME;
    static const int KEY_END;
    static const int KEY_PAGE_UP;
    static const int KEY_PAGE_DOWN;
    static const int KEY_INSERT;
    static const int KEY_DELETE;
    static const int KEY_F1;
    static const int KEY_F2;
    static const int KEY_F3;
    static const int KEY_F4;
    static const int KEY_F5;
    static const int KEY_F6;
    static const int KEY_F7;
    static const int KEY_F8;
    static const int KEY_F9;
    static const int KEY_F10;
    static const int KEY_F11;
    static const int KEY_F12;

    // keyboard modifiers
    static const int KEY_SHIFT;
    static const int KEY_CONTROL;
    static const int KEY_ALT;
    static const int KEY_COMMAND;

    // mouse buttons
    static const int MOUSE_LEFT_BUTTON;
    static const int MOUSE_MIDDLE_BUTTON;
    static const int MOUSE_RIGHT_BUTTON;

    // mouse state
    static const int MOUSE_UP;
    static const int MOUSE_DOWN;

    // mouse modifiers
    static const int MODIFIER_CONTROL;
    static const int MODIFIER_LBUTTON;
    static const int MODIFIER_MBUTTON;
    static const int MODIFIER_RBUTTON;
    static const int MODIFIER_SHIFT;

protected:
    // The hookup to the 'main' entry point into the executable.
    static int Run (int iQuantity, char** apcArgument);

    // constructor inputs
    const char* m_acWindowTitle;
    int m_iXPosition, m_iYPosition, m_iWidth, m_iHeight;
    ColorRGB m_kBackgroundColor;

    // an identifier for the window (representation is platform-specific)
    int m_iWindowID;

    // the renderer (used for 2D and 3D applications)
    Renderer* m_pkRenderer;
};

#include "Wm3WindowApplication.inl"

}

#endif
