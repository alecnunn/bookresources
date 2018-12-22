#ifndef _BASE_FILE_H_
#define _BASE_FILE_H_

#include "resultcodes.h"

enum EFileFlags
{
	READONLY = 0,
	WRITEONLY,
	APPEND,
	READWRITE,
	READWRITE_OVERWRITE
};

class BaseFile
{
public:
	BaseFile(){}
	virtual ~BaseFile(){}

	virtual GEN_RESULT Open(const char* szFilename, EFileFlags eFlags) = 0;
	virtual void Close() = 0;
	virtual GEN_RESULT Read(void* pBuffer, int nBytes) = 0;
	virtual GEN_RESULT Write(void* pBuffer, int nBytes) = 0;
	virtual GEN_RESULT SeekTo(long nOffset) = 0;

protected:

};

#endif
