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
#include "eb_string.h"
#include "nvfilesearch.h"
#include "d3d8.h"

typedef enum tagEBSHADERTYPE
{
	// D3D shader types
	SHADERTYPE_VERTEXSHADER  = 0,
	SHADERTYPE_PIXELSHADER   = 1,

	// OpenGL shader types
	SHADERTYPE_VERTEXPROG    = 2,
	SHADERTYPE_TEXTURESHADER = 3,
	SHADERTYPE_REGCOMBSTATE  = 4,

	// D3D shader types (old names)
	SHADERTYPE_VERTEX        = 0,
	SHADERTYPE_PIXEL         = 1,

	SHADERTYPE_FORCEDWORD    = 0xFFFFFFFF
} EBSHADERTYPE;

namespace effect_api
{
	//////////////////////////////////////////////////////////////////////////////
	// These variables provide information to effects at creation time

	// Set when the browser is rescanning for new effects...
	extern bool Rescanning;

	// This variable provides a way for effects to save a piece of state at scan
	// time that will again be available when the effect is later created.
	//
	// If the browser is rescanning, it will save the value of this variable in
	// the registry for each effect.  The variable should be set as a side-effect
	// by the creation of each effect.
	//
	// If the browser is NOT rescanning, it restore this variable to the value it
	// contained when this effect was created during the original scan.
	//
	extern EBString EffectState;

	// This variable allows us to recursively search for media
	class NVMediaSearch : public NVFileSearch
	{
		public:
		std::string m_strPath;
		virtual bool FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDir)
		{
			m_strPath = strDir;
			
			return false;
		}
	};
	//////////////////////////////////////////////////////////////////////////////
	// These functions and static object remember the path to base of the current
	// effect.  Each effect has its own copy of this static string object.  The
	// object is constructed by a macro defined in eb_effect.h.
	extern std::string strLastError;
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
	
	//////////////////////////////////////////////////////////////////////////////
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
		 
		// Last resort - check the filename in case it is a complete path
		hFile = FindFirstFile(strFileName.c_str(), &FindData);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			return strFileName;
		}

		//recursively search media dir
		std::string mediaDir = strStartPath;
		int numDirsBack = 3;
		for(int i = 0; i < numDirsBack; ++i)
		{
			std::string::size_type backslashPos = mediaDir.find_last_of("\\", mediaDir.size());
			mediaDir = mediaDir.substr(0, backslashPos);
		}

		mediaDir += "\\Common\\media";

		NVMediaSearch mediaSearch;
		mediaSearch.m_strPath = "";
		mediaSearch.FindFile(strFileName.c_str(), mediaDir, true);
		if(mediaSearch.m_strPath != "")
			return (mediaSearch.m_strPath + "\\" + strFileName);

		// Can't find it in the path, return it so that other API's can report the find fail.
		NVASSERT(0, "Couldn't find: " << strFileName);
		return strFileName;
	};

	//////////////////////////////////////////////////////////////////////////////
	// Loads a .vso file and creates a D3D vertex or pixel shader for it
	inline HRESULT LoadAndCreateShader(	LPDIRECT3DDEVICE8 m_pD3DDev, const std::string& strFilePath, 
							const DWORD* pDeclaration, DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle)
	{
		assert(m_pD3DDev);
		try	{
			HANDLE hFile;
			HRESULT hr;
			
			hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE) {
				strLastError = "Could not find file " + strFilePath;
				return E_FAIL;
			}
			
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			
			const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
			if (!pShader) {
				strLastError = "Failed to allocate memory to load shader " + strFilePath;
				return E_FAIL;
			}
			
			ReadFile(hFile, (void*)pShader, dwFileSize, &dwFileSize, NULL);
			
			CloseHandle(hFile);
			
			if (ShaderType == SHADERTYPE_VERTEX) {
				hr = m_pD3DDev->CreateVertexShader(pDeclaration, pShader, pHandle, Usage);
			} else if (ShaderType == SHADERTYPE_PIXEL) {
				hr = m_pD3DDev->CreatePixelShader(pShader, pHandle);
			}
			
			HeapFree(GetProcessHeap(), 0, (void*)pShader);
			
			if (FAILED(hr))	{
				strLastError = "Failed to create shader " + strFilePath + '\n'; 
				return E_FAIL;
			}
		} catch(...) {
			strLastError = "Error opening file " + strFilePath;
			return E_FAIL;
		}
		
		return S_OK;
	};

}; // namespace effect_api

#endif