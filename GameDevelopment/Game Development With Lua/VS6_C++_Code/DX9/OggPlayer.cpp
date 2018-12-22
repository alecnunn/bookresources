#include "StdAFX.h"
#include "oggplayer.h"

OggPlayer::OggPlayer(LPDIRECTSOUND _pDS)
{
    bFileOpened     = false;
    pDS             = _pDS;
    pDSB            = NULL;
    bLoop           = false;
    bDone           = false;
    bAlmostDone     = false;
}

OggPlayer::~OggPlayer()
{
    if (bFileOpened)
        Close();

}

bool OggPlayer::OpenOgg(const char *filename )
{
    if (bFileOpened)
        Close();

    FILE    *f;
	// prepend the directory name for music files
	std::string path = "Music\\";
	path += filename;

    f = fopen(path.c_str(), "rb");
    if (!f) return false;

    ov_open(f, &vf, NULL, 0);

    // ok now the tricky part

    // the vorbis_info struct keeps the most of the interesting format info
    vorbis_info *vi = ov_info(&vf,-1);

    // set the wave format
	WAVEFORMATEX	    wfm;

    memset(&wfm, 0, sizeof(wfm));

    wfm.cbSize          = sizeof(wfm);
    wfm.nChannels       = vi->channels;
    wfm.wBitsPerSample  = 16;                    // ogg vorbis is always 16 bit
    wfm.nSamplesPerSec  = vi->rate;
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec*wfm.nChannels*2;
    wfm.nBlockAlign     = 2*wfm.nChannels;
    wfm.wFormatTag      = 1;


    // set up the buffer
	DSBUFFERDESC desc;

	desc.dwSize         = sizeof(desc);
	desc.dwFlags        = 0;
	desc.lpwfxFormat    = &wfm;
	desc.dwReserved     = 0;

    desc.dwBufferBytes  = BUFSIZE*2;
    int res = pDS->CreateSoundBuffer(&desc, &pDSB, NULL );

    // fill the buffer

    DWORD   pos = 0;
    int sec = 0;
    int     ret = 1;
    DWORD   size = BUFSIZE*2;

    char    *buf;

    res = pDSB->Lock(0, size, (LPVOID*)&buf, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    
    // now read in the bits
    while(ret && pos<size)
    {
        ret = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
        pos += ret;
    }

	res = pDSB->Unlock( buf, size, NULL, NULL );

    nCurSection         =
    nLastSection        = 0;

    return bFileOpened = true;
}

void OggPlayer::Close()
{
    bFileOpened = false;

    SAFE_RELEASE(pDSB);
}


void OggPlayer::Play(bool loop)
{
    if (bFileOpened)
	{
		// play looping because we will fill the buffer
		HRESULT hr = pDSB->Play(0,0,DSBPLAY_LOOPING);    

		bLoop = loop;
		bDone = false;
		bAlmostDone = false;
	}
}

void OggPlayer::Stop()
{
    if (bFileOpened)
	{
	    pDSB->Stop();
	}
}

void OggPlayer::Update()
{
    DWORD pos;

	if(pDSB)
	{
		pDSB->GetCurrentPosition(&pos, NULL);

		nCurSection = pos<BUFSIZE?0:1;

		// section changed?
		if (nCurSection != nLastSection)
		{
			if (bDone && !bLoop)
				Stop();

			// gotta use this trick 'cause otherwise there wont be played all bits
			if (bAlmostDone && !bLoop)
				bDone = true;

			DWORD   size = BUFSIZE;
			char    *buf;

			// fill the section we just left
			pDSB->Lock( nLastSection*BUFSIZE, size, (LPVOID*)&buf, &size, NULL, NULL, 0 );

			DWORD   pos = 0;
			int     ret = 1;
                
			int sec;
			while(ret && pos<size)
			{
				ret = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
				pos += ret;
			}

			printf("Pos:%d\n", pos);

			// reached the and?
			if (!ret && bLoop)
			{
				// we are looping so restart from the beginning
				// NOTE: sound with sizes smaller than BUFSIZE may be cut off

				ret = 1;
				ov_pcm_seek(&vf, 0);
				while(ret && pos<size)
				{
					ret = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
					pos += ret;
				}
			}
			else if (!ret && !(bLoop))
			{
				// not looping so fill the rest with 0
				while(pos<size)
					*(buf+pos)=0; pos ++;

				// and say that after the current section no other sectin follows
				bAlmostDone = true;
			}
                
			pDSB->Unlock( buf, size, NULL, NULL );
    
			nLastSection = nCurSection;
		}
	}
}