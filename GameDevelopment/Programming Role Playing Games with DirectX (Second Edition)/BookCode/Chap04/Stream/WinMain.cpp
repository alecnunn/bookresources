/**************************************************
WinMain.cpp
Chapter 4 Streaming Playback Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DSOUND.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dsound.h>
#include "resource.h"

// .WAV file header
typedef struct sWaveHeader {
  char  RiffSig[4];         // 'RIFF'
  long  WaveformChunkSize;  // 8
  char  WaveSig[4];         // 'WAVE'
  char  FormatSig[4];       // 'fmt ' (notice space after)
  long  FormatChunkSize;    // 16
  short FormatTag;          // WAVE_FORMAT_PCM
  short Channels;           // # of channels
  long  SampleRate;         // sampling rate
  long  BytesPerSec;        // bytes per second
  short BlockAlign;         // sample block alignment
  short BitsPerSample;      // bits per second
  char  DataSig[4];         // 'data'
  long  DataSize;           // size of waveform data
} sWaveHeader;

// Application variables ////////////////////////
HWND g_hWnd;                    // Window handle
char g_szClass[] = "StreamDemo";  // Class name

IDirectSound8 *g_pDS;             // DirectSound component
IDirectSoundBuffer8 *g_pDSBuffer; // Sound Buffer object
IDirectSoundNotify8 *g_pDSNotify; // Notification object

HANDLE g_hThread;     // Thread handle
DWORD g_ThreadID;     // Thread ID #
HANDLE g_hEvents[4];  // Notification handles
FILE   *g_fp;         // .WAV file handle
long g_Size, g_Pos, g_Left;  // Streaming pointers

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void PlayStreamedSound(char *Filename);
IDirectSoundBuffer8 *CreateBufferFromWAV(FILE *fp, sWaveHeader *Hdr);
BOOL LoadSoundData(IDirectSoundBuffer8 *pDSBuffer, long LockPos, FILE *fp, long Size);
DWORD HandleNotifications(LPVOID lpvoid);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG Msg;
  long i;

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_STREAM), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize and configure DirectSound
  if(FAILED(DirectSoundCreate8(NULL, &g_pDS, NULL))) {
    MessageBox(NULL, "Unable to create DirectSound object", "Error", MB_OK);
    return 0;
  }
  g_pDS->SetCooperativeLevel(g_hWnd, DSSCL_NORMAL);

  // Play a streaming sound
  PlayStreamedSound("test.wav");

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // Stop sound
  g_pDSBuffer->Stop();

  // Kill the thread
  if(g_hThread != NULL) {
    TerminateThread(g_hThread, 0);
    CloseHandle(g_hThread);
  }

  // Release the handles
  for(i=0;i<4;i++)
    CloseHandle(g_hEvents[i]);
    
  // Close the source file
  fclose(g_fp);

  // Release DirectSound objects
  g_pDSBuffer->Release();
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

IDirectSoundBuffer8 *CreateBufferFromWAV(FILE *fp, sWaveHeader *Hdr)
{
  IDirectSoundBuffer *pDSB;
  IDirectSoundBuffer8 *pDSBuffer;
  DSBUFFERDESC dsbd;
  WAVEFORMATEX wfex;

  // read in the header from beginning of file
  fseek(fp, 0, SEEK_SET);
  fread(Hdr, 1, sizeof(sWaveHeader), fp);

  // check the sig fields, returning if an error
  if(memcmp(Hdr->RiffSig, "RIFF", 4) ||                     \
     memcmp(Hdr->WaveSig, "WAVE", 4) ||                      \
     memcmp(Hdr->FormatSig, "fmt ", 4) ||                    \
     memcmp(Hdr->DataSig, "data",4))
  return NULL;

  // setup the playback format
  ZeroMemory(&wfex, sizeof(WAVEFORMATEX));
  wfex.wFormatTag = WAVE_FORMAT_PCM;
  wfex.nChannels = Hdr->Channels;
  wfex.nSamplesPerSec = Hdr->SampleRate;
  wfex.wBitsPerSample = Hdr->BitsPerSample;
  wfex.nBlockAlign = wfex.wBitsPerSample / 8 * wfex.nChannels;
  wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

  // create the sound buffer using the header data
  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
  dsbd.dwBufferBytes = 65536;
  dsbd.lpwfxFormat = &wfex;
  if(FAILED(g_pDS->CreateSoundBuffer(&dsbd, &pDSB, NULL)))
    return NULL;
  
  // get newer interface
  if(FAILED(pDSB->QueryInterface(IID_IDirectSoundBuffer8,     \
                   (void**)&pDSBuffer))) {
    pDSB->Release();
    return NULL;
  }

  // return the interface
  return pDSBuffer;
}

BOOL LoadSoundData(IDirectSoundBuffer8 *pDSBuffer,            \
                   long LockPos, FILE *fp, long Size)
{
  BYTE *Ptr1, *Ptr2;
  DWORD Size1, Size2;

  if(!Size)
    return FALSE;

  // lock the sound buffer at position specified
  if(FAILED(pDSBuffer->Lock(LockPos, Size,                    \
              (void**)&Ptr1, &Size1,                          \
              (void**)&Ptr2, &Size2, 0)))
    return FALSE;

  // read in the data
  fread(Ptr1, 1, Size1, fp);
  if(Ptr2 != NULL)
    fread(Ptr2, 1, Size2, fp);
  
  // unlock it
  pDSBuffer->Unlock(Ptr1, Size1, Ptr2, Size2);

  // return a success
  return TRUE;
}

void PlayStreamedSound(char *Filename)
{
  DSBPOSITIONNOTIFY   dspn[4];
  sWaveHeader Hdr;
  long i;

  // Open the source file
  if((g_fp=fopen(Filename, "rb"))==NULL)
    return;

  // Create a 2 second buffer to stream in wave
  if((g_pDSBuffer = CreateBufferFromWAV(g_fp, &Hdr)) == NULL) {
    fclose(g_fp);
    return;
  }

  // Get streaming size and pointers
  g_Size = Hdr.DataSize;
  g_Pos = sizeof(sWaveHeader);
  g_Left = g_Size;

  // Create a thread for notifications
  if((g_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, NULL, 0, &g_ThreadID)) == NULL)
    return;

  // Create the notification interface
  if(FAILED(g_pDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&g_pDSNotify)))
    return;

  // Create the event handles and set the notifications
  for(i=0;i<4;i++) {
    g_hEvents[i] = CreateEvent(NULL,FALSE,FALSE,NULL);
    dspn[i].dwOffset = 16384 * (i+1) - 1;
    dspn[i].hEventNotify = g_hEvents[i];
  }
  g_pDSNotify->SetNotificationPositions(4, dspn);

  // Fill buffer completely with sound
  fseek(g_fp, sizeof(sWaveHeader), SEEK_SET);
  LoadSoundData(g_pDSBuffer, 0, g_fp, 65536);
  g_Left -= 65536;
  g_Pos  += 65536;

  // Play sound looping
  g_pDSBuffer->SetCurrentPosition(0);
  g_pDSBuffer->SetVolume(DSBVOLUME_MAX);
  g_pDSBuffer->Play(0,0,DSBPLAY_LOOPING);

  return;
}

DWORD HandleNotifications(LPVOID lpvoid)
{
  DWORD dwResult, Num;

  while(1) {
    // Wait for a message
    dwResult = MsgWaitForMultipleObjects(4, g_hEvents,
                                         FALSE, INFINITE, 
                                         QS_ALLEVENTS);

    // Get notification #
    Num = dwResult - WAIT_OBJECT_0;

    // Check for update #
    if(Num >=0 && Num < 4) {
      // Stop sound and quit thread if no more
      if(!g_Left)
        ExitThread(0);

      // Seek to read position in file
      fseek(g_fp, g_Pos, SEEK_SET);

      // Stream in data based on amount of data left
      if(g_Left <= 16384) {
        // If this is reached, then end of sound is coming up
        LoadSoundData(g_pDSBuffer, Num*16384, g_fp, g_Left);
        g_Left = 0;
      } else {
        LoadSoundData(g_pDSBuffer, Num*16384, g_fp, 65536);
        g_Left -= 16384;
        g_Pos  += 16384;
      }
    }
  }

  return 0L;
}
