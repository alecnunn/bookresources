#include "stdafx.h"
#include "binaryfile.h"

//C-runtime libs
#include <string.h>

//API includes
#include <core_defs.h>

BinaryFile::BinaryFile():
m_pFileHandle(NULL)
{
}

BinaryFile::~BinaryFile()
{
	if(NULL != m_pFileHandle)
	{
		fclose(m_pFileHandle);
		m_pFileHandle = NULL;
	}
}

GEN_RESULT BinaryFile::Open(const char* szFilename, EFileFlags eFlags)
{
	char szFlags[8];
	memset(szFlags, 0, 8);

	switch(eFlags) 
	{
	case READONLY:
		{
			strncpy(szFlags, "rb", 7);
		}
		break;
	case WRITEONLY:
		{
			strncpy(szFlags, "wb", 7);
		}
		break;
	case APPEND:
		{
			strncpy(szFlags, "ab", 7);
		}
		break;
	case READWRITE:
		{
			strncpy(szFlags, "r+", 7);
		}
		break;
	case READWRITE_OVERWRITE:
		{
			strncpy(szFlags, "w+", 7);
		}
		break;
	default:
		{
			// the default is read only...
			strncpy(szFlags, "rb", 7);
		}
		break;
	}

	m_pFileHandle = fopen(szFilename, szFlags);

	if(NULL == m_pFileHandle)
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

void BinaryFile::Close()
{
	if(NULL != m_pFileHandle)
	{
		fclose(m_pFileHandle);
		m_pFileHandle = NULL;
	}
}

GEN_RESULT BinaryFile::Read(void* pBuffer, int nBytes)
{
	assert(m_pFileHandle);	
	assert(pBuffer);

	if(NULL == m_pFileHandle)
	{
		return RESULT_ERROR;
	}

	// read it.
	size_t ret = fread(pBuffer, 1, nBytes, m_pFileHandle);

	// check the result.
	if((0 == ret) || ((int)ret < nBytes))
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

GEN_RESULT BinaryFile::Write(void* pBuffer, int nBytes)
{
	assert(m_pFileHandle);
	assert(pBuffer);

	if(NULL == m_pFileHandle)
	{
		return RESULT_ERROR;
	}

	// write it.
	size_t ret = fwrite(pBuffer, 1, nBytes, m_pFileHandle);

	// check the result.
	if((0 == ret) || ((int)ret < nBytes))
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

GEN_RESULT BinaryFile::SeekTo(long nOffset)
{
	int ret = fseek(m_pFileHandle, nOffset, SEEK_SET);
	if(ret != 0)
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}
