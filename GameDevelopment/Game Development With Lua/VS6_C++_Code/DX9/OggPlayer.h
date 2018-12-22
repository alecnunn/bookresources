#include    <windows.h>                 // from your fave include dir ;)
#include    <mmreg.h>

#include    <dsound.h>                  // from the directx 8 sdk

#include    "vorbis/codec.h"            // from the vorbis sdk
#include    "vorbis/vorbisfile.h"       // also :)

#define     BUFSIZE    65536*2         // buffer length

class OggPlayer
{
protected:

    bool        bFileOpened;            // have we opened an ogg yet?

    LPDIRECTSOUND
                pDS;                    // the directsound 8 object

    LPDIRECTSOUNDBUFFER
                pDSB;                   // the buffer

    OggVorbis_File 
                vf;                     // for the vorbisfile interface

    int         nLastSection,           // which half of the buffer are/were
                nCurSection;            // we playing?

    bool        bAlmostDone;            // only one half of the buffer to play
    bool        bDone;                  // done playing
    bool        bLoop;                  // loop?

public:

    OggPlayer(LPDIRECTSOUND _pDS);
    ~OggPlayer();

    bool                                // this opens an oggvorbis for playing
        OpenOgg(const char *filename );

    void                                // and this one closes it :)
        Close();

    void                                // play it again sam
        Play(bool loop = false);

    void                                // stop it
        Stop();

    void                                // be sure to call this from time to time
        Update();                       

    inline bool IsPlaying()
    {
        return !bDone;
    }
};