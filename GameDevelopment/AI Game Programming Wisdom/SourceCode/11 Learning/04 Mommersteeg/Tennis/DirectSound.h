//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    DirectSound.h
//----------------------------------------------------------------------------------------------

#ifndef __DIRECTSOUND_H
#define __DIRECTSOUND_H

//----------------------------------------------------------------------------------------------
// Global type definitions (new version independant names for weird DX identifiers)
//----------------------------------------------------------------------------------------------

typedef LPDIRECTSOUND			LPDS;
typedef LPDIRECTSOUNDBUFFER		LPDSB;
typedef LPDIRECTSOUNDNOTIFY		LPDSN;
typedef DSBUFFERDESC			DSBD;
typedef DSBPOSITIONNOTIFY		DSBPN;

//----------------------------------------------------------------------------------------------
// External declarations
//----------------------------------------------------------------------------------------------

extern LPDS			lpds;

//----------------------------------------------------------------------------------------------
// Exported functions
//----------------------------------------------------------------------------------------------

extern BOOL InitDS(DWORD dwSamplesPerSec = 22050, WORD nChannels = 1, WORD nBitsPerSample = 16);
extern void ShutdownDS();
extern void FillWaveFormat(WAVEFORMATEX * wfx, DWORD dwSamplesPerSec, WORD nChannels, WORD nBitsPerSample);
extern void MatchPrimaryBuffer(WAVEFORMATEX * pwfx);

//----------------------------------------------------------------------------------------------

#endif // __DIRECTSOUND_H
