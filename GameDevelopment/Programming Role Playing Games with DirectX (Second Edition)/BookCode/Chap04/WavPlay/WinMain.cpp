/**************************************************
WinMain.cpp
Chapter 4 Wave Playing Demo

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
char g_szClass[] = "WavPlayDemo";  // Class name

IDirectSound8 *g_pDS;             // DirectSound component
IDirectSoundBuffer8 *g_pDSBuffer; // Sound Buffer object

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

IDirectSoundBuffer8 *CreateBufferFromWAV(FILE *fp, sWaveHeader *Hdr);
BOOL LoadSoundData(IDirectSoundBuffer8 *pDSBuffer, long LockPos, FILE *fp, long Size);
IDirectSoundBuffer8 *LoadWAV(char *Filename);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG Msg;

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_WavPlay), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Initialize and configure DirectSound
  if(FAILED(DirectSoundCreate8(NULL, &g_pDS, NULL))) {
    MessageBox(NULL, "Unable to create DirectSound object", "Error", MB_OK);
    return 0;
  }
  g_pDS->SetCooperativeLevel(g_hWnd, DSSCL_NORMAL);

  // Load a sound to play
  g_pDSBuffer = LoadWAV("test.wav");
  if(g_pDSBuffer != NULL) {
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
  dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
  dsbd.dwBufferBytes = Hdr->DataSize;
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

IDirectSoundBuffer8 *LoadWAV(char *Filename)
{
  IDirectSoundBuffer8 *pDSBuffer;
  sWaveHeader Hdr;
  FILE *fp;

  // open the source file
  if((fp=fopen(Filename, "rb"))==NULL)
    return NULL;

  // create the sound buffer
  if((pDSBuffer = CreateBufferFromWAV(fp, &Hdr)) == NULL) {
    fclose(fp);
    return NULL;
  }

  // read in the data
  fseek(fp, sizeof(sWaveHeader), SEEK_SET);
  LoadSoundData(pDSBuffer, 0, fp, Hdr.DataSize);

  // close the source file
  fclose(fp);

  // return the new sound buffer fully loaded with sound
  return pDSBuffer;
}
