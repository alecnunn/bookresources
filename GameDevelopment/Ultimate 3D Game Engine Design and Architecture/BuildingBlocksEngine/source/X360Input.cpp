/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<windows.h>
#include<X360Input.h>

#pragma comment(lib, "xinput.lib")


unsigned long GetButtonCode(BB_INPUT_BUTTON button)
{
   switch(button)
      {
         case BB_BUTTON_LEFT_SHOULDER:
            return XINPUT_GAMEPAD_LEFT_SHOULDER;
         break;

         case BB_BUTTON_RIGHT_SHOULDER:
            return XINPUT_GAMEPAD_RIGHT_SHOULDER;
         break;

         case BB_BUTTON_LEFT_THUMB:
            return XINPUT_GAMEPAD_LEFT_THUMB;
         break;

         case BB_BUTTON_RIGHT_THUMB:
            return XINPUT_GAMEPAD_RIGHT_THUMB;
         break;

         case BB_BUTTON_START:
            return XINPUT_GAMEPAD_START;
         break;

         case BB_BUTTON_BACK:
            return XINPUT_GAMEPAD_BACK;
         break;

         case BB_BUTTON_ARROW_UP:
            return XINPUT_GAMEPAD_DPAD_UP;
         break;

         case BB_BUTTON_ARROW_DOWN:
            return XINPUT_GAMEPAD_DPAD_DOWN;
         break;

         case BB_BUTTON_ARROW_LEFT:
            return XINPUT_GAMEPAD_DPAD_LEFT;
         break;

         case BB_BUTTON_ARROW_RIGHT:
            return XINPUT_GAMEPAD_DPAD_RIGHT;
         break;

         case BB_BUTTON_A:
            return XINPUT_GAMEPAD_A;
         break;

         case BB_BUTTON_B:
            return XINPUT_GAMEPAD_B;
         break;

         case BB_BUTTON_X:
            return XINPUT_GAMEPAD_X;
         break;

         case BB_BUTTON_Y:
            return XINPUT_GAMEPAD_Y;
         break;

         default:
            return -1;
         break;
      }

   return -1;
}


DECLARE_ENGINE_NAMESPACE


BRESULT X360Controller::Initialize(int controllerPort)
{
   if(controllerPort < 0 || controllerPort > 3)
      return BB_FAIL;

   m_controllerPort = controllerPort;

   return Update();
}


void X360Controller::Enable(bool flag)
{
   XInputEnable(flag);
}


BRESULT X360Controller::Update()
{
   if(m_controllerPort < 0 || m_controllerPort > 3)
      return BB_FAIL;

   m_oldState = m_state;

   m_status = XInputGetState(m_controllerPort, &m_state);

   if(m_status != ERROR_SUCCESS)
      return BB_FAIL;

   return BB_SUCCESS;
}


int X360Controller::GetLeftTriggerPressure()
{
   return (int)m_state.Gamepad.bLeftTrigger;
}


int X360Controller::GetRightTriggerPressure()
{
   return (int)m_state.Gamepad.bRightTrigger;
}


BSTATE X360Controller::isButtonDown(BB_INPUT_BUTTON button)
{
   if(m_controllerPort < 0 || m_controllerPort > 3)
      return BB_FAIL;

   unsigned long buttonCode = GetButtonCode(button);

   if(button == BB_BUTTON_LEFT_TRIGGER)
      if(m_state.Gamepad.bLeftTrigger > 0)
         return BB_SUCCESS;

   if(button == BB_BUTTON_RIGHT_TRIGGER)
      if(m_state.Gamepad.bRightTrigger > 0)
         return BB_SUCCESS;

   if(buttonCode == -1)
      return BB_FAIL;

   if(m_state.Gamepad.wButtons & buttonCode)
      return BB_SUCCESS;

   return BB_FAIL;
}


BSTATE X360Controller::isButtonUp(BB_INPUT_BUTTON button)
{
   if(m_controllerPort < 0 || m_controllerPort > 3)
      return BB_FAIL;

   unsigned long buttonCode = GetButtonCode(button);

   if(button == BB_BUTTON_LEFT_TRIGGER)
      if(m_state.Gamepad.bLeftTrigger == 0)
         if(m_oldState.Gamepad.bLeftTrigger > 0)
            return BB_SUCCESS;

   if(button == BB_BUTTON_RIGHT_TRIGGER)
      if(m_state.Gamepad.bRightTrigger == 0)
         if(m_oldState.Gamepad.bRightTrigger > 0)
            return BB_SUCCESS;

   if(buttonCode == -1)
      return BB_FAIL;

   if(!(m_state.Gamepad.wButtons & buttonCode))
      if(m_oldState.Gamepad.wButtons & buttonCode)
         return BB_SUCCESS;

   return BB_FAIL;
}


void X360Controller::GetStickPos(int deadzone, int *lx, int *ly, int *rx, int *ry)
{
   if(m_controllerPort < 0 || m_controllerPort > 3)
      {
         if(lx) *lx = 0;
         if(ly) *ly = 0;
         if(rx) *rx = 0;
         if(ry) *ry = 0;
         return;
      }

   if(deadzone)
      {
         BB_X360_DEADZONE_CHECK(m_state.Gamepad.sThumbLX,
                              m_state.Gamepad.sThumbLY,
                              deadzone);

         BB_X360_DEADZONE_CHECK(m_state.Gamepad.sThumbRX,
                              m_state.Gamepad.sThumbRY,
                              deadzone);
      }

   if(lx) *lx = (int)m_state.Gamepad.sThumbLX;
   if(ly) *ly = (int)m_state.Gamepad.sThumbLY;
   if(rx) *rx = (int)m_state.Gamepad.sThumbRX;
   if(ry) *ry = (int)m_state.Gamepad.sThumbRY;
}


void X360Controller::SetFeedBackPressure(int left, int right)
{
   if(m_controllerPort < 0 || m_controllerPort > 3)
      return;

   if(left < 0)
      left = 0;

   if(right < 0)
      right = 0;

   if(m_controllerPort >= 0 && m_controllerPort < 4)
      {
         m_vibration.wLeftMotorSpeed = left;
         m_vibration.wLeftMotorSpeed = right;

         XInputSetState(m_controllerPort, &m_vibration);
      }
}


END_ENGINE_NAMESPACE