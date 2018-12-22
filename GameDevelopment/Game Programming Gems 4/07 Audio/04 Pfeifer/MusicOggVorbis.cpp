#include "stdafx.h"
#include "MusicOggVorbis.h"
#include "OggVorbisFile.h"


MusicOggVorbis::MusicOggVorbis(const char* szFileName)
:	Music(),
	m_pOVFile(NULL)
{
	m_pOVFile = new OggVorbisFile();
	if (m_pOVFile)
	{
		if (!m_pOVFile->Open(szFileName))
		{
			delete m_pOVFile;
			m_pOVFile = NULL;
		}
	}

	ASSERT(m_pOVFile && "Couldn't find/open .ogg file.");
}


MusicOggVorbis::~MusicOggVorbis()
{
	if (m_pOVFile)
	{
		m_pOVFile->Close();
		delete m_pOVFile;
		m_pOVFile = NULL;
	}
}


bool MusicOggVorbis::FillBuffer(LPDIRECTSOUNDBUFFER pDSB,
								DWORD startIndex,
								DWORD amount,
								DWORD* pAmtRead)
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    VOID*   pDSLockedBuffer2      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize2 = 0;    // Size of the locked DirectSound buffer

    if (pDSB == NULL)
        return false;

    if (FAILED(hr = pDSB->Lock(startIndex, amount, 
                               &pDSLockedBuffer, &dwDSLockedBufferSize, 
							   NULL, NULL, 0L)))
	{
        DXTRACE_ERR(TEXT("Lock"), hr);
		return false;
	}


	int read = dwDSLockedBufferSize;
	bool bMore = m_pOVFile->Read((char*)pDSLockedBuffer, read);

	// Fill Buffer with silence first because we won't get the exact
	// amount read back from ogg. Assume bit stream > 8bit, therefore
	// silence is 0, not 128.
    FillMemory(((BYTE*)pDSLockedBuffer)+read, dwDSLockedBufferSize-read, 0);

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

	if (pAmtRead)
		*pAmtRead = read;

    return bMore;
}

void MusicOggVorbis::Reset()
{
	if (m_pOVFile)
	{
		m_pOVFile->Reset();
	}
}

unsigned int MusicOggVorbis::GetTotalTime()
{
	ASSERT(m_pOVFile);
	return m_pOVFile->GetTotalTime();
}