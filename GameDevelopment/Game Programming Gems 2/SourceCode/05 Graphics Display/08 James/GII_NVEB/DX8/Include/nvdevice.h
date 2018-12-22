/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVDEVICE_H
#define __NVDEVICE_H

#include "nvinc.h"

namespace nv_objects
{

// Base frame
class NVDevice
{
public:
	NVDevice(LPDIRECT3DDEVICE8 pDevice)
		: m_pD3DDev(pDevice)
	{
		m_pD3DDev->AddRef();
	}
	
	~NVDevice()
	{
		if (m_pD3DDev)
		{
			m_pD3DDev->Release();
			m_pD3DDev = NULL;
		}
	}

	LPDIRECT3DDEVICE8 m_pD3DDev;

	LPDIRECT3DDEVICE8 GetDevice() { return m_pD3DDev; }
	void SetDevice(LPDIRECT3DDEVICE8 pDevice) { if (m_pD3DDev) m_pD3DDev->Release(); m_pD3DDev = pDevice; }

	// CD3DMeshRender callback defaults
    virtual HRESULT SetMaterial(D3DMATERIAL8* pMat) { return m_pD3DDev->SetMaterial(pMat); }
    virtual HRESULT GetMaterial(D3DMATERIAL8* pMat) { return m_pD3DDev->GetMaterial(pMat); }
    virtual HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { return m_pD3DDev->SetRenderState(State, Value); }
    virtual HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { return m_pD3DDev->GetRenderState(State, pValue); }
	virtual HRESULT SetTexture(DWORD Stage,IDirect3DBaseTexture8* pTexture) { return m_pD3DDev->SetTexture(Stage, pTexture); }
    virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) { return m_pD3DDev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); }
    virtual HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT minIndex,UINT NumIndices,UINT startIndex,UINT primCount) { return m_pD3DDev->DrawIndexedPrimitive(PrimitiveType, minIndex, NumIndices, startIndex, primCount); }
	virtual HRESULT SetVertexShader(DWORD dwHandle) { return m_pD3DDev->SetVertexShader(dwHandle); }
    virtual HRESULT SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer8 *pStreamData,UINT Stride) { return m_pD3DDev->SetStreamSource(StreamNumber, pStreamData, Stride); }
    virtual HRESULT SetIndices(IDirect3DIndexBuffer8 *pIndexData,UINT BaseVertexIndex) { return m_pD3DDev->SetIndices(pIndexData, BaseVertexIndex); }

	// Device API
	virtual void SetWorldTransform(const D3DXMATRIX* pMatrix) { if (pMatrix) m_matWorld = *pMatrix; }
	virtual D3DXMATRIX GetWorldTransform() const { return m_matWorld; }

	virtual void SetViewTransform(const D3DXMATRIX* pMatrix) { if (pMatrix) m_matView = *pMatrix; }
	virtual D3DXMATRIX GetViewTransform() const { return m_matView; }

	virtual void SetProjectionTransform(const D3DXMATRIX* pMatrix) { if (pMatrix) m_matProj = *pMatrix; }
	virtual D3DXMATRIX GetProjectionTransform() const { return m_matProj; }

	virtual void SetRenderPart(DWORD dwRenderPart) { m_dwRenderPart = dwRenderPart; }
	virtual DWORD GetRenderPart() const { return m_dwRenderPart; }

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;

	DWORD m_dwRenderPart;
};


}; // nv_objects

#endif // __NVDEVICE_H
	