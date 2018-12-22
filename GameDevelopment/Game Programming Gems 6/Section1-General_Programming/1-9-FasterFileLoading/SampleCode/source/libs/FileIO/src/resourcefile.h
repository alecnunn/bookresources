#ifndef _RESOURCE_FILE_
#define _RESOURCE_FILE_

#include "core_defs.h"
#include "basefile.h"
#include <vector>
#include <string>
#include <map>

#define ResourceFileFilenameMaxLen 256
typedef std::vector<std::string> ResourceFileStringTable;

class FileInterface;

struct ResourceFileHeader
{
	uint32 nVersion;
	uint32 nNumFiles;
};

struct ResourceFileEntry
{
	uint16	nStringTableIndex;
	uint32	nFlags;
	uint32	nFileSize;
	uint32	nCompressedFileSize;
	uint32	nFilepointer;
};

struct ResourceFileStats
{
	uint32	nFlags;
	uint32	nFileSize;
};

typedef std::vector<ResourceFileEntry> FileEntryTable;
typedef std::map<std::string, ResourceFileEntry> FileTable;

class ResourceFile
{
public:
	ResourceFile();
	~ResourceFile();

	GEN_RESULT	Open(const char* szFilename);
	GEN_RESULT	Close();
	GEN_RESULT	ClosePackedFile(BaseFile* pFile);
	BaseFile*	Find(const char* szFilename, ResourceFileStats *pEntry);
	uint32		GetNumFiles();
	BaseFile*	GetFileFromIndex(uint32 nIndex, ResourceFileStats *pEntry);
	BaseFile*	GetFileHandle(){ return m_pFileHandle; }

protected:
	BaseFile		*m_pFileHandle;
	FileInterface	*m_pFileSystem;

	//Current file system stats
	ResourceFileHeader		m_FileHeader;
	ResourceFileStringTable m_FileStringTable;
	FileEntryTable			m_FileEntryHeaderTable;
	FileTable				m_FileTable;
};

#endif
