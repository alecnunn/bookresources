// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include <windows.h>
#pragma comment(lib,"comctl32.lib")

#include "Wm3WindowApplication.h"
using namespace Wm3;

const int WindowApplication::KEY_ESCAPE = VK_ESCAPE;
const int WindowApplication::KEY_LEFT_ARROW = VK_LEFT;
const int WindowApplication::KEY_RIGHT_ARROW = VK_RIGHT;
const int WindowApplication::KEY_UP_ARROW = VK_UP;
const int WindowApplication::KEY_DOWN_ARROW = VK_DOWN;
const int WindowApplication::KEY_HOME = VK_HOME;
const int WindowApplication::KEY_END = VK_END;
const int WindowApplication::KEY_PAGE_UP = VK_PRIOR;
const int WindowApplication::KEY_PAGE_DOWN = VK_NEXT;
const int WindowApplication::KEY_INSERT = VK_INSERT;
const int WindowApplication::KEY_DELETE = VK_DELETE;
const int WindowApplication::KEY_F1 = VK_F1;
const int WindowApplication::KEY_F2 = VK_F2;
const int WindowApplication::KEY_F3 = VK_F3;
const int WindowApplication::KEY_F4 = VK_F4;
const int WindowApplication::KEY_F5 = VK_F5;
const int WindowApplication::KEY_F6 = VK_F6;
const int WindowApplication::KEY_F7 = VK_F7;
const int WindowApplication::KEY_F8 = VK_F8;
const int WindowApplication::KEY_F9 = VK_F9;
const int WindowApplication::KEY_F10 = VK_F10;
const int WindowApplication::KEY_F11 = VK_F11;
const int WindowApplication::KEY_F12 = VK_F12;

const int WindowApplication::KEY_SHIFT = MK_SHIFT;
const int WindowApplication::KEY_CONTROL = MK_CONTROL;
const int WindowApplication::KEY_ALT = 0;      // not currently handled
const int WindowApplication::KEY_COMMAND = 0;  // not currently handled

const int WindowApplication::MOUSE_LEFT_BUTTON = 0;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = 1;
const int WindowApplication::MOUSE_RIGHT_BUTTON = 2;
const int WindowApplication::MOUSE_UP = 0;
const int WindowApplication::MOUSE_DOWN = 1;

const int WindowApplication::MODIFIER_CONTROL = MK_CONTROL;
const int WindowApplication::MODIFIER_LBUTTON = MK_LBUTTON;
const int WindowApplication::MODIFIER_MBUTTON = MK_MBUTTON;
const int WindowApplication::MODIFIER_RBUTTON = MK_RBUTTON;
const int WindowApplication::MODIFIER_SHIFT = MK_SHIFT;

extern Renderer* MsCreateRenderer (HWND hWnd, int iWidth, int iHeight);

//----------------------------------------------------------------------------
void WindowApplication::RequestTermination ()
{
    PostMessage((HWND)IntToPtr(m_iWindowID),WM_DESTROY,0,0);
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* acText) const
{
    if ( !acText || strlen(acText) == 0 )
        return 0;

    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    HDC hDC = GetDC(hWnd);
    SIZE kSize;
    GetTextExtentPoint32(hDC,acText,(int)strlen(acText),&kSize);
    ReleaseDC(hWnd,hDC);
    return (int)kSize.cx;
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharWidth (const char cCharacter) const
{
    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    HDC hDC = GetDC(hWnd);
    SIZE kSize;
    GetTextExtentPoint32(hDC,&cCharacter,1,&kSize);
    ReleaseDC(hWnd,hDC);
    return (int)kSize.cx;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    HDC hDC = GetDC(hWnd);
    TEXTMETRIC kMetric;
    GetTextMetrics(hDC,&kMetric);
    ReleaseDC(hWnd,hDC);
    return (int)kMetric.tmHeight;
}
//----------------------------------------------------------------------------
LRESULT CALLBACK MsWindowEventHandler (HWND hWnd, UINT uiMsg, WPARAM wParam,
    LPARAM lParam)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if ( !pkTheApp || !pkTheApp->GetWindowID() )
        return DefWindowProc(hWnd,uiMsg,wParam,lParam);

    switch ( uiMsg ) 
    {
        case WM_PAINT:
        {
            PAINTSTRUCT kPS;
            HDC hDC = BeginPaint(hWnd,&kPS);
            pkTheApp->OnDisplay();
            EndPaint(hWnd,&kPS);
            return 0;
        }
        case WM_ERASEBKGND:
        {
            // This tells Windows not to erase the background (and that the
            // application is doing so).
            return 1;
        }
        case WM_MOVE:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMove(iXPos,iYPos);
            return 0;
        }
        case WM_SIZE:
        {
            int iWidth = int(LOWORD(lParam));
            int iHeight = int(HIWORD(lParam));
            pkTheApp->OnResize(iWidth,iHeight);
            return 0;
        }
        case WM_CHAR:
        {
            unsigned char ucKey = (unsigned char)(char)wParam;

            // get the cursor position in client coordinates
            POINT kPoint;
            GetCursorPos(&kPoint);
            ScreenToClient(hWnd,&kPoint);
            int iXPos = (int)kPoint.x;
            int iYPos = (int)kPoint.y;

            pkTheApp->OnKeyDown(ucKey,iXPos,iYPos);
            return 0;
        }
        case WM_KEYDOWN:
        {
            int iVirtKey = int(wParam);

            // get cursor position client coordinates
            POINT kPoint;
            GetCursorPos(&kPoint);
            ScreenToClient(hWnd,&kPoint);
            int iXPos = (int)kPoint.x;
            int iYPos = (int)kPoint.y;

            if ( (VK_F1 <= iVirtKey && iVirtKey <= VK_F12)
            ||   (VK_PRIOR <= iVirtKey && iVirtKey <= VK_DOWN)
            ||   (iVirtKey == VK_INSERT) )
            {
                pkTheApp->OnSpecialKeyDown(iVirtKey,iXPos,iYPos);
            }
            return 0;
        }
        case WM_KEYUP:
        {
            int iVirtKey = int(wParam);

            // get the cursor position in client coordinates
            POINT kPoint;
            GetCursorPos(&kPoint);
            ScreenToClient(hWnd,&kPoint);
            int iXPos = (int)kPoint.x;
            int iYPos = (int)kPoint.y;

            if ( (VK_F1 <= iVirtKey && iVirtKey <= VK_F12)
            ||   (VK_PRIOR <= iVirtKey && iVirtKey <= VK_DOWN)
            ||   (iVirtKey == VK_INSERT) )
            {
                pkTheApp->OnSpecialKeyUp(iVirtKey,iXPos,iYPos);
            }
            else
            {
                pkTheApp->OnKeyUp((unsigned char)iVirtKey,iXPos,iYPos);
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_LEFT_BUTTON,
                WindowApplication::MOUSE_DOWN,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_LBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_LEFT_BUTTON,
                WindowApplication::MOUSE_UP,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_MIDDLE_BUTTON,
                WindowApplication::MOUSE_DOWN,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_MBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_MIDDLE_BUTTON,
                WindowApplication::MOUSE_UP,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_RIGHT_BUTTON,
                WindowApplication::MOUSE_DOWN,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_RBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_RIGHT_BUTTON,
                WindowApplication::MOUSE_UP,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));

            int iButton = -1;
            if ( wParam & MK_LBUTTON )
                iButton = WindowApplication::MOUSE_LEFT_BUTTON;
            else if ( wParam & MK_MBUTTON )
                iButton = WindowApplication::MOUSE_MIDDLE_BUTTON;
            else if ( wParam & MK_RBUTTON )
                iButton = WindowApplication::MOUSE_RIGHT_BUTTON;

            if ( iButton >= 0 )
                pkTheApp->OnMotion(iButton,iXPos,iYPos);
            else
                pkTheApp->OnPassiveMotion(iXPos,iYPos);

            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd,uiMsg,wParam,lParam);
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int iQuantity, char** apcArgument)
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;

    // allow work to be done before the window is created
    if ( !pkTheApp->OnPrecreate() )
        return -1;

    // register the window class
    static char s_acWindowClass[] = "Wild Magic Application";
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = MsWindowEventHandler;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = 0;
    wc.hIcon         = LoadIcon(0,IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = s_acWindowClass;
    wc.lpszMenuName  = 0;
    RegisterClass(&wc);

    // require the window to have the specified client area
    RECT kRect = { 0, 0, pkTheApp->GetWidth()-1, pkTheApp->GetHeight()-1 };
    AdjustWindowRect(&kRect,WS_OVERLAPPEDWINDOW,false);

    // create the application window
    HWND hWnd = CreateWindow(s_acWindowClass,pkTheApp->GetWindowTitle(),
        WS_OVERLAPPEDWINDOW,pkTheApp->GetXPosition(),pkTheApp->GetYPosition(),
        kRect.right-kRect.left+1,kRect.bottom-kRect.top+1,0,0,0,0);

    pkTheApp->SetWindowID(PtrToInt(hWnd));

    pkTheApp->SetRenderer(MsCreateRenderer(hWnd,pkTheApp->GetWidth(),
        pkTheApp->GetHeight()));

    if ( pkTheApp->OnInitialize() )
    {
        // display the window
        ShowWindow(hWnd,SW_SHOW);
        UpdateWindow(hWnd);

        // start the message pump
        MSG kMsg;
        while ( TRUE )
        {
            if ( PeekMessage(&kMsg,0,0,0,PM_REMOVE) )
            {
                if ( kMsg.message == WM_QUIT )
                    break;

                HACCEL hAccel = 0;
                if ( !TranslateAccelerator(hWnd,hAccel,&kMsg) )
                {
                    TranslateMessage(&kMsg);
                    DispatchMessage(&kMsg);
                }
            }
            else
            {
                pkTheApp->OnIdle();
            }
        }
    }

    pkTheApp->OnTerminate();
    return 0;
}
//----------------------------------------------------------------------------
