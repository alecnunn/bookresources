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
#include "Wm3GlutRenderer.h"
using namespace Wm3;

#ifdef WIN32

// The DirectShow header dshow.h includes windowsx.h.  The latter header file
// #define-s GetWindowID(hwnd) to be GetDlgCtrlID(hwnd).  I received a report
// that WmlGlutApplication.cpp fails to compile using VC 7.1, the failure
// being that the #define interferes with WindowApplication::GetWindowID.  I was
// not able to reproduce the problem, but just in case...
#ifdef GetWindowID
#undef GetWindowID
#endif

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glut32.lib")
#endif

const int WindowApplication::KEY_ESCAPE = 0x1B;
const int WindowApplication::KEY_LEFT_ARROW = GLUT_KEY_LEFT;
const int WindowApplication::KEY_RIGHT_ARROW = GLUT_KEY_RIGHT;
const int WindowApplication::KEY_UP_ARROW = GLUT_KEY_UP;
const int WindowApplication::KEY_DOWN_ARROW = GLUT_KEY_DOWN;
const int WindowApplication::KEY_HOME = GLUT_KEY_HOME;
const int WindowApplication::KEY_END = GLUT_KEY_END;
const int WindowApplication::KEY_PAGE_UP = GLUT_KEY_PAGE_UP;
const int WindowApplication::KEY_PAGE_DOWN = GLUT_KEY_PAGE_DOWN;
const int WindowApplication::KEY_INSERT = GLUT_KEY_INSERT;
const int WindowApplication::KEY_DELETE = 0x2E;
const int WindowApplication::KEY_F1 = GLUT_KEY_F1;
const int WindowApplication::KEY_F2 = GLUT_KEY_F2;
const int WindowApplication::KEY_F3 = GLUT_KEY_F3;
const int WindowApplication::KEY_F4 = GLUT_KEY_F4;
const int WindowApplication::KEY_F5 = GLUT_KEY_F5;
const int WindowApplication::KEY_F6 = GLUT_KEY_F6;
const int WindowApplication::KEY_F7 = GLUT_KEY_F7;
const int WindowApplication::KEY_F8 = GLUT_KEY_F8;
const int WindowApplication::KEY_F9 = GLUT_KEY_F9;
const int WindowApplication::KEY_F10 = GLUT_KEY_F10;
const int WindowApplication::KEY_F11 = GLUT_KEY_F11;
const int WindowApplication::KEY_F12 = GLUT_KEY_F12;

const int WindowApplication::KEY_SHIFT = GLUT_ACTIVE_SHIFT;
const int WindowApplication::KEY_CONTROL = GLUT_ACTIVE_CTRL;
const int WindowApplication::KEY_ALT = GLUT_ACTIVE_ALT;
const int WindowApplication::KEY_COMMAND = 0;

const int WindowApplication::MOUSE_LEFT_BUTTON = GLUT_LEFT_BUTTON;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = GLUT_MIDDLE_BUTTON;
const int WindowApplication::MOUSE_RIGHT_BUTTON = GLUT_RIGHT_BUTTON;
const int WindowApplication::MOUSE_UP = GLUT_UP;
const int WindowApplication::MOUSE_DOWN = GLUT_DOWN;

const int WindowApplication::MODIFIER_CONTROL = GLUT_ACTIVE_CTRL;
const int WindowApplication::MODIFIER_LBUTTON = GLUT_LEFT_BUTTON;
const int WindowApplication::MODIFIER_MBUTTON = GLUT_MIDDLE_BUTTON;
const int WindowApplication::MODIFIER_RBUTTON = GLUT_RIGHT_BUTTON;
const int WindowApplication::MODIFIER_SHIFT = GLUT_ACTIVE_SHIFT;

// Reading the state of the modifiers from GLUT cannot be done within
// the motion callback. Hence, we cache it here.
static unsigned int gs_uiGLUTModifiers = 0;
static int gs_iButton = -1;

//----------------------------------------------------------------------------
void WindowApplication::RequestTermination ()
{
    exit(0);
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* acText) const
{
    assert( acText && strlen(acText) > 0 );
    return 8*(int)strlen(acText);
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharWidth (const char) const
{
    return 8;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    return 13;
}
//----------------------------------------------------------------------------
static void ReshapeCallback (int iWidth, int iHeight)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnResize(iWidth,iHeight);
}
//----------------------------------------------------------------------------
static void DisplayCallback ()
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnDisplay();
}
//----------------------------------------------------------------------------
static void IdleCallback ()
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnIdle();
}
//----------------------------------------------------------------------------
static void KeyDownCallback (unsigned char ucKey, int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
static void KeyUpCallback (unsigned char ucKey, int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnKeyUp(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
static void SpecialKeyDownCallback (int iKey, int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnSpecialKeyDown(iKey,iX,iY);
}
//----------------------------------------------------------------------------
static void SpecialKeyUpCallback (int iKey, int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnSpecialKeyUp(iKey,iX,iY);
}
//----------------------------------------------------------------------------
static void MouseClickCallback (int iButton, int iState, int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
    {
        int iModifiers = glutGetModifiers();
        gs_uiGLUTModifiers = *(unsigned int*)&iModifiers;
        if ( iState == WindowApplication::MOUSE_DOWN )
            gs_iButton = iButton;
        else
            gs_iButton = -1;
        pkTheApp->OnMouseClick(iButton,iState,iX,iY,gs_uiGLUTModifiers);
    }
}
//----------------------------------------------------------------------------
static void MotionCallback (int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnMotion(gs_iButton,iX,iY);
}
//----------------------------------------------------------------------------
static void PassiveMotionCallback (int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
        pkTheApp->OnPassiveMotion(iX,iY);
}
//----------------------------------------------------------------------------
static void Terminate ()
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( pkTheApp )
    {
        pkTheApp->OnTerminate();
        glutDestroyWindow(pkTheApp->GetWindowID());
        delete Application::TheCommand;
        Application::TheCommand = 0;
        Main::Terminate();
        delete Application::TheApplication;
        Application::TheApplication = 0;
    }
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int iQuantity, char** apcArgument)
{
    if ( atexit(Terminate) != 0 )
        return -1;

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( !pkTheApp->OnPrecreate() )
        return -2;

    glutInit(&iQuantity,apcArgument);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(pkTheApp->GetWidth(),pkTheApp->GetHeight());

    pkTheApp->SetWindowID(glutCreateWindow(pkTheApp->GetWindowTitle()));

    pkTheApp->SetRenderer(new GlutRenderer(pkTheApp->GetWindowID(),
        pkTheApp->GetWidth(),pkTheApp->GetHeight()));

    if ( !pkTheApp->OnInitialize() )
        return -3;

    glutReshapeFunc(ReshapeCallback);
    glutDisplayFunc(DisplayCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyDownCallback);
    glutKeyboardUpFunc(KeyUpCallback);
    glutSpecialFunc(SpecialKeyDownCallback);
    glutSpecialUpFunc(SpecialKeyUpCallback);
    glutMouseFunc(MouseClickCallback);
    glutMotionFunc(MotionCallback);
    glutPassiveMotionFunc(PassiveMotionCallback);

    glutMainLoop();
    return 0;
}
//----------------------------------------------------------------------------
