/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_X360_INPUT_H_
#define _BB_X360_INPUT_H_

#include<NameSpace.h>
#include<Defines.h>
#include<windows.h>
#include<xinput.h>


DECLARE_ENGINE_NAMESPACE


class X360Controller
{
   public:
      X360Controller() : m_controllerPort(-1) { }
      ~X360Controller() { }

      BRESULT Initialize(int controllerPort);
      void Enable(bool flag);
      BRESULT Update();

      BSTATE isButtonDown(BB_INPUT_BUTTON button);
      BSTATE isButtonUp(BB_INPUT_BUTTON button);

      int GetLeftTriggerPressure();
      int GetRightTriggerPressure();

      void GetStickPos(int deadzone, int *lx, int *ly,
                       int *rx, int *ry);

      void SetFeedBackPressure(int left, int right);

      unsigned long GetStatus()
      {
         return m_status;
      }


   private:
      XINPUT_STATE m_state;
      XINPUT_STATE m_oldState;
      unsigned long m_status;
      XINPUT_VIBRATION m_vibration;
      short m_controllerPort;
};


END_ENGINE_NAMESPACE

#endif