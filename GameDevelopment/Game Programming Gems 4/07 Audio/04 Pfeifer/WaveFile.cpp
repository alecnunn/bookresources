#include "stdafx.h"
#include "wavefile.h"

#include <mmreg.h>

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
// Name: WaveFile::WaveFile()
// Desc: Constructs the class.  Call Open() to open a wave file for reading.  
//       Then call Read() as needed.  Calling the destructor or Close() 
//       will close the file.  
//-----------------------------------------------------------------------------
WaveFile::WaveFile()
:	m_pwfx(NULL),
	m_hmmio(NULL),
	m_dwSize(0),
	m_bIsReadingFromMemory(FALSE),
	m_pbData(NULL),
	m_pbDataCur(NULL),
	m_ulDataSize(0)
{
}




//-----------------------------------------------------------------------------
// Name: WaveFile::~WaveFile()
// Desc: Destructs the class
//-----------------------------------------------------------------------------
WaveFile::~WaveFile()
{
    Close();

    delete [] m_pwfx;
}




//-----------------------------------------------------------------------------
// Name: WaveFile::Open()
// Desc: Opens a wave file for reading
//-----------------------------------------------------------------------------
bool WaveFile::Open(LPCTSTR strFileName, WAVEFORMATEX* pwfx)
{
    HRESULT hr;

    m_bIsReadingFromMemory = FALSE;

    if (strFileName == NULL)
        return false;

    delete [] m_pwfx;

    m_hmmio = mmioOpen((char*)strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);

    if (NULL == m_hmmio)
    {
        HRSRC   hResInfo;
        HGLOBAL hResData;
        DWORD   dwSize;
        VOID*   pvRes;

        // Loading it as a file failed, so try it as a resource
        if (NULL == (hResInfo = FindResource(NULL, strFileName, TEXT("WAVE"))))
        {
            if (NULL == (hResInfo = FindResource( NULL, strFileName, TEXT("WAV"))))
			{
                DXTRACE_ERR(TEXT("FindResource"), E_FAIL);
				return false;
			}
        }

        if (NULL == (hResData = LoadResource(NULL, hResInfo)))
		{
            DXTRACE_ERR(TEXT("LoadResource"), E_FAIL);
			return false;
		}

        if (0 == (dwSize = SizeofResource(NULL, hResInfo))) 
		{
            DXTRACE_ERR(TEXT("SizeofResource"), E_FAIL);
			return false;
		}

        if (NULL == (pvRes = LockResource(hResData)))
		{
            DXTRACE_ERR(TEXT("LockResource"), E_FAIL);
			return false;
		}

        CHAR* pData = new CHAR[ dwSize ];
        memcpy( pData, pvRes, dwSize );

        MMIOINFO mmioInfo;
        ZeroMemory(&mmioInfo, sizeof(mmioInfo));
        mmioInfo.fccIOProc = FOURCC_MEM;
        mmioInfo.cchBuffer = dwSize;
        mmioInfo.pchBuffer = (CHAR*) pData;

        m_hmmio = mmioOpen(NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ);
    }

    if (FAILED(hr = ReadMMIO()))
    {
        // ReadMMIO will fail if its an not a wave file
        mmioClose(m_hmmio, 0);
        DXTRACE_ERR(TEXT("ReadMMIO"), hr);
		return false;
    }

    if (FAILED(hr = ResetFile()))
    {
		DXTRACE_ERR(TEXT("ResetFile"), hr);
		return false;
	}

    // After the reset, the size of the wav file is m_ck.cksize so store it now
    m_dwSize = m_ck.cksize;
	m_pbData = new BYTE[m_dwSize];
	Read(m_pbData, 0, m_dwSize, &m_ulDataSize);
	m_bIsReadingFromMemory = true;

	return SUCCEEDED(hr);
}


//-----------------------------------------------------------------------------
// Name: WaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx. 
//-----------------------------------------------------------------------------
HRESULT WaveFile::ReadMMIO()
{
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

    // Check to make sure this is a valid wave file
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        return DXTRACE_ERR( TEXT("mmioFOURCC"), E_FAIL ); 

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
       if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
           return DXTRACE_ERR( TEXT("sizeof(PCMWAVEFORMAT)"), E_FAIL );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
            return DXTRACE_ERR( TEXT("m_pwfx"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
            return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
            return DXTRACE_ERR( TEXT("new"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            delete m_pwfx;
            return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        delete m_pwfx;
        return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WaveFile::GetSize()
// Desc: Retuns the size of the read access wave file 
//-----------------------------------------------------------------------------
DWORD WaveFile::GetSize()
{
    return m_dwSize;
}




//-----------------------------------------------------------------------------
// Name: WaveFile::ResetFile()
// Desc: Resets the internal m_ck pointer so reading starts from the 
//       beginning of the file again 
//-----------------------------------------------------------------------------
HRESULT WaveFile::ResetFile()
{
    if( m_bIsReadingFromMemory )
    {
        m_pbDataCur = m_pbData;
    }
    else 
    {
        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        // Seek to the data
        if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
                        SEEK_SET ) )
            return DXTRACE_ERR( TEXT("mmioSeek"), E_FAIL );

        // Search the input file for the 'data' chunk.
        m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
        if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
          return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WaveFile::Read()
// Desc: Reads section of data from a wave file into pBuffer and returns 
//       how much read in pdwSizeRead, reading not more than dwSizeToRead.
//       This uses m_ck to determine where to start reading from.  So 
//       subsequent calls will be continue where the last left off unless 
//       Reset() is called.
//-----------------------------------------------------------------------------
bool WaveFile::Read(BYTE* pBuffer, DWORD dwOffset,
					   DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
    if( m_bIsReadingFromMemory )
    {
        if (m_pbData == NULL )
            return false;

        if (pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if (dwOffset > m_ulDataSize)
			return false;

		DWORD dwBufSize = dwSizeToRead;

        if (dwOffset + dwSizeToRead > m_ulDataSize)
            dwSizeToRead = m_ulDataSize - dwOffset;
        
        CopyMemory(pBuffer, m_pbData+dwOffset, dwSizeToRead);
        
		// if buffer size is larger, fill the rest with silence
		if (dwSizeToRead < dwBufSize)
		{
			FillMemory(pBuffer + dwSizeToRead, 
                       dwBufSize - dwSizeToRead, 
                       (BYTE)(m_pwfx->wBitsPerSample == 8 ? 128 : 0));
		}

        if( pdwSizeRead != NULL )
            *pdwSizeRead = dwSizeToRead;

        return true;
    }
    else 
    {
        MMIOINFO mmioinfoIn; // current status of m_hmmio

        if( m_hmmio == NULL )
            return false;
        if( pBuffer == NULL || pdwSizeRead == NULL )
            return false;

        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
		{
            DXTRACE_ERR(TEXT("mmioGetInfo"), E_FAIL);
			return false;
		}
                
        UINT cbDataIn = dwSizeToRead;
        if( cbDataIn > m_ck.cksize ) 
            cbDataIn = m_ck.cksize;       

        m_ck.cksize -= cbDataIn;
    
        for( DWORD cT = 0; cT < cbDataIn; cT++ )
        {
            // Copy the bytes from the io to the buffer.
            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
            {
                if (0 != mmioAdvance(m_hmmio, &mmioinfoIn, MMIO_READ))
				{
                    DXTRACE_ERR(TEXT("mmioAdvance"), E_FAIL);
					return false;
				}

                if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
				{
                    DXTRACE_ERR(TEXT("mmioinfoIn.pchNext"), E_FAIL);
					return false;
				}
            }

            // Actual copy.
            *((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
            mmioinfoIn.pchNext++;
        }

        if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
		{
            DXTRACE_ERR(TEXT("mmioSetInfo"), E_FAIL);
			return false;
		}

        if( pdwSizeRead != NULL )
            *pdwSizeRead = cbDataIn;

        return true;
    }
}




//-----------------------------------------------------------------------------
// Name: WaveFile::Close()
// Desc: Closes the wave file 
//-----------------------------------------------------------------------------
void WaveFile::Close()
{
    mmioClose( m_hmmio, 0 );
    m_hmmio = NULL;
	delete [] m_pbData;
	m_pbData = NULL;
}



