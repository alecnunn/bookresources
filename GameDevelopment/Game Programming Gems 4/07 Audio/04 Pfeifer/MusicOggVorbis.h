#ifndef _MUSICOGGVORBIS_H_
#define _MUSICOGGVORBIS_H_

#include "music.h"

class OggVorbisFile;

// MusicOggVorbis is an implementation of the abstract Music
// class supporting .ogg playback
class MusicOggVorbis : public Music
{
    public:
        MusicOggVorbis(const char* szVorbisFile);
        virtual ~MusicOggVorbis();

		virtual bool FillBuffer(LPDIRECTSOUNDBUFFER pDSB,
								DWORD startIndex,
								DWORD amount,
								DWORD* pAmtRead = NULL);

		virtual void Reset();

		virtual unsigned int GetTotalTime();


    protected:
		OggVorbisFile* m_pOVFile;
};



#endif