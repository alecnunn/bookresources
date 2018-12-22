/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<windows.h>
#include"blackJackWin32.h"


BlackJackGameWin32 *g_blackJackGame = NULL;


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

            if(g_blackJackGame != NULL)
               g_blackJackGame->Resize(width, height);

            return 0;
            break;

         default: break;
      }

   return (DefWindowProc(hwnd, m, wp, lp));
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmd, int show)
{
   int result = 0;

   g_blackJackGame = new BlackJackGameWin32(hInstance);

   if(g_blackJackGame == NULL)
      {
         MessageBox(NULL, "Error creating g_blackJackGame!",
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

   if(g_blackJackGame->GameInitialize() == false)
      {
         MessageBox(NULL, "Error in initialize!", "Error", MB_OK);
      }
   else
      {
         result = g_blackJackGame->EnterGameLoop();
      }

   g_blackJackGame->GameShutdown();
   delete g_blackJackGame;
   g_blackJackGame = NULL;

   return result;
}