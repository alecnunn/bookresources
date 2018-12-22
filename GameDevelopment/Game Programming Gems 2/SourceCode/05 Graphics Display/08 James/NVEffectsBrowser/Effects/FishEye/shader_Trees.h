/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __SHADER_TREES_H
#define __SHADER_TREES_H

typedef enum tagTreeType
{
	TREETYPE_FISHEYE = 0,
	TREETYPE_HEIGHTFOG,
} TreeType;

#define NUM_TREES 500
#define D3DFVF_TREEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
struct TREEVERTEX
{
    D3DXVECTOR3 p;
    DWORD   color;
    float tu, tv;
};

class Tree
{
public:
    TREEVERTEX  v[4];
    D3DXVECTOR3 vPos;
};

class CShaderTrees;

// Tree device overrides
class NVTreeDevice : public nv_objects::NVDevice
{
public:
	NVTreeDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderTrees* pTreeShader = NULL)
		: NVDevice(pDev),
			m_pTreeShader(pTreeShader)
	{ };

	// Device functions
	virtual void SetWorldTransform(const D3DXMATRIX* pValue);

	CShaderTrees* m_pTreeShader;
};

class CShaderTrees : public EBEffect
{
public:
	CShaderTrees(TreeType _TreeType);
	~CShaderTrees();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

protected:
	friend NVTreeDevice;

    bool       mbWireFrame;
	bool	   m_bPause;

	// Matrices
    D3DXMATRIX m_matBillboardMatrix;


	NVTreeDevice* m_pNVDevice;

	// x files, textures
	nv_objects::NVMesh*		m_pTerrain;
	LPDIRECT3DTEXTURE8		m_pTreeTexture;
	LPDIRECT3DTEXTURE8		m_pFishViewTexture;

    LPDIRECT3DVERTEXBUFFER8 m_pFishViewVB;
    LPDIRECT3DVERTEXBUFFER8 m_pTreeVB;

	std::vector<Tree> m_vecTrees;

	DWORD m_dwTreeShader;
	DWORD m_dwTerrainShader;
	DWORD m_dwCurrentShader;
	DWORD m_dwWidth;
	DWORD m_dwHeight;

	float m_fDuration;

	TreeType m_TreeType;
	
	float HeightField(float x, float y);
	HRESULT SetTransform();
	HRESULT DrawFishViewport();
	HRESULT DrawTrees(bool bDistort);
	HRESULT DrawTerrain(bool bDistort);

};

class CShaderFishEye : public CShaderTrees
{
public:
	CShaderFishEye()
		: CShaderTrees(TREETYPE_FISHEYE)
	{
		m_strEffectName = "Fish Eye Lens";
		m_strEffectLocation = "Vertex Shaders\\Deformation";
		m_strEffectPixelShader = "";
		m_strEffectVertexShader = GetFilePath("FishEye_Terrain.nvv");
	}

	virtual void UpdateProperties()
	{
		EBEffect::UpdateProperties();

        AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(mbWireFrame), EBTYPE_BOOL_PROP));
        AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));
		m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Vertex Distortion", GetFilePath("FishEye_Terrain.nvv"), EBTYPE_STRING_PROP));
	}
private:

};

#endif // _SHADER_TREES_H