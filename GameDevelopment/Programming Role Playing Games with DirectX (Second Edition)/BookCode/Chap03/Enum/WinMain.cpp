/**************************************************
WinMain.cpp
Chapter 3 Input Device Enum Demo

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
char g_szClass[] = "EnumDemo";  // Class name

IDirectInput8 *g_pDI;        // DirectInput component

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK EnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG      Msg;

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ENUM), 0, NULL);

  // Initialize DirectInput and enumerate devices
  DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL);
  g_pDI->EnumDevices(DI8DEVCLASS_ALL, EnumDevices, (LPVOID)g_hWnd, DIEDFL_ALLDEVICES);

  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // Release DirectInput object
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

BOOL CALLBACK EnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef)
{
  // Add device name to list
  SendMessage(GetDlgItem(g_hWnd, IDC_DEVICES), LB_ADDSTRING, 0, (LPARAM)pdInst->tszInstanceName);

  // Continue enumeration
  return DIENUM_CONTINUE;
}
