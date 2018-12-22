//----------------------------------------------------------------------------
// File: demo.cpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#include "demo.hpp"
#include <math.h>
#include "soundsystem.hpp"

//----------------------------------------------------------------------------
static const unsigned int SOUND_STREAMER_BUFFER_SIZE = 128 * 1024;
static const unsigned int CHANNEL_INSIDE_ROOM = 0;
static const unsigned int CHANNEL_OUTSIDE_ROOM = 1;

//----------------------------------------------------------------------------
Demo::Demo( float roomLength, const char* pPianoSoundFileName )
    : 
    mpPianoSoundStreamer( NULL ),
    mpPianoSoundFile( NULL ),
    mRoomLength( roomLength)
{
    // Prepare the positional information for the piano
    ::memset( &mPiano3dBufferInfo, 0, sizeof( mPiano3dBufferInfo ) );
    mPiano3dBufferInfo.dwSize = sizeof( DS3DBUFFER );
    mPiano3dBufferInfo.dwInsideConeAngle = DS3D_DEFAULTCONEANGLE;
    mPiano3dBufferInfo.dwOutsideConeAngle = DS3D_DEFAULTCONEANGLE;
    mPiano3dBufferInfo.vConeOrientation.z = 1.0f;
    mPiano3dBufferInfo.lConeOutsideVolume = DS3D_DEFAULTCONEOUTSIDEVOLUME;
    mPiano3dBufferInfo.flMinDistance = roomLength / 2;
    mPiano3dBufferInfo.flMaxDistance = mPiano3dBufferInfo.flMinDistance * 3;
    mPiano3dBufferInfo.dwMode = DS3DMODE_NORMAL;

    // Open the piano sound file
    mpPianoSoundFile = new SoundFile( );
    mpPianoSoundFile->Open( pPianoSoundFileName );

    // Create a sound streamer to represent the piano
    mpPianoSoundStreamer = new SoundStreamer( );
    mpPianoSoundStreamer->SetFile( mpPianoSoundFile, SOUND_STREAMER_BUFFER_SIZE, &mPiano3dBufferInfo );
    mpPianoSoundStreamer->SetActiveChannel( CHANNEL_OUTSIDE_ROOM );
    mpPianoSoundStreamer->Play( );
}

//----------------------------------------------------------------------------
Demo::~Demo( void )
{
    if ( NULL != mpPianoSoundStreamer )
    {
        delete mpPianoSoundStreamer;
        mpPianoSoundStreamer = NULL;
    }
    
    if ( NULL != mpPianoSoundFile )
    {
        delete mpPianoSoundFile;
        mpPianoSoundFile = NULL;
    }
}

//----------------------------------------------------------------------------
void Demo::Update( void )
{
    // Update the streamers
    if ( NULL != mpPianoSoundStreamer )
    {
        if ( IsListenerInsideRoom( ) )
        {
            // The listener is inside the room.  We select the channel that
            // sounds like a piano from within the room.
            mpPianoSoundStreamer->SetActiveChannel( CHANNEL_INSIDE_ROOM );
        }
        else
        {
            // The listener is outside the room.  We select the channel that
            // sounds like a piano from outside the room.
            mpPianoSoundStreamer->SetActiveChannel( CHANNEL_OUTSIDE_ROOM );
        }
        
        // Update the streamer to keep files loading, etc.
        mpPianoSoundStreamer->Update( );
    }
}

//----------------------------------------------------------------------------
bool Demo::IsListenerInsideRoom( void ) const
{
    D3DVECTOR listenerPos;
    SoundSystem::Get( )->GetListenerPosition( listenerPos );
    float room = mRoomLength / 2.0f;
    return ( ( fabs( listenerPos.x ) < room ) && ( fabs( listenerPos.y ) < room ) && ( fabs( listenerPos.z ) < room ) );
}
