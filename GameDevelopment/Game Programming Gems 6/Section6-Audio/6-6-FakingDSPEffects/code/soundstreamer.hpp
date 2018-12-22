//----------------------------------------------------------------------------
// File: soundstreamer.hpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#ifndef SOUNDSTREAMER_HPP
#define SOUNDSTREAMER_HPP

//----------------------------------------------------------------------------
#include "soundfile.hpp"

//----------------------------------------------------------------------------
#define MAX_NUM_BUFFERS 2

//----------------------------------------------------------------------------
class SoundStreamer
{
    public:
    
        SoundStreamer( void );
        ~SoundStreamer( void );
        
        void SetFile( SoundFile* pFile, unsigned int bufferSize, DS3DBUFFER* pDs3dBuffer );
        void Play( void );
        void SetActiveChannel( unsigned int channel );
        void Update( void );

    private:

        void UpdateSettings( void );
        void UpdatePlay( void );
        void UpdateLoad( void );

        // Maintain a constant power level between the channel volumes
        int CalculateSecondaryVolume( int primaryVolume );

        // Calculate the next volume to use while fading in
        int IncrementFadeInVolume( int volume );


        SoundFile* mpFile;
        IDirectSoundBuffer* mpIDirectSoundBuffers[ MAX_NUM_BUFFERS ];
        DS3DBUFFER* mpDs3dBuffer;
        unsigned int mActiveChannel;
        unsigned int mBufferSize;
        unsigned int mWrite;
        unsigned int mWriteCount;
        unsigned int mPlay;
        unsigned int mPlayCount;
        bool mIsPlaying;
};

#endif // SOUNDSTREAMER_HPP
