//--------------------------------------------------------------------------------------
// File: IndexBuffer.h
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

#ifndef __INDEX_BUFFER_H_
#define __INDEX_BUFFER_H_

class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();

	HRESULT Initialize(IDirect3DDevice9 *pd3dDevice, int iNumRows, int iNumCols, bool bSkirt);
	void OnDestroyDevice();

	inline int GetNumRows() const { return m_iNumRows; }
	inline int GetNumCols() const { return m_iNumCols; }
	inline int GetNumIndices() const { return m_iNumIndices; }
	inline bool HasSkirt() const { return m_bSkirt; }

	inline IDirect3DIndexBuffer9* GetIndexBuffer() const { return m_pIB; }

private:
	HRESULT InitializeNoSkirt(IDirect3DDevice9 *pd3dDevice);
	HRESULT InitializeSkirt(IDirect3DDevice9 *pd3dDevice);

private:
	IDirect3DIndexBuffer9*			m_pIB;
	int								m_iNumRows;
	int								m_iNumCols;
	int								m_iNumIndices;
	bool							m_bSkirt;
};

#endif // __INDEX_BUFFER_H_
