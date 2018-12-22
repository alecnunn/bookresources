/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _WALKTHROUGH_LX_H_
#define _WALKTHROUGH_LX_H_


#include"../walkthrough.h"


// Linux version of the game.
class WalkThroughLinux : public WalkThrough3D
{
   public:
      WalkThroughLinux(Display *disp) : WalkThrough3D()
      {
         m_disp = disp;
      }

      virtual ~WalkThroughLinux()
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
