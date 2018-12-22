//----------------------------------------------------------------------------
// File: soundfile.cpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#include "soundfile.hpp"
#include <mmsystem.h>
#include "util.hpp"

//----------------------------------------------------------------------------
SoundFile::SoundFile( void )
    :
    mhMMIO( NULL ),
    mSize( 0 ),
    mBytesRemaining( 0 )
{
    ::memset( &mFormat, 0, sizeof( mFormat ) );
}

//----------------------------------------------------------------------------
SoundFile::~SoundFile( void )
{
    if ( NULL != mhMMIO )
    {
        if ( 0 != mmioClose( mhMMIO, 0 ) )
        {
            ASSERT( false );
        }
        mhMMIO = NULL;
    }
}

//----------------------------------------------------------------------------
bool SoundFile::Open( const char* pFileName )
{
    MMCKINFO ckIn;           
    MMCKINFO ckRiff;
    MMCKINFO ckData;
    PCMWAVEFORMAT pcmWaveFormat;    
    
    if ( NULL == pFileName )
    {
        return false;
    }
    
    mhMMIO = mmioOpen( ( LPSTR ) pFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

    // Try getting the RIFF chunk, the main "resource interchange file format" chunk
    if ( 0 != mmioDescend( mhMMIO, &ckRiff, NULL, 0 ) )
    {
        ASSERT( false );
        return false;
    }

    // The ID fied should say RIFF and the type field should say WAVE
    if ( FOURCC_RIFF != ckRiff.ckid || mmioFOURCC('W', 'A', 'V', 'E') != ckRiff.fccType )
    {
        ASSERT( false );
        return false;
    }
    
    // Now look for the format of the file, it's in the FMT chunk
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ( 0 != mmioDescend( mhMMIO, &ckIn, &ckRiff, MMIO_FINDCHUNK ) )
    {
        ASSERT( false );
        return false;
    }        

    // Right size?    
    if( ckIn.cksize < (LONG) sizeof( pcmWaveFormat ) )
    {
        ASSERT( false );
        return false;
    }        

    // Read the format
    if ( sizeof( pcmWaveFormat ) != mmioRead( mhMMIO, (HPSTR)&pcmWaveFormat, sizeof( pcmWaveFormat) ) )
    {
        ASSERT( false );
        return false;
    }
    
    // For the purposes of this demo, only PCM is supported
    if ( WAVE_FORMAT_PCM != pcmWaveFormat.wf.wFormatTag )
    {
        ASSERT( WAVE_FORMAT_PCM == pcmWaveFormat.wf.wFormatTag );
        return false;
    }

    // Remember the format
    mFormat.wFormatTag = pcmWaveFormat.wf.wFormatTag; 
    mFormat.nChannels = pcmWaveFormat.wf.nChannels; 
    mFormat.nSamplesPerSec = pcmWaveFormat.wf.nSamplesPerSec; 
    mFormat.nAvgBytesPerSec = pcmWaveFormat.wf.nAvgBytesPerSec; 
    mFormat.nBlockAlign = pcmWaveFormat.wf.nBlockAlign; 
    mFormat.wBitsPerSample = pcmWaveFormat.wBitsPerSample;
    
    // We're done with the FMT chunk
    if ( 0 != mmioAscend( mhMMIO, &ckIn, 0 ) )
    {
        ASSERT( false );
        return false;
    }

    // Skip to the data chunk
    if ( mmioSeek( mhMMIO, ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET ) < 0 )
    {
        ASSERT( false );
        return false;
    }

    // Search for DATA chunk
    ckData.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if( 0 != mmioDescend( mhMMIO, &ckData, &ckRiff, MMIO_FINDCHUNK ) )
    {
        ASSERT( false );
        return false;
    }        
    
    // Keep the file size
    mSize = ckData.cksize;
    mBytesRemaining = mSize;
    return true;
}

//----------------------------------------------------------------------------
unsigned int SoundFile::Read( void* pBuffer, unsigned int numBytes )
{
    ASSERT( NULL != mhMMIO );
    ASSERT( NULL != pBuffer );

    MMRESULT mmr;
    MMIOINFO mmioInfo; 

    // Get the current read position in the IO buffer
    mmr = mmioGetInfo( mhMMIO, &mmioInfo, 0 );
    ASSERT( SUCCEEDED( mmr ) );
    
    // We can only read as many bytes as we have available
    unsigned int actualBytes = numBytes;
    if ( actualBytes > mBytesRemaining )
    {
        actualBytes = mBytesRemaining;
    }  
    
    for( unsigned int i = 0; i < actualBytes; i++ )
    {
        // Keep updating the IO buffer when required
        if ( mmioInfo.pchNext == mmioInfo.pchEndRead )
        {
            mmr = mmioAdvance( mhMMIO, &mmioInfo, MMIO_READ );
            ASSERT( SUCCEEDED( mmr ) );
            ASSERT( mmioInfo.pchNext != mmioInfo.pchEndRead );
        }
        
        // Copy it a byte at a time
        *( ( char* )( pBuffer ) + i ) = *( ( char* )mmioInfo.pchNext );
        mmioInfo.pchNext++;
        mBytesRemaining--;
    }

    // Update the IO buffer read position
    mmr = mmioSetInfo( mhMMIO, &mmioInfo, 0 );
    ASSERT( SUCCEEDED( mmr ) );
    
    // Clear buffer if we don't fill it.
    if ( actualBytes < numBytes )
    {
        ::memset( ( ( char* )( pBuffer ) + actualBytes ), 0, numBytes - actualBytes );
    }

    // Return the actual number of bytes that we read
    return actualBytes;
}
