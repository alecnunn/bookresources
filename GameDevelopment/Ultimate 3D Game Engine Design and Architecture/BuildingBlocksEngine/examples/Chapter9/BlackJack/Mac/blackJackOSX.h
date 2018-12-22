/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _BLACK_JACK_GAME_OSX_H_
#define _BLACK_JACK_GAME_OSX_H_


#include"../blackJack.h"


// Win32 version of the game.
class BlackJackGameOSX : public BlackJackGame
{
   public:
      BlackJackGameOSX(WindowRef window) : BlackJackGame()
      {
         m_window = window;
      }

      virtual ~BlackJackGameOSX()
      {
      }

      bool GameInitialize();
      int EnterGameLoop();

   private:
      WindowRef m_window;
};


#endif