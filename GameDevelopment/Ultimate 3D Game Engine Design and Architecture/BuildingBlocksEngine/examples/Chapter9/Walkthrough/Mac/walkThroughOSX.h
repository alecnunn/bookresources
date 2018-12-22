/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _WALKTHROUGH_OSX_H_
#define _WALKTHROUGH_OSX_H_


#include"../walkThrough.h"


class WalkThroughOSX : public WalkThrough3D
{
   public:
      WalkThroughOSX(WindowRef window) : WalkThrough3D()
      {
         m_window = window;
      }

      virtual ~WalkThroughOSX()
      {
      }

      bool GameInitialize();
      int EnterGameLoop();

   private:
      WindowRef m_window;
};


#endif