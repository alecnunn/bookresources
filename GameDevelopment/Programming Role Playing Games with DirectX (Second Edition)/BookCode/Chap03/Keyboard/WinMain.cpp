/**************************************************
WinMain.cpp
Chapter 3 Keyboard Device Demo

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
char g_szClass[] = "KeyboardDemo";  // Class name

IDirectInput8 *g_pDI;             // DirectInput component
IDirectInputDevice8 *g_pDIDevice; // Keyboard device

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

IDirectInputDevice8 *InitKeyboard(HWND hWnd, IDirectInput8 *pDI);
BOOL ReadDevice(IDirectInputDevice8 *pDIDevice, void *DataBuffer, long BufferSize);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG Msg;
  char KeyStateBuffer[256];

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_KEYBOARD), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize DirectInput and get keyboard device
  DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL);
  g_pDIDevice = InitKeyboard(g_hWnd, g_pDI);

  // Loop until ESCAPE pressed
  ZeroMemory(&Msg, sizeof(MSG));
  while(Msg.message != WM_QUIT) {

    // Handle Windows messages (if any)
    if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    } 

    // Read in keyboard and break if ESCAPE pressed
    ReadDevice(g_pDIDevice, (void*)KeyStateBuffer, 256);
    if(KeyStateBuffer[DIK_ESCAPE] & 0x80)
      break;
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
