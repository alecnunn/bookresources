/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\SimpleFog
File:  shader_SimpleFog.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef __SHADER_SimpleFog_H
#define __SHADER_SimpleFog_H




class CShaderSimpleFog;
class SimpleObject8;



class CShaderSimpleFog : public EBEffect
{
public:
	CShaderSimpleFog();
	~CShaderSimpleFog();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void UpdateProperties();

	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

	void SetDefaultView();
	void SetDefaultObj();
	void GenerateObj();

	void SetView();

	int m_nHDiv, m_nVDiv;
	float m_fHtFogFac, m_fWidth;
	
	D3DXVECTOR4		m_vHtFogColor;


protected:

	LPDIRECT3DTEXTURE8 m_pDecalTexture;
	LPDIRECT3DTEXTURE8 m_pLightObjTexture;
	
	SimpleObject8 * m_pObj;
	SimpleObject8 * m_pObjLight;

	float m_fAngle;
	
	BOOL	m_bWireframe;

	DWORD m_dwSimpleFogPixelShader;
	DWORD m_dwSimpleFogNoLightShader;

	DWORD m_dwSimpleFogVShader;


	D3DXVECTOR3  m_LightPos;		// in world space
	D3DXVECTOR3  m_EyePos;
	D3DXVECTOR3  m_vLookAt, m_vUp;


	D3DXMATRIX	m_matProj;
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matViewInv;
	D3DXMATRIX  m_matWorld;


};


#endif // __SHADER_SimpleFog
