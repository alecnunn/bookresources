//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SoundBuffer.h
//----------------------------------------------------------------------------------------------

#ifndef __SOUNDBUFFER_H
#define __SOUNDBUFFER_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "DirectSound.h"

//----------------------------------------------------------------------------------------------
// Defined constants
//----------------------------------------------------------------------------------------------

#define WAVEVERSION 		1
#define ER_MEM 				0xe000
#define ER_CANNOTOPEN 		0xe100
#define ER_NOTWAVEFILE 		0xe101
#define ER_CANNOTREAD 		0xe102
#define ER_CORRUPTWAVEFILE	0xe103
#define ER_CANNOTWRITE		0xe104
#define DSB_DEFAULT			0

//----------------------------------------------------------------------------------------------
// CSoundBuffer: Convenient wrapper class for a DirectSound buffer
//----------------------------------------------------------------------------------------------

class CSoundBuffer {
public:
						CSoundBuffer();
						~CSoundBuffer();

public:
	void				Create(LPDS lpds, DWORD dwBufferSize, DWORD dwFrequency = 22050, WORD nChannels = 2, WORD wBitsPerSample = 8, DWORD dwFlags = DSB_DEFAULT);
	void				CreateWave(LPDS lpds, LPSTR strWave, DWORD dwFlags = DSB_DEFAULT);
	void				CreateCopy(LPDS lpds, LPDSB lpDSB);
	void				CreateDuplicate(LPDS lpds, LPDSB lpDSB);

	void				Release();
	void				ReleaseCopy();

	void				Play(BOOL bLooping = FALSE);
	void				Stop();
	void				Silence();

private:
	int					WaveOpenFile(TCHAR*, HMMIO *, WAVEFORMATEX **, MMCKINFO *);
	int					WaveStartDataRead(HMMIO *, MMCKINFO *, MMCKINFO *);
	int					WaveReadFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *);
	int					WaveCloseReadFile(HMMIO *, WAVEFORMATEX **);
	int					WaveLoadFile(TCHAR*, UINT *, WAVEFORMATEX **, BYTE **);

private:
	WAVEFORMATEX *		pwfx;			  // Wave format info.
	HMMIO				hmmio;            // File handle.
	MMCKINFO			mmckinfo;         // Chunk info.
	MMCKINFO			mmckinfoParent;   // Parent chunk info.

	LPDS				lpDS;
	LPDSB				lpDSB;
};

//----------------------------------------------------------------------------------------------
#endif // __SOUNDBUFFER_H

