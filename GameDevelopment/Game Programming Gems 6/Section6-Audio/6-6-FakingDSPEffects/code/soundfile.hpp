//----------------------------------------------------------------------------
// File: soundfile.hpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#ifndef SOUNDFILE_HPP
#define SOUNDFILE_HPP

//----------------------------------------------------------------------------
#include <dsound.h>

//----------------------------------------------------------------------------
class SoundFile
{
    public:
        SoundFile( void );
        ~SoundFile( void );
        
        bool Open( const char* pFileName );
        unsigned int Read( void* pBuffer, unsigned int numBytes );
        
        const WAVEFORMATEX& GetFormat( void ) const { return mFormat; }
        unsigned int GetSize( void ) const { return mSize; }

    private:        
        
        WAVEFORMATEX mFormat;
        HMMIO mhMMIO;
        unsigned int mSize;
        unsigned int mBytesRemaining;
};

#endif // SOUNDFILE_HPP
