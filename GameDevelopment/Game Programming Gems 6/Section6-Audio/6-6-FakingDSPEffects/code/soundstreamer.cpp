//----------------------------------------------------------------------------
// File: soundstreamer.cpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#include "soundstreamer.hpp"
#include "soundsystem.hpp"
#include "util.hpp"
#include <stdio.h>
#include <math.h>

//----------------------------------------------------------------------------
static const float FADE_INCREMENT_VALUE = 0.16f;

//----------------------------------------------------------------------------
// Helper struct for locking direct sound buffers
struct BufferLock
{
    BufferLock( void ) 
        : 
        pBuffer1( NULL),
        numBytes1( 0 ),
        pBuffer2( NULL ),
        numBytes2( 0 )
    {
    }        
    
    void* pBuffer1;
    DWORD numBytes1;
    void* pBuffer2;
    DWORD numBytes2;
};

//----------------------------------------------------------------------------
SoundStreamer::SoundStreamer( void )
    :
    mpFile( NULL ),
    mpDs3dBuffer( NULL ),
    mActiveChannel( 0 ),
    mBufferSize( 0 ),
    mWrite( 0 ),
    mWriteCount( 0 ),
    mPlay( 0 ),
    mPlayCount( 0 )
{
    ::memset( mpIDirectSoundBuffers, 0, sizeof( mpIDirectSoundBuffers ) );
}

//----------------------------------------------------------------------------
SoundStreamer::~SoundStreamer( void )
{   
    for ( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
    {
        if ( NULL != mpIDirectSoundBuffers[ i ] )
        {
            mpIDirectSoundBuffers[ i ]->Release( );
            mpIDirectSoundBuffers[ i ] = NULL;
        }
    }        
}

//----------------------------------------------------------------------------
void SoundStreamer::SetFile( SoundFile* pFile, unsigned int bufferSize, DS3DBUFFER* pDs3dBuffer )
{
    // Clean up old resources
    if ( NULL != mpFile )
    {
        mpFile = NULL;
    }
    
    for ( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
    {
        if ( NULL != mpIDirectSoundBuffers[ i ] )
        {
            mpIDirectSoundBuffers[ i ]->Release( );
            mpIDirectSoundBuffers[ i ] = NULL;
        }
    }        
    
    // Reset trackers
    mpFile = pFile;
    mpDs3dBuffer = pDs3dBuffer;
    mBufferSize = bufferSize;
    mWrite = 0;
    mWriteCount = 0;
    mPlay = 0;
    mPlayCount = 0;

    if ( NULL != mpFile )
    {
        WAVEFORMATEX wfx = mpFile->GetFormat( );
        
        // DirectSound can only play mono sounds positionally. If we want
        // to play stereo sounds positionally, we need to create 2 mono buffers.
        unsigned int numBuffersToCreate = 1;
        if ( wfx.nChannels > 1 && NULL != mpDs3dBuffer )
        {
            numBuffersToCreate = wfx.nChannels;
        }

        // Adjust the format for mono buffers
        if ( numBuffersToCreate > 1 )
        {
            wfx.nChannels = 1;
            wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
            wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
        }            

        // Create the buffers.  The buffers need to be 3D buffers if we were supplied
        // with 3D buffer settings     
        for ( unsigned int i = 0; i < numBuffersToCreate; i++ )
        {
            SoundSystem::Get( )->CreateDirectSoundBuffer( &mpIDirectSoundBuffers[ i ], &wfx, mBufferSize, NULL != pDs3dBuffer );
        }        
    }   

    // Starting Loading right away
    UpdateLoad( );
}

//----------------------------------------------------------------------------
void SoundStreamer::Play( void )
{
    ASSERT( NULL != mpFile );
    HRESULT hr;

    // Make sure the buffers hav fresh data before starting to play
    UpdateSettings( );
    UpdateLoad( );

    // Start playing in reverse order so we can always use buffer zero to 
    // detect the playback position.
    for ( int i = MAX_NUM_BUFFERS - 1; i >= 0; i-- )
    {
        if ( NULL != mpIDirectSoundBuffers[ i ] )
        {
            hr = mpIDirectSoundBuffers[ i ]->Play( 0, 0, DSBPLAY_LOOPING );
            ASSERT( SUCCEEDED( hr ) );
        }
    }                
    mIsPlaying = true;
}

//----------------------------------------------------------------------------
void SoundStreamer::SetActiveChannel( unsigned int channel )
{
    mActiveChannel = channel;
}

//----------------------------------------------------------------------------
void SoundStreamer::Update( void )
{
    if ( !mIsPlaying )
    {
        return;
    }

    UpdateSettings( );
    UpdatePlay( );
    UpdateLoad( );
}

//----------------------------------------------------------------------------
void SoundStreamer::UpdateSettings( void )
{
    HRESULT hr;

    LONG activeChannelVolume = DSBVOLUME_MAX;
    // Get volume from active streamer
    if ( NULL != mpIDirectSoundBuffers[ mActiveChannel ] )
    {
        hr = mpIDirectSoundBuffers[ mActiveChannel ]->GetVolume( &activeChannelVolume );
        ASSERT( SUCCEEDED( hr ) );
    }

    // Fade the volume towards full volume.  Give it an ease in ease out feeling
    activeChannelVolume = IncrementFadeInVolume( activeChannelVolume );

    // Calculate the volume of the other channel.
    int inActiveChannelVolume = CalculateSecondaryVolume( activeChannelVolume );

    // Update the buffer settings
    for( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
    {
        if ( NULL != mpIDirectSoundBuffers[ i ] )
        {
            // Update 3d settings if we have any
            if ( NULL != mpDs3dBuffer )
            {
                IDirectSound3DBuffer* pIDirectSound3DBuffer = NULL;
                mpIDirectSoundBuffers[ i ]->QueryInterface( IID_IDirectSound3DBuffer, ( void** )&pIDirectSound3DBuffer );
                ASSERT( NULL != pIDirectSound3DBuffer );
                pIDirectSound3DBuffer->SetAllParameters( mpDs3dBuffer, DS3D_IMMEDIATE );
            }
            
            // Only let the active channel be heard.
            // Note: Ideally in a game this would be a fade.  It sounds better
            // as a cut in this sample however.
            if ( i == mActiveChannel )
            {   
                hr = mpIDirectSoundBuffers[ i ]->SetVolume( activeChannelVolume );
                ASSERT( SUCCEEDED( hr ) );
            }
            else
            {
                hr = mpIDirectSoundBuffers[ i ]->SetVolume( inActiveChannelVolume );
                ASSERT( SUCCEEDED( hr ) );
            }
        }
    }
}            

//----------------------------------------------------------------------------
void SoundStreamer::UpdatePlay( void )
{
    HRESULT hr;
    
    if ( NULL == mpFile )
    {
        return;
    }
    
    // Monitor only one of the buffers
    if ( NULL != mpIDirectSoundBuffers[ 0 ] )
    {
        // Get the current play position
        DWORD playPosition = 0;
        DWORD writePosition = 0;    
        hr = mpIDirectSoundBuffers[ 0 ]->GetCurrentPosition( &playPosition, &writePosition );
        ASSERT( SUCCEEDED( hr ) );

        // Calculate how many bytes have been played
        unsigned int bytesPlayed;
        if ( playPosition > mPlay )
        {   
            bytesPlayed = playPosition - mPlay;
        }
        else
        {
            bytesPlayed = mBufferSize - ( mPlay - playPosition );
        }
        
        // Update our trackers for total count and current position
        mPlayCount += bytesPlayed;
        mPlay = playPosition;
        
        // Detect skipping and keep mPlayCount less than mWriteCount
        if ( mPlayCount > mWriteCount )
        {
            mPlayCount = mPlayCount - mBufferSize;
        }
    }
    
    // Remember that total play count needs to be multiplied by the number of buffers
    if ( mPlayCount * mpFile->GetFormat( ).nChannels >= mpFile->GetSize( ) )
    {
        // We've played the entire file.  It's time to stop
        mIsPlaying = false;
        for( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
        {
            if ( NULL != mpIDirectSoundBuffers[ i ] )
            {
                hr = mpIDirectSoundBuffers[ i ]->Stop( );
                ASSERT( SUCCEEDED( hr ) );
            }
        }            
    }        
}

//----------------------------------------------------------------------------
void SoundStreamer::UpdateLoad( void )
{
    HRESULT hr;

    // Calculate the number of bytes the in the buffer that have already been played
    // and need to be loaded again
    unsigned int dirtyBytes = mBufferSize - ( mWriteCount - mPlayCount );

    // If there are dirty bytes, we'll replace them with good ones
    if ( dirtyBytes > 0 )
    {
        BufferLock lock[ MAX_NUM_BUFFERS ];
    
        // Lock the requested number of bytes in the sound buffers
        for( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
        {
            if ( NULL != mpIDirectSoundBuffers[ i ] )
            {
                hr = mpIDirectSoundBuffers[ i ]->Lock( 
                    mWrite,                    // lock position
                    dirtyBytes,                // num bytes to lock
                    &lock[ i ].pBuffer1,       // returns address of write buffer 1
                    &lock[ i ].numBytes1,      // returns size of write buffer 1
                    &lock[ i ].pBuffer2,       // returns address of write buffer 2
                    &lock[ i ].numBytes2,      // returns size of write buffer 2
                    NULL );                    // optionally specify flags
                ASSERT( SUCCEEDED( hr ) );  
            }
        }
        
        // Write sound data from each channel into the buffers
        // If the data is stereo, then the left and right channels
        // are interleaved: LRLRLRLRLRLRLR.
        // Note that the number of bytes to write will always be a 
        // multiple of the sample size
        unsigned int bytesWritten = 0;
        unsigned int bytesToWrite = lock[ 0 ].numBytes1;
        unsigned int bytesPerSample = mpFile->GetFormat( ).wBitsPerSample / 8;
        ASSERT( ( bytesToWrite % bytesPerSample ) == 0 ); 
        
        while( bytesWritten < bytesToWrite )        
        {
            for( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
            {
                if ( NULL != lock[ i ].pBuffer1 )
                {
                    // File will clear buffer if it has no more data
                    mpFile->Read( ( char* ) lock[ i ].pBuffer1 + bytesWritten, bytesPerSample );
                }     
            }                
            
            bytesWritten += bytesPerSample;
        } 
        
        // Update our trackers
        mWriteCount += bytesToWrite;
        mWrite = ( mWrite + bytesToWrite ) % mBufferSize;

        // Unlock the buffers
        for( unsigned int i = 0; i < MAX_NUM_BUFFERS; i++ )
        {
            if ( NULL != mpIDirectSoundBuffers[ i ] )
            {
                // Unlock the buffer
                hr = mpIDirectSoundBuffers[ i ]->Unlock( 
                    lock[ i ].pBuffer1,
                    lock[ i ].numBytes1,
                    lock[ i ].pBuffer2,
                    lock[ i ].numBytes2 );
                ASSERT( SUCCEEDED( hr ) );                
            }
        }
    }        
}       

//----------------------------------------------------------------------------
// This formula will maintain a constant power between the channels
//----------------------------------------------------------------------------
int SoundStreamer::CalculateSecondaryVolume( int primaryVolume )
{
    // Clamp value to something we can work with.  
    if ( primaryVolume >= DSBVOLUME_MAX )
    {
        return DSBVOLUME_MIN;
    }

    // Get out of hundreths of deciBels (milliBels), which is what DirectSound
    // works in.
    float primaryPower = pow( 10.0f, primaryVolume / 1000.0f );

    // We can maintain contant power by ensure both values add to 1
    float secondaryPower = 1.0f - primaryPower;

    // Now go back to milliBels
    int secondaryVolume = ( int ) (1000.0f * log10( secondaryPower ));

    // Clamp value
    if ( secondaryVolume < DSBVOLUME_MIN )
    {
        secondaryVolume = DSBVOLUME_MIN;
    }
    if ( secondaryVolume > DSBVOLUME_MAX )
    {
         secondaryVolume = DSBVOLUME_MAX;
    }


    return (int) secondaryVolume;
}
    
//----------------------------------------------------------------------------
// This formula will the next volume value while fading int
//----------------------------------------------------------------------------
int SoundStreamer::IncrementFadeInVolume( int volume )
{
    if ( volume >= DSBVOLUME_MAX )
    {
        return volume;
    }

    if ( volume < DSBVOLUME_MIN )
    {
        volume = DSBVOLUME_MIN;
    }

    // We map the current volume to a curve and then increment to
    // the next value
    float y = volume / ( float ) DSBVOLUME_MIN;
    float x = pow( y, 0.5f );
    x = x - FADE_INCREMENT_VALUE;
    if ( x < 0.0f )
    {
        x = 0.0f;
    }     
    y = x * x;
    return ( int ) y * DSBVOLUME_MIN;
}



