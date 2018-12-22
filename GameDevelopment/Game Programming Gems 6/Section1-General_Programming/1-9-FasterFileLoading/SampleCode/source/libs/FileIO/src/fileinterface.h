#ifndef _INTERFACE_FILEIO_H_
#define _INTERFACE_FILEIO_H_

#include "baseinterface.h"
#include "basefile.h"

enum EFileType
{
	BINARY_FILE = 0,
	TEXT_FILE
};

struct FileStats
{
	int nSize;
};

class FileInterface : public BaseInterface
{
public:
	FileInterface(){}
	~FileInterface(){}

	GEN_RESULT Init();
	GEN_RESULT Term();

	BaseFile* OpenFile(const char* szFilename, EFileType eType, EFileFlags eFlags);
	GEN_RESULT CloseFile(BaseFile* pFile);
	bool	StatFile(const char* szFilename, FileStats* pStats);
};

#endif
