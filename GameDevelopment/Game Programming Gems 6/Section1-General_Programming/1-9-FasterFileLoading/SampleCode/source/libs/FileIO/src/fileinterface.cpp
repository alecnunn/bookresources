#include "stdafx.h"
#include "fileinterface.h"
#include "binaryfile.h"
#include "textfile.h"

#ifdef __GNUC__
#include <dirent.h>
#include<sys/stat.h>
#endif

GEN_RESULT FileInterface::Init()
{
	return RESULT_OK;
}

GEN_RESULT FileInterface::Term()
{
	return RESULT_OK;
}

BaseFile* FileInterface::OpenFile(const char* szFilename, EFileType eType, EFileFlags eFlags)
{
	switch(eType)
	{
	case BINARY_FILE:
		{
			BinaryFile* pFile = new BinaryFile;
			if(pFile)
			{
				GEN_RESULT res = pFile->Open(szFilename, eFlags);
				if(RESULT_OK == res)
				{
					return (BaseFile*)pFile;
				}
			}
		}
		break;
	case TEXT_FILE:
		{
			//TODO:
		}
		break;
	default:
		break;
	}

	return NULL;
}

GEN_RESULT FileInterface::CloseFile(BaseFile* pFile)
{
	assert(pFile);

	if(pFile)
	{
		pFile->Close();
		delete pFile;

		return RESULT_OK;
	}

	return RESULT_ERROR;
}

bool FileInterface::StatFile(const char* szFilename, FileStats* pStats)
{

#ifdef _MSC_VER
	intptr_t hFile;
	struct _finddata_t file;
	if( (hFile = _findfirst( szFilename, &file )) != -1L )
	{
		pStats->nSize = (int)file.size;
		
		return true;
	}
#elif __GNUC__
	struct stat s;
	if (stat(szFilename, &s) == -1)
	{
		// Error
		return false;
	}
	else
	{
		pStats->nSize = (int)s.st_size;
		return true;
	}
#endif

	return false;
}