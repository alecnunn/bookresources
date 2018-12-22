#ifndef _MUSIC_H_
#define _MUSIC_H_

#include "audio.h"

// The Music class is the base class for any streamed music.
class Music : public Audio
{
    public:
        Music();
        virtual ~Music();

		virtual unsigned int GetTotalTime() = 0;
    protected:
};


inline Music::Music()
:	Audio(Audio::MUSIC)
{
}

inline Music::~Music()
{
}

#endif