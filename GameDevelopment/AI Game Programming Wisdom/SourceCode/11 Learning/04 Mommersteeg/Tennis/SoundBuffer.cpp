//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SoundBuffer.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "SoundBuffer.h"

//----------------------------------------------------------------------------------------------
// CSoundBuffer(): constructor
//----------------------------------------------------------------------------------------------

CSoundBuffer::CSoundBuffer()
{
	lpDS = NULL;
	lpDSB = NULL;
}

//----------------------------------------------------------------------------------------------
// ~CSoundBuffer(): destructor
//----------------------------------------------------------------------------------------------

CSoundBuffer::~CSoundBuffer()
{
	Release();
}

//----------------------------------------------------------------------------------------------
// CSoundBuffer(): creates a new soundbuffer according the specified requirements
//----------------------------------------------------------------------------------------------

void CSoundBuffer::Create(LPDS lpds, DWORD dwBufferSize, DWORD dwFrequency, WORD nChannels, WORD wBitsPerSample, DWORD dwFlags )
{
	lpDS = lpds;

	DSBD dsbd;
	WAVEFORMATEX wfx;

	// set up wave format
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = nChannels;
	wfx.nSamplesPerSec = dwFrequency;
	wfx.wBitsPerSample = wBitsPerSample;
	wfx.nBlockAlign = nChannels * wBitsPerSample / 8;
	wfx.nAvgBytesPerSec = dwFrequency * wfx.nBlockAlign;
	wfx.cbSize = 0;

	// set up sound buffer description
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = dwFlags | DSBCAPS_STATIC;
	dsbd.dwBufferBytes = dwBufferSize;
	dsbd.lpwfxFormat = &wfx;

	// create sound buffer
	lpDS->CreateSoundBuffer(&dsbd, &lpDSB, NULL);

	// fill sound buffer with silence
	Silence();
}

//----------------------------------------------------------------------------------------------
// CSoundBuffer(): creates a new soundbuffer using the specified wave file
//----------------------------------------------------------------------------------------------

void CSoundBuffer::CreateWave(LPDS lpds, LPSTR strWave, DWORD dwFlags)
{
	lpDS = lpds;

	DSBD	dsbd;
	LPVOID	lpvAudio;
	DWORD	dwBytes;
	UINT	cbBytesRead;

	// open file for input
    WaveOpenFile( strWave, &hmmio, &pwfx, &mmckinfoParent );

	// prepare file for input
    WaveStartDataRead( &hmmio, &mmckinfo, &mmckinfoParent );

	// set up buffer description
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = dwFlags | DSBCAPS_STATIC;
	dsbd.dwBufferBytes = mmckinfo.cksize;
	dsbd.lpwfxFormat = pwfx;

	// create sound buffer
	lpDS->CreateSoundBuffer(&dsbd, &lpDSB, NULL);

	// read wave data into buffer
	lpDSB->Lock(0, 0, &lpvAudio, &dwBytes, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	WaveReadFile( hmmio, dwBytes, (BYTE *)lpvAudio, &mmckinfo, &cbBytesRead );
	lpDSB->Unlock( lpvAudio, dwBytes, NULL, 0);

	// close file
	WaveCloseReadFile( &hmmio, &pwfx );
}

//----------------------------------------------------------------------------------------------
// CreateCopy(): creates a reference copy of the sound buffer
//----------------------------------------------------------------------------------------------

void CSoundBuffer::CreateCopy(LPDS lpds, LPDSB lpDSB)
{
	lpDS = lpds;
	lpDSB = lpDSB;
}

//----------------------------------------------------------------------------------------------
// CreateDuplicate(): creates a duplicate of the sound buffer
//----------------------------------------------------------------------------------------------

void CSoundBuffer::CreateDuplicate(LPDS lpds, LPDSB lpDSB)
{
	lpDS = lpds;
	lpDS->DuplicateSoundBuffer(lpDSB, &this->lpDSB);
}

//----------------------------------------------------------------------------------------------
// Release(): releases the sound buffer
//----------------------------------------------------------------------------------------------

void CSoundBuffer::Release()
{
	if ( lpDSB != NULL )
	{
		lpDSB->Release();
		lpDSB = NULL;
	}
	lpDS = NULL;
}

//----------------------------------------------------------------------------------------------
// ReleaseCopy(): releases a copy of the sound buffer
//----------------------------------------------------------------------------------------------

void CSoundBuffer::ReleaseCopy()
{
	lpDSB = NULL;
	lpDS = NULL;
}

//----------------------------------------------------------------------------------------------
// Play(): plays the soundbuffer
//----------------------------------------------------------------------------------------------

void CSoundBuffer::Play(BOOL bLooping)
{
	HRESULT hr = lpDSB->Play(0, 0, bLooping ? DSBPLAY_LOOPING : 0);

	if ( FAILED( hr ) )
	{
		if ( hr == DSERR_BUFFERLOST )
		{
			// restore buffer and resume
			lpDSB->Restore();
			lpDSB->Play(0, 0, bLooping ? DSBPLAY_LOOPING : 0);
		}
	}
}

//----------------------------------------------------------------------------------------------
// Stop(): stops a playing sound buffer
//----------------------------------------------------------------------------------------------

void CSoundBuffer::Stop()
{
	lpDSB->Stop();
}

//----------------------------------------------------------------------------------------------
// Silence(): fills a sound buffer with silence
//----------------------------------------------------------------------------------------------

void CSoundBuffer::Silence()
{
    WAVEFORMATEX    wfx;
    DWORD           dwSizeWritten;
    PBYTE			pb;
    DWORD			cb;

    lpDSB->GetFormat( &wfx, sizeof( WAVEFORMATEX ), &dwSizeWritten );

    lpDSB->Lock( 0, 0, (LPVOID *)&pb, &cb, NULL, NULL, DSBLOCK_ENTIREBUFFER );
    FillMemory( pb, cb, ( wfx.wBitsPerSample == 8 ) ? 128 : 0 );
    lpDSB->Unlock( pb, cb, NULL, 0 );
}

//----------------------------------------------------------------------------------------------
// WaveOpenFile(): This function will open a wave input file and prepare it for reading, so the 
// data can be easily read with WaveReadFile. Returns 0 if successful, the error code if not.
//
//      pszFileName - Input filename to load.
//      phmmioIn    - Pointer to handle which will be used for further mmio routines.
//      ppwfxInfo   - Ptr to ptr to WaveFormatEx structure with all info about the file.
//----------------------------------------------------------------------------------------------

int CSoundBuffer::WaveOpenFile(
    TCHAR*pszFileName,                              // (IN)
    HMMIO *phmmioIn,                                // (OUT)
    WAVEFORMATEX **ppwfxInfo,                       // (OUT)
    MMCKINFO *pckInRIFF                             // (OUT)
            )
{
    HMMIO           hmmioIn;
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.
    WORD            cbExtraAlloc;   // Extra bytes for waveformatex
    int             nError;         // Return value.


    // Initialization...
    *ppwfxInfo = NULL;
    nError = 0;
    hmmioIn = NULL;

    if ((hmmioIn = mmioOpen(pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
        {
        nError = ER_CANNOTOPEN;
        goto ERROR_READING_WAVE;
        }

    if ((nError = (int)mmioDescend(hmmioIn, pckInRIFF, NULL, 0)) != 0)
        {
        goto ERROR_READING_WAVE;
        }


    if ((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
        {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
        }

    /* Search the input file for for the 'fmt ' chunk.     */
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ((nError = (int)mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
        {
        goto ERROR_READING_WAVE;
        }

    /* Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    * if there are extra parameters at the end, we'll ignore them */

    if (ckIn.cksize < (long) sizeof(PCMWAVEFORMAT))
        {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
        }

    /* Read the 'fmt ' chunk into <pcmWaveFormat>.*/
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, (long) sizeof(pcmWaveFormat)) != (long) sizeof(pcmWaveFormat))
        {
        nError = ER_CANNOTREAD;
        goto ERROR_READING_WAVE;
        }


    // Ok, allocate the waveformatex, but if its not pcm
    // format, read the next word, and thats how many extra
    // bytes to allocate.
    if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
        cbExtraAlloc = 0;

    else
        {
        // Read in length of extra bytes.
        if (mmioRead(hmmioIn, (LPTSTR) &cbExtraAlloc,
            (long) sizeof(cbExtraAlloc)) != (long) sizeof(cbExtraAlloc))
            {
            nError = ER_CANNOTREAD;
            goto ERROR_READING_WAVE;
            }

        }

    // Ok, now allocate that waveformatex structure.
    if ((*ppwfxInfo = (WAVEFORMATEX *)GlobalAlloc(GMEM_FIXED, sizeof(WAVEFORMATEX)+cbExtraAlloc)) == NULL)
        {
        nError = ER_MEM;
        goto ERROR_READING_WAVE;
        }

    // Copy the bytes from the pcm structure to the waveformatex structure
    memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
    (*ppwfxInfo)->cbSize = cbExtraAlloc;

    // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
    if (cbExtraAlloc != 0)
        {
        if (mmioRead(hmmioIn, (LPTSTR) (((BYTE*)&((*ppwfxInfo)->cbSize))+sizeof(cbExtraAlloc)),
            (long) (cbExtraAlloc)) != (long) (cbExtraAlloc))
            {
            nError = ER_NOTWAVEFILE;
            goto ERROR_READING_WAVE;
            }
        }

    /* Ascend the input file out of the 'fmt ' chunk. */
    if ((nError = mmioAscend(hmmioIn, &ckIn, 0)) != 0)
        {
        goto ERROR_READING_WAVE;

        }


    goto TEMPCLEANUP;

ERROR_READING_WAVE:
    if (*ppwfxInfo != NULL)
        {
        GlobalFree(*ppwfxInfo);
        *ppwfxInfo = NULL;
        }

    if (hmmioIn != NULL)
    {
    mmioClose(hmmioIn, 0);
        hmmioIn = NULL;
        }

TEMPCLEANUP:
    *phmmioIn = hmmioIn;

    return(nError);

}

//----------------------------------------------------------------------------------------------
// WaveStartDataRead(): This routine has to be called before WaveReadFile as it searchs for the 
// chunk to descend into for reading, that is, the 'data' chunk.  For simplicity, this used to 
// be in the open routine, but was taken out and moved to a separate routine so there was more 
// control on the chunks that are before the data chunk, such as 'fact', etc...
//----------------------------------------------------------------------------------------------

int CSoundBuffer::WaveStartDataRead(
                    HMMIO *phmmioIn,
                    MMCKINFO *pckIn,
                    MMCKINFO *pckInRIFF
                    )
{
    int                     nError;

    nError = 0;

    // Do a nice little seek...
    if ((nError = mmioSeek(*phmmioIn, pckInRIFF->dwDataOffset + sizeof(FOURCC), SEEK_SET)) == -1)
        {
        _ASSERT(FALSE);
        }

    nError = 0;
    //      Search the input file for for the 'data' chunk.
    pckIn->ckid = mmioFOURCC('d', 'a', 't', 'a');
    if ((nError = mmioDescend(*phmmioIn, pckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
        {
        goto ERROR_READING_WAVE;
        }

    goto CLEANUP;

ERROR_READING_WAVE:

CLEANUP:
    return(nError);
}

//----------------------------------------------------------------------------------------------
// WaveFileRad(): This will read wave data from the wave file. Make sure we're descended into
// the data chunk, else this will fail bigtime!
//
//  hmmioIn         - Handle to mmio.
//  cbRead          - # of bytes to read.
//  pbDest          - Destination buffer to put bytes.
//  cbActualRead- # of bytes actually read.
//----------------------------------------------------------------------------------------------

int CSoundBuffer::WaveReadFile(
        HMMIO hmmioIn,                          // IN
        UINT cbRead,                            // IN
        BYTE *pbDest,                           // IN
        MMCKINFO *pckIn,                        // IN.
        UINT *cbActualRead                      // OUT.

        )
{

    MMIOINFO    mmioinfoIn;         // current status of <hmmioIn>
    int                     nError;
    UINT            cT, cbDataIn;

    nError = 0;

    if (nError = mmioGetInfo(hmmioIn, &mmioinfoIn, 0) != 0)
        {
        goto ERROR_CANNOT_READ;
        }

    cbDataIn = cbRead;
    if (cbDataIn > pckIn->cksize)
        cbDataIn = pckIn->cksize;

    pckIn->cksize -= cbDataIn;

    for (cT = 0; cT < cbDataIn; cT++)
        {
        /* Copy the bytes from the io to the buffer. */
        if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
            {
        if ((nError = mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ)) != 0)
                {
        goto ERROR_CANNOT_READ;
                }
        if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
                {
                nError = ER_CORRUPTWAVEFILE;
        goto ERROR_CANNOT_READ;
                }
            }

        // Actual copy. Transform.
        *((BYTE*)pbDest+cT) = *((BYTE*)mmioinfoIn.pchNext);
		mmioinfoIn.pchNext = ((char *)mmioinfoIn.pchNext+1);
        }

    if ((nError = mmioSetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
        {
        goto ERROR_CANNOT_READ;
        }

    *cbActualRead = cbDataIn;
    goto FINISHED_READING;

ERROR_CANNOT_READ:
    *cbActualRead = 0;

FINISHED_READING:
    return(nError);

}


//----------------------------------------------------------------------------------------------
// WaveCloseReadFile(): This will close the wave file opened with WaveOpenFile.
//    phmmioIn - Pointer to the handle to input MMIO.
//    ppwfxSrc - Pointer to pointer to WaveFormatEx structure.
//
//    Returns 0 if successful, non-zero if there was a warning.
//----------------------------------------------------------------------------------------------

int CSoundBuffer::WaveCloseReadFile(
            HMMIO *phmmio,                          // IN
            WAVEFORMATEX **ppwfxSrc                 // IN
            )
{

    if (*ppwfxSrc != NULL)
        {
        GlobalFree(*ppwfxSrc);
        *ppwfxSrc = NULL;
        }

    if (*phmmio != NULL)
        {
        mmioClose(*phmmio, 0);
        *phmmio = NULL;
        }

    return(0);

}

//----------------------------------------------------------------------------------------------
// WaveLoadFile(): This routine loads a full wave file into memory. Be careful, wave files can 
// get pretty big these days :).
//
//    szFileName      -       sz Filename
//    cbSize          -       Size of loaded wave (returned)
//    cSamples        -       # of samples loaded.
//    ppwfxInfo       -       Pointer to pointer to waveformatex structure. The wfx structure 
//							  IS ALLOCATED by this routine!  Make sure to free it!
//    ppbData         -       Pointer to a byte pointer (globalalloc) which is allocated by this
//							  routine.  Make sure to free it!
//
//    Returns 0 if successful, else the error code.
//----------------------------------------------------------------------------------------------

int CSoundBuffer::WaveLoadFile(
            TCHAR*pszFileName,                      // (IN)
            UINT *cbSize,                           // (OUT)
            WAVEFORMATEX **ppwfxInfo,				// (OUT)
            BYTE **ppbData                          // (OUT)
            )
{

    HMMIO                           hmmioIn;
    MMCKINFO                        ckInRiff;
    MMCKINFO                        ckIn;
    int                                     nError;
    UINT                            cbActualRead;

    *ppbData = NULL;
    *ppwfxInfo = NULL;
    *cbSize = 0;

    if ((nError = WaveOpenFile(pszFileName, &hmmioIn, ppwfxInfo, &ckInRiff)) != 0)
        {
        goto ERROR_LOADING;
        }

    if ((nError = WaveStartDataRead(&hmmioIn, &ckIn, &ckInRiff)) != 0)
        {
        goto ERROR_LOADING;
        }

    // Ok, size of wave data is in ckIn, allocate that buffer.
    if ((*ppbData = (BYTE *)GlobalAlloc(GMEM_FIXED, ckIn.cksize)) == NULL)
        {
        nError = ER_MEM;
        goto ERROR_LOADING;
        }

    if ((nError = WaveReadFile(hmmioIn, ckIn.cksize, *ppbData, &ckIn, &cbActualRead)) != 0)
        {
        goto ERROR_LOADING;
        }

    *cbSize = cbActualRead;
    goto DONE_LOADING;

ERROR_LOADING:
    if (*ppbData != NULL)
        {
        GlobalFree(*ppbData);
        *ppbData = NULL;
        }
    if (*ppwfxInfo != NULL)
        {
        GlobalFree(*ppwfxInfo);
        *ppwfxInfo = NULL;
        }

DONE_LOADING:
    // Close the wave file.
    if (hmmioIn != NULL)
        {
        mmioClose(hmmioIn, 0);
        hmmioIn = NULL;
        }

    return(nError);

}
