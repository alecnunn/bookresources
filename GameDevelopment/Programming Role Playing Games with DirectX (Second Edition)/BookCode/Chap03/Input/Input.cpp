/**************************************************
WinMain.cpp
Chapter 3 Device initialization functions

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DINPUT8.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dinput.h>
#include "resource.h"

IDirectInputDevice8 *InitKeyboard(HWND hWnd, IDirectInput8 *pDI);
IDirectInputDevice8 *InitMouse(HWND hWnd, IDirectInput8 *pDI);
IDirectInputDevice8 *InitJoystick(HWND hWnd, IDirectInput8 *pDI);
BOOL CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);
BOOL ReadDevice(IDirectInputDevice8 *pDIDevice, void *DataBuffer, long BufferSize);

IDirectInputDevice8 *InitKeyboard(HWND hWnd, IDirectInput8 *pDI)
{
  IDirectInputDevice8 *pDIDevice;

  // Create the device object
  if(FAILED(pDI->CreateDevice(GUID_SysKeyboard, &pDIDevice, NULL)))
    return NULL;

  // Set the data format
  if(FAILED(pDIDevice->SetDataFormat(&c_dfDIKeyboard))) {
    pDIDevice->Release();
    return NULL;
  }

  // Set the cooperative mode
  if(FAILED(pDIDevice->SetCooperativeLevel(hWnd,              \
                    DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
    pDIDevice->Release();
    return NULL;
  }

  // Acquire the device for use
  if(FAILED(pDIDevice->Acquire())) {
    pDIDevice->Release(); 
    return NULL;
  }

  // Everything was a success, return the pointer
  return pDIDevice;
}

IDirectInputDevice8 *InitMouse(HWND hWnd, IDirectInput8 *pDI)
{
  IDirectInputDevice8 *pDIDevice;

  // Create the device object
  if(FAILED(pDI->CreateDevice(GUID_SysMouse, &pDIDevice, NULL)))
    return NULL;

  // Set the data format
  if(FAILED(pDIDevice->SetDataFormat(&c_dfDIMouse))) {
    pDIDevice->Release();
    return NULL;
  }

  // Set the cooperative mode
  if(FAILED(pDIDevice->SetCooperativeLevel(hWnd,              \
                    DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
    pDIDevice->Release();
    return NULL;
  }

  // Acquire the device for use
  if(FAILED(pDIDevice->Acquire())) {
    pDIDevice->Release(); 
    return NULL;
  }

  // Everything was a success, return the pointer
  return pDIDevice;
}

IDirectInputDevice8 *InitJoystick(HWND hWnd, IDirectInput8 *pDI)
{
  g_pDI->EnumDevices(DI8DEVTYPE_JOYSTICK, EnumJoysticks,         \
                   NULL, DIEDFL_ATTACHEDONLY);

  // Everything was a success, return the pointer
  return g_pDIDJoystick;
}

BOOL CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef)
{
  DIPROPRANGE dipr;
  DIPROPDWORD dipdw;
  
  g_pDIDJoystick = NULL;

  // Create the device object using global DirectInput object
  if(FAILED(g_pDI->CreateDevice(pdInst->guidInstance,         \
           &g_pDIDJoystick, NULL)))
    return DIENUM_CONTINUE;

  // Set the data format
  if(FAILED(g_pDIDJoystick->SetDataFormat(&c_dfDIJoystick))) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }

  // Set the cooperative mode
  if(FAILED(g_pDIDJoystick->SetCooperativeLevel(g_hWnd,       \
            DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }

  // Clear out the structure first
  ZeroMemory(&dipr, sizeof(DIPROPRANGE));
  dipr.diph.dwSize = sizeof(dipr);
  dipr.diph.dwHeaderSize = sizeof(dipr);

  dipr.diph.dwObj = DIJOFS_X;
  dipr.diph.dwHow = DIPH_BYOFFSET;  // offset into data format

  dipr.lMin = -1024;
  dipr.lMax = 1024;

   if(FAILED(g_pDIDJoystick->SetProperty(DIPROP_RANGE,        \
                                        &dipr.diph))) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }

  dipr.diph.dwObj = DIJOFS_Y;
  if(FAILED(g_pDIDJoystick->SetProperty(DIPROP_RANGE,        \
                                        &dipr.diph))) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }
    
  // Set X deadzone to 15%
  dipdw.diph.dwObj = DIJOFS_X;
  dipdw.dwData = 1500;
  if(FAILED(g_pDIDevice->SetProperty(DIPROP_DEADZONE, &dipdw.diph))) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }

  // Set Y deadzone
  dipdw.diph.dwObj = DIJOFS_Y;
  if(FAILED(g_pDIDevice->SetProperty(DIPROP_DEADZONE, &dipdw.diph))) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }

  // Acquire the device for use
  if(FAILED(g_pDIDJoystick->Acquire())) {
    g_pDIDJoystick->Release();
    g_pDIDJoystick = NULL;
    return DIENUM_CONTINUE;
  }

  // Stop enumeration
  return DIENUM_STOP;
}

BOOL ReadDevice(IDirectInputDevice8 *pDIDevice,               \
                void *DataBuffer, long BufferSize)
{
  HRESULT hr;

  while(1) {
    // Poll device
    g_pDIDevice->Poll();

    // Read in state
    if(SUCCEEDED(hr = g_pDIDevice->GetDeviceState(BufferSize,(LPVOID)DataBuffer)))
      break;

    // Return on an unknown error  
    if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
      return FALSE;

    // Reacquire and try again
    if(FAILED(g_pDIDevice->Acquire()))
      return FALSE;
  }

  // Return a success
  return TRUE;
}
