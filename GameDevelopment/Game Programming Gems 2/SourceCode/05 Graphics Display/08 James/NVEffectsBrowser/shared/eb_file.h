/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

// eb_file.h -- Common file loading functions
//
//////////////////////////////////////////////////////////////////////

#ifndef __EBFILE_H
#define __EBFILE_H

#include <windows.h>
#include "nvdebug.h"

namespace effect_api
{
	// These functions and static object remember the path to base of the current
	// effect.  Each effect has its own copy of this static string object.  The
	// object is constructed by a macro defined in eb_effect.h.
	extern std::string strStartPath;
	inline std::string GetModulePath() { return strStartPath; }
	inline void        SetModulePath(const std::string &strPath)
	{
		std::string::size_type Pos = strPath.find_last_of("\\", strPath.size());
		if (Pos != strPath.npos)
			strStartPath = strPath.substr(0, Pos);
		else
			strStartPath = ".";
	}
	
	// This function tries really hard to find the file you have asked for.
	// It looks in the current .dll's directory, it's media directory, 
	// the source process directory (nveffectsbrowser), and it's media directory.
		
	inline std::string GetFilePath(const std::string& strFileName)
	{
		HANDLE hFile;
		WIN32_FIND_DATA FindData;
		std::string strSearch;
		std::string strStartPath = GetModulePath();
		
		// Must call SetModulePath before using this function
		assert(!strStartPath.empty());
		
		// First check the root directory of the module
		strSearch = strStartPath + "\\" + strFileName;
		hFile = FindFirstFile(strSearch.c_str(), &FindData);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			return strSearch;
		}
		
		// Now check the media directory
		strSearch = strStartPath + "\\media\\" + strFileName;
		hFile = FindFirstFile(strSearch.c_str(), &FindData);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			return strSearch;
		}
		
		// Look in the process that loaded us
		HMODULE hProcess = GetModuleHandle(NULL);
		std::string strProcessPath;
		strProcessPath.resize(MAX_PATH);
		DWORD dwReturnSize = GetModuleFileName((HINSTANCE)hProcess, &strProcessPath[0], MAX_PATH);
		NVASSERT(dwReturnSize <= MAX_PATH, "Path too long"); // Should never happen
		std::string::size_type Pos = strProcessPath.find_last_of("\\", strProcessPath.size());
		if (Pos != strProcessPath.npos)
		{
			// Check the root directory of the running process
			strSearch = strProcessPath.substr(0, Pos) + "\\" + strFileName;
			hFile = FindFirstFile(strSearch.c_str(), &FindData);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				FindClose(hFile);
				return strSearch;
			}
			
			// Check the media directory of the running process
			strSearch = strProcessPath.substr(0, Pos) + "\\media\\" + strFileName;
			hFile = FindFirstFile(strSearch.c_str(), &FindData);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				FindClose(hFile);
				return strSearch;
			}
		}
		
		// Can't find it in the path, return it so that other API's can report the find fail.
		NVASSERT(0, "Couldn't find: " << strFileName);
		return strFileName;
	};

}; // namespace effect_api

#endif