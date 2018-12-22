/*

*/


#include<DXInput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


DECLARE_ENGINE_NAMESPACE


int GetDICode(BB_INPUT_BUTTON button)
{
   switch(button)
      {
         case BB_BUTTON_A: return DIK_A; break;
         case BB_BUTTON_B: return DIK_B; break;
         case BB_BUTTON_C: return DIK_C; break;
         case BB_BUTTON_D: return DIK_D; break;
         case BB_BUTTON_E: return DIK_E; break;
         case BB_BUTTON_F: return DIK_F; break;
         case BB_BUTTON_G: return DIK_G; break;
         case BB_BUTTON_H: return DIK_H; break;
         case BB_BUTTON_I: return DIK_I; break;
         case BB_BUTTON_J: return DIK_J; break;
         case BB_BUTTON_K: return DIK_K; break;
         case BB_BUTTON_L: return DIK_L; break;
         case BB_BUTTON_M: return DIK_M; break;
         case BB_BUTTON_N: return DIK_N; break;
         case BB_BUTTON_O: return DIK_O; break;
         case BB_BUTTON_P: return DIK_P; break;
         case BB_BUTTON_Q: return DIK_Q; break;
         case BB_BUTTON_R: return DIK_R; break;
         case BB_BUTTON_S: return DIK_S; break;
         case BB_BUTTON_T: return DIK_T; break;
         case BB_BUTTON_U: return DIK_U; break;
         case BB_BUTTON_V: return DIK_V; break;
         case BB_BUTTON_W: return DIK_W; break;
         case BB_BUTTON_X: return DIK_X; break;
         case BB_BUTTON_Y: return DIK_Y; break;
         case BB_BUTTON_Z: return DIK_Z; break;

         case BB_BUTTON_0: return DIK_0; break;
         case BB_BUTTON_1: return DIK_1; break;
         case BB_BUTTON_2: return DIK_2; break;
         case BB_BUTTON_3: return DIK_3; break;
         case BB_BUTTON_4: return DIK_4; break;
         case BB_BUTTON_5: return DIK_5; break;
         case BB_BUTTON_6: return DIK_6; break;
         case BB_BUTTON_7: return DIK_7; break;
         case BB_BUTTON_8: return DIK_8; break;
         case BB_BUTTON_9: return DIK_9; break;

         default:
            break;
      }

   return 0;
}


DIKeyboard::DIKeyboard()
{
   m_device =  NULL;
   memset(m_keys, 0, BB_KEYBOARD_KEY_NUM);
   memset(m_oldKeys, 0, BB_KEYBOARD_KEY_NUM);
}


BRESULT DIKeyboard::Initialize(LPDIRECTINPUT8 dInput, HWND hwnd)
{
   if(FAILED(dInput->CreateDevice(GUID_SysKeyboard, &m_device, NULL)))
      return BB_FAIL;

   if(FAILED(m_device->SetDataFormat(&c_dfDIKeyboard)))
      return BB_FAIL;

   if(FAILED(m_device->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
      return BB_FAIL;
   
   if(FAILED(m_device->Acquire()))
      return BB_FAIL;

   return BB_SUCCESS;
}


void DIKeyboard::Shutdown()
{
   if(m_device)
      {
         m_device->Unacquire();
         m_device->Release();
         m_device = NULL;
      }
}


BRESULT DIKeyboard::Update()
{
   if(!m_device)
      return BB_FAIL;

   memcpy(m_oldKeys, m_keys, sizeof(m_keys));

   if(FAILED(m_device->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
      {
         if(FAILED(m_device->Acquire()))
            return BB_FAIL;

         if(FAILED(m_device->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
            return BB_FAIL;
      }

   return BB_SUCCESS;
}


BSTATE DIKeyboard::isButtonDown(BB_INPUT_BUTTON button)
{
   int key = GetDICode(button);

   if(m_keys[key] & 0x80)
      return BB_SUCCESS;

   return BB_FAIL;
}


BSTATE DIKeyboard::isButtonUp(BB_INPUT_BUTTON button)
{
   int key = GetDICode(button);

   if(!(m_keys[key] & 0x80))
      if(m_oldKeys[key] & 0x80)
         return BB_SUCCESS;

   return BB_FAIL;
}


DIMouse::DIMouse()
{
   m_device =  NULL;
   memset(&m_state, 0, sizeof(DIMOUSESTATE));
   memset(&m_oldState, 0, sizeof(DIMOUSESTATE));
   m_xPos = m_yPos = m_wheelPos = 0;
}


BRESULT DIMouse::Initialize(LPDIRECTINPUT8 dInput, HWND hwnd, bool exclusive)
{
   DWORD flag;

   if(FAILED(dInput->CreateDevice(GUID_SysMouse, &m_device, NULL)))
      return BB_FAIL;

   if(FAILED(m_device->SetDataFormat(&c_dfDIMouse)))
      return BB_FAIL;

   if(exclusive)
      flag = DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY;
   else
      flag = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

   if(FAILED(m_device->SetCooperativeLevel(hwnd, flag)))
      return BB_FAIL;

   if(FAILED(m_device->Acquire()))
      return BB_FAIL;

   return BB_SUCCESS;
}


void DIMouse::Shutdown()
{
   if(m_device)
      {
         m_device->Unacquire();
         m_device->Release();
         m_device = NULL;
      }

   m_xPos = m_yPos = m_wheelPos = 0;
}


BRESULT DIMouse::Update()
{
   if(!m_device)
      return BB_FAIL;

   memcpy(&m_oldState, &m_state, sizeof(m_state));

   if(FAILED(m_device->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
      {
         if(FAILED(m_device->Acquire()))
            return BB_FAIL;

         if(FAILED(m_device->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
            return BB_FAIL;
      }

   m_xPos += m_state.lX;
   m_yPos += m_state.lY;
   m_wheelPos = m_state.lZ;

   return BB_SUCCESS;
}


BSTATE DIMouse::isButtonDown(BB_INPUT_BUTTON button)
{
   int b = -1;
   
   if(button == BB_BUTTON_MOUSE_LEFT)
      b = 0;
   else if(button == BB_BUTTON_MOUSE_RIGHT)
      b = 1;

   if(b == -1)
      return BB_FAIL;

   if(m_state.rgbButtons[b] & 0x80)
      return BB_SUCCESS;

   return BB_FAIL;
}


BSTATE DIMouse::isButtonUp(BB_INPUT_BUTTON button)
{
   int b = -1;
   
   if(button == BB_BUTTON_MOUSE_LEFT)
      b = 0;
   else if(button == BB_BUTTON_MOUSE_RIGHT)
      b = 1;

   if(b == -1)
      return BB_FAIL;

   if(!(m_state.rgbButtons[b] & 0x80))
      if(m_oldState.rgbButtons[b] & 0x80)
         return BB_SUCCESS;

   return BB_FAIL;
}


void DIMouse::GetDevicePosition(int *x, int *y, int *wheel)
{
   if(x) *x = (int)m_xPos;
   if(y) *y = (int)m_yPos;
   if(wheel) *wheel = (int)m_wheelPos;
}


END_ENGINE_NAMESPACE