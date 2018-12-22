/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _BLACK_JACK_GAME_LX_H_
#define _BLACK_JACK_GAME_LX_H_


#include"../blackJack.h"


// Linux version of the game.
class BlackJackGameLinux : public BlackJackGame
{
   public:
      BlackJackGameLinux(Display *disp) : BlackJackGame()
      {
         m_disp = disp;
      }

      virtual ~BlackJackGameLinux()
      {
      }

      bool GameInitialize();
      int EnterGameLoop();

      bool isQuitDemo() { return m_quitDemo; }
      void SetQuitDemo(bool val) { m_quitDemo = val; }

   private:
      Display *m_disp;
};


#endif
