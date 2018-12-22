/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
	Simple debug support with stream input for complex arguments.
	Also writes to a debug log style file.

	Declare this somewhere in your .cpp file:

	NVDebug dbg(DebugLevel, "outfile.txt")
	
	// Output Like this 
	DISPDBG(0, "This" << " is " << "a useful debug class");

  cmaughan@nvidia.com
      
******************************************************************************/
#ifndef __NVDEBUG_H
#define __NVDEBUG_H

#include <windows.h>
#pragma warning(disable: 4786)
#include <tchar.h>
#pragma warning(disable: 4786)
#include <iostream>
#pragma warning(disable: 4786)
#include <sstream>
#pragma warning(disable: 4786)
#include <iomanip>
#pragma warning(disable: 4786)
#include <strstream>
#pragma warning(disable: 4786)
#include <fstream>
#pragma warning(disable: 4786)
#include <assert.h>
#include "singleton.h"
#include <crtdbg.h>

static const DWORD MaxDebugFileSize = 100000;

class NVDebug : public Singleton<NVDebug>
{
public:

	NVDebug(unsigned int GlobalDebugLevel, const char* pszFileName)
		: m_GlobalDebugLevel(GlobalDebugLevel),
		m_dbgLog(pszFileName, std::ios::out | std::ios::trunc)	// Open a log file for debug messages
	{
		OutputDebugString(TEXT("CDebug::CDebug\n"));
	}
	
	virtual ~NVDebug()
	{
		m_dbgLog.flush();
		m_dbgLog.close();

		m_strStream.flush();
		m_strStream.clear();
	}

	// Flush the current output
	void NVDebug::EndOutput()
	{ 
		m_strStream << std::endl << std::ends;

		// Don't make a huge debug file.
		if (m_dbgLog.tellp() < MaxDebugFileSize)
		{
			m_dbgLog << m_strStream.str();
			FlushLog();
		}

		OutputDebugString(m_strStream.str());

		m_strStream.freeze(false);
		m_strStream.seekp(0);
	}

	unsigned int GetLevel() const { return m_GlobalDebugLevel; };
	std::ostrstream& GetStream() { return m_strStream; }
	void FlushLog() { m_dbgLog.flush(); }
	
private:
	std::ostrstream m_strStream;
	std::ofstream m_dbgLog;
	unsigned int m_GlobalDebugLevel;
};

#ifdef _DEBUG
#define DISPDBG(a, b)											\
do																\
{																\
	if (NVDebug::GetSingletonPtr() != NULL)						\
	if (a <= NVDebug::GetSingleton().GetLevel()) {	\
		NVDebug::GetSingleton().GetStream() << b;			\
		NVDebug::GetSingleton().EndOutput(); }				\
} while(0)

#define NVASSERT(a, b)														\
do																			\
{																			\
	static bool bIgnore = false;											\
	if (!bIgnore && ((int)(a) == 0))										\
	{																		\
		std::ostringstream strOut;											\
		strOut << b << "\nAt: " << __FILE__ << ", " << __LINE__;			\
		int Ret = MessageBoxEx(NULL, strOut.str().c_str(), "NVASSERT", MB_ABORTRETRYIGNORE, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ));	\
		if (Ret == IDIGNORE)			\
		{								\
			bIgnore = true;				\
		}								\
		else if (Ret == IDABORT)		\
		{								\
			_asm { int 3 }				\
		}								\
	}									\
} while (0)

#else
#define DISPDBG(a, b)
#define NVASSERT(a, b)
#endif

// Note that the cast ensures that the stream
// doesn't try to interpret pointers to objects in different ways.
// All we want is the object's address in memory.
#define BASE_DBG_PTR(a)					\
"0x" << std::hex << (DWORD)(a) << std::dec
	
#endif // __NVDEBUG_H