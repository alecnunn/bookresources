//--------------------------------------------------------------------------------------
// File: VertexBuffer.cpp
//
// Copyright (C) 2005 - Harald Vistnes (harald@vistnes.org)
// All rights reserved worldwide.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (C) Harald Vistnes, 2005"
//--------------------------------------------------------------------------------------

#include "dxstdafx.h"
#include "VertexBuffer.h"

//----------------------------------------------------------------------------------

VertexBuffer::VertexBuffer()
:
	m_pVB			(0),
	m_iNumRows		(0),
	m_iNumCols		(0),
	m_iNumVertices	(0),
	m_bSkirt		(true)
{
}

//----------------------------------------------------------------------------------

VertexBuffer::~VertexBuffer()
{
}

//----------------------------------------------------------------------------------

HRESULT VertexBuffer::Initialize(IDirect3DDevice9 *pd3dDevice, int iNumRows, int iNumCols, bool bSkirt)
{
	if (iNumRows != m_iNumRows || iNumCols != m_iNumCols || bSkirt != m_bSkirt) {
		m_iNumRows = iNumRows;
		m_iNumCols = iNumCols;
		m_iNumVertices = iNumRows * iNumCols;
		m_bSkirt = bSkirt;

		if (m_bSkirt) {
			// If we are using a skirt along the edge, 
			// we get an additional border of vertices.
			m_iNumVertices += 2*iNumRows + 2*iNumCols - 4;
		}

		return OnResetDevice(pd3dDevice);
	}
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT VertexBuffer::OnResetDevice(IDirect3DDevice9 *pd3dDevice)
{
	if (!pd3dDevice || m_iNumRows <= 0 || m_iNumCols <= 0) {
		return E_INVALIDARG;
	}

	// create the vertex buffer 
	SAFE_RELEASE(m_pVB);
	if (FAILED(pd3dDevice->CreateVertexBuffer(m_iNumVertices * sizeof(D3DXVECTOR3), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVB, NULL))) {
		return E_FAIL;
	}

	// lock the entire vertex buffer 
	D3DXVECTOR3 *pVertices = NULL;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertices, 0))) {
		return E_FAIL;
	}

	// fill the vertex buffer with the 'normal' vertices.
	if (!FillVertices(pVertices)) {
		return E_FAIL;
	}

	// if using skirt, fill in the border/skirt vertices at the
	// end of the vertex buffer.
	if (m_bSkirt) {
		if (!FillSkirtVertices(pVertices+m_iNumRows*m_iNumCols)) {
			return E_FAIL;
		}
	}

	// unlock the vertex buffer
	m_pVB->Unlock();

	// we are done!
	return S_OK;
}

//----------------------------------------------------------------------------------

bool VertexBuffer::FillVertices(D3DXVECTOR3 *pVertices)
{
	D3DXVECTOR3 *pVertex = pVertices;

	const float fInvScaleX = 1.0f / (m_iNumCols-1.0f);
	const float fInvScaleZ = 1.0f / (m_iNumRows-1.0f);

	float s, t;
	for (int i = 0; i < m_iNumRows; i++) {
		t = i * fInvScaleZ;
		for (int j = 0; j < m_iNumCols; j++) {
			s = j * fInvScaleX;
	
			pVertex->x = s;
			pVertex->y = 1.0f;
			pVertex->z = t;
			pVertex++;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------

bool VertexBuffer::FillSkirtVertices(D3DXVECTOR3 *pVertices)
{
	D3DXVECTOR3 *pVertex = pVertices;

	const float fInvScaleX = 1.0f / (m_iNumCols-1.0f);
	const float fInvScaleZ = 1.0f / (m_iNumRows-1.0f);

	float s, t;
	t = 0.0f;
	for (int j = 0; j < m_iNumCols; j++) {
		s = j * fInvScaleX;
		pVertex->x = s;
		pVertex->y = -1.0f;
		pVertex->z = t;
		pVertex++;
	}

	for (int i = 1; i < m_iNumRows-1; i++) {
		t = i * fInvScaleZ;
		s = 0.0f;
		pVertex->x = s;
		pVertex->y = -1.0f;
		pVertex->z = t;
		pVertex++;
		s = 1.0f;
		pVertex->x = s;
		pVertex->y = -1.0f;
		pVertex->z = t;
		pVertex++;
	}

	t = 1.0f;
	for (int j = 0; j < m_iNumCols; j++) {
		s = j * fInvScaleX;
		pVertex->x = s;
		pVertex->y = -1.0f;
		pVertex->z = t;
		pVertex++;
	}

	return true;
}

//----------------------------------------------------------------------------------

void VertexBuffer::OnLostDevice()
{
	SAFE_RELEASE(m_pVB);
}

//----------------------------------------------------------------------------------

void VertexBuffer::OnDestroyDevice()
{
	SAFE_RELEASE(m_pVB);
}
