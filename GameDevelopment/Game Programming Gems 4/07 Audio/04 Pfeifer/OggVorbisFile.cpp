#include "stdafx.h"
#include <stdio.h>
#include "OggVorbisFile.h"
#include "vorbis\vorbisfile.h"

OggVorbisFile::OggVorbisFile()
:	m_pVorbisFile(NULL),
	m_size(0),
	m_read(0)
{
}


OggVorbisFile::~OggVorbisFile()
{
	if (m_pVorbisFile)
	{
		ASSERT(!"OggVorbisFile: File not closed before deletion");
		Close();
		delete m_pVorbisFile;
		m_pVorbisFile = NULL;
	}
}


bool OggVorbisFile::Open(const char* szFileName)
{
	ASSERT(!m_pVorbisFile);

	if (m_pVorbisFile)
		return false;
	FILE* pFile = fopen (szFileName, "rb");

	if (pFile)
	{
		m_pVorbisFile = new OggVorbis_File();
		if (0 == ov_open(pFile, m_pVorbisFile, NULL, 0))
		{
			vorbis_info *vi = ov_info(m_pVorbisFile, -1);
			m_size = ov_pcm_total(m_pVorbisFile, 0);
			m_read = 0;
			return true;
		}
		else
		{
			delete m_pVorbisFile;
			m_pVorbisFile = NULL;
			fclose(pFile);
		}
	}

	return false;
}


bool OggVorbisFile::Read(char* pBuf, int& amount)
{
	ASSERT(m_pVorbisFile);

	if (m_pVorbisFile)
	{
		int bitstream = 0;
		int currentRead = 0;
		int totalRead = 0;
		int leftToRead = amount;

		// Because ov_read only decodes AT MOST one vorbis packet, we may have
		// to read multiple times to get the desired amount. 
		do
		{
			currentRead = ov_read(m_pVorbisFile, pBuf+totalRead, leftToRead, 0, 2, 1, &bitstream);
			
			if (currentRead > 0)
			{
				totalRead += currentRead;
				m_read += currentRead;
				leftToRead -= currentRead;
			}
			else if (currentRead < 0)
			{
				ASSERT(false);
			}
		}
		while (currentRead > 0 && totalRead < amount);

		amount = totalRead;

		return ov_time_total(m_pVorbisFile, -1) - ov_time_tell(m_pVorbisFile) > 0.0f; //m_read < m_size;
	}
	else
	{
		amount = 0;
		return false;
	}
}


bool OggVorbisFile::Close()
{
	if (m_pVorbisFile)
	{
		int nRet = ov_clear(m_pVorbisFile);

		if (0 == nRet)
		{
			delete m_pVorbisFile;
			m_pVorbisFile = NULL;
			return true;
		}
	}

	return false;
}


bool OggVorbisFile::Reset()
{
	return (0 == ov_pcm_seek(m_pVorbisFile, 0));
}


unsigned int OggVorbisFile::GetTotalTime()
{
	double secs = ov_time_total(m_pVorbisFile, -1);
	return (unsigned int)(secs*1000);
}