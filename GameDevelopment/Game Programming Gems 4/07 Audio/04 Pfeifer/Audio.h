#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <vector>

class Audio;

// Interface class to receive notifications that an audio
// piece has finished playing 
class IAudioListener
{
	public:
		virtual void AudioFinished(Audio* pAudio) = 0;
};


// The Audio class is the base class for any sound we'll play
class Audio
{
    public:
		enum Type
		{
			SOUND,
			SOUND3D,
			MUSIC
		};

        Audio(Type type);
        virtual ~Audio();

		// returns true if more data exists to be read
		virtual bool FillBuffer(LPDIRECTSOUNDBUFFER pDSB,
								DWORD startIndex,
								DWORD amount,
								DWORD* pAmtRead = NULL) = 0;

		// sets the buffer back to the beginning so the audio
		// manager can loop the sound
		virtual void Reset() = 0;

		Type GetType() const;

		void AddListener(IAudioListener* pListener);
		void Finish();

	protected:
		Type m_type;
		std::vector<IAudioListener*> m_listeners;
};

inline Audio::Audio(Type type)
:	m_type(type)
{
}

inline Audio::~Audio()
{
}

inline Audio::Type Audio::GetType() const
{
	return m_type;
}

inline void Audio::AddListener(IAudioListener* pListener)
{
	if (pListener)
		m_listeners.push_back(pListener);
}

inline void Audio::Finish()
{
	// notify all our listeners that we're done
	int numListeners = m_listeners.size();
	
	for (int i = 0; i < numListeners; ++i)
	{
		m_listeners[i]->AudioFinished(this);
	}
}

#endif