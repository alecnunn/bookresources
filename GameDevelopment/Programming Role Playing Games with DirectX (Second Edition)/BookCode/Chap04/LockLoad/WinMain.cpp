/**************************************************
WinMain.cpp
Chapter 4 Lock Load Playing Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DSOUND.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dsound.h>
#include "resource.h"

// Application variables ////////////////////////
HWND g_hWnd;                    // Window handle
char g_szClass[] = "LockLoadDemo";  // Class name

IDirectSound8 *g_pDS;             // DirectSound component
IDirectSoundBuffer8 *g_pDSBuffer; // Sound Buffer object

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG Msg;
  IDirectSoundBuffer *pDSB = NULL;

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_LOCKLOAD), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize and configure DirectSound
  if(FAILED(DirectSoundCreate8(NULL, &g_pDS, NULL))) {
    MessageBox(NULL, "Unable to create DirectSound object", "Error", MB_OK);
    return 0;
  }
  g_pDS->SetCooperativeLevel(g_hWnd, DSSCL_NORMAL);
  // Create a sound buffer
  // Setup the WAVEFORMATEX structure
  WAVEFORMATEX wfex;
  ZeroMemory(&wfex, sizeof(WAVEFORMATEX));
  wfex.wFormatTag = WAVE_FORMAT_PCM;
  wfex.nChannels = 1;           // mono
  wfex.nSamplesPerSec = 11025;  // 22050hz
  wfex.wBitsPerSample = 16;     // 16-bit
  wfex.nBlockAlign = (wfex.wBitsPerSample / 8) * wfex.nChannels;
  wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

  // Setup the DSBUFFERDESC structure
  DSBUFFERDESC dsbd;
  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));  // zero out structure
  dsbd.dwSize = sizeof(DSBUFFERDESC);       // need to zero-out
  dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
  dsbd.dwBufferBytes = wfex.nAvgBytesPerSec * 2;  // 2 seconds
  dsbd.lpwfxFormat = &wfex;

  // Create the first version object
  if(FAILED(g_pDS->CreateSoundBuffer(&dsbd, &pDSB, NULL))) {
    // Error occurred
    MessageBox(NULL, "Unable to create sound buffer", "Error", MB_OK);
  } else {
    // Get the version 8 interface
    if(FAILED(pDSB->QueryInterface(IID_IDirectSoundBuffer8,     \
                     (void**)&g_pDSBuffer))) {
      // Error occurred - free first interface first 
      // and then do something
      pDSB->Release();
    } else {
      // release the original interface - all a success!
      pDSB->Release();
    }

    // Lock buffer, fill with random values, and unlock
    char *Ptr;
    DWORD Size;
    if(SUCCEEDED(g_pDSBuffer->Lock(0,0,(void**)&Ptr,              \
                    (DWORD*)&Size,NULL,0,DSBLOCK_ENTIREBUFFER))) {
      for(DWORD i=0;i<Size;i++)
        Ptr[i] = rand() % 256;
    }
    g_pDSBuffer->Unlock((void*)Ptr, Size, NULL, 0);

    // Play sound looping
    g_pDSBuffer->SetCurrentPosition(0);
    g_pDSBuffer->SetVolume(DSBVOLUME_MAX);
    g_pDSBuffer->Play(0,0,DSBPLAY_LOOPING);
  }

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
    
  // Release DirectSound objects
  g_pDS->Release();

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
