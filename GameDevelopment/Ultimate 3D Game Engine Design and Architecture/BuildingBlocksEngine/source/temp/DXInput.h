/*

*/


#ifndef _BB_DIRECT_INPUT_H_
#define _BB_DIRECT_INPUT_H_

#include<NameSpace.h>
#include<Defines.h>
#include<dinput.h>


DECLARE_ENGINE_NAMESPACE


class DIKeyboard
{
   public:
      DIKeyboard();
      virtual ~DIKeyboard() { Shutdown(); }

      BRESULT Initialize(LPDIRECTINPUT8 dInput, HWND hwnd);
      void Shutdown();

      BRESULT Update();

      BSTATE isButtonDown(BB_INPUT_BUTTON button);
      BSTATE isButtonUp(BB_INPUT_BUTTON button);


   private:
      LPDIRECTINPUTDEVICE8 m_device;
      char m_keys[BB_KEYBOARD_KEY_NUM];
      char m_oldKeys[BB_KEYBOARD_KEY_NUM];
};


class DIMouse
{
   public:
      DIMouse();
      virtual ~DIMouse() { Shutdown(); }

      BRESULT Initialize(LPDIRECTINPUT8 dInput, HWND hwnd, bool exclusive);
      void Shutdown();

      BRESULT Update();

      BSTATE isButtonDown(BB_INPUT_BUTTON button);
      BSTATE isButtonUp(BB_INPUT_BUTTON button);

      void GetDevicePosition(int *x, int *y, int *wheel);


   private:
      LPDIRECTINPUTDEVICE8 m_device;
      DIMOUSESTATE m_state;
      DIMOUSESTATE m_oldState;

      long m_xPos;
      long m_yPos;
      long m_wheelPos;
};


END_ENGINE_NAMESPACE

#endif