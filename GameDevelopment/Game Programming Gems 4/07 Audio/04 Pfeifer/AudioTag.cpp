#include "stdafx.h"
#include "AudioTag.h"
#include "AudioManager.h"
#include "sound3D.h"
#include "musicoggvorbis.h"

// XML setup
#import "msxml4.dll"  
#include <msxml2.h>
using namespace MSXML2;

////////////////////////////
// AudioTag class members
////////////////////////////

AudioTag::AudioTag()
:	m_priority(0),
	m_volAdjust(0),
	m_volAdjustRange(0),
	m_loopDelay(-1),
	m_loopDelayRange(0),
	m_loopTimes(0),
	m_curLoop(0)
{
}

AudioTag::~AudioTag()
{
}

bool AudioTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	_variant_t varAttribute;

	// tag priority
	varAttribute = pDOMElement->getAttribute("PRIORITY");
	if (VT_NULL != varAttribute.vt)
		m_priority = (long)varAttribute;

	// amount to adjust audio volume from the overall system volume
	varAttribute = pDOMElement->getAttribute("VOLUME_ADJUST");
	if (VT_NULL != varAttribute.vt)
		m_volAdjust = ((float)varAttribute)*100;

	// randomization on the volume adjust
	varAttribute = pDOMElement->getAttribute("VOLUME_ADJUST_RANGE");
	if (VT_NULL != varAttribute.vt)
		m_volAdjustRange = (float)varAttribute*100;

	// number of times to loop - not specifying a value means no loop,
	// 0 means infinite, any other number is the number of times to repeat
	varAttribute = pDOMElement->getAttribute("LOOP_TIMES");
	if (VT_NULL != varAttribute.vt)
		m_loopTimes = (long)varAttribute;

	// seconds to delay loopiung playback
	varAttribute = pDOMElement->getAttribute("LOOP_DELAY");
	if (VT_NULL != varAttribute.vt)
		m_loopDelay = (float)varAttribute*1000;	// convert sec to millisec

	// randomization for loop delay
	varAttribute = pDOMElement->getAttribute("LOOP_DELAY_RANGE");
	if (VT_NULL != varAttribute.vt)
		m_loopDelayRange = (float)varAttribute*1000;	// convert sec to millisec

	return true;
}

void AudioTag::AudioFinished(Audio* pAudio)
{
	// check if the tag is set to loop
	int delay = GetLoopDelay();

	if (delay >= 0)
	{
		if (0 == m_loopTimes)		// play infinitely
		{
			AudioManager::Instance()->Play(this, NULL, 0, delay, this);
		}
		else
		{
			if (m_curLoop < m_loopTimes - 1)
			{
				AudioManager::Instance()->Play(this, NULL, 0, delay, this);
				++m_curLoop;
			}
			else
			{
				m_curLoop = 0;
			}
		}
	}
}

int AudioTag::GetVolumeAdjust()
{
	// use range to calculate random volume adjustment (volAdjust +/- range)
	if (m_volAdjustRange > 0)
		return m_volAdjust + ((rand() % (m_volAdjustRange*2)) - m_volAdjustRange);
	else
		return m_volAdjust;
}

int AudioTag::GetLoopDelay()
{
	// calculate number of millseconds to delay looping of audio
	// if audio is not meant to be looped, return -1.

	int delay = -1;

	if (m_loopDelay >= 0)
	{
		if (0 == m_loopDelayRange)
		{
			delay = m_loopDelay;
		}
		else
		{
			delay = m_loopDelay + (rand() % (m_loopDelayRange*2) - m_loopDelayRange);
		}

		if (delay < 0)
		{
			delay = 0;
		}
	}

	return delay;
}

/////////////////////////////////
// AudioEffectTag class members
/////////////////////////////////

AudioEffectTag::AudioEffectTag()
:	AudioTag(),
	m_minDist(0.0f),
	m_maxDist(1000.0f),
	m_cascadeNum(0x7FFFFFFF),
	m_cascadeTag("")
{
}


AudioEffectTag::~AudioEffectTag()
{
}


Audio* AudioEffectTag::CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify)
{
	Sound3D* pSound3D = new Sound3D(m_fileName.c_str());
	if (!pSound3D->GetWaveFile())
	{
		ASSERT(!"Bad wav file name");
		delete pSound3D;
		return NULL;
	}

	pSound3D->AddListener(pNotify);
	pSound3D->AddListener(this);
	return pSound3D;
}


bool AudioEffectTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	_variant_t varAttribute;

	AudioTag::LoadTag(pDOMElement);

	// .wav file for this effect
	varAttribute = pDOMElement->getAttribute("FILE");
	if (VT_NULL == varAttribute.vt)
		return false;
	m_fileName = (_bstr_t)varAttribute;

	// minimum required distance for 3D audio culling
	varAttribute = pDOMElement->getAttribute("MINDIST");
	if (VT_NULL != varAttribute.vt)
		m_minDist = (long)varAttribute;

	// maximum allowed distance for 3D audio culling
	varAttribute = pDOMElement->getAttribute("MAXDIST");
	if (VT_NULL != varAttribute.vt)
		m_maxDist = (long)varAttribute;

	// number of times to play this sound before we switch to a new one
	varAttribute = pDOMElement->getAttribute("CASCADENUM");
	if (VT_NULL != varAttribute.vt)
		m_cascadeNum = (long)varAttribute;

	// the new tag to play when we hit the cascadenum
	varAttribute = pDOMElement->getAttribute("CASCADETAG");
	if (VT_NULL != varAttribute.vt)
		m_cascadeTag = (_bstr_t)varAttribute;

	return true;
}

/////////////////////////////////
// AudioMusicTag class members
/////////////////////////////////

AudioMusicTag::AudioMusicTag()
:	AudioTag(),
	m_file("")
{
}


AudioMusicTag::~AudioMusicTag()
{
}


Audio* AudioMusicTag::CreateAudio(WorldObject* pObj,
							      int msDuration,
								  int msDelay, 
								  IAudioListener* pNotify)
{
	// currently only Ogg Vorbis files are supported for streaming music
	MusicOggVorbis* pMusic = new MusicOggVorbis(m_file.c_str());
	pMusic->AddListener(pNotify);
	if (pNotify != (IAudioListener*)this)
		pMusic->AddListener(this);
	return pMusic;
}

bool AudioMusicTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	_variant_t varAttribute;

	AudioTag::LoadTag(pDOMElement);

	// the .ogg file for this music tag
	varAttribute = pDOMElement->getAttribute("FILE");
	if (VT_NULL == varAttribute.vt)
		return false;
	m_file = (_bstr_t)varAttribute;

	return true;
}


/////////////////////////////////////
// AudioCompositionTag class members
/////////////////////////////////////

AudioCompositionTag::AudioCompositionTag()
:	AudioTag(),
	m_inTag(""),
	m_loopTag(""),
	m_outTag(""),
	m_lastSection(SECTION_NONE),
	m_crossFadeToOutTime(0)
{
}


AudioCompositionTag::~AudioCompositionTag()
{
}


Audio* AudioCompositionTag::CreateAudio(WorldObject* pObj,
										int msDuration,
										int msDelay,
										IAudioListener* pNotify)
{
	// play the desired section of the composition
	switch (m_lastSection)
	{
		case SECTION_NONE:
		case SECTION_OUT:
			m_lastSection = SECTION_IN;
			AudioManager::Instance()->Play(m_inTag.c_str(), pObj, msDuration, msDelay, this);
			break;

		case SECTION_IN:
		{
			int delay;
			AudioManager::Instance()->Play(m_loopTag.c_str(), pObj, msDuration, msDelay, this);

			// set out music to fade in if we need it to
			if (msDuration > 0)
			{
				delay = msDuration - m_crossFadeToOutTime;
			}
			else
			{
				delay = AudioManager::Instance()->GetRemainingMusicPlayback() - m_crossFadeToOutTime;
			}

			m_lastSection = SECTION_LOOP;
			AudioManager::Instance()->Play(m_outTag.c_str(), pObj, 0, delay, this);
			break;
		}

		case SECTION_LOOP:
			m_lastSection = SECTION_OUT;
			// already played out piece above, with delay
			break;

		default:
			ASSERT(!"Bad Composition section type");
			break;
	}

	return NULL;
}


bool AudioCompositionTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	_variant_t varAttribute;

	AudioTag::LoadTag(pDOMElement);

	// the first music tag to play
	varAttribute = pDOMElement->getAttribute("IN");
	if (VT_NULL == varAttribute.vt)
		return false;
	m_inTag = (_bstr_t)varAttribute;

	// the music tag to loop for the duration of the composition
	varAttribute = pDOMElement->getAttribute("LOOP");
	if (VT_NULL == varAttribute.vt)
		return false;
	m_loopTag = (_bstr_t)varAttribute;

	// the music tag to play on the exit of the composition
	varAttribute = pDOMElement->getAttribute("OUT");
	if (VT_NULL == varAttribute.vt)
		return false;
	m_outTag = (_bstr_t)varAttribute;

	// the amount of time to crossfade between LOOP and OUT
	varAttribute = pDOMElement->getAttribute("CROSSFADE_TO_OUT_TIME");
	if (VT_NULL != varAttribute.vt)
		m_crossFadeToOutTime = (float)varAttribute*1000;	// convert sec to millisec

	return true;
}

void AudioCompositionTag::AudioFinished(Audio* pAudio)
{
	if (SECTION_OUT == m_lastSection)
	{
		// repeat the composition tag if necessary 
		AudioTag::AudioFinished(pAudio);
	}
	else
	{
		// otherwise we're not done with the composition, 
		// play the next section
		AudioManager::Instance()->Play(this, NULL, 0, 0, this);
	}
}


/////////////////////////////////
// AudioRandomTag class members
/////////////////////////////////

AudioRandomTag::AudioRandomTag()
:	AudioTag()
{
}


AudioRandomTag::~AudioRandomTag()
{
}


Audio* AudioRandomTag::CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify)
{
	int percent = (rand() % 100) + 1;
	int size = m_probabilities.size();
	int runningPercent = 0;
	int i;

	// determine which tag we've randomly selected, according to
	// each tag's percentage chance
	for (i = 0; i < size; ++i)
	{
		runningPercent += m_probabilities[i]; 
		if (percent <= runningPercent)
		{
			break;
		}
	}

	// play the tag, if we've selected one
	if (i < size)
	{
		AudioManager::Instance()->Play(m_tags[i].c_str(), pObj, msDuration, msDelay, this);
	}

	return NULL;
}

bool AudioRandomTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	IXMLDOMNodeListPtr	pChildNodes;
	IXMLDOMElementPtr	pChildDOMElement;
	_variant_t			varAttribute;

	AudioTag::LoadTag(pDOMElement);

	pChildNodes = pDOMElement->GetchildNodes();
	
	for (int i = 0; i < pChildNodes->Getlength(); ++i)
	{
		pChildDOMElement = pChildNodes->Getitem(i);

		// The tag name of the random element
		varAttribute = pChildDOMElement->getAttribute("TAG");
		if (VT_NULL != varAttribute.vt)
		{
			m_tags.push_back(std::string((_bstr_t)varAttribute));
	
			// the probability of playing this tag
			varAttribute = pChildDOMElement->getAttribute("PROB");

			if (VT_NULL != varAttribute.vt)
				m_probabilities.push_back((long)varAttribute);
		}
	}

	if (m_tags.size() != m_probabilities.size())
		return false;

	return true;
}


/////////////////////////////////
// AudioAmbientTag class members
/////////////////////////////////
AudioAmbientTag::AudioAmbientTag()
:	AudioEffectTag(),
	m_xPos(0.0f),
	m_yPos(0.0f),
	m_zPos(0.0f),
	m_xRange(0.0f),
	m_yRange(0.0f),
	m_zRange(0.0f),
	m_obj()
{
}


AudioAmbientTag::~AudioAmbientTag()
{
}


Audio* AudioAmbientTag::CreateAudio(WorldObject* pObj,
									int msDuration,
								    int msDelay,
									IAudioListener* pNotify)
{
	Sound3D* pSound3D = new Sound3D(m_fileName.c_str());
	pSound3D->AddListener(pNotify);
	if (pNotify != (IAudioListener*)this)
		pSound3D->AddListener(this);
	pSound3D->SetWorldObject(&m_obj);
	return pSound3D;
}

bool AudioAmbientTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	_variant_t varAttribute;

	AudioEffectTag::LoadTag(pDOMElement);

	// X position for 3D audio
	varAttribute = pDOMElement->getAttribute("X");
	if (VT_NULL != varAttribute.vt)
		m_xPos = (float)varAttribute;

	// Y position for 3D audio
	varAttribute = pDOMElement->getAttribute("Y");
	if (VT_NULL != varAttribute.vt)
		m_yPos = (float)varAttribute;

	// Z position for 3D audio
	varAttribute = pDOMElement->getAttribute("Z");
	if (VT_NULL != varAttribute.vt)
		m_zPos = (float)varAttribute;

	// Randomization for X position
	varAttribute = pDOMElement->getAttribute("XRANGE");
	if (VT_NULL != varAttribute.vt)
		m_xRange = (float)varAttribute;

	// Randomization for Y position
	varAttribute = pDOMElement->getAttribute("YRANGE");
	if (VT_NULL != varAttribute.vt)
		m_yRange = (float)varAttribute;

	// Randomization for Z position
	varAttribute = pDOMElement->getAttribute("ZRANGE");
	if (VT_NULL != varAttribute.vt)
		m_zRange = (float)varAttribute;

	RandomizeWorldPosition();
	return true;
}

void AudioAmbientTag::AudioFinished(Audio* pAudio)
{
	RandomizeWorldPosition();
	AudioTag::AudioFinished(pAudio);
}	

void AudioAmbientTag::RandomizeWorldPosition()
{
	// this function calculates a new world position for the ambient sound
	// based on the tag's randomization attributes

	D3DXVECTOR3 pos;
	if (m_xRange != 0.0f)
		pos.x = m_xPos + ((float)(rand() % ((int)(m_xRange*2000))))/1000.0f - m_xRange; 
	else
		pos.x = m_xPos;

	if (m_yRange != 0.0f)
		pos.y = m_yPos + ((float)(rand() % ((int)(m_yRange*2000))))/1000.0f - m_yRange; 
	else
		pos.y = m_yPos;

	if (m_zRange != 0.0f)
		pos.z = m_zPos + ((float)(rand() % ((int)(m_zRange*2000))))/1000.0f - m_zRange; 
	else
		pos.z = m_zPos;

	m_obj.SetPosition(pos.x, pos.y, pos.z);
}

/////////////////////////////////
// AudioGroupTag class members
/////////////////////////////////

AudioGroupTag::AudioGroupTag()
:	AudioTag()
{
}


AudioGroupTag::~AudioGroupTag()
{
}


Audio* AudioGroupTag::CreateAudio(WorldObject* pObj,
								  int msDuration,
								  int msDelay,
								  IAudioListener* pNotify)
{
	int numTags = m_tags.size();
	int delay;

	for (int i=0; i<numTags; ++i)
	{
		// calculate number of millseconds to delay individual elements
		delay = -1;

		if (m_delays[i] > 0)
		{
			if (0 == m_delayRanges[i])
				delay = m_delays[i];
			else
				delay = m_delays[i] + (rand() % (m_delayRanges[i]*2) - m_delayRanges[i]);
		}

		if (delay < 0)
			delay = 0;

		AudioManager::Instance()->Play(m_tags[i].c_str(), pObj, msDuration, msDelay+delay, this);
	}

	return NULL;
}

bool AudioGroupTag::LoadTag(IXMLDOMElementPtr pDOMElement)
{
	IXMLDOMNodeListPtr	pChildNodes;
	IXMLDOMElementPtr	pChildDOMElement;
	_variant_t			varAttribute;

	AudioTag::LoadTag(pDOMElement);

	pChildNodes = pDOMElement->GetchildNodes();
	
	for (int i = 0; i < pChildNodes->Getlength(); ++i)
	{
		pChildDOMElement = pChildNodes->Getitem(i);

		// the tag this element in the group refers to
		varAttribute = pChildDOMElement->getAttribute("TAG");
		if (VT_NULL != varAttribute.vt)
		{
			m_tags.push_back(std::string((_bstr_t)varAttribute));

			// the delay to play this tag when starting the group
			varAttribute = pChildDOMElement->getAttribute("DELAY");

			if (VT_NULL != varAttribute.vt)
				m_delays.push_back(((float)varAttribute)*1000.0f);
			else
				m_delays.push_back(-1);

			// randomization for the delay
			varAttribute = pChildDOMElement->getAttribute("DELAY_RANGE");

			if (VT_NULL != varAttribute.vt)
				m_delayRanges.push_back(((float)varAttribute)*1000.0f);
			else
				m_delayRanges.push_back(0);
		}
	}

	return true;
}

