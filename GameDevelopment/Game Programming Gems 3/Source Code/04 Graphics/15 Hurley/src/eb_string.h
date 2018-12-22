/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
 ******************************************************************************/
#ifndef __EB_STRING_H
#define __EB_STRING_H

#include <string>
#include <assert.h>

template<class T>
class EBStr
{
public:
	EBStr(const EBStr& strNew)
		: m_dwLength(0),
			m_pString(NULL)
	{
		operator = (strNew);
	}

	EBStr(const std::string& strNew)
		: m_dwLength(0),
			m_pString(NULL)
	{
		operator = (strNew);
	}

	EBStr(const T* strNew)
		: m_dwLength(0),
			m_pString(NULL)
	{
		operator = (strNew);
	}

	EBStr()
		: m_dwLength(0),
			m_pString(NULL)
	{}

	virtual ~EBStr()
	{
		clear();
	}

	// Assignment
	virtual const EBStr& operator = (const T* rhs)
	{
		AllocString(strlen(rhs));
		CopyString(m_pString, rhs, StringLength(rhs));
		return *this;
	}
	virtual const EBStr& operator = (const EBStr& rhs)
	{
		AllocString(rhs.length());
		CopyString(m_pString, rhs.c_str(), rhs.length());
		return *this;
	}
	virtual const EBStr& operator = (const std::string& rhs)
	{
		AllocString(rhs.length());
		CopyString(m_pString, rhs.c_str(), rhs.length());
		return *this;
	}

	// Addition
	virtual const EBStr& operator + (const T* rhs)
	{
		DWORD dwOriginalLength = m_dwLength;
		ReAllocString(m_dwLength + StringLength(rhs));
		CopyString(m_pString + (sizeof(T) * dwOriginalLength), rhs, StringLength(rhs));
		return *this;
	}
	virtual const EBStr& operator + (const EBStr& rhs)
	{
		DWORD dwOriginalLength = m_dwLength;
		ReAllocString(m_dwLength + rhs.length());
		CopyString(m_pString + (sizeof(T) * dwOriginalLength), rhs.c_str(), rhs.length());
		return *this;
	}
	virtual const EBStr& operator + (const std::string& rhs)
	{
		AllocString(rhs.length());
		CopyString(m_pString, rhs.c_str(), rhs.length());
		return *this;
	}
	virtual const EBStr& operator += (const T* rhs) { return operator+ (rhs); }
	virtual const EBStr& operator += (const EBStr& rhs) { return operator+ (rhs); }
	virtual const EBStr& operator += (const std::string& rhs) { return operator+ (rhs); }

	// Conversion
	virtual operator const std::string() const { return m_pString; }
	virtual operator std::string() const { return m_pString; }
	virtual operator const T*() const { return m_pString; }

	// Comparison
	virtual bool operator == (const EBStr& rhs) const
	{
		if (rhs.length() != m_dwLength)
			return false;

		for (DWORD i = 0; i < m_dwLength; i++)
		{
			if (m_pString[i] != rhs.m_pString[i])
				return false;
		}

		return true;
	}

	// Less-than needed for stl lists/maps, etc. which require
	// it for sorting.
	virtual bool operator < (const EBStr& rhs) const
	{
		int ret = 0 ;
		const char* dst = rhs.m_pString;
		const char* src = m_pString;

		while( ! (ret = *(const unsigned char *)src - *(const unsigned char *)dst) && *dst)
			++src, ++dst;

		if ( ret < 0 )
			return true ;

		return false;
	}

	virtual const T* c_str() const 
	{
		return m_pString; 
	}
	
	virtual void clear()
	{
		if (m_pString)
		{
			HeapFree(GetProcessHeap(), 0, m_pString);
		}
		m_pString = NULL;
		m_dwLength = 0;
	}

	virtual bool empty() const
	{
		return (m_dwLength == 0);
	}

	virtual DWORD length() const
	{
		return m_dwLength;
	}

	virtual int compare(const EBStr& rhs) const
	{
		if (operator == (rhs))
			return 0;
		else if (operator < (rhs))
			return -1;
	
		return 1;
	}

private:
	virtual DWORD StringLength(const T* pString) const
	{
		DWORD dwLength = 0;
		while(*pString++ != 0) 
			dwLength++;

		return dwLength;
	}

	virtual void CopyString(void* pDest, const void* pSource, DWORD dwSize)
	{
		memcpy(pDest, pSource, dwSize * sizeof(T));
	}

	virtual bool AllocString(DWORD dwSize)
	{
		clear();

		m_pString = (T*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (dwSize + 1) * sizeof(T));
		if (m_pString)
		{
			m_dwLength = dwSize;
			return true;
		}
	
		return false;
	}

	virtual bool ReAllocString(DWORD dwSize)
	{
		assert(m_pString);
		assert(m_dwLength != 0);

		T* pStringNew = (T*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (dwSize + 1) * sizeof(T));
		if (!pStringNew)
			return false;

		CopyString(pStringNew, m_pString, m_dwLength);
		clear();

		m_pString = pStringNew;
		m_dwLength = dwSize;
		
		return true;
	}

	T* m_pString;
	DWORD m_dwLength;

};

typedef EBStr<char> EBString;


#endif // EB_STRING_H	