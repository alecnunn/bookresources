/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _BLACK_JACK_GAME_WIN32_H_
#define _BLACK_JACK_GAME_WIN32_H_


#include"../blackJack.h"
#include<X360Input.h>


// Win32 version of the game.
class BlackJackGameWin32 : public BlackJackGame
{
   public:
      BlackJackGameWin32(HINSTANCE hInst) : BlackJackGame()
      {
         m_hInst = hInst;
      }

      virtual ~BlackJackGameWin32()
      {
      }

      bool GameInitialize();
      void GameUpdate();
      int EnterGameLoop();
      void GameShutdown();

   private:
      HWND m_hwnd;
      HINSTANCE m_hInst;

      bbe::X360Controller m_360Pads[4];
};


#endif