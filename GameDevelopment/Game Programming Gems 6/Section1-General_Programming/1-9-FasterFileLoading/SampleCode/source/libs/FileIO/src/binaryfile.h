#ifndef _BINARY_FILE_H_
#define _BINARY_FILE_H_

#include "basefile.h"
#include <stdio.h>

class BinaryFile : public BaseFile
{
public:
	BinaryFile();
	virtual ~BinaryFile();

	virtual GEN_RESULT Open(const char* szFilename, EFileFlags eFlags);
	virtual void Close();
	virtual GEN_RESULT Read(void* pBuffer, int nBytes);
	virtual GEN_RESULT Write(void* pBuffer, int nBytes);
	virtual GEN_RESULT SeekTo(long nOffset);

protected:

	FILE* m_pFileHandle;

};

#endif
