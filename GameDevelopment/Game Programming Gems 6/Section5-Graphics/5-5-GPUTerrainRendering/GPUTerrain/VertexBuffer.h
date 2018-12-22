//--------------------------------------------------------------------------------------
// File: VertexBuffer.h
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

#ifndef __VERTEX_BUFFER_H_
#define __VERTEX_BUFFER_H_

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	HRESULT Initialize(IDirect3DDevice9 *pd3dDevice, int iNumRows, int iNumCols, bool bSkirt);
	HRESULT OnResetDevice(IDirect3DDevice9 *pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

	inline int GetNumRows() const { return m_iNumRows; }
	inline int GetNumCols() const { return m_iNumCols; }
	inline int GetNumVertices() const { return m_iNumVertices; }
	inline bool HasSkirt() const { return m_bSkirt; }

	inline IDirect3DVertexBuffer9* GetVertexBuffer() const { return m_pVB; }

private:
	bool FillVertices(D3DXVECTOR3 *pVertices);
	bool FillSkirtVertices(D3DXVECTOR3 *pVertices);

private:
	IDirect3DVertexBuffer9*			m_pVB;
	int								m_iNumRows;
	int								m_iNumCols;
	int								m_iNumVertices;
	bool							m_bSkirt;
};

#endif // __VERTEX_BUFFER_H_
