//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    DirectSound.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DirectSound.h"
#include <assert.h>

//----------------------------------------------------------------------------------------------
// The following functions are from some old, long forgotten personal DX library. I couldn't 
// find the courage to document them properly, but take a look if you like...
//----------------------------------------------------------------------------------------------


extern HWND		g_hWnd;
LPDS			lpds = NULL;

BOOL InitDS(DWORD dwSamplesPerSec, WORD nChannels, WORD nBitsPerSample) {
	HRESULT	dsrval;

	// create DirectSound object
	dsrval = DirectSoundCreate(NULL, &lpds, 0);
	if (dsrval != DD_OK)
        return FALSE;

	dsrval = lpds->SetCooperativeLevel(g_hWnd, DSSCL_PRIORITY );
	if (dsrval != DD_OK)
        return FALSE;

	// set format for primary sound buffer
	WAVEFORMATEX wfx;
	FillWaveFormat(&wfx, dwSamplesPerSec, nChannels, nBitsPerSample);
	MatchPrimaryBuffer( &wfx );

	return TRUE;
}

void ShutdownDS() {
	if ( lpds != NULL ) {
		lpds->Release();
		lpds = NULL;
	}
}

void FillWaveFormat(WAVEFORMATEX * wfx, DWORD dwSamplesPerSec, WORD nChannels, WORD nBitsPerSample)
{
	assert( nBitsPerSample % 8 == 0 );
	assert( nChannels == 1 || nChannels == 2 );
	wfx->wFormatTag = WAVE_FORMAT_PCM;
	wfx->nChannels = nChannels;
	wfx->nSamplesPerSec = dwSamplesPerSec;
	wfx->nBlockAlign = nChannels * nBitsPerSample / 8;
	wfx->nAvgBytesPerSec = wfx->nBlockAlign * dwSamplesPerSec;
	wfx->wBitsPerSample = nBitsPerSample;
	wfx->cbSize = 0;
}

void MatchPrimaryBuffer(WAVEFORMATEX * pwfx)
{
	assert( lpds != NULL );
	assert( pwfx != NULL );

	LPDSB lpDSB;
	DSBD dsbd;

	// set up primary buffer description
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;

	// create primary buffer
	lpds->CreateSoundBuffer(&dsbd, &lpDSB, NULL);

	// set primary format
	lpDSB->SetFormat( pwfx );

	// release reference to primary format
	lpDSB->Release();
}

