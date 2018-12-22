/**************************************************
Sound.cpp
Chapter 4 Sound Loading and Initializing Functions

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

IDirectSoundBuffer8 *CreateBufferFromWAV(FILE *fp, sWaveHeader *Hdr);
BOOL LoadSoundData(IDirectSoundBuffer8 *pDSBuffer, long LockPos, FILE *fp, long Size);
IDirectSoundBuffer8 *LoadWAV(char *Filename);

// Application //////////////////////////////////
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
