/**************************************************
WinMain.cpp
Chapter 3 Joystick Device Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DINPUT8.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dinput.h>
#include "resource.h"

// Application variables ////////////////////////
HWND g_hWnd;                    // Window handle
char g_szClass[] = "JoystickDemo";  // Class name

IDirectInput8 *g_pDI;             // DirectInput component
IDirectInputDevice8 *g_pDIDJoystick; // Enum Joystick device
IDirectInputDevice8 *g_pDIDevice; // Joystick device

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

IDirectInputDevice8 *InitJoystick(HWND hWnd, IDirectInput8 *pDI);
BOOL ReadDevice(IDirectInputDevice8 *pDIDevice, void *DataBuffer, long BufferSize);
BOOL CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG Msg;
  DIJOYSTATE JoyState;
  char Text[256];

  // Register window class
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WindowProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = DLGWINDOWEXTRA;
  wc.hInstance     = hInst;
  wc.hIcon         = LoadIcon(hInst, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = g_szClass;
  RegisterClass(&wc);

  // Create the dialog box window and show it
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_Joystick), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize DirectInput and get Joystick device
  DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL);
  g_pDIDevice = InitJoystick(g_hWnd, g_pDI);

  if(g_pDIDevice != NULL) {
    // Enter message loop
    ZeroMemory(&Msg, sizeof(MSG));
    while(Msg.message != WM_QUIT) {

      // Handle Windows messages (if any)
      if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
      } 

      // Read in Joystick and display coordinates
      ReadDevice(g_pDIDevice, (void*)&JoyState, sizeof(DIJOYSTATE));
      sprintf(Text, "%ld, %ld", JoyState.lX, JoyState.lY);
      SetWindowText(GetDlgItem(g_hWnd, IDC_COORDINATES), Text);
    }
  } else {
    MessageBox(g_hWnd, "No Joysticks!", "Error", MB_OK);
    return 0;
  }
   
  // Release DirectInput objects
  g_pDIDevice->Unacquire();
  g_pDIDevice->Release();
  g_pDI->Release();

  // Clean up
  UnregisterClass(g_szClass, hInst);

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

IDirectInputDevice8 *InitJoystick(HWND hWnd, IDirectInput8 *pDI)
{
  g_pDI->EnumDevices(DI8DEVTYPE_JOYSTICK, EnumJoysticks,         \
                   NULL, DIEDFL_ATTACHEDONLY);

  // Everything was a success, return the pointer
  return g_pDIDJoystick;
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
