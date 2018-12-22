// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "XDSinternal.h"
#include <memory.h>
#include <ctype.h>

CxdsBuffer::CxdsBuffer(const unsigned char *buf)
{
	m_buf = (unsigned char *)buf;
	m_iBufSize = (unsigned long)-1;
	m_cursor = m_buf;
	m_iUnreadBytes = (unsigned long)-1;
}

CxdsBuffer::CxdsBuffer(unsigned long nBytes)
{
	m_buf = (unsigned char *)XDS_ALLOC(NULL, nBytes, XDS_MEMTYPE_BUFFER);
	m_iBufSize = nBytes;

	Erase();
}

CxdsBuffer::~CxdsBuffer()
{
	if(m_iBufSize != -1)	// avoid freeing buffers passed into special constructor
		XDS_FREE(m_buf);
}

void CxdsBuffer::Erase(void)
{
	m_cursor = m_buf;
	m_iUnreadBytes = 0;
}

void CxdsBuffer::Sync(void)
{
	// move existing data (if any) to beginning of buffer
	memcpy(m_buf, m_cursor, m_iUnreadBytes);
	m_cursor = m_buf;
}

bool CxdsBuffer::Flush(void)
{
	u32 iLen = m_cursor - m_buf;

	if(XDS_WRITE(m_hFile, m_buf, iLen) != iLen)
		return false;

	Erase();
//	m_cursor = m_buf;
	return true;
}

bool CxdsBuffer::ExpandToFit(unsigned long nTotalBytes)
{
	// resize buffer if necessary
	if(m_iBufSize < nTotalBytes)
	{
		unsigned long curOffset = m_cursor - m_buf;

		// double size until it fits
		while(m_iBufSize < nTotalBytes)
			m_iBufSize <<= 1;

		m_buf = (unsigned char *)XDS_ALLOC(m_buf, m_iBufSize, XDS_MEMTYPE_BUFFER);
		if(!m_buf)
		{
			XDS_ERROR("Alloc failed in CxdsBuffer::ExpandToFit");
			return false;
		}
		m_cursor = m_buf+curOffset;	// point cursor to beginning of valid data (resized)
	}

	return true;
}

// fill buffer to contain at least iNumChars bytes past cursor
bool CxdsBuffer::Fill(u32 iNumChars)
{
	// short-circuit unnecessary fills
	if(m_iUnreadBytes >= iNumChars)
		return true;

	// move existing data (if any) to beginning of buffer
	Sync();

	if(!ExpandToFit(iNumChars))
		return false;

	// read additional data into buffer (retries if not enough is read and end-of-file is not reached)
	int nchrs = 0;
	while(nchrs >= 0 && m_iUnreadBytes < iNumChars)
	{
		nchrs = XDS_READ(m_hFile, (void*)(m_cursor + m_iUnreadBytes), m_iBufSize - UnreadBytes());
		if(nchrs > 0)
		{
			AddUnreadBytes(nchrs);
		}
	}

	return (m_iUnreadBytes >= iNumChars);
}

void CxdsBuffer::SkipWhite(void)
{
	while(isspace(Lookahead(0)))
		(void)ReadByte();
}

int CxdsBuffer::Lookahead(int iDistance)
{
	if(Fill(iDistance+1))
	{
		return m_cursor[iDistance];
	}
	return -1;
}


// special-purpose look-ahead function 
bool CxdsBuffer::MatchString(const char *sz, int len)
{
	if(Fill(len))
	{
		if(!strncmp((const char*)m_cursor, sz, len))
			return true;
	}
	return false;
}
int CxdsBuffer::LookaheadFind(const char *sz, int len)
{
	int iIndex = 0;
	for(;;)
	{
		if(!Fill(len+iIndex))
			return -1;
		if(!strncmp((const char *)m_cursor+iIndex, sz, len))
			return iIndex;
		++iIndex;
	}
}



/*
bool CxdsBuffer::Seek(long lOffset, int iWhence)
{
	// calculate number of bytes of data in buffer
	unsigned long iBytesBefore = (m_cursor - m_buf);

	if(iWhence == 0)
	{
		// move from current
		if((lOffset > 0) && ((unsigned long)lOffset > m_iUnreadBytes))
			return false;	// move past end
		if((lOffset < 0) && ((unsigned long)(-lOffset) > iBytesBefore))
			return false;	// move past beginning

		m_cursor += lOffset;
		m_iUnreadBytes -= lOffset;
		return true;
	}

	unsigned long iUsedBuf = iBytesBefore + m_iUnreadBytes;

	if(iWhence > 0)
	{
		// move from end of buffer
		if(lOffset > 0)
			return false;	// move past end
		if((unsigned long)(-lOffset) > iUsedBuf)
			return false;	// move past beginning

		m_cursor = m_buf + iUsedBuf + lOffset;
		m_iUnreadBytes = iUsedBuf - (m_cursor - m_buf);
	}
	else if(iWhence < 0)
	{
		// move from start of buffer
		if((unsigned long)lOffset > iUsedBuf)
			return false;	// move past end
		if(lOffset < 0)
			return false;	// move past beginning

		m_cursor = m_buf + lOffset;
		m_iUnreadBytes = iUsedBuf - lOffset;
	}
	return true;
}
*/