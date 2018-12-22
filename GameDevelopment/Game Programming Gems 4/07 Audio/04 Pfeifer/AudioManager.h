#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <map>
#include <string>
#include <list>

class Camera;
class Audio;
class IAudioListener;
class AudioTag;
class Sound3D;
class Music;
class WorldObject;

// The Audio Manager class creates audio based on the XML
// tags it loads and handles all the sound buffer updates,
class AudioManager
{
    public:
		enum HRTF
		{
			HRTF_FULL,
			HRTF_LIGHT,
			HRTF_NONE
		};

		enum Constants
		{
			MAX_EFFECT_BUFFERS = 32,
			EFFECT_BUFFER_SIZE = 141120,
			EFFECT_INITIAL_READ_SIZE = 141120/2,
			EFFECT_READ_SIZE = 141120/2,

			MAX_MUSIC_BUFFERS = 2,
			MUSIC_BUFFER_SIZE = 1411200,
			MUSIC_INITIAL_READ_SIZE = 1411200/10,
			MUSIC_READ_SIZE = 1411200/10,
		};

		// Gets the instance of the singleton class
		static AudioManager* Instance();

        AudioManager();
        ~AudioManager();

        bool Init(HWND hWnd,
				  DWORD dwPrimaryChannels = 2,
                  DWORD dwPrimaryFreq = 44100,
				  DWORD dwPrimaryBitRate = 16,
				  HRTF hrtf = HRTF_FULL);

		// loads tags from an XML file
		bool LoadAudioTags(const char* szFileName);

		void ClearAudioTags();
		
		AudioTag* GetAudioTag(const char* szTagName);
		
		// Update needs to be called periodically to stream
		// new audio into the sound buffers
		void Update(int msElapsed);

		void Play(const char* szTagName,
				  WorldObject* pObj = NULL,		// used for positional sound effects
				  int msDuration = 0,			// length of time to play sound
				  int msDelay = 0,				// time to wait before playing
				  IAudioListener* pNotify = NULL); // listener to receive notification when sound is done

		void Play(AudioTag* pTag,
				  WorldObject* pObj = NULL,		// used for positional sound effects
				  int msDuration = 0,			// length of time to play sound
				  int msDelay = 0,				// time to wait before playing
				  IAudioListener* pNotify = NULL);	// listener to receive notification when sound is done
		
		void StopAll();
		
		// The game would set this to the player's camera to affect 
		// how 3D audio is heard
		void SetListenerCamera(Camera* pCam);  

		void SetDistanceFactor(float dF = DS3D_DEFAULTDISTANCEFACTOR);
		void SetRolloffFactor(float rF = DS3D_DEFAULTROLLOFFFACTOR);
		void SetDopplerFactor(float dF = DS3D_DEFAULTDOPPLERFACTOR);

		// The master volume that affects all sound buffers
		void SetOverallVolume(int volume); 

		// Normally wouldn't need the following functions,
		// but they're exposed so the sample can list all
		// the audio tags for the simple testing interface.
		int GetNumAudioTags();
		const char* GetAudioTagName(int index);

		// returns time remaining of current music playing
		unsigned int GetRemainingMusicPlayback();	// in milliseconds

    protected:
		// static singleton instance
		static AudioManager* s_pInstance;

		typedef std::map<std::string, AudioTag*> AudioTagMap;
		typedef AudioTagMap::iterator AudioTagIterator;

		struct Buffer
		{
			LPDIRECTSOUNDBUFFER		pDSBuf;			// DirectSound buffer for playback
			LPDIRECTSOUND3DBUFFER	pDSBuf3D;		// DirectSound 3D buffer for effects (positional audio)
			Audio*					pAudio;			// Audio created by the tag
			AudioTag*				pTag;			// Tag that decided to play this sound
			bool					bMoreInBuffer;	// if more needs to be streamed into buffer
			bool					bPlaying;		// if this is an active buffer or can it be reused
			DWORD					dwLastWritePos; // last place we updated the buffer
			DWORD					dwNextWritePos; // next place to update the buffer
			long					volume;			// volume for this buffer (taking into account tag volume adjustment)
			int                     msDuration;		// desired duration, in milliseconds
			int						msLifetime;		// elapsed playback time, in milliseconds
			long					curVolume;      // current volume (if we're fading in or out)
			float					volumeTransitionAdjust;	// amount to fade by per millsecond
		};

		struct AudioWaitingToBePlayed
		{
			AudioTag*				pTag;		// the tag waiting to be played
			WorldObject*			pObj;		// the obj we want to attach it to (for 3D sound effects)
			int		                msDuration; // time to play
			int                     msDelay;	// wait before playing
			IAudioListener*			pNotify;	// the listener to receive finished notification
		};

		typedef std::list<AudioWaitingToBePlayed> AudioWaitingList;
		typedef AudioWaitingList::iterator AudioWaitingIter;

		// internal class functions
        HRESULT SetPrimaryBufferFormat(DWORD dwPrimaryChannels,
                                       DWORD dwPrimaryFreq,
									   DWORD dwPrimaryBitRate);
		bool CreateBuffer(Buffer* pBuf, DWORD size, bool b3D);
		bool RestoreBuffer(Buffer* pBuf);
		void StopBuffer(Buffer* pBuf);
		void UpdateBuffer(int msElapsed, Buffer* pBuf, DWORD dwBufSize, DWORD dwWriteAmt);
		void UpdateListener();

		void PlayEffect(AudioTag* pTag, Sound3D* pSound3D, int msDuration);
		void PlayMusic(AudioTag* pTag, Music* pMusic, int msDuration);

	protected:
        LPDIRECTSOUND8 m_pDS;
		LPDIRECTSOUNDBUFFER m_pDSBPrimary;
		LPDIRECTSOUND3DLISTENER m_pDSListener;

		Camera* m_pListenerCam;   // current game view to use for 3D Audio

		// DirectSound 3D Audio parameters
		float m_distanceFactor;
		float m_rolloffFactor;
		float m_dopplerFactor;		

		// initialization parameters
		DWORD m_frequency;			// sample rate (usually 44.1 kHz)
		DWORD m_bitRate;			// 
		int m_volume;				// in hundredths of decibels
		HRTF m_hrtf;				// Head related transfer function - how to calculate 3D audio

		Buffer m_effectBufs[MAX_EFFECT_BUFFERS];	
		Buffer m_musicBufs[MAX_MUSIC_BUFFERS];

		AudioTagMap m_tags;			// tag database
		AudioWaitingList m_toBePlayed; // queue of delayed audio
};


inline AudioManager* AudioManager::Instance()
{ 
	if (!s_pInstance)
		s_pInstance = new AudioManager();

	return s_pInstance;
}

inline void AudioManager::SetDistanceFactor(float dF)
{
	m_distanceFactor = dF;
}

inline void AudioManager::SetRolloffFactor(float rF)
{
	m_rolloffFactor = rF;
}

inline void AudioManager::SetDopplerFactor(float dF)
{
	m_dopplerFactor = dF;
}


#endif