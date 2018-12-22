#ifndef _PACKED_FILE_H_
#define _PACKED_FILE_H_

#include <core_defs.h>
#include "basefile.h"

class ResourceFile;

class PackedFile : public BaseFile
{
public:
	PackedFile();
	virtual ~PackedFile();

	virtual GEN_RESULT Open(const char* szFilename, EFileFlags eFlags){return RESULT_OK;}
	virtual void Close();
	virtual GEN_RESULT Read(void* pBuffer, int nBytes);
	virtual GEN_RESULT Write(void* pBuffer, int nBytes){ return RESULT_ERROR; } //NO!
	virtual GEN_RESULT SeekTo(long nOffset){ return RESULT_ERROR; } //NO!

	void SetResourceFile(ResourceFile* pResFile) { m_pResourceFile = pResFile; }
	void SetFileSize(uint32 nSize) { m_nFileSize = nSize; }

protected:

	uint32	m_nFileSize;
	uint32	m_nBytesRead;
	ResourceFile *m_pResourceFile;

};

#endif
