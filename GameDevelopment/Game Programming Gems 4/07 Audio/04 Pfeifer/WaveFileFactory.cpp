#include "stdafx.h"
#include "wavefile.h"
#include "WaveFileFactory.h"

WaveFileFactory* WaveFileFactory::s_pInstance = NULL;

WaveFileFactory::WaveFileFactory()
{
}


WaveFileFactory::~WaveFileFactory()
{
	WavIterator iWav = m_wavs.begin();
	WavIterator endWav = m_wavs.end();
	
	for (; iWav != endWav; ++iWav)
	{
		delete iWav->second;
	}

	m_wavs.clear();
}


WaveFile* WaveFileFactory::Get(const char* szFileName)
{
	WavIterator found = m_wavs.find(szFileName);
	
	if (found != m_wavs.end())
	{
		return found->second;
	}
	else  // not found, we need to load the wav file
	{
	    WaveFile* pWaveFile = new WaveFile();
	    if (!pWaveFile->Open(szFileName, NULL))
		{
			delete pWaveFile;
			return NULL;
		}

	    WAVEFORMATEX* pwfx = pWaveFile->GetFormat();

		// check for invalid format or too many channels in wave,
	    // sound must be mono and PCM when using DSBCAPS_CTRL3D,
	    if (pwfx == NULL || pwfx->nChannels > 1 || pwfx->wFormatTag != WAVE_FORMAT_PCM) 
	    {
			delete pWaveFile;
	        return NULL;
	    }
		else
		{
			m_wavs.insert(WavMap::value_type(szFileName, pWaveFile));
			return pWaveFile;
		}
	}
}
