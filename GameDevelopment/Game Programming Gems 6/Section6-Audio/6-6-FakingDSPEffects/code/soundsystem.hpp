//----------------------------------------------------------------------------
// File: soundsystem.hpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#ifndef SOUNDSYSTEM_HPP
#define SOUNDSYSTEM_HPP

#include <dsound.h>

//----------------------------------------------------------------------------
class SoundSystem
{
    public:
        void CreateDirectSoundBuffer( IDirectSoundBuffer** ppIDirectSoundBuffer, WAVEFORMATEX* pWfx, unsigned int bufferBytes, bool isPositional );
        void SetListenerPosition( const D3DVECTOR& pos );
        void GetListenerPosition( D3DVECTOR& pos ) const;

        static void Initialize( HWND hWnd );
        static void Terminate( void );
        static SoundSystem* Get( void ) { return spSoundSystem; }

    private:

        SoundSystem( HWND hWnd );
        ~SoundSystem( void );
        
        static SoundSystem* spSoundSystem;
    
        IDirectSound8* mpDirectSound;
        IDirectSoundBuffer* mpPrimaryDirectSoundBuffer;
        IDirectSound3DListener* mpDirectSound3DListener;
};

#endif // SOUNDSYSTEM_HPP
