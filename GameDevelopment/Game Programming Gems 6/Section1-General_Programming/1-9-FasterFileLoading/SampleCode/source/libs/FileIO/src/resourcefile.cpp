#include "stdafx.h"
#include "resourcefile.h"
#include "packedfile.h"
#include "fileinterface.h"
#include <stdio.h>
#include <math.h>

ResourceFile::ResourceFile():
m_pFileHandle(NULL),
m_pFileSystem(NULL)
{
}
ResourceFile::~ResourceFile()
{
	Close();
}

GEN_RESULT ResourceFile::Open(const char* szFilename)
{
	m_pFileSystem = new FileInterface;
	if(NULL == m_pFileSystem)
	{
		return RESULT_ERROR;
	}

	char szBuf[1024];

	// Open the file system
	m_pFileHandle = m_pFileSystem->OpenFile(szFilename, BINARY_FILE, READONLY);
	
	if(m_pFileHandle)
	{
		// Read in the file header
		m_pFileHandle->Read(&m_FileHeader, sizeof(ResourceFileHeader));


		// Read in the string table.
		int nFileCounter = 0;
		for(uint32 i = 0; i < m_FileHeader.nNumFiles; ++i)
		{
			memset(szBuf, 0, 1024);				
			uint16 nStringLen = 0;
			m_pFileHandle->Read(&nStringLen, sizeof(uint16));
			m_pFileHandle->Read(szBuf, nStringLen);
			m_FileStringTable.push_back(szBuf);
			++nFileCounter;
		}

		// Read in the file entries
		m_FileEntryHeaderTable.reserve(m_FileHeader.nNumFiles);
		m_pFileHandle->Read(&m_FileEntryHeaderTable[0], (sizeof(ResourceFileEntry) *m_FileHeader.nNumFiles));
		
		// Fill the map
		for(uint32 i = 0; i < m_FileHeader.nNumFiles; ++i)
		{
			m_FileTable[ m_FileStringTable[ m_FileEntryHeaderTable[i].nStringTableIndex ]] = m_FileEntryHeaderTable[i];
		}
	}
	else
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

GEN_RESULT ResourceFile::Close()
{
	if(m_pFileHandle)
	{	
		// Close the resource file.
		m_pFileSystem->CloseFile(m_pFileHandle);
		m_pFileHandle = NULL;
	}

	if(m_pFileSystem)
	{
		// Delete the file system.
		delete m_pFileSystem;
		m_pFileSystem = NULL;
	}

	return RESULT_OK;
}

GEN_RESULT	ResourceFile::ClosePackedFile(BaseFile* pFile)
{
	if(pFile)
	{
		delete pFile;
	}

	return RESULT_OK;
}

BaseFile* ResourceFile::Find(const char* szFilename, ResourceFileStats *pEntry)
{
	//get entry			
	FileTable::iterator findFile = m_FileTable.find(szFilename);
	if(findFile != m_FileTable.end())
	{
		//Ok, we're good, now setup pointers		
		m_pFileHandle->SeekTo(findFile->second.nFilepointer);

		PackedFile *pFile = new PackedFile();
		if(pFile)
		{
			pFile->SetResourceFile(this);
			pFile->SetFileSize(findFile->second.nFileSize);

			pEntry->nFileSize = findFile->second.nFileSize;
			pEntry->nFlags = findFile->second.nFlags;

			return (BaseFile*)pFile;
		}
	}		

	return NULL;
}

BaseFile* ResourceFile::GetFileFromIndex(uint32 nIndex, ResourceFileStats *pEntry)
{
	if(nIndex >= m_FileTable.size())
	{
		return NULL;
	}

	//get entry			
	FileTable::iterator findFile = m_FileTable.find(m_FileStringTable[ nIndex ]);
	if(findFile != m_FileTable.end())
	{
		//Ok, we're good, now setup pointers		
		m_pFileHandle->SeekTo(findFile->second.nFilepointer);

		PackedFile *pFile = new PackedFile();
		if(pFile)
		{
			pFile->SetResourceFile(this);
			pFile->SetFileSize(findFile->second.nFileSize);

			pEntry->nFileSize = findFile->second.nFileSize;
			pEntry->nFlags = findFile->second.nFlags;

			return (BaseFile*)pFile;
		}
	}		

	return NULL;
}

uint32 ResourceFile::GetNumFiles()
{
	(uint32)m_FileTable.size();

	return 0;
}
