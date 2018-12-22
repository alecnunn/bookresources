/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_Fire
File:  ShaderManager.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

ShaderManager - a class for loading various pixel & vertex shaders, & not having
	to load shaders twice for two classes that use the same shader


******************************************************************************/


#ifndef __NV_SHADER_MANAGER_H
#define	__NV_SHADER_MANAGER_H


#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <vector>

#include "eb_string.h"



typedef DWORD		SHADER_INDEX;



typedef enum tagSM_SHADERTYPE
{
	SM_SHADERTYPE_VERTEX = 0,
	SM_SHADERTYPE_PIXEL = 1,
	SM_SHADERTYPE_FORCEDWORD = 0xFFFFFFFF
} SM_SHADERTYPE;


struct ShaderDescription
{

	EBString	m_ShaderDefinitionFile;	// text file name (.nvv, .nvp file)
	EBString	m_ShaderObjectFile;		// compiled shader object file (.vso, .pso)
										//  as created by nvasm

	DWORD		m_dwShaderHandle;		// DWORD handle returned by DX8 runtime
	SM_SHADERTYPE	m_Type;
};


class ShaderManager
{
private:


	std::vector < ShaderDescription >	m_Shaders;			// m_Shaders[0] is always = 0
	std::vector < SHADER_INDEX >		m_ShaderIndices;	// indirection to m_Shaders
											// Programs hold an index to this array for
											// their shaders.  The array int at that index
											// holds the index into the m_Shaders array.
	LPDIRECT3DDEVICE8 m_pD3DDev;

public:


	ShaderManager();
	~ShaderManager();

	HRESULT Free();
	HRESULT Initialize( LPDIRECT3DDEVICE8 pDev );


	HRESULT	LoadAndCreateShader( const std::string & strFilePath, const std::string & strSourceFile,
								const DWORD * pDeclaration,	DWORD Usage,
								SM_SHADERTYPE ShaderType, SHADER_INDEX * outIndex );

	DWORD	GetShaderHandle( SHADER_INDEX Index );

};






#endif			// __NV_SHADER_MANAGER_H