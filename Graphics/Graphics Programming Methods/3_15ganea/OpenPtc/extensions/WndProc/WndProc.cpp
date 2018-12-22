//
// WndProc example for OpenPTC 1.0 C++ Implementation
// Copyright (c) Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"

// control defines
//#define FULLSCREEN


inline int random(int max)
{
    // random number
    return rand() % max;
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    // message handler
    switch (uMsg)
    { 
        case WM_KEYDOWN:
        {    
            // handle keypress
            if (wParam==VK_ESCAPE)
            {
                // exit on escape key
                PostMessage(hWnd,WM_DESTROY,0,0);
            }
        }
        break;

        case WM_CLOSE:
        {
            // destroy window on close message
            PostMessage(hWnd,WM_DESTROY,0,0);
        }
        break;

        case WM_DESTROY:
        {
            // post quit message
            PostQuitMessage(0);
        }
        break;

        default:
        {
            // default window action
            return DefWindowProc(hWnd,uMsg,wParam,lParam);
        }
    }

    // handled
    return 0;
}


HWND create(HINSTANCE hInstance,int nCmdShow)
{
#ifdef FULLSCREEN

    // fullscreen output style
    DWORD style = WS_POPUP;

    // window position
    const int x = 0;
    const int y = 0;

    // setup window rect to cover the entire screen
    RECT rect = {0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};

#else

    // windowed output style
    DWORD style = WS_OVERLAPPEDWINDOW;

    // setup window position
    const int x = CW_USEDEFAULT;
    const int y = CW_USEDEFAULT;

    // setup client area rect
    RECT rect = {0,0,320,200};

    // adjust window rect for window style
    AdjustWindowRect(&rect,style,0);

#endif

    // setup window class
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance,"IDI_MAIN");
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "WNDPROC_EXAMPLE_WINDOW";

    // register window class
    RegisterClass(&wc);

    // create window
    HWND hWnd = CreateWindow("WNDPROC_EXAMPLE_WINDOW","WndProc example",WS_OVERLAPPEDWINDOW,x,y,rect.right-rect.left,rect.bottom-rect.top,0,0,hInstance,0);
    
    // check window was created successfully
    if (!hWnd) throw Error("could not create window");

    // show the window
    ShowWindow(hWnd,nCmdShow);

    // return window
    return hWnd;
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{   
    try
    {
        // create output window manually
        HWND hWnd = create(hInstance,nCmdShow);
        
        // create console
        Console console;
        
#ifdef FULLSCREEN

        // force fullscreen output
        console.option("DirectX");
        console.option("fullscreen output");

#else

        // force windowed output
        console.option("DirectX");
        console.option("windowed output");

#endif

        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);

        // open console on window
        console.open(hWnd,320,200,format);

        // disable console key buffering
        console.option("disable key buffering");

        // create surface
        Surface surface(320,200,format);

        // main loop
        while (true)
        {
            // msg data
            MSG message;
        
            // process all window messages
            while (PeekMessage(&message,hWnd,0,0,PM_REMOVE))
            {
                // handle WM_QUIT message
                if (message.message==WM_QUIT)
                {
                    return 0;
                }

                // translate and dispatch
                TranslateMessage(&message);
                DispatchMessage(&message);
            }

            // lock surface pixels
            int32 *pixels = (int32*) surface.lock();

            // get surface dimensions
            const int width  = surface.width();
            const int height = surface.height();

            // draw random pixels
            for (int i=0; i<100; i++)
            {
                // get random position
                const int x = random(width);
                const int y = random(height);

                // get random color
                const int r = random(256);
                const int g = random(256);
                const int b = random(256);

                // draw color [r,g,b] at position [x,y]
                pixels[x+y*width] = (r<<16) + (g<<8) + b;
            }
        
            // unlock surface
            surface.unlock();

            // copy to console
            surface.copy(console);
            
            // update console
            console.update();
        }
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }

    // exit
    return 0;
}
