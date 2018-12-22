/**************************************************
WinMain.cpp
Chapter 4 MP3 Playing Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  STRMIIDS.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dshow.h>
#include "resource.h"

// Application variables ////////////////////////
HWND g_hWnd;                    // Window handle
char g_szClass[] = "MP3PlayDemo";  // Class name

// DirectShow components
IGraphBuilder *g_pDSGraphBuilder = NULL;
IMediaControl *g_pDSMediaControl = NULL;
IMediaEvent   *g_pDSMediaEvent   = NULL;

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL PlaySong(char *Filename);


// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG Msg;

  // Initialize COM
  CoInitialize(0);

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MP3Play), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize and configure DirectShow
  if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&g_pDSGraphBuilder))) {
    MessageBox(NULL, "Unable to create DirectShow Graph Builder object", "Error", MB_OK);
    return FALSE;
  }
  
  
  // Query for the media control and event objects
  g_pDSGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&g_pDSMediaControl);
  g_pDSGraphBuilder->QueryInterface(IID_IMediaEvent,   (void**)&g_pDSMediaEvent);

  // Play some music
  PlaySong("escape.mp3");

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);

    // Get the status of the song.
    // If it's done, exit program
    long Event, Param1, Param2;
    if(SUCCEEDED(g_pDSMediaEvent->GetEvent(&Event, &Param1, &Param2, 1))) {
      if(Event == EC_COMPLETE) {
        g_pDSMediaEvent->FreeEventParams(Event, Param1, Param2);
        break;
      }
    }
    g_pDSMediaEvent->FreeEventParams(Event, Param1, Param2);
  }
    
  // Stop music and release DirectShow objects
  g_pDSMediaControl->Stop();
  g_pDSMediaEvent->Release();   g_pDSMediaEvent   = NULL;
  g_pDSMediaControl->Release(); g_pDSMediaControl = NULL;
  g_pDSGraphBuilder->Release(); g_pDSGraphBuilder = NULL;

  // Clean up
  UnregisterClass(g_szClass, hInst);

  // Release COM system
  CoUninitialize();

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

BOOL PlaySong(char *Filename)
{
  // Convert filename to wide-character string
  WCHAR wFilename[MAX_PATH];
  mbstowcs(wFilename, Filename, MAX_PATH);

  // Render the file
  g_pDSGraphBuilder->RenderFile(wFilename, NULL);

  // Play the file
  g_pDSMediaControl->Run();

  return TRUE;
}
