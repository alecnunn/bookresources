//--------------------------------------------------------------------------------------
// File: IndexBuffer.cpp
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
#include "IndexBuffer.h"

//----------------------------------------------------------------------------------

IndexBuffer::IndexBuffer()
:
	m_pIB			(0),
	m_iNumRows		(0),
	m_iNumCols		(0),
	m_iNumIndices	(0),
	m_bSkirt		(true)
{
}

//----------------------------------------------------------------------------------

IndexBuffer::~IndexBuffer()
{
}

//----------------------------------------------------------------------------------

HRESULT IndexBuffer::Initialize(IDirect3DDevice9 *pd3dDevice, int iNumRows, int iNumCols, bool bSkirt)
{
	if (!pd3dDevice || iNumRows <= 0 || iNumCols <= 0) {
		// illegal argument
		return E_INVALIDARG;
	}

	m_iNumRows = iNumRows;
	m_iNumCols = iNumCols;

	HRESULT hr;

	if (bSkirt) {
		hr = InitializeSkirt(pd3dDevice);
	} else {
		hr = InitializeNoSkirt(pd3dDevice);
	}
	return hr;
}

//----------------------------------------------------------------------------------

HRESULT IndexBuffer::InitializeNoSkirt(IDirect3DDevice9 *pd3dDevice)
{
	unsigned short uiNumStrips = static_cast<unsigned short>(m_iNumRows-1);
	unsigned short uiLineStep = static_cast<unsigned short>(m_iNumCols);
	unsigned short uiStartVertex = 0;
	unsigned short uiStepX = 1;

	m_iNumIndices = 2*uiNumStrips*(m_iNumCols+1) - 2;

	SAFE_RELEASE(m_pIB);
	if (FAILED(pd3dDevice->CreateIndexBuffer(m_iNumIndices*sizeof(unsigned short),D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, 0))) {
		return E_FAIL;
	}

	unsigned short *p = 0;
	if (FAILED(m_pIB->Lock(0, 0, (void**)&p, 0))) {
		return E_FAIL;
	}
	for (unsigned short j = 0; j < uiNumStrips; j++) { 
		unsigned short uiVert = uiStartVertex;
		for (unsigned short k = 0; k < m_iNumCols; k++) {
			*(p++) = uiVert;
			*(p++) = uiVert + uiLineStep;
			uiVert = uiVert + uiStepX;
		}
		uiStartVertex = uiStartVertex + uiLineStep;
		if (j+1 < uiNumStrips) { // add degenerate triangle
			*(p++) = (uiVert-uiStepX)+uiLineStep;
			*(p++) = uiStartVertex;
		}
	}
	m_pIB->Unlock();
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT IndexBuffer::InitializeSkirt(IDirect3DDevice9 *pd3dDevice)
{
	unsigned short uiNumRows = static_cast<unsigned short>(m_iNumRows);
	unsigned short uiNumCols = static_cast<unsigned short>(m_iNumCols);
	unsigned short uiNumStrips = uiNumRows-1;
	unsigned short uiLineStep = uiNumCols;
	unsigned short uiStartVertex = 0;
	unsigned short uiStepX = 1;

	unsigned short uiNumStripIndices = 2*uiNumStrips*(uiNumCols+1) - 2 + 2 + 4*uiNumCols * 2*uiNumStrips + 2*(uiNumStrips-1);
	m_iNumIndices = uiNumStripIndices;

	SAFE_RELEASE(m_pIB);
	if (FAILED(pd3dDevice->CreateIndexBuffer(m_iNumIndices*sizeof(unsigned short),D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, 0))) {
		return E_FAIL;
	}

    unsigned short *p = NULL;
	if (FAILED(m_pIB->Lock(0, 0, (void**)&p, 0))) {
		return E_FAIL;
	}

	// regular grid
	for (unsigned short j = 0; j < uiNumStrips; j++) { 
	    unsigned short uiVert = uiStartVertex;
	    for (unsigned short k = 0; k < uiNumCols; k++) {
	        *(p++) = uiVert;
	        *(p++) = uiVert + uiLineStep;
	        uiVert = uiVert + uiStepX;
	    }
	    uiStartVertex = uiStartVertex + uiLineStep;
	    if (j+1 < uiNumStrips) { // add degenerate triangle
	        *(p++) = (uiVert-uiStepX)+uiLineStep;
	        *(p++) = uiStartVertex;
	    }
	}

	// add degenerate triangles to start over
	uiStartVertex = uiStartVertex + uiLineStep;
	*(p++) = *(p-1);
	*(p++) = uiStartVertex;
	
	// bottom border
	unsigned short uiSkirt = uiStartVertex;
	for (unsigned short k = 0; k < uiNumCols; k++) {
		*(p++) = uiSkirt;
		*(p++) = k;
		uiSkirt++;
	}

	// right border
	uiSkirt++;
	unsigned short uiVert = uiNumCols + uiLineStep - 1;
	for (unsigned short j = 0; j < uiNumStrips-1; j++) {
		*(p++) = uiSkirt;
		*(p++) = uiVert;
		uiSkirt += 2;
		uiVert = uiVert + uiLineStep;
	}

	// top border, from right to left
	uiVert = uiNumRows*uiNumCols-1; 
	uiSkirt =  uiVert + 2*uiLineStep + 2*(uiLineStep-2);
	for (unsigned short k = 0; k < uiNumCols; k++) {
		*(p++) = uiSkirt;
		*(p++) = uiVert;
		uiSkirt--;
		uiVert--;
	}

	// left border, from top to bottom
	uiSkirt = uiSkirt - 1;
	uiVert = uiVert - uiLineStep + 1;
	for (unsigned short j = 0; j < uiNumStrips-1; j++) {
		*(p++) = uiSkirt;
		*(p++) = uiVert;
		uiSkirt -= 2;
		uiVert = uiVert - uiLineStep;
	}
	uiSkirt = uiNumRows*uiNumCols;
	*(p++) = uiSkirt;
	*(p++) = uiVert;

	m_pIB->Unlock();
	return S_OK;
}

//----------------------------------------------------------------------------------

void IndexBuffer::OnDestroyDevice()
{
	SAFE_RELEASE(m_pIB);
}

