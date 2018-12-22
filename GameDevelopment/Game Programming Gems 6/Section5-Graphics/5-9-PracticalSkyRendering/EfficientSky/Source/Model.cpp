// Entity.h: A Scene Entity Class.
// Date: 08/02/05
// Created by: Aurelio Reis

#include "dxstdafx.h"

#include "../Include/Model.h"


// Constructor.
CD3DModel::CD3DModel()
{
	Reset();
}

// Destructor.
CD3DModel::~CD3DModel() {}

// Load the model into this Object, returning false for failure.
bool CD3DModel::LoadModel( const WCHAR *strFileName, const D3DVERTEXELEMENT9 VertexElements[] )
{
	// Load the mesh
	if ( FAILED( ::LoadMesh( g_pd3dDevice, strFileName, VertexElements, 
		&m_pVB, &m_pIB, &m_iNumPolys, &m_iNumVerts, &m_iNumIndices ) ) )
	{
		return false;
	}

	D3DXVECTOR3 *pData; 
	if ( FAILED( m_pVB->Lock( 0, 0, (LPVOID*) &pData, 0 ) ) )
	{
		return false;
	}

	if ( FAILED( D3DXComputeBoundingSphere( pData, m_iNumVerts, 56, &m_vCenter, &m_fRadius ) ) )
	{
		return false;
	}

	if ( FAILED( m_pVB->Unlock() ) )
	{
		return false;
	}

	if ( FAILED( g_pd3dDevice->CreateVertexDeclaration( VertexElements, &m_pDecl ) ) )
	{
		return false;
	}

	m_iVertexSize = D3DXGetDeclVertexSize( VertexElements, 0 );

	return true;
}

// Prepare the Models states for rendering.
void CD3DModel::PrepareForRender()
{
	g_pd3dDevice->SetVertexDeclaration( m_pDecl );
	g_pd3dDevice->SetStreamSource( 0, m_pVB, 0, m_iVertexSize );
	g_pd3dDevice->SetIndices( m_pIB );
}

// Render the Model.
void CD3DModel::Render()
{
	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_iNumVerts, 0, m_iNumPolys );
}

// Release the Model.
void CD3DModel::Release()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );
	SAFE_RELEASE( m_pDecl );

	Reset();
}

void CD3DModel::Reset()
{
	m_pDecl = NULL;
	m_pVB = NULL;
	m_pIB = NULL;
	m_iNumPolys = 0;
	m_iNumVerts = 0;
	m_iNumIndices = 0;
	m_iVertexSize = 0;
}

// Lock Vertices for writing to.
void CD3DModel::LockVertices( void **ppVertices )
{
	m_pVB->Lock( 0, 0, ppVertices, 0 );
}

// Unlock the Vertices.
void CD3DModel::UnlockVertices()
{
	m_pVB->Unlock();
}