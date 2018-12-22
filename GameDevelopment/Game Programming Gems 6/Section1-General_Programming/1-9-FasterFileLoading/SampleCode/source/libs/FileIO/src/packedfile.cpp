#include "stdafx.h"
#include "packedfile.h"
#include "resourcefile.h"

PackedFile::PackedFile():
m_nFileSize(0),
m_nBytesRead(0),
m_pResourceFile(NULL)
{
}

PackedFile::~PackedFile()
{
}

void PackedFile::Close()
{
	delete this;
}

GEN_RESULT PackedFile::Read(void* pBuffer, int nBytes)
{
	assert(m_pResourceFile);	
	assert(pBuffer);

	BaseFile* pResFileHandle = m_pResourceFile->GetFileHandle();
	if(NULL == pResFileHandle)
	{
		return RESULT_ERROR;
	}

	if((m_nBytesRead + nBytes) > m_nFileSize)
	{
		return RESULT_ERROR;
	}

	if(pResFileHandle->Read(pBuffer, nBytes) == RESULT_ERROR)
	{
		return RESULT_ERROR;
	}

	m_nBytesRead += nBytes;

	return RESULT_OK;
}

//TODO: add SeekTo()
