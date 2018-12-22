/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<windows.h>
#include"walkThroughWin32.h"


WalkThrough3DWin32 *g_walkThoughDemo = NULL;


LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
   int width, height;

   switch(m)
      {
         case WM_CREATE: return 0; break;

         case WM_CLOSE:
         case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            break;

         case WM_SIZE:
            height = HIWORD(lp);
            width = LOWORD(lp);

            if(g_walkThoughDemo != NULL)
               g_walkThoughDemo->Resize(width, height);

            return 0;
            break;

         default: break;
      }

   return (DefWindowProc(hwnd, m, wp, lp));
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmd, int show)
{
   int result = 0;

   g_walkThoughDemo = new WalkThrough3DWin32(hInstance);

   if(g_walkThoughDemo == NULL)
      {
         MessageBox(NULL, "Error creating g_walkThoughDemo!",
                    "Error", MB_OK);
         return 0;
      }

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

   if(!RegisterClassEx(&windowClass))
      return 0;

   if(g_walkThoughDemo->GameInitialize() == false)
      {
         MessageBox(NULL, "Error in initialize!", "Error", MB_OK);
      }
   else
      {
         result = g_walkThoughDemo->EnterGameLoop();
      }

   g_walkThoughDemo->GameShutdown();
   delete g_walkThoughDemo;
   g_walkThoughDemo = NULL;

   return result;
}