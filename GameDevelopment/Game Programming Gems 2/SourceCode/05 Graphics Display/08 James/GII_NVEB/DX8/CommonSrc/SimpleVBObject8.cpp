/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\CommonSrc
File:  SimpleVBObject8.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include <assert.h>
#include "SimpleObject8.h"
#include "SimpleVBObject8.h"

#include "NV_Error.h"


#include <math.h>
#include <d3d8.h>
#include <d3dx8.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define SAFE_RELEASE(p) { if(p != NULL) { (p)->Release(); (p)=NULL; } }

#ifndef ASSERT_IF_FAILED
	#define ASSERT_IF_FAILED( hres )	\
	{									\
		if( FAILED(hres) )				\
		   assert( false );				\
	}
#endif


//////////////////////////////////////
// Construction/destruction

SimpleVBObject8::SimpleVBObject8()
{

	m_pVertexBuffer = NULL;
	m_wNumVerts = 0;

	m_pIndexBuffer = NULL;
	m_wNumInd = 0;

	m_PrimType = D3DPT_TRIANGLESTRIP;
	m_bIsValid = false;

	m_pD3DDev = NULL;
}


SimpleVBObject8::~SimpleVBObject8()
{
	Free();
}


//////////////////////////////////////

HRESULT SimpleVBObject8::Free()
{

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer );

	if( m_pD3DDev != NULL )
	{
		SAFE_RELEASE( m_pD3DDev );
	}

	m_wNumVerts = 0;
	m_wNumInd = 0;

	m_PrimType = D3DPT_TRIANGLESTRIP;
	m_bIsValid = false;


	return( S_OK );
}


HRESULT SimpleVBObject8::CreateFromSimpleObject( SimpleObject8 * pObj, LPDIRECT3DDEVICE8  pD3DDev )
{

	Free();			// de-allocate if we're already created.


	assert( pD3DDev != NULL );
	// get the device
	m_pD3DDev = pD3DDev;
	pD3DDev->AddRef();			// released on Free()


	m_PrimType = pObj->m_PrimType;


	HRESULT hr;

	int nvert = pObj->GetNumVerts();
	int nind  = pObj->GetNumIndices();


	hr = m_pD3DDev->CreateVertexBuffer( nvert * sizeof( SimpleObjVertex ),
										D3DUSAGE_WRITEONLY,
										SOBJVERT_FVF,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer );
	if( FAILED(hr))
	{
		assert(false);
		return( hr );
	}

	hr = m_pD3DDev->CreateIndexBuffer( nind * sizeof( WORD ),
										D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,
										D3DPOOL_DEFAULT,
										&m_pIndexBuffer );
	if( FAILED(hr))
	{
		assert(false);
		return( hr );
	}


	SimpleObjVertex	* pVertices;
	WORD			* pIndices;
	

	hr = m_pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
	{
		assert(false);
		return( hr );
	}


	hr = m_pIndexBuffer->Lock(0, 0, (BYTE**)&pIndices, 0);
	if (FAILED(hr))
	{
		assert(false);
		return( hr );
	}

	int i;

	for( i=0; i < nvert; i++ )
	{
		pVertices[i].pos = pObj->m_pVertices[i].pos;
		pVertices[i].nrm = pObj->m_pVertices[i].nrm;
		pVertices[i].diffuse = pObj->m_pVertices[i].diffuse;
		pVertices[i].t0 = pObj->m_pVertices[i].t0;
	}

	for( i=0; i < nind; i++ )
	{
		pIndices[i] = pObj->m_pIndices[i];
	}


	m_pVertexBuffer->Unlock();
	pVertices = 0;
	m_pIndexBuffer->Unlock();
	pIndices = 0;

	m_wNumVerts = nvert;
	m_wNumInd   = nind;
	m_bIsValid = true;

	return( S_OK );
}



HRESULT SimpleVBObject8::Draw()
{
	assert( m_pD3DDev != NULL );
	assert( m_bIsValid == true );

	HRESULT hr = S_OK;


	switch( m_PrimType )
	{
	case D3DPT_TRIANGLELIST:

		hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof( SimpleObjVertex ) );
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->SetIndices( m_pIndexBuffer, 0 );
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->DrawIndexedPrimitive( m_PrimType, 0, m_wNumVerts, 0, m_wNumInd / 3 );
		ASSERT_IF_FAILED(hr);

		break;

	default:
		assert( false );
		hr = E_FAIL;

	}

	return( hr );
}