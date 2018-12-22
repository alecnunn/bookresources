//----------------------------------------------------------------------------
// File: demo.hpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#ifndef DEMO_HPP
#define DEMO_HPP

//----------------------------------------------------------------------------
#include "soundstreamer.hpp"

//----------------------------------------------------------------------------
// The demo consists of a square room centered around the origin.
// A piano is located in the middle of the room.
// The sound of the piano reflects whether the listener is inside 
// or outside of the room.
//----------------------------------------------------------------------------
class Demo
{
    public:
        
        // Specify the length of the walls of the square room.
        // Specify the length of the world
        // Specify the filename for the piano music.
        Demo( float roomLength, const char* pPianoSoundFileName );
        ~Demo( void );

        // Determine if the listener is located inside the room
        bool IsListenerInsideRoom( void ) const;

        // Update the demo and keep it going.  
        void Update( void );
        
    private:     
    
        // DirectSound positional information for the piano
        DS3DBUFFER mPiano3dBufferInfo;
        
        // A sound player that will play the music of the piano
        SoundStreamer* mpPianoSoundStreamer;
        
        // The sound file containing the piano music.  This is a stereo file.
        // One channel of the file sounds like the piano from inside the room,
        // the other sounds like the piano from outside the room.
        SoundFile* mpPianoSoundFile;
        
        // The dimension of the square room that contains the piano.
        // The room is centered around the origin, which means this
        // dimension can be used to determine if the listener is inside
        // or outside.
        float mRoomLength;
};

#endif // DEMO_HPP