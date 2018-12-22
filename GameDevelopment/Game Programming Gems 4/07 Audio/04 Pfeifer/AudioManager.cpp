#include "stdafx.h"
#include <math.h>
#include "audiotag.h"
#include "sound3D.h"
#include "music.h"
#include "audiomanager.h"
#include "worldobject.h"
#include "camera.h"
#include "wavefile.h"
#include "wavefilefactory.h"

// XML setup
#import "msxml4.dll"  
#include <msxml2.h>
using namespace MSXML2;

#include <mmreg.h>

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define CLAMP(var, min, max)   (((var) < (min)) ? (var) = (min) : ((var) > (max)) ? (var) = (max) : (var))

AudioManager* AudioManager::s_pInstance = NULL;  // singleton instance


//-----------------------------------------------------------------------------
// Name: AudioManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
AudioManager::AudioManager()
:	m_pDS(NULL),
	m_pDSBPrimary(NULL),
	m_pDSListener(NULL),
	m_pListenerCam(NULL),
	m_frequency(22050),
	m_bitRate(16),
	m_hrtf(HRTF_FULL),
	m_distanceFactor(DS3D_DEFAULTDISTANCEFACTOR),
	m_rolloffFactor(DS3D_DEFAULTROLLOFFFACTOR),
	m_dopplerFactor(DS3D_DEFAULTDOPPLERFACTOR),
	m_volume(DSBVOLUME_MAX-2000)
{
	ZeroMemory(m_effectBufs, sizeof(Buffer)*MAX_EFFECT_BUFFERS);
	ZeroMemory(m_musicBufs, sizeof(Buffer)*MAX_MUSIC_BUFFERS);

	new WaveFileFactory();
}


AudioManager::~AudioManager()
{
	int i;

	for (i = 0; i < MAX_EFFECT_BUFFERS; ++i)
	{
		delete m_effectBufs[i].pAudio;
		SAFE_RELEASE(m_effectBufs[i].pDSBuf);
		SAFE_RELEASE(m_effectBufs[i].pDSBuf3D);
	}

	for (i = 0; i < MAX_MUSIC_BUFFERS; ++i)
	{
		delete m_musicBufs[i].pAudio;
		SAFE_RELEASE(m_musicBufs[i].pDSBuf);
	}

	ClearAudioTags();

	m_pListenerCam = NULL;

	delete WaveFileFactory::Instance();

    SAFE_RELEASE(m_pDSListener);
    SAFE_RELEASE(m_pDSBPrimary);
    SAFE_RELEASE(m_pDS); 
}



//-----------------------------------------------------------------------------
// Name: AudioManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
bool AudioManager::Init(HWND  hWnd, 
                        DWORD dwPrimaryChannels, 
                        DWORD dwPrimaryFreq, 
                        DWORD dwPrimaryBitRate,
						HRTF hrtf)
{
    HRESULT hr;
	int i;

	m_frequency = dwPrimaryFreq;
	m_bitRate = dwPrimaryBitRate;
	m_hrtf = hrtf;

    SAFE_RELEASE(m_pDS);

    // Create IDirectSound using the primary sound device
    if (FAILED(hr = DirectSoundCreate8(NULL, &m_pDS, NULL)))
	{
        DXTRACE_ERR(TEXT("AudioManager::Init, DirectSoundCreate8"), hr);
		return false;
	}

    // Set DirectSound coop level 
    if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
        DXTRACE_ERR(TEXT("AudioManager::Init, SetCooperativeLevel"), hr);
		return false;
	}
    
    DSBUFFERDESC dsbdesc;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
    
	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbdesc, &m_pDSBPrimary, NULL)))
	{
        DXTRACE_ERR(TEXT("AudioManager::Init, CreateSoundBuffer"), hr);
		return false;
	}

    if (FAILED(hr = m_pDSBPrimary->QueryInterface(IID_IDirectSound3DListener, 
                                                  (VOID**)&m_pDSListener)))
	{
		DXTRACE_ERR(TEXT("AudioManager::Init, QueryInterface"), hr );
		return false;
	}

    SetPrimaryBufferFormat(dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate);

	for (i = 0; i < MAX_EFFECT_BUFFERS; ++i)
	{
		CreateBuffer(&(m_effectBufs[i]), EFFECT_BUFFER_SIZE, true);
	}

	for (i = 0; i < MAX_MUSIC_BUFFERS; ++i)
	{
		CreateBuffer(&(m_musicBufs[i]), MUSIC_BUFFER_SIZE, false);
	}

    return true;
}




//-----------------------------------------------------------------------------
// Name: AudioManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT AudioManager::SetPrimaryBufferFormat(DWORD dwPrimaryChannels, 
                                             DWORD dwPrimaryFreq, 
                                             DWORD dwPrimaryBitRate )
{
    HRESULT hr;

    if (NULL == m_pDS || NULL == m_pDSBPrimary)
        return CO_E_NOTINITIALIZED;

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if( FAILED(hr = m_pDSBPrimary->SetFormat(&wfx) ) )
		return DXTRACE_ERR( TEXT("SetFormat"), hr );

    return S_OK;
}


bool AudioManager::LoadAudioTags(const char* szFileName)
{
	IXMLDOMDocumentPtr	pDOMDoc;
	_variant_t			varUrl;
	_variant_t			varbSuccess(true);
	HRESULT				hRes;

	hRes = pDOMDoc.CreateInstance("Msxml2.DOMDocument.4.0");

	if (SUCCEEDED(hRes))
	{
		varUrl.SetString(szFileName);

        pDOMDoc->put_validateOnParse(true);
		
        pDOMDoc->put_async(false);

		varbSuccess = pDOMDoc->load(varUrl);

		if ((bool)varbSuccess)
		{
			IXMLDOMNodeListPtr	pNodeList;
			IXMLDOMElementPtr	pDOMElement;
			_variant_t			varAttribute;
			std::string			strTagName;
			AudioTag*			pTag;

			pNodeList = pDOMDoc->documentElement->GetchildNodes();
			
			for (int i = 0; i < pNodeList->Getlength(); ++i)
			{
				pDOMElement = pNodeList->Getitem(i);
				strTagName = pDOMElement->GettagName();
				pTag = NULL;

				if ( stricmp("Effect", strTagName.c_str()) == 0)
				{
					pTag = new AudioEffectTag();
				}
				else if ( stricmp("Music", strTagName.c_str()) == 0)
				{
					pTag = new AudioMusicTag();
				}
				else if ( stricmp("Ambient", strTagName.c_str()) == 0)
				{
					pTag = new AudioAmbientTag();
				}
				else if ( stricmp("Composition", strTagName.c_str()) == 0)
				{
					pTag = new AudioCompositionTag();
				}
				else if ( stricmp("Group", strTagName.c_str()) == 0)
				{
					pTag = new AudioGroupTag();
				}
				else if ( stricmp("Random", strTagName.c_str()) == 0)
				{
					pTag = new AudioRandomTag();
				}
				else
				{
					ASSERT(!"Bad Audio Tag");
				}

				if (pTag)
				{
					if (!pTag->LoadTag(pDOMElement))
					{
						delete pTag;
						return false;
					}

					varAttribute = pDOMElement->getAttribute("ID");
					if (VT_NULL == varAttribute.vt)
					{
						delete pTag;
						return false;
					}

					strTagName = (_bstr_t)varAttribute;
					m_tags.insert(AudioTagMap::value_type(strTagName.c_str(), pTag));
				}
			}
		}
		else 
		{
			MessageBox(NULL, "Unable to process the XML document.", "Error", MB_OK);
			return false;
		}
	}

	return true;
}


void AudioManager::ClearAudioTags()
{
	AudioTagIterator iTag = m_tags.begin();
	AudioTagIterator endTag = m_tags.end();

	for (; iTag != endTag; ++iTag)
	{
		delete iTag->second;
	}

	m_tags.clear();
}

AudioTag* AudioManager::GetAudioTag(const char* szTagName)
{
	AudioTagIterator foundTag = m_tags.find(szTagName);

	if (foundTag != m_tags.end())
	{
		return foundTag->second;
	}
	else
	{
		return NULL;
	}
}


bool AudioManager::CreateBuffer(Buffer* pBuf, DWORD size, bool b3D)
{
	ASSERT(size >= DSBSIZE_MIN && size <= DSBSIZE_MAX);
	ASSERT(b3D ? (size >= (DSBSIZE_FX_MIN * m_frequency * (m_bitRate / 8)) / 1000) : true);

	HRESULT hr;
    DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwBufferBytes   = size;
	dsbd.dwFlags         = DSBCAPS_GLOBALFOCUS | 
						   DSBCAPS_CTRLVOLUME |
						   DSBCAPS_GETCURRENTPOSITION2;
	wfx.wFormatTag = WAVE_FORMAT_PCM;

	if (b3D)
	{
		dsbd.dwFlags    |= DSBCAPS_CTRL3D | 
						   DSBCAPS_CTRLFX |
						   DSBCAPS_CTRLFREQUENCY;
		switch (m_hrtf)
		{
			case HRTF_FULL: dsbd.guid3DAlgorithm = DS3DALG_HRTF_FULL; break;
			case HRTF_LIGHT: dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT; break;
			case HRTF_NONE: dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION ; break;
			default: ASSERT(false); break;
		}
		wfx.nChannels = 1; 
	}
	else
	{
		dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
		wfx.nChannels       = 2; 
	}

    wfx.nSamplesPerSec  = m_frequency; 
    wfx.wBitsPerSample  = m_bitRate; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;
    dsbd.lpwfxFormat     = &wfx; 

	pBuf->volume = m_volume;

    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &(pBuf->pDSBuf), NULL)))
	{
        DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);
		return false;
	}

	pBuf->pDSBuf3D = 0;
	if (b3D)
	{
		if (FAILED(hr = pBuf->pDSBuf->QueryInterface(IID_IDirectSound3DBuffer, (VOID**)&(pBuf->pDSBuf3D))))
		{
			DXTRACE_ERR(TEXT("QueryInterface for IDirectSound3DBuffer"), hr);
			return false;
		}
	}

	return true;
}

bool AudioManager::RestoreBuffer(Buffer* pBuf)
{
    if( !pBuf || !pBuf->pDSBuf)
        return true;

    HRESULT hr;
    DWORD dwStatus;

    if(FAILED(hr = pBuf->pDSBuf->GetStatus(&dwStatus)))
	{
        DXTRACE_ERR(TEXT("GetStatus"), hr);
		return false;
	}


    if (dwStatus & DSBSTATUS_BUFFERLOST)
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pBuf->pDSBuf->Restore();
            if (hr == DSERR_BUFFERLOST)
                Sleep( 10 );
        }
        while (hr = pBuf->pDSBuf->Restore());

		return true;
    }
    else
    {
        return false;
    }
}


void AudioManager::Play(const char* szTagName,
						WorldObject* pObj,
						int msDuration,
						int msDelay,
						IAudioListener* pNotify)
{
	AudioTag* pTag = GetAudioTag(szTagName);
	ASSERT(pTag);

	if (pTag)
	{
		if (msDelay > 0)
		{
			AudioWaitingToBePlayed wait;
			wait.pTag = pTag;
			wait.msDuration = msDuration;
			wait.pObj = pObj;
			wait.msDelay = msDelay;
			wait.pNotify = pNotify;
			m_toBePlayed.push_back(wait);
		}
		else
		{
			Play(pTag, pObj, msDuration, msDelay, pNotify);
		}
	}
}

void AudioManager::Play(AudioTag* pTag,
						WorldObject* pObj,
						int msDuration,
						int msDelay,
						IAudioListener* pNotify)
{
	ASSERT(pTag);

	if (msDelay > 0)
	{
		AudioWaitingToBePlayed wait;
		wait.pTag = pTag;
		wait.msDuration = msDuration;
		wait.pObj = pObj;
		wait.msDelay = msDelay;
		wait.pNotify = pNotify;
		m_toBePlayed.push_back(wait);
	}
	else
	{
		Audio* pAudio = pTag->CreateAudio(pObj, msDuration, msDelay, pNotify);
		
		if (pAudio)			// some tags don't create any actual audio
		{
			switch (pAudio->GetType())
			{
				case Audio::SOUND:
				{
					Sound* pSound = (Sound*)(pAudio);
					return;
				}

				case Audio::SOUND3D:
				{
					Sound3D* pSound3D = (Sound3D*)(pAudio);
					if (pObj)
					{
						pSound3D->SetWorldObject(pObj);
					}

					PlayEffect(pTag, pSound3D, msDuration);
					return;
				}

				case Audio::MUSIC:
				{
					Music* pMusic = (Music*)(pAudio);
					PlayMusic(pTag, pMusic, msDuration);
					return;
				}

				default:
					ASSERT(!"Bad Audio type");
					break;
			}
		}
	}
}

void AudioManager::PlayEffect(AudioTag* pTag, Sound3D* pSound3D,
							  int msDuration)
{
	int availableIndex = -1;
	int lowPriority = pTag->GetPriority();
	int lowestPriorityIndex = -1;
	int i, numTimesPlaying = 0;
	WaveFile* pWaveFile = pSound3D->GetWaveFile();

	// pick the appropriate buffer to use
	for (i = 0; i < MAX_EFFECT_BUFFERS && -1 == availableIndex; ++i)
	{
		if (m_effectBufs[i].pTag == pTag)
		{
			++numTimesPlaying;
		}

		// try and find one that's not currently playing
		if (!m_effectBufs[i].bPlaying)
		{
			availableIndex = i;
		}
		else
		{
			// otherwise, if we can't find an empty slot, find one based on the 
			// priority of the audio tag

			// todo add another test, in case of multiple lower priority choices
			// like the buffer that's closest to completion, or check for similar sounds first
			if (m_effectBufs[i].pTag->GetPriority() <= lowPriority)
			{
				lowPriority = m_effectBufs[i].pTag->GetPriority();
				lowestPriorityIndex = i;
			}
		}
	}

	if (-1 == availableIndex)
	{
		if (-1 == lowestPriorityIndex)
		{
			// sound is not high enough priority to play, so quit
			return;
		}
		else
		{
			StopBuffer(&m_effectBufs[lowestPriorityIndex]);
			availableIndex = lowestPriorityIndex;
		}
	}

	if (numTimesPlaying >= ((AudioEffectTag*)pTag)->GetCascadeNumber())
	{
		for (i = 0; i < MAX_EFFECT_BUFFERS; ++i)
		{
			if (m_effectBufs[i].pTag == pTag)
			{
				StopBuffer(&m_effectBufs[i]);
			}
		}

		Play(((AudioEffectTag*)pTag)->GetCascadeTag(), pSound3D->GetWorldObject());
		delete pSound3D; // not going to play this sound, clean it up
	}
	else 
	{
		// get rid of old Audio object from last use
		delete m_effectBufs[availableIndex].pAudio;

		// Load wavefile into sound buffer
		// Set up 3d data, position and velocity
		m_effectBufs[availableIndex].pDSBuf->SetCurrentPosition(0);
		m_effectBufs[availableIndex].pAudio = pSound3D;
		m_effectBufs[availableIndex].pTag = pTag;
		m_effectBufs[availableIndex].volume = m_volume + pTag->GetVolumeAdjust();
		CLAMP(m_effectBufs[availableIndex].volume, DSBVOLUME_MIN, DSBVOLUME_MAX);
		m_effectBufs[availableIndex].curVolume = m_effectBufs[availableIndex].volume;
		m_effectBufs[availableIndex].volumeTransitionAdjust = 0;
		m_effectBufs[availableIndex].pDSBuf->SetVolume(m_effectBufs[availableIndex].volume);
		m_effectBufs[availableIndex].bPlaying = true;
		m_effectBufs[availableIndex].dwLastWritePos = 0;
		m_effectBufs[availableIndex].dwNextWritePos = EFFECT_INITIAL_READ_SIZE;
		m_effectBufs[availableIndex].pDSBuf3D->SetMaxDistance(10000.0f, DS3D_DEFERRED);
		m_effectBufs[availableIndex].pDSBuf3D->SetMinDistance(100.0f, DS3D_DEFERRED);
		m_effectBufs[availableIndex].msDuration = msDuration;
		m_effectBufs[availableIndex].msLifetime = 0;

		if (pSound3D->GetWorldObject())
		{
			D3DXVECTOR3 v(pSound3D->GetWorldObject()->GetPosition());
			m_effectBufs[availableIndex].pDSBuf3D->SetPosition(v.x, v.y, v.z, DS3D_DEFERRED);
			v = pSound3D->GetWorldObject()->GetVelocity();
			m_effectBufs[availableIndex].pDSBuf3D->SetVelocity(v.x, v.y, v.z, DS3D_DEFERRED);
			m_pDSListener->CommitDeferredSettings();
		}

		// todo check if wav file doesn't need loading into buf
		m_effectBufs[availableIndex].bMoreInBuffer = 
			pSound3D->FillBuffer(m_effectBufs[availableIndex].pDSBuf,
								 0, EFFECT_INITIAL_READ_SIZE,
								 &(m_effectBufs[availableIndex].dwNextWritePos));

		m_effectBufs[availableIndex].pDSBuf->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void AudioManager::PlayMusic(AudioTag* pTag, Music* pMusic, int msDuration)
{
	int availableIndex = -1;
	int lowPriority = pTag->GetPriority();
	int lowestPriorityIndex = -1;
	int fadeOutMusicIndex = -1;

	// pick the appropriate buffer to use
	for (int i = 0; i < MAX_MUSIC_BUFFERS && -1 == availableIndex; ++i)
	{
		// if one music tag is already playing, 
		// prepare to fade it out
		// try and find one that's not currently playing
		if (!m_musicBufs[i].bPlaying)
		{
			availableIndex = i;
		}
		else
		{
			// otherwise, if we can't find an empty slot, find one based on the 
			// priority of the audio tag

			// todo add another test, in case of multiple lower priority choices
			// like the buffer that's closest to completion, or check for similar sounds first
			if (m_musicBufs[i].pTag->GetPriority() < lowPriority)
			{
				lowPriority = m_musicBufs[i].pTag->GetPriority();
				lowestPriorityIndex = i;
			}

			// fade out the other playing music if the new one plays
			fadeOutMusicIndex = i;
		}
	}

	if (-1 == availableIndex)
	{
		if (-1 == lowestPriorityIndex)
		{
			// music is not high enough priority to play, so quit
			return;
		}
		else
		{
			StopBuffer(&m_musicBufs[lowestPriorityIndex]);
			availableIndex = lowestPriorityIndex;
		}
	}

	delete m_musicBufs[availableIndex].pAudio;
	m_musicBufs[availableIndex].pAudio = pMusic;
	m_musicBufs[availableIndex].pTag = pTag;
	m_musicBufs[availableIndex].volume = m_volume + pTag->GetVolumeAdjust();
	CLAMP(m_musicBufs[availableIndex].volume, DSBVOLUME_MIN, DSBVOLUME_MAX);
	m_musicBufs[availableIndex].bMoreInBuffer = 
		m_musicBufs[availableIndex].pAudio->FillBuffer(m_musicBufs[availableIndex].pDSBuf, 0, MUSIC_INITIAL_READ_SIZE);
	m_musicBufs[availableIndex].pDSBuf->SetCurrentPosition(0);
	m_musicBufs[availableIndex].bPlaying = true;
	m_musicBufs[availableIndex].dwLastWritePos = 0;
	m_musicBufs[availableIndex].dwNextWritePos = MUSIC_INITIAL_READ_SIZE;
	m_musicBufs[availableIndex].msDuration = msDuration;
	m_musicBufs[availableIndex].msLifetime = 0;
	m_musicBufs[availableIndex].pDSBuf->Play(0, 0, DSBPLAY_LOOPING);

	if (fadeOutMusicIndex >= 0)
	{
		unsigned int totalTime = ((Music*)(m_musicBufs[fadeOutMusicIndex].pAudio))->GetTotalTime();
		float timeLeft = ((float)totalTime) - ((float)(m_musicBufs[fadeOutMusicIndex].msLifetime));
		if (timeLeft > 0.25f)
		{	// take into account delay in updating by a max of a quarter second
			timeLeft -= 0.25;
			m_musicBufs[availableIndex].volumeTransitionAdjust = ((float)(m_musicBufs[availableIndex].volume-DSBVOLUME_MIN))/timeLeft;
			m_musicBufs[availableIndex].curVolume = DSBVOLUME_MIN;
			m_musicBufs[fadeOutMusicIndex].volume = DSBVOLUME_MIN;
			m_musicBufs[fadeOutMusicIndex].volumeTransitionAdjust = -m_musicBufs[availableIndex].volumeTransitionAdjust;
		}
		else
		{
			m_musicBufs[availableIndex].curVolume = m_musicBufs[availableIndex].volume;
			m_musicBufs[availableIndex].volumeTransitionAdjust = 0;
		}
	}
	else
	{
		m_musicBufs[availableIndex].curVolume = m_musicBufs[availableIndex].volume;
		m_musicBufs[availableIndex].volumeTransitionAdjust = 0;
	}
	
	m_musicBufs[availableIndex].pDSBuf->SetVolume(m_musicBufs[availableIndex].curVolume);
}


void AudioManager::UpdateBuffer(int msElapsed, 
								Buffer* pBuf,
								DWORD dwBufSize,
								DWORD dwWriteAmt)
{
	ASSERT(pBuf && pBuf->pDSBuf);

	if (!pBuf->bPlaying)
		return;

	pBuf->msLifetime += msElapsed;

	DWORD dwPlayPos = 0;
	pBuf->pDSBuf->GetCurrentPosition(&dwPlayPos, NULL);

	// if duration is set to be longer than the sound, reset the next
	// write pos. This could be optimized for short effects, because most of
	// them will fit into the buffer at once, but it currently re-streams
	// the sound in the buffer to handle effects and music
	if (0 != pBuf->msDuration && pBuf->msLifetime < pBuf->msDuration)
	{
		if (!pBuf->bMoreInBuffer && 
			(dwPlayPos >= pBuf->dwLastWritePos && dwPlayPos <= pBuf->dwLastWritePos + dwWriteAmt))
		{
			pBuf->dwLastWritePos = 0;
			pBuf->dwNextWritePos = dwWriteAmt;
			pBuf->bMoreInBuffer = true;
			pBuf->pAudio->Reset();
		}
	}

	// stream in next section of the sound into the buffer, if we've
	// passed the threshold point (one chunk before the end)
	if (pBuf->bMoreInBuffer)
	{
		if (dwPlayPos >= pBuf->dwLastWritePos && dwPlayPos <= pBuf->dwLastWritePos + dwWriteAmt)
		{
			// update the last position we wrote to the buffer
			pBuf->dwLastWritePos = pBuf->dwNextWritePos;

			// fill in more of the buffer
			DWORD dwFinalRead = 0;
			pBuf->bMoreInBuffer = pBuf->pAudio->FillBuffer(pBuf->pDSBuf, pBuf->dwLastWritePos, dwWriteAmt, &dwFinalRead);

			pBuf->dwNextWritePos += dwFinalRead;
			if (pBuf->dwNextWritePos >= dwBufSize)
				pBuf->dwNextWritePos = 0;

			if (!pBuf->bMoreInBuffer)	// no more sound so change over to non-looping playback
			{
				pBuf->pDSBuf->Play(0, 0, 0); 
				if (Audio::MUSIC == pBuf->pAudio->GetType())
				{
					pBuf->pAudio->Finish();
					delete pBuf->pAudio;
					pBuf->pAudio = 0;
				}
			}
		}
	}
	else
	{
		if (dwPlayPos >= pBuf->dwNextWritePos && 
			dwPlayPos > pBuf->dwLastWritePos)
		{
			// there's no more to be read and we're past the end
			StopBuffer(pBuf);
		}
	}

	// update sound properties
	if (pBuf->bPlaying)
	{
		if (pBuf->pDSBuf3D && pBuf->pAudio && pBuf->pAudio->GetType() == Audio::SOUND3D)
		{
			Sound3D* pSound3D = (Sound3D*)(pBuf->pAudio);
			if (pSound3D->GetWorldObject())
			{
				D3DXVECTOR3 pos(pSound3D->GetWorldObject()->GetPosition());
				D3DXVECTOR3 vel(pSound3D->GetWorldObject()->GetVelocity());

				// assumes settings are commited elswhere, in Update()
				pBuf->pDSBuf3D->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
				pBuf->pDSBuf3D->SetVelocity(vel.x, vel.y, vel.z, DS3D_DEFERRED);
			}
		}
		else if (pBuf->pAudio && pBuf->pAudio->GetType() == Audio::MUSIC)
		{
			if ((pBuf->volumeTransitionAdjust < 0 && pBuf->curVolume > pBuf->volume) ||
				(pBuf->volumeTransitionAdjust > 0 && pBuf->curVolume < pBuf->volume))
			{
				pBuf->curVolume += (int)(pBuf->volumeTransitionAdjust*((float)msElapsed));
				if ((pBuf->volumeTransitionAdjust < 0 && pBuf->curVolume <= pBuf->volume) ||
					(pBuf->volumeTransitionAdjust > 0 && pBuf->curVolume >= pBuf->volume))
				{
					pBuf->curVolume = pBuf->volume;
					pBuf->volumeTransitionAdjust = 0;
				}
				CLAMP(pBuf->curVolume, DSBVOLUME_MIN, DSBVOLUME_MAX);
				pBuf->pDSBuf->SetVolume(pBuf->curVolume);
			}
		}
	}
}


void AudioManager::UpdateListener()
{
	// update listener position
	if (m_pListenerCam)
	{
		DS3DLISTENER listenerInfo;
		listenerInfo.dwSize = sizeof(DS3DLISTENER);
		listenerInfo.vPosition = m_pListenerCam->GetPosition();
		// currently Camera does not keep track of velocity
		listenerInfo.vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		listenerInfo.vOrientFront = m_pListenerCam->GetFront();
		listenerInfo.vOrientTop = m_pListenerCam->GetUp();
		listenerInfo.flDistanceFactor = m_distanceFactor;
		listenerInfo.flRolloffFactor = m_rolloffFactor;
		listenerInfo.flDopplerFactor = m_dopplerFactor;

		// assumes settings are commited elswhere, in Update()
		m_pDSListener->SetAllParameters(&listenerInfo, DS3D_DEFERRED);
	}
}


void AudioManager::Update(int msElapsed)
{
	int i;
	AudioWaitingIter iToBePlayed = m_toBePlayed.begin();
	AudioWaitingIter endToBePlayed = m_toBePlayed.end();

	// update delayed sounds to see if we should play any yet.
	while (iToBePlayed != endToBePlayed)
	{
		if (iToBePlayed->msDelay <= msElapsed)
		{
			Play(iToBePlayed->pTag, iToBePlayed->pObj, iToBePlayed->msDuration, 0, iToBePlayed->pNotify);
			iToBePlayed = m_toBePlayed.erase(iToBePlayed);
		}
		else
		{
			iToBePlayed->msDelay -= msElapsed;
			++iToBePlayed;
		}
	}

	// update currently playing sounds
	for (i = 0; i < MAX_EFFECT_BUFFERS; ++i)
	{
		UpdateBuffer(msElapsed, &m_effectBufs[i], EFFECT_BUFFER_SIZE, EFFECT_READ_SIZE);
	}

	for (i = 0; i < MAX_MUSIC_BUFFERS; ++i)
	{
		UpdateBuffer(msElapsed, &m_musicBufs[i], MUSIC_BUFFER_SIZE, MUSIC_READ_SIZE);
	}

	UpdateListener();

	// Apply all the updated 3D settings
	m_pDSListener->CommitDeferredSettings();
}

void AudioManager::SetListenerCamera(Camera* pCam)
{
	m_pListenerCam = pCam;
}


// Normally wouldn't need the following functions,
// but they're exposed so the sample can list all
// the audio tags for the simple testing interface.
int AudioManager::GetNumAudioTags()
{
	return m_tags.size();
}

const char* AudioManager::GetAudioTagName(int index)
{
	int i = 0;
	AudioTagIterator tagIter = m_tags.begin();

	while (i < index && i < m_tags.size())
	{
		++tagIter;
		++i;
	}

	return tagIter->first.c_str();
}


void AudioManager::StopAll()
{
	int i;

	// stop buffers without called Finish Audio on tags
	// (that might create more audio)
	for (i = 0; i < MAX_EFFECT_BUFFERS; ++i)
	{
		m_effectBufs[i].pDSBuf->Stop();
		m_effectBufs[i].bPlaying = false;
	}

	for (i = 0; i < MAX_MUSIC_BUFFERS; ++i)
	{
		m_musicBufs[i].pDSBuf->Stop();
		m_musicBufs[i].bPlaying = false;
	}
}

void AudioManager::StopBuffer(Buffer* pBuf)
{
	pBuf->pDSBuf->Stop();

	if (pBuf->pAudio)
	{
		pBuf->pAudio->Finish();
	}

	pBuf->bPlaying = false;
}


unsigned int AudioManager::GetRemainingMusicPlayback()
{
	unsigned int ms = 0;
	unsigned int totalTime;
	DWORD dwPlayPos;

	for (int i = 0; i < MAX_MUSIC_BUFFERS; ++i)
	{
		if (m_musicBufs[i].bPlaying)
		{
			dwPlayPos = 0;
			m_musicBufs[i].pDSBuf->GetCurrentPosition(&dwPlayPos, NULL);
			totalTime = ((Music*)(m_musicBufs[i].pAudio))->GetTotalTime();
			totalTime -= m_musicBufs[i].msLifetime % totalTime;

			if (totalTime > ms)
				ms = totalTime;
		}
	}

	return ms;
}


void AudioManager::SetOverallVolume(int volume)
{
	int i;

	m_volume = volume;

	// Adjust volume for all playing buffers (taking into account
	// volume adjustments for each tag
	for (i = 0; i < MAX_EFFECT_BUFFERS; ++i)
	{
		if (m_effectBufs[i].bPlaying)
		{
			m_effectBufs[i].volume = m_volume + m_effectBufs[i].pTag->GetVolumeAdjust();
			CLAMP(m_effectBufs[i].volume, DSBVOLUME_MIN, DSBVOLUME_MAX);
			m_effectBufs[i].pDSBuf->SetVolume(m_effectBufs[i].volume);
		}
	}

	for (i = 0; i < MAX_MUSIC_BUFFERS; ++i)
	{
		if (m_musicBufs[i].bPlaying)
		{
			m_musicBufs[i].volume = m_volume + m_musicBufs[i].pTag->GetVolumeAdjust();
			CLAMP(m_musicBufs[i].volume, DSBVOLUME_MIN, DSBVOLUME_MAX);
			m_musicBufs[i].pDSBuf->SetVolume(m_musicBufs[i].volume);
		}
	}
}
