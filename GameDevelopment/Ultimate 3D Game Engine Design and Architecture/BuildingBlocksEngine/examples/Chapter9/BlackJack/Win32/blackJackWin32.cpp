/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"blackJackWin32.h"


bool BlackJackGameWin32::GameInitialize()
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

   // Setup X360 controller and render params.
   params.m_handle.m_hwnd = m_hwnd;
   params.m_handle.m_hInstance = m_hInst;

   m_360Pads[0].Initialize(0);

   // General game initialize.
   return BlackJackGame::GameInitialize(params);
}


void BlackJackGameWin32::GameUpdate()
{
   // Win32 specific input using Xbox 360 controllers.
   if(m_360Pads[0].Update() == BB_SUCCESS)
      {
         switch(m_currentGameState)
            {
               case BJ_GAME_MENU:
                  if(m_360Pads[0].isButtonUp(BB_BUTTON_BACK))
                     m_quitDemo = true;

                  if(m_360Pads[0].isButtonUp(BB_BUTTON_START))
                     m_currentGameState = BJ_GAME_PLAY_START;
                  break;

               case BJ_GAME_PLAY_START:
               case BJ_GAME_PLAY_PLAYER_WIN:
               case BJ_GAME_PLAY_CPU_WIN:
               case BJ_GAME_PLAY_PLAYER_CPU_TIE:
                  if(m_360Pads[0].isButtonUp(BB_BUTTON_BACK))
                     m_currentGameState = BJ_GAME_MENU;

                  if(m_360Pads[0].isButtonUp(BB_BUTTON_START))
                     m_currentGameState = BJ_GAME_PLAY_DEALING;
                  break;

               case BJ_GAME_PLAY_PLAYER_MOVE:
                  if(m_playerStay == false)
                     {
                        if(m_360Pads[0].isButtonUp(BB_BUTTON_A))
                           {
                              PlayerHit();
                              m_currentGameState = BJ_GAME_PLAY_CPU_MOVE;
                           }
                        else if(m_360Pads[0].isButtonUp(BB_BUTTON_B))
                           {
                              m_playerStay = true;
                              m_currentGameState = BJ_GAME_PLAY_CPU_MOVE;
                           }
                     }
                  break;

               default:
                  break;
            }
      }

   BlackJackGame::GameUpdate();
}


int BlackJackGameWin32::EnterGameLoop()
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


void BlackJackGameWin32::GameShutdown()
{
   BlackJackGame::GameShutdown();

   if(WINDOW_FULLSCREEN)
      ChangeDisplaySettings(NULL, 0);

   UnregisterClass(WINDOW_CLASS, m_hInst);
}