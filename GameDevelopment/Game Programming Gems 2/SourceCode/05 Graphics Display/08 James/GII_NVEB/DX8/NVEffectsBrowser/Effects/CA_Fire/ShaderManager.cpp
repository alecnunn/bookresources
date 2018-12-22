/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_Fire
File:  ShaderManager.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#include "ShaderManager.h"
#include "NV_Error.h"
#include "NV_Common.h"



////////////////////////////////////////////////////////


ShaderManager::ShaderManager()
{
	m_Shaders.clear();
	m_ShaderIndices.clear();

	std::vector < ShaderDescription >	m_Shaders;
	std::vector < int >					m_ShaderIndices;	// indirection to m_Shaders

	m_pD3DDev = NULL;
}

ShaderManager::~ShaderManager()
{
	Free();

}


////////////////////////////////////////////////////////


HRESULT ShaderManager::Free()
{
	int i;

	if( m_pD3DDev != NULL )
	{

		// Start from 1.  
		// Shader 0 is always null shader

		for( i=1; i < m_Shaders.size() ; i++ )
		{
			switch( m_Shaders[i].m_Type )
			{
			case	SM_SHADERTYPE_VERTEX:
				m_pD3DDev->DeleteVertexShader( m_Shaders[i].m_dwShaderHandle );
				m_Shaders[i].m_dwShaderHandle = 0;

				break;

			case	SM_SHADERTYPE_PIXEL:
				m_pD3DDev->DeletePixelShader( m_Shaders[i].m_dwShaderHandle );
				m_Shaders[i].m_dwShaderHandle = 0;

				break;
			}
		}


		SAFE_RELEASE(m_pD3DDev);		// we AddRef()'d in Initialize
	}

	m_Shaders.clear();
	m_ShaderIndices.clear();

	return( S_OK );
}


HRESULT ShaderManager::Initialize( LPDIRECT3DDEVICE8 pDev )
{
	assert( pDev != NULL );

	Free();

	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();

	// Add one "NULL" shader with handle value of 0
	//  so that GetShaderHandle(0) returns a null shader
	//  to disable the vertex or pixel shader.

	ShaderDescription Desc;
	Desc.m_ShaderDefinitionFile = "NULL Shader";
	Desc.m_ShaderObjectFile		= "NULL Shader";
	Desc.m_Type					= SM_SHADERTYPE_VERTEX;
	Desc.m_dwShaderHandle		= 0;

	m_Shaders.push_back( Desc );


	return( S_OK );
}



////////////////////////////////////////////////////////////////
//  Loads a .vso or .pso shader object file
//  Also tracks the original text file name.

HRESULT ShaderManager::LoadAndCreateShader(	const std::string & strFilePath, const std::string & strSourceFile,
											const DWORD * pDeclaration,	DWORD Usage,
											SM_SHADERTYPE ShaderType,
											SHADER_INDEX * outIndex )

{
	assert( m_pD3DDev );
	
	//@@@@@@@@@ ADD check for if shader already exists!!! =)



	try	{
		HANDLE hFile;
		HRESULT hr;
		DWORD handle;

		ShaderDescription	Desc;

		Desc.m_ShaderDefinitionFile = strSourceFile.c_str();
		Desc.m_ShaderObjectFile		= strFilePath.c_str();
		Desc.m_Type					= ShaderType;


		hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE) 
		{
			FDebug("Could not find file %s\n", strFilePath.c_str() );
			return E_FAIL;
		}
		
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		
		const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
		if (!pShader)
		{
			FDebug( "Failed to allocate memory to load shader %s\n", strFilePath.c_str() );
			return E_FAIL;
		}
		
		ReadFile(hFile, (void*)pShader, dwFileSize, &dwFileSize, NULL);
		
		CloseHandle(hFile);
		
		if (ShaderType == SM_SHADERTYPE_VERTEX)
		{
			hr = m_pD3DDev->CreateVertexShader(pDeclaration, pShader, & handle, Usage);
		} else if (ShaderType == SM_SHADERTYPE_PIXEL)
		{
			hr = m_pD3DDev->CreatePixelShader(pShader, & handle );
		}
		
		HeapFree(GetProcessHeap(), 0, (void*)pShader);
		
		if (FAILED(hr))	
		{
			FDebug("Failed to create shader %s\n", strFilePath.c_str() );
			return E_FAIL;
		}

		// Add shader description to the array, set its index and 
		//  return the index
		
		Desc.m_dwShaderHandle		= handle;

		m_Shaders.push_back( Desc );
		* outIndex = m_Shaders.size() - 1;

	}
	catch(...)
	{
		FDebug("Error opening file %s\n", strFilePath.c_str() );
		return E_FAIL;
	}


	
	return S_OK;
}




DWORD	ShaderManager::GetShaderHandle( SHADER_INDEX Index )
{
	DWORD handle;

	if( Index < m_Shaders.size() )
	{

		handle = m_Shaders[ Index ].m_dwShaderHandle;

	}
	else
	{
		FDebug("ShaderManager Index out of bounds!\n");
		handle = 0;
	}


	return( handle );
}



