#include "stdafx.h"
#include "sound.h"
#include "wavefile.h"
#include "wavefilefactory.h"
#include "audiomanager.h"

#include <mmreg.h>

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
// Name: Sound::Sound()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
Sound::Sound(const char* szWaveFile)
:	Audio(Audio::SOUND),
	m_pos(0)
{
	m_pWaveFile = WaveFileFactory::Instance()->Get(szWaveFile);
}


Sound::~Sound()
{
	if (m_pWaveFile)
	{
		m_pWaveFile = NULL;
	}
}


bool Sound::FillBuffer(LPDIRECTSOUNDBUFFER pDSB,
					   DWORD startIndex,
					   DWORD amount,
					   DWORD* pAmtRead)
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

	if (pAmtRead)
		*pAmtRead = 0;

    if (pDSB == NULL)
        return false;

    // Lock the buffer down - assumes length defined by audio manager
    if (FAILED(hr = pDSB->Lock(startIndex, amount, 
                               &pDSLockedBuffer, &dwDSLockedBufferSize, 
                               NULL, NULL, 0L)))
	{
        DXTRACE_ERR(TEXT("Lock"), hr);
		return false;
	}

    // Reset the wave file to the beginning 
    m_pWaveFile->ResetFile();

    if (FAILED( hr = m_pWaveFile->Read((BYTE*)pDSLockedBuffer,
									   m_pos,
                                       dwDSLockedBufferSize, 
                                       &dwWavDataRead)))        
	{
	    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);
		DXTRACE_ERR(TEXT("Read"), hr);
		return false;
	}

    if (dwWavDataRead < amount)
    {
        // Wav is blank, so just fill with silence
        FillMemory((BYTE*) pDSLockedBuffer+dwWavDataRead, dwDSLockedBufferSize-dwWavDataRead-1, 
                   (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
    }

	m_pos += dwWavDataRead;

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

	if (pAmtRead)
		*pAmtRead = dwWavDataRead;

    return (m_pos < m_pWaveFile->GetSize());
}

void Sound::Reset()
{
	// reset the reading position back to 0
	m_pos = 0;

	if (m_pWaveFile)
	{
		m_pWaveFile->ResetFile();
	}
}