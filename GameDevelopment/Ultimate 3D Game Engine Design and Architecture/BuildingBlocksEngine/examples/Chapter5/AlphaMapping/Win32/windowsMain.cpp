/*
   Alpha Mapping Demo - Chapter 5
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<windows.h>
#include"../main.h"


LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
   int width, height;

   switch(m)
      {
         case WM_CREATE:
            return 0;
            break;

         case WM_CLOSE:
         case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            break;

         case WM_SIZE:
            height = HIWORD(lp);
            width = LOWORD(lp);
            DemoResize(width, height);
            return 0;
            break;

         default:
            break;
      }

   return (DefWindowProc(hwnd, m, wp, lp));
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmd, int show)
{
   MSG msg;
   HWND hwnd;
   DEVMODE dmScreenSettings;
   
   WNDCLASSEX windowClass;
   memset(&windowClass, 0, sizeof(WNDCLASSEX));
   windowClass.cbSize = sizeof(WNDCLASSEX);
   windowClass.style = CS_HREDRAW | CS_VREDRAW;
   windowClass.lpfnWndProc = WndProc;
   windowClass.hInstance = hInstance;
   windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
   windowClass.lpszClassName = WINDOW_CLASS;
   windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
   if(!RegisterClassEx(&windowClass)) return 0;

   if(!WINDOW_FULLSCREEN)
      {
         hwnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_NAME,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU |
            WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 100, 100,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInstance, NULL);
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
	         CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) return 0;

         hwnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_NAME,
            WS_POPUP | WS_VISIBLE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
            0, 0, hInstance, NULL);

         SetForegroundWindow(hwnd);
      }

   if(!hwnd)
      return 0;

   ShowWindow(hwnd, SW_SHOW);
   UpdateWindow(hwnd);

   bbe::RenderParams params;
   params.m_handle.m_hwnd = hwnd;
   params.m_handle.m_hInstance = hInstance;

   if(!DemoInitialize(params))
      {
         MessageBox(NULL, "Error in initialize!", "Error", MB_OK);
      }
   else
      {
         while(!g_quitDemo)
            {
               if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                  {
                     if(msg.message == WM_QUIT) break;
                     TranslateMessage(&msg);
                     DispatchMessage(&msg);
                  }
               else
                  {
                     DemoUpdate();
                     DemoRender();
                  }
            }

         DemoShutdown();
      }

   if(WINDOW_FULLSCREEN)
      ChangeDisplaySettings(NULL, 0);

   UnregisterClass(WINDOW_CLASS, windowClass.hInstance);
   return (int)msg.wParam;
}