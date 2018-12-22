/**************************************************
WinMain.cpp
Chapter 4 Midi Playing Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DSOUND.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dsound.h>
#include "dmusici.h"
#include "resource.h"

// Application variables ////////////////////////
HWND g_hWnd;                    // Window handle
char g_szClass[] = "MidiPlayDemo";  // Class name

// DirectSound component
IDirectSound8            *g_pDS = NULL;

// DirectMusic Performance, loader, and segment objects
IDirectMusicPerformance8 *g_pDMPerformance;
IDirectMusicLoader8      *g_pDMLoader;
IDirectMusicSegment8     *g_pDMSegment;

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MidiPlay), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize and configure DirectSound
  if(FAILED(DirectSoundCreate8(NULL, &g_pDS, NULL))) {
    MessageBox(NULL, "Unable to create DirectSound object", "Error", MB_OK);
    return 0;
  }
  g_pDS->SetCooperativeLevel(g_hWnd, DSSCL_NORMAL);

  // Create the DirectMusic loader object
  CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                   IID_IDirectMusicLoader8, (void**)&g_pDMLoader);

  // Create the DirectMusic performance object
  CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                   IID_IDirectMusicPerformance8, (void**)&g_pDMPerformance);

  // Initialize the performance with the standard audio path.
  // This initializes both DirectMusic and DirectSound and 
  // sets up the synthesizer. 
  g_pDMPerformance->InitAudio(NULL, NULL, g_hWnd,
                              DMUS_APATH_SHARED_STEREOPLUSREVERB, 128,
                              DMUS_AUDIOF_ALL, NULL);

  // Tell DirectMusic where the default search path is
  char strPath[MAX_PATH];
  WCHAR wstrSearchPath[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, strPath);
  MultiByteToWideChar(CP_ACP, 0, strPath, -1, wstrSearchPath, MAX_PATH);
  g_pDMLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wstrSearchPath, FALSE);

  // Play some music
  PlaySong("escape.mid");

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
    
  // Release DirectSound objects
  g_pDMSegment->Unload(g_pDS);
  g_pDMLoader->ReleaseObjectByUnknown(g_pDMSegment);
  g_pDMSegment->Release();
  g_pDS->Release();

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
  DMUS_OBJECTDESC dmod;

  // Get the object
  ZeroMemory(&dmod, sizeof(DMUS_OBJECTDESC));
  dmod.dwSize = sizeof(DMUS_OBJECTDESC);
  dmod.guidClass = CLSID_DirectMusicSegment;
  dmod.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH;
  mbstowcs(dmod.wszFileName, Filename, MAX_PATH);
  if(FAILED(g_pDMLoader->GetObject(&dmod, IID_IDirectMusicSegment8, (LPVOID*)&g_pDMSegment)))
    return FALSE;

  // Setup MIDI playing
  if(strstr(Filename, ".mid") != NULL) {
    if(FAILED(g_pDMSegment->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, 0, 0, NULL)))
      return FALSE;
  }

  // Download the band
  if(FAILED(g_pDMSegment->Download(g_pDMPerformance)))
    return FALSE;

  // Set to loop forever
  g_pDMSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);

  // Play on default audio path
  g_pDMPerformance->PlaySegmentEx(g_pDMSegment, NULL, NULL, 0, 0, NULL, NULL, NULL);
    return FALSE;
}
