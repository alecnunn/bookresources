// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#ifndef XDSBUFFER_INCL
#define XDSBUFFER_INCL

#include <memory.h>

#define XDSBUFFER_BUFSIZE 256		// default size of buffer

// expandable buffer class
class CxdsBuffer {
private:
	unsigned char *m_buf;			// the buffer
	unsigned char *m_cursor;		// cursor position in buffer
	unsigned long  m_iUnreadBytes;	// number of used characters after m_cursor
	unsigned long  m_iBufSize;		// allocated size of buffer
	void *m_hFile;

public:
	CxdsBuffer::CxdsBuffer(const unsigned char *buf);
	CxdsBuffer::CxdsBuffer(unsigned long nBytes = XDSBUFFER_BUFSIZE);
	CxdsBuffer::~CxdsBuffer();

	void SetFile(void *hFile) { m_hFile = hFile; };
	void Erase(void);									// throws away all data

	bool ExpandToFit(unsigned long nTotalBytes);
	unsigned long Size(void) const { return (m_cursor - m_buf) + m_iUnreadBytes; };
	unsigned long UnreadBytes(void) const { return m_iUnreadBytes; };
	unsigned char *Data(void) { return m_buf; };

	// Utility routines used when using as an input buffer
	void AddUnreadBytes(unsigned long nBytes) { m_iUnreadBytes += nBytes; };
	void Sync(void);									// throws away data before cursor
	bool Flush(void);
	bool Fill(u32 iNumChars);

	// I/O like functions (affect cursor)
	const unsigned char *Cursor(void) const { return m_cursor; };
	int Write(const void *data, unsigned long nBytes);
//	bool Seek(long lOffset, int iWhence);		// <0 from beginning, 0 from current, >0 from end

	int   ReadByte(void);
	int   ReadShort(void);
	long  ReadLong(void);
	long  ReadValue(u8 iValueSize);
	char *ReadTag(void);
	bool  Read(unsigned char *pBuf, unsigned int iLength);

	bool  MatchString(const char *sz, int iLength);
	int   LookaheadShort(void);
	int   Lookahead(int iDistance);
	int   LookaheadFind(const char *sz, int iLength);

	bool  Skip(unsigned int iNumChars);
	void  SkipWhite();
	bool  SkipTag(void);

	int WriteByte(u8 b);
	int WriteShort(u16 w);
	int WriteLong(u32 dw);
	int WriteValue(u32 dw, u8 iValueSize);
	int WriteValueAt(u32 lOffset, u32 dw, u8 iValueSize);
	int WriteTag(const char *sz, unsigned int length);
};

inline int CxdsBuffer::ReadByte(void)
{
	if(Fill(sizeof(u8)))
	{
		u8 byte = *m_cursor++;
		--m_iUnreadBytes;
		return byte;
	}
	else
		return -1;
}
inline int CxdsBuffer::ReadShort(void)
{
	if(Fill(sizeof(u16)))
	{
		u16 word = *(const u16 *)m_cursor;
		m_cursor += sizeof(u16);
		m_iUnreadBytes -= sizeof(u16);
		return word;
	}
	else
		return -1;
}
inline int CxdsBuffer::LookaheadShort()
{
	if(Fill(sizeof(u16)))
	{
		u16 word = *(const u16 *)m_cursor;
		return word;
	}
	else
		return -1;
}

inline long CxdsBuffer::ReadLong(void)
{
	if(Fill(sizeof(u32)))
	{
		u32 dword = *(const u32 *)m_cursor;
		m_cursor += sizeof(u32);
		m_iUnreadBytes -= sizeof(u32);
		return dword;
	}
	else
		return -1;
}
inline long CxdsBuffer::ReadValue(u8 iValueSize)
{
	if(iValueSize == sizeof(u16))
		return ReadShort();
	else if(iValueSize == sizeof(u32))
		return ReadLong();
	else if(iValueSize == sizeof(u8))
		return ReadByte();
	else
		return -1;
}

inline bool CxdsBuffer::Read(unsigned char *pBuf, unsigned int iLength)
{
	if(Fill(iLength))
	{
		memcpy(pBuf, m_cursor, iLength);
		m_cursor += iLength;
		m_iUnreadBytes -= iLength;
		return true;
	}
	else
		return false;
}

extern char *xdsStringNDup(const char *sz, int iCount);
inline char *CxdsBuffer::ReadTag(void)
{
	long len = ReadValue(XDS_CONFIG_TAGSIZE);
	if(len < 0)
		return NULL;

	if(Fill(len))
	{
		char *sz = xdsStringNDup((const char *)m_cursor, len);
		m_cursor += len;
		m_iUnreadBytes -= len;
		return sz;
	}
	else
		return NULL;
}
inline bool CxdsBuffer::SkipTag(void)
{
	int len = ReadByte();
	if(len < 0)
		return false;

	if(Fill(len))
	{
		m_cursor += len;
		m_iUnreadBytes -= len;

		return true;
	}
	else
		return false;
}

inline bool CxdsBuffer::Skip(unsigned int iNumChars)
{
	if(Fill(iNumChars))
	{
		m_cursor += iNumChars;
		m_iUnreadBytes -= iNumChars;
		return true;
	}
	else
		return false;
}

inline int CxdsBuffer::WriteByte(u8 b)
{
	if(ExpandToFit((m_cursor-m_buf) + 1))
	{
		*m_cursor++ = b;
		return 1;
	}
	else
		return 0;
}

inline int CxdsBuffer::WriteShort(u16 w)
{
	if(ExpandToFit((m_cursor-m_buf) + sizeof(u16)))
	{
		*(u16 *)m_cursor = w;
		m_cursor += sizeof(u16);
		return sizeof(u16);
	}
	else
		return 0;
}
inline int CxdsBuffer::WriteLong(u32 dw)
{
	if(ExpandToFit((m_cursor-m_buf) + sizeof(u32)))
	{
		*(u32 *)m_cursor = dw;
		m_cursor += sizeof(u32);
		return sizeof(u32);
	}
	else
		return 0;
}

inline int CxdsBuffer::WriteValue(u32 dw, u8 iValueSize)
{
	if(iValueSize == sizeof(u16))
		return WriteShort((u16)dw);
	else if(iValueSize == 1)
		return WriteByte((u8)dw);
	else if(iValueSize == sizeof(u32))
		return WriteLong(dw);
	else
		return 0;
}

inline int CxdsBuffer::WriteTag(const char *sz, unsigned int length)
{
	if(!WriteValue(length, XDS_CONFIG_TAGSIZE))
		return 0;
	return Write(sz, length);
}


inline int CxdsBuffer::WriteValueAt(u32 lOffset, u32 dw, u8 iValueSize)
{
	if(iValueSize == 2)
	{
		*(u16 *)(m_buf + lOffset) = (u16)dw;
		return sizeof(u16);
	}
	else if(iValueSize == 4)
	{
		*(u32 *)(m_buf + lOffset) = dw;
		return sizeof(u32);
	}
	else if(iValueSize == 1)
	{
		*(u8 *)(m_buf + lOffset) = (u8)dw;
		return 1;
	}
	else
		return 0;
}

inline int CxdsBuffer::Write(const void *data, unsigned long nBytes)
{
	if(ExpandToFit((m_cursor-m_buf) + nBytes))
	{
		memcpy(m_cursor, data, nBytes);
		m_cursor += nBytes;
		return nBytes;
	}
	else
		return 0;
}

#endif // XDSBUFFER_INCL