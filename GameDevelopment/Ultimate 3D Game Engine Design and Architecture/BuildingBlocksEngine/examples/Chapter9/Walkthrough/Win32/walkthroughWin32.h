/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _WALK_THROUGH_3D_WIN32_H_
#define _WALK_THROUGH_3D_WIN32_H_


#include"../walkThrough.h"


// Win32 version of the game.
class WalkThrough3DWin32 : public WalkThrough3D
{
   public:
      WalkThrough3DWin32(HINSTANCE hInst) : WalkThrough3D()
      {
         m_hInst = hInst;
      }

      virtual ~WalkThrough3DWin32()
      {

      }

      bool GameInitialize();
      int EnterGameLoop();
      void GameShutdown();

   private:
      HWND m_hwnd;
      HINSTANCE m_hInst;
};


#endif