#ifndef _BASE_FILE_H_
#define _BASE_FILE_H_

#include "basefile.h"

class TextFile
{
public:
	virtual BaseFile(){}
	virtual ~BaseFile(){}

	virtual BaseFile* Open() = 0;
	virtual void Close() = 0;
	virtual GEN_RESULT Read(void* pBuffer, int nBytes) = 0;
	virtual GEN_RESULT Write(void* pBuffer, int nBytes) = 0;

protected:

};

#endif
