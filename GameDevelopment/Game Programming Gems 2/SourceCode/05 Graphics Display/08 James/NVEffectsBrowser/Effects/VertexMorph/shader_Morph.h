/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __SHADER_MORPH_H
#define __SHADER_MORPH_H

class CShaderMorph;
 
class DolphinVertex
{
public:
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

typedef DolphinVertex SeaFloorVertex;
#define DOLPHINVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define SEAFLOORVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define WATER_COLOR         0x00006688
 
// Fog device overrides
class NVMorphDevice : public nv_objects::NVDevice
{ 
public:
	NVMorphDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderMorph* pMorph = NULL) 
		: NVDevice(pDev),
			m_pMorphShader(pMorph)
	{ };
	
	CShaderMorph* m_pMorphShader;
}; 
  
class CShaderMorph : public EBEffect
{
public:
	CShaderMorph();
	~CShaderMorph(); 
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void UpdateProperties();
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

private:
	friend class NVMorphDevice;
  
	HRESULT SetTransform();

	NVMorphDevice* m_pNVDevice;

	// The sea floor
	nv_objects::NVMesh* m_pFloorMesh;
	LPDIRECT3DTEXTURE8 m_pDolphinMap;

	DWORD m_dwDolphinShader;
	DWORD m_dwSeaFloorShader;

	IDirect3DVertexBuffer8* m_pDolphinVB[3];
	IDirect3DIndexBuffer8* m_pDolphinIB;

	std::vector<LPDIRECT3DTEXTURE8> m_vecCausticTextures;

	DWORD m_dwNumIndices;
	DWORD m_dwNumVertices;

	D3DXVECTOR3 m_vCenter;
	float m_fRadius;
	float m_fWeight;

	bool m_bWireframe;
	bool m_bPause;

};

#endif //__SHADER_MORPH_H