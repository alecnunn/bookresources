/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"walkThroughWin32.h"


bool WalkThrough3DWin32::GameInitialize()
{
   DEVMODE dmScreenSettings;
   bbe::RenderParams params;

   // Create the OS specific window.

   if(!WINDOW_FULLSCREEN)
      {
         m_hwnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_NAME,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU |
            WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 100, 100,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, m_hInst, NULL);
      }
   else
      {
	      ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
	      dmScreenSettings.dmSize = sizeof(DEVMODE);
	      dmScreenSettings.dmPelsWidth = WINDOW_WIDTH;
	      dmScreenSettings.dmPelsHeight = WINDOW_HEIGHT;
	      dmScreenSettings.dmBitsPerPel = 32;
	      dmScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	      if(ChangeDisplaySettings(&dmScreenSettings,
	         CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) return false;

         m_hwnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_NAME,
            WS_POPUP | WS_VISIBLE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
            0, 0, m_hInst, NULL);

         SetForegroundWindow(m_hwnd);
      }

   if(!m_hwnd)
      return false;

   ShowWindow(m_hwnd, SW_SHOW);
   UpdateWindow(m_hwnd);

   // Render params.
   params.m_handle.m_hwnd = m_hwnd;
   params.m_handle.m_hInstance = m_hInst;

   // General game initialize.
   return WalkThrough3D::GameInitialize(params);
}


int WalkThrough3DWin32::EnterGameLoop()
{
   MSG msg;

   while(!m_quitDemo)
      {
         if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
               if(msg.message == WM_QUIT)
                  break;

               TranslateMessage(&msg);
               DispatchMessage(&msg);
            }
         else
            {
               GameUpdate();
               GameRender();
            }
      }

   GameShutdown();

   return (int)msg.wParam;
}


void WalkThrough3DWin32::GameShutdown()
{
   WalkThrough3D::GameShutdown();

   if(WINDOW_FULLSCREEN)
      ChangeDisplaySettings(NULL, 0);

   UnregisterClass(WINDOW_CLASS, m_hInst);
}