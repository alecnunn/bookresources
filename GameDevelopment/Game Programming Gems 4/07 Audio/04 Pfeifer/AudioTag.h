#ifndef _AUDIOTAG_H_
#define _AUDIOTAG_H_

#import "msxml4.dll"

#include <msxml2.h>
#include <string>
#include <vector>

#include "worldobject.h"
#include "audio.h"

using namespace MSXML2;


// The base class for any audio tag to be read from the XML database
class AudioTag : public IAudioListener
{
	public:
		AudioTag();
		virtual ~AudioTag();

		int GetPriority() { return m_priority; }
		int GetVolumeAdjust(); // returns hundredths of decibels to adjust by
		int GetLoopDelay();  // returns milliseconds to delay next play of tag

		// called to audio create Audio object based on this tag's data
		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify) = 0;

		// called when to load tag data from XML file
		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

		// called when audio created by this tag has finished playing
		virtual void AudioFinished(Audio* pAudio);

	protected:
		int m_priority;						// priority for interrupting other audio
		int m_volAdjust, m_volAdjustRange;	// volume adjustment parameters
		int m_loopDelay, m_loopDelayRange;	// looping audio parameters
		int m_loopTimes, m_curLoop;		
};


typedef std::vector<AudioTag*> AudioTagVector;
typedef AudioTagVector::iterator AudioTagIterator;


// The EFFECT tag class - handles any basic 3D sound effect
class AudioEffectTag : public AudioTag
{
	public:
		AudioEffectTag();
		virtual ~AudioEffectTag();
	
		const char* GetFileName() { return m_fileName.c_str(); }

		float GetMinDistance() { return m_minDist; }
		float GetMaxDistance() { return m_maxDist; }
		int GetCascadeNumber() { return m_cascadeNum; }
		const char* GetCascadeTag() { return m_cascadeTag.c_str(); }

		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify);
		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

	protected:
		std::string m_fileName;		// our .wav file
		float m_minDist;			// minimum distance for 3D audio culling
		float m_maxDist;			// maximum distance for 3D audio culling
		int m_cascadeNum;			// number of times to play before we switch
		std::string m_cascadeTag;	// to this cascade tag
};


// The AMBIENT tag class - this handles any ambient audio
// it also allows for any of the EFFECT parameters
class AudioAmbientTag : public AudioEffectTag
{
	public:
		AudioAmbientTag();
		virtual ~AudioAmbientTag();
	
		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify);
		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

		// called when audio created by this tag has finished playing
		virtual void AudioFinished(Audio* pAudio);

	protected:
		void RandomizeWorldPosition();

		// members to keep track of randomized world position
		float m_xPos, m_xRange;
		float m_yPos, m_yRange;
		float m_zPos, m_zRange;
		WorldObject m_obj;
};

// The RANDOM tag controls selection of several tags with a certain
// probability for each tag
class AudioRandomTag : public AudioTag
{
	public:
		AudioRandomTag();
		virtual ~AudioRandomTag();

		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify);
		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

	protected:
		std::vector<std::string> m_tags;		// the tags to choose from
		std::vector<int> m_probabilities;		// their probabilities
};


// The MUSIC tag plays an .ogg file
class AudioMusicTag : public AudioTag
{
	public:
		AudioMusicTag();
		virtual ~AudioMusicTag();

		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify);

		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

	protected:
		std::string m_file;		// the .ogg file
};


// The COMPOSITION tag plays a bunch of music tags in
// a certain order
class AudioCompositionTag : public AudioTag
{
	public:
		AudioCompositionTag();
		virtual ~AudioCompositionTag();

		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify);

		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

		virtual void AudioFinished(Audio* pAudio);

	protected:
		enum Section
		{
			SECTION_NONE,
			SECTION_IN,
			SECTION_LOOP,
			SECTION_OUT
		};

		std::string m_inTag;			// the intro music
		std::string m_outTag;			// the exit music
		std::string m_loopTag;			// music to loop for the duration of the audio
		Section m_lastSection;			// the last section we played
		int m_crossFadeToOutTime;		// the time to crossfade between LOOP and OUT
};


// The GROUP tag plays a bunch of audio tags, and
// any individual tag in the group can be run at a delay 
// from the time the group is requested to play
class AudioGroupTag : public AudioTag
{
	public:
		AudioGroupTag();
		virtual ~AudioGroupTag();

		virtual Audio* CreateAudio(WorldObject* pObj,
								   int msDuration,
								   int msDelay,
								   IAudioListener* pNotify);
		virtual bool LoadTag(IXMLDOMElementPtr pDOMElement);

	protected:
		std::vector<std::string> m_tags;	// the tags in the group
		std::vector<int> m_delays;			// the delays for each tag
		std::vector<int> m_delayRanges;		// randomization for each delay
};

#endif