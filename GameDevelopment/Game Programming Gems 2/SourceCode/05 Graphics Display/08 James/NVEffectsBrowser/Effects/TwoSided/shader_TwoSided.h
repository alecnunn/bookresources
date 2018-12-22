/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\TwoSided
File:  shader_TwoSided.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef __SHADER_TwoSided_H
#define __SHADER_TwoSided_H




class CShaderTwoSided;
class SimpleObject8;


// TwoSided Vertex
struct TSVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u0;         //texcoord for stage 0
	float u1;         //texcoord for stage 1
};



class TwoSidedVertex
{
public:
	TwoSidedVertex(const D3DXVECTOR3& _Position,
		const D3DXVECTOR3& _Normal,
		D3DCOLOR _Diffuse,
		const D3DXVECTOR2& _Texture0,
		const D3DXVECTOR2& _Texture1,
		const D3DXVECTOR2& _Texture2
		)
		: Position(_Position), 
			Normal(_Normal),
			Texture0(_Texture0),
			Texture1(_Texture1),
			Texture2(_Texture2)
	{};

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture0;
	D3DXVECTOR2 Texture1;
	D3DXVECTOR2 Texture2;
};


#define D3DFVF_TWOSIDEDVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3)


// device overrides
class NVTwoSidedDevice : public nv_objects::NVDevice
{
public:
	NVTwoSidedDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderTwoSided* pTwoSided = NULL)
		: NVDevice(pDev),
			m_pTwoSidedShader(pTwoSided)
	{ };

	CShaderTwoSided* m_pTwoSidedShader;
};

typedef enum tageTwoSidedDisplayOptions
{
	DISPLAY_TwoSidedRESULT = 1,
	DISPLAY_TwoSidedFORCEDWORD = 0xFFFFFFFF
} eTwoSidedDisplayOptions;




class CShaderTwoSided : public EBEffect
{
public:
	CShaderTwoSided();
	~CShaderTwoSided();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void UpdateProperties();

	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

	void SetDefaultView();
	void SetView();

	int m_nHDiv, m_nVDiv;
	float m_fTwist, m_fWidth;
	

protected:
	friend NVTwoSidedDevice;
	NVTwoSidedDevice* m_pNVDevice;


	LPDIRECT3DTEXTURE8 m_pDecalTexture;
	LPDIRECT3DTEXTURE8 m_pEnvMapTexture;
	LPDIRECT3DTEXTURE8 m_pBumpMapTexture;
	LPDIRECT3DTEXTURE8 m_pLightObjTexture;
	LPDIRECT3DTEXTURE8 m_pDecalBackTexture;


	IDirect3DVertexBuffer8* m_pVertexBuffer;
	IDirect3DIndexBuffer8* m_pIndexBuffer;
	
	SimpleObject8 * m_pObj;
	SimpleObject8 * m_pObjLight;

	eTwoSidedDisplayOptions m_eDisplayOption;
	float m_fAngle;
	
	BOOL	m_bWireframe;

	DWORD m_dwTwoSidedPixelShader;
	DWORD m_dwTwoSidedNoLightShader;

	DWORD m_dwTwoSidedVShader;


	D3DXVECTOR3  m_LightPos;		// in world space
	D3DXVECTOR3  m_EyePos;
	D3DXVECTOR3  m_vLookAt, m_vUp;


	D3DXMATRIX	m_matProj;
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matViewInv;
	D3DXMATRIX  m_matWorld;


};


#endif // __SHADER_TwoSided
