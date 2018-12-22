/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      A simple class to manage rendering state.  Created as a singleton.
	  Create it as a static global, or with new.  It doesn't matter as long as it is created 
	  before you use the NVStateManager::GetSingleton() API to get a reference to it.

	  Call it with NVSTATEMANAGER.SetRenderState(...)
	  Call it with NVSTATEMANAGER.SetTextureStageState(...), etc.

	  Call the 'Save' versions of the function if you want to deviate from the current state.
	  Call the 'Restore' version to retrieve the last Save.

	  There are two levels of caching:
	  - All Sets/Saves/Restores are tracked for redundancy.  This reduces the size of the batch to
	  be flushed
	  - The flush function is called before rendering, and only copies state that is 
	  different from the current chip state.

  If you get an assert it is probably because an API call failed.

  See NVLink for a good example of how this class is used.

  Don't be afraid ofthe vector being used to track the flush batch.  It will grow as big as
  it needs to be and then stop, so it shouldn't be reallocated.

  The state manager holds a reference to the d3d device.


  - cmaughan@nvidia.com
        
******************************************************************************/
#ifndef __NVSTATEMANAGER_H
#define __NVSTATEMANAGER_H

#include "singleton.h"
#include "nvdebug.h"

namespace nv_objects
{

static const DWORD NVSTATEMANAGER_MAX_RENDERSTATES = 256;
static const DWORD NVSTATEMANAGER_MAX_TEXTURESTATES = 128;
static const DWORD NVSTATEMANAGER_MAX_STAGES = 8;
static const DWORD NVSTATEMANAGER_MAX_VCONSTANTS = 96;
static const DWORD NVSTATEMANAGER_MAX_PCONSTANTS = 8;
static const DWORD NVSTATEMANAGER_MAX_TRANSFORMSTATES = 300;	// World1 lives way up there...
static const DWORD NVSTATEMANAGER_MAX_STREAMS = 16;

class NVStreamData
{
public:
	NVStreamData(LPDIRECT3DVERTEXBUFFER8 pStreamData = NULL, UINT Stride = 0)
		: m_pStreamData(pStreamData),
		m_Stride(Stride)
	{}

	bool operator == (const NVStreamData& rhs) const { return ((m_pStreamData == rhs.m_pStreamData) && (m_Stride == rhs.m_Stride)); }
	LPDIRECT3DVERTEXBUFFER8 m_pStreamData;
	UINT m_Stride;
};

class NVIndexData
{
public:
	NVIndexData(LPDIRECT3DINDEXBUFFER8 pIndexData = NULL, UINT BaseVertexIndex = 0)
		: m_pIndexData(pIndexData),
		m_BaseVertexIndex(BaseVertexIndex)
	{}

	bool operator == (const NVIndexData& rhs) const { return ((m_pIndexData == rhs.m_pIndexData) && (m_BaseVertexIndex == rhs.m_BaseVertexIndex)); }
	LPDIRECT3DINDEXBUFFER8 m_pIndexData;
	UINT m_BaseVertexIndex;
};

// State types managed by the class
typedef enum eStateType
{
	STATE_MATERIAL = 0,
	STATE_RENDER,
	STATE_TEXTURE,
	STATE_TEXTURESTAGE,
	STATE_VSHADER,
	STATE_PSHADER,
	STATE_TRANSFORM,
	STATE_VCONSTANT,
	STATE_PCONSTANT,
	STATE_STREAM,
	STATE_INDEX
} eStateType;

class NVStateID
{
public:
	NVStateID(eStateType Type, DWORD dwValue0 = 0, DWORD dwValue1 = 0)
		: m_Type(Type),
		m_dwValue0(dwValue0),
		m_dwValue1(dwValue1)
	{}

	NVStateID(eStateType Type, DWORD dwStage, D3DTEXTURESTAGESTATETYPE StageType)
		: m_Type(Type),
		m_dwStage(dwStage),
		m_TextureStageStateType(StageType)
	{}

	NVStateID(eStateType Type, D3DRENDERSTATETYPE RenderType)
		: m_Type(Type),
		m_RenderStateType(RenderType)
	{}

	eStateType m_Type;
	union
	{
		DWORD m_dwValue0;
		DWORD m_dwStage;
		D3DRENDERSTATETYPE m_RenderStateType;
		D3DTRANSFORMSTATETYPE m_TransformStateType;
	};

	union
	{
		DWORD m_dwValue1;
		D3DTEXTURESTAGESTATETYPE m_TextureStageStateType;
	};

};

typedef std::vector<NVStateID> tStateID;

class NVStateManagerState
{
public:
	NVStateManagerState()
	{
	}

	void ResetState()
	{

		DWORD i, y;
		for (i = 0; i < NVSTATEMANAGER_MAX_RENDERSTATES; i++)
		{
			m_RenderStates[i] = 0;
		}

		for (i = 0; i < NVSTATEMANAGER_MAX_STAGES; i++)
		{
			for (y = 0; y < NVSTATEMANAGER_MAX_TEXTURESTATES; y++)
			{
				m_TextureStates[i][y] = 0;
			}
		}

		for (i = 0; i < NVSTATEMANAGER_MAX_STREAMS; i++)
		{
			m_StreamData[i] = NVStreamData();
		}

		m_IndexData = NVIndexData();

		for (i = 0; i < NVSTATEMANAGER_MAX_STAGES; i++)
		{
			m_Textures[i] = 0;
		}

		// Matrices and constants are not cached, just restored.  It's silly to check all the 
		// data elements (by which time the driver could have been sent them).
		for (i = 0; i < NVSTATEMANAGER_MAX_TRANSFORMSTATES; i++)
		{
			D3DXMatrixIdentity(&m_Matrices[i]);
		}
		for (i = 0; i < NVSTATEMANAGER_MAX_VCONSTANTS; i++)
		{
			m_VertexShaderConstants[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		for (i = 0; i < NVSTATEMANAGER_MAX_PCONSTANTS; i++)
		{
			m_PixelShaderConstants[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		
		m_dwPixelShader = 0;
		m_dwVertexShader = D3DFVF_XYZ;

		ZeroMemory(&m_Matrices, sizeof(D3DXMATRIX) * NVSTATEMANAGER_MAX_TRANSFORMSTATES);
	}

	// Renderstates
	DWORD m_RenderStates[NVSTATEMANAGER_MAX_RENDERSTATES];

	// Texture stage states
	DWORD m_TextureStates[NVSTATEMANAGER_MAX_STAGES][NVSTATEMANAGER_MAX_TEXTURESTATES];

	// Vertex shader constants
	D3DXVECTOR4 m_VertexShaderConstants[NVSTATEMANAGER_MAX_VCONSTANTS];
	
	// Pixel shader constants
	D3DXVECTOR4 m_PixelShaderConstants[NVSTATEMANAGER_MAX_PCONSTANTS];
	
	// Textures
	LPDIRECT3DBASETEXTURE8 m_Textures[NVSTATEMANAGER_MAX_STAGES];

	// Shaders
	DWORD m_dwPixelShader;
	DWORD m_dwVertexShader;

	D3DXMATRIX m_Matrices[NVSTATEMANAGER_MAX_TRANSFORMSTATES];

	D3DMATERIAL8 m_D3DMaterial;

	NVStreamData m_StreamData[NVSTATEMANAGER_MAX_STREAMS];
	NVIndexData m_IndexData;
};

class NVStateManager : public Singleton <NVStateManager>
{
public:
	NVStateManager(LPDIRECT3DDEVICE8 pDevice)
		: m_pD3DDev(pDevice)
	{
		assert(pDevice);
		pDevice->AddRef();
		SetDefaultState();
	}

	virtual ~NVStateManager()
	{
		SAFE_RELEASE(m_pD3DDev);
	}

	virtual void SetDefaultState()
	{
		m_CurrentState.ResetState();
		m_CopyState.ResetState();
		m_ChipState.ResetState();

		m_bForce = true;

		D3DXMATRIX Identity;
		D3DXMatrixIdentity(&Identity);
		SetTransform(D3DTS_WORLD, &Identity);
		SetTransform(D3DTS_VIEW, &Identity);
		SetTransform(D3DTS_PROJECTION, &Identity);

		D3DMATERIAL8 DefaultMat;
		ZeroMemory(&DefaultMat, sizeof(D3DMATERIAL8));
		DefaultMat.Diffuse.r = 1.0f;
		DefaultMat.Diffuse.g = 1.0f;
		DefaultMat.Diffuse.b = 1.0f;
		DefaultMat.Diffuse.a = 1.0f;
		SetMaterial(&DefaultMat);

		SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
		
		SetRenderState(D3DRS_LINEPATTERN, 0xFFFFFFFF);
		SetRenderState(D3DRS_LASTPIXEL, FALSE);
		SetRenderState(D3DRS_ALPHAREF, 0);
		SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
		SetRenderState(D3DRS_ZVISIBLE, FALSE);
		SetRenderState(D3DRS_FOGSTART, 0);
		SetRenderState(D3DRS_FOGEND, 0);
		SetRenderState(D3DRS_FOGDENSITY, 0);
		SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
		SetRenderState(D3DRS_ZBIAS, 0);
	    SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		SetRenderState(D3DRS_AMBIENT, 0x00000000);
		SetRenderState(D3DRS_LOCALVIEWER, FALSE);
		SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);
		SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
		SetRenderState(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
		SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
		SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		SetRenderState(D3DRS_FOGENABLE, FALSE);
		SetRenderState(D3DRS_FOGCOLOR, 0xFF0000);
		SetRenderState(D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
		SetRenderState(D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
		SetRenderState(D3DRS_RANGEFOGENABLE, FALSE );
		SetRenderState(D3DRS_ZENABLE, TRUE);
		SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		SetRenderState(D3DRS_DITHERENABLE, TRUE);
		SetRenderState(D3DRS_STENCILENABLE, FALSE);
		SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		SetRenderState(D3DRS_CLIPPING, TRUE);
		SetRenderState(D3DRS_LIGHTING, FALSE);
		SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		SetRenderState(D3DRS_COLORVERTEX, FALSE);
		SetRenderState(D3DRS_WRAP0, 0);
		SetRenderState(D3DRS_WRAP1, 0);
		SetRenderState(D3DRS_WRAP2, 0);
		SetRenderState(D3DRS_WRAP3, 0);
		SetRenderState(D3DRS_WRAP4, 0);
		SetRenderState(D3DRS_WRAP5, 0);
		SetRenderState(D3DRS_WRAP6, 0);
		SetRenderState(D3DRS_WRAP7, 0);

		SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(3, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(4, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(4, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(4, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(4, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(4, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(5, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(5, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(5, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(5, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(5, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(5, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(6, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(6, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(6, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(6, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(6, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(6, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(7, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(7, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(7, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		SetTextureStageState(7, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(7, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(7, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
		SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
		SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);
		SetTextureStageState(4, D3DTSS_TEXCOORDINDEX, 4);
		SetTextureStageState(5, D3DTSS_TEXCOORDINDEX, 5);
		SetTextureStageState(6, D3DTSS_TEXCOORDINDEX, 6);
		SetTextureStageState(7, D3DTSS_TEXCOORDINDEX, 7);
		
		SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(4, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(4, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(4, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(5, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(5, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(5, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(6, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(6, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(6, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(7, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(7, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetTextureStageState(7, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(4, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(4, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(5, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(5, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(6, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(6, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		SetTextureStageState(7, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		SetTextureStageState(7, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

		SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(4, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(5, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(6, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
		SetTextureStageState(7, D3DTSS_TEXTURETRANSFORMFLAGS, 0);

		SetTexture(0, NULL);
		SetTexture(1, NULL);
		SetTexture(2, NULL);
		SetTexture(3, NULL);
		SetTexture(4, NULL);
		SetTexture(5, NULL);
		SetTexture(6, NULL);
		SetTexture(7, NULL);

		SetPixelShader(0);
		SetVertexShader(D3DFVF_XYZ);

		SetVertexShaderConstant(0, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), NVSTATEMANAGER_MAX_VCONSTANTS);
		SetPixelShaderConstant(0, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), NVSTATEMANAGER_MAX_PCONSTANTS);
	
		FlushState();

		m_bForce = false;
	}

	// Material
	virtual void SaveMaterial(const D3DMATERIAL8* pMaterial)
	{
		// Check that we have set this up before, if not, the default is this.
		m_CopyState.m_D3DMaterial = m_CurrentState.m_D3DMaterial;
		SetMaterial(pMaterial);
	}

	virtual void RestoreMaterial()
	{
		SetMaterial(&m_CopyState.m_D3DMaterial);
	}

	virtual void SetMaterial(const D3DMATERIAL8* pMaterial)
	{
		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_MATERIAL));
		m_CurrentState.m_D3DMaterial = *pMaterial;
	}

	// Renderstates
	virtual void SaveRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue)
	{
		// Check that we have set this up before, if not, the default is this.
		m_CopyState.m_RenderStates[Type] = m_CurrentState.m_RenderStates[Type];
		SetRenderState(Type, dwValue);
	}

	virtual void RestoreRenderState(D3DRENDERSTATETYPE Type)
	{
		SetRenderState(Type, m_CopyState.m_RenderStates[Type]);
	}

	virtual void SetRenderState(D3DRENDERSTATETYPE Type, DWORD Value)
	{
		assert(Type < NVSTATEMANAGER_MAX_RENDERSTATES);
		if (!m_bForce &&  (m_CurrentState.m_RenderStates[Type] == Value))
			return;

		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_RENDER, (DWORD)Type));
		m_CurrentState.m_RenderStates[Type] = Value;
	}

	// Textures
	virtual void SaveTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture)
	{
		// Check that we have set this up before, if not, the default is this.
		m_CopyState.m_Textures[dwStage] = m_CurrentState.m_Textures[dwStage];
		SetTexture(dwStage, pTexture);
	}

	virtual void RestoreTexture(DWORD dwStage)
	{
		SetTexture(dwStage, m_CopyState.m_Textures[dwStage]);
	}

	virtual void SetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture)
	{
		assert(dwStage < NVSTATEMANAGER_MAX_STAGES);
		if (!m_bForce && (m_CurrentState.m_Textures[dwStage] == pTexture))
			return;

		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_TEXTURE, dwStage));
		m_CurrentState.m_Textures[dwStage] = pTexture;
	}

	// Texture stage states
	virtual void SaveTextureStageState(DWORD dwStage,D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue)
	{
		// Check that we have set this up before, if not, the default is this.
		m_CopyState.m_TextureStates[dwStage][Type] = m_CurrentState.m_TextureStates[dwStage][Type];
		SetTextureStageState(dwStage, Type, dwValue);
	}

	virtual void RestoreTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type)
	{
		SetTextureStageState(dwStage, Type, m_CopyState.m_TextureStates[dwStage][Type]);
	}

	virtual void SetTextureStageState(DWORD dwStage,D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue)
	{
		assert(dwStage < NVSTATEMANAGER_MAX_STAGES);
		assert(Type < NVSTATEMANAGER_MAX_TEXTURESTATES);
		if (!m_bForce && m_CurrentState.m_TextureStates[dwStage][Type] == dwValue)
			return;

		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_TEXTURESTAGE, dwStage, (DWORD)Type));
		m_CurrentState.m_TextureStates[dwStage][Type] = dwValue;
	}

	// Vertex Shader
	virtual void SaveVertexShader(DWORD dwShader)
	{
		m_CopyState.m_dwVertexShader = m_CurrentState.m_dwVertexShader;
		SetVertexShader(dwShader);
	}

	virtual void RestoreVertexShader()
	{
		SetVertexShader(m_CopyState.m_dwVertexShader);
	}

	virtual void SetVertexShader(DWORD dwShader)
	{
		if (!m_bForce && m_CurrentState.m_dwVertexShader == dwShader)
			return;

		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_VSHADER));
		m_CurrentState.m_dwVertexShader = dwShader;
	}

	// Pixel Shader
	virtual void SavePixelShader(DWORD dwShader)
	{
		m_CopyState.m_dwPixelShader = m_CurrentState.m_dwPixelShader;
		SetPixelShader(dwShader);
	}

	virtual void RestorePixelShader()
	{
		SetPixelShader(m_CopyState.m_dwPixelShader);
	}

	virtual void SetPixelShader(DWORD dwShader)
	{
		if (!m_bForce && m_CurrentState.m_dwPixelShader == dwShader)
			return;

		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_PSHADER));
		m_CurrentState.m_dwPixelShader = dwShader;
	}

	// *** These states are cached, but not protected from multiple sends of the same value.
	// Transform
	virtual void SaveTransform(D3DTRANSFORMSTATETYPE Transform, const D3DMATRIX* pMatrix)
	{
		m_CopyState.m_Matrices[Transform] = m_CurrentState.m_Matrices[Transform];
		SetTransform(Transform, (D3DXMATRIX*)pMatrix);
	}

	virtual void RestoreTransform(D3DTRANSFORMSTATETYPE Transform)
	{
		SetTransform(Transform, &m_CopyState.m_Matrices[Transform]);
	}

	// Don't cache-check the transform.  To much to do
	virtual void SetTransform(D3DTRANSFORMSTATETYPE Type, const D3DMATRIX* pMatrix)
	{
		assert(Type < NVSTATEMANAGER_MAX_TRANSFORMSTATES);
		assert(pMatrix);

		// Set the renderstate and remember it.
		m_DirtyStates.push_back(NVStateID(STATE_TRANSFORM, (DWORD)Type));
		m_CurrentState.m_Matrices[Type] = *pMatrix;
	}

	// SetVertexShaderConstant
	virtual void SaveVertexShaderConstant(DWORD dwRegister,CONST void* pConstantData,DWORD dwConstantCount)
	{
		DWORD i;
		for (i = 0; i < dwConstantCount; i++)
		{
			assert((dwRegister + i) < NVSTATEMANAGER_MAX_VCONSTANTS);
			
			m_CopyState.m_VertexShaderConstants[dwRegister + i] = m_CurrentState.m_VertexShaderConstants[dwRegister + i];
		}
		SetVertexShaderConstant(dwRegister, pConstantData, dwConstantCount);
	}

	virtual void RestoreVertexShaderConstant(DWORD dwRegister, DWORD dwConstantCount)
	{
		SetVertexShaderConstant(dwRegister, &m_CopyState.m_VertexShaderConstants[dwRegister], dwConstantCount);
	}

	virtual void SetVertexShaderConstant(DWORD dwRegister,CONST void* pConstantData,DWORD dwConstantCount)
	{
		DWORD i;
		
		assert(pConstantData);
		assert(dwRegister < NVSTATEMANAGER_MAX_VCONSTANTS);

		if (!m_bForce)
		{
			for (i = 0; i < dwConstantCount; i++)
			{	
				assert((dwRegister + i) < NVSTATEMANAGER_MAX_VCONSTANTS);
				if (m_CurrentState.m_VertexShaderConstants[dwRegister + i] != *(((D3DXVECTOR4*)pConstantData + i)))
					break;
			}
			if (i == dwConstantCount)
				return;
		}

		// Set the renderstate and remember it.
		for (i = 0; i < dwConstantCount; i++)
		{
			assert((dwRegister + i) < NVSTATEMANAGER_MAX_VCONSTANTS);
			m_DirtyStates.push_back(NVStateID(STATE_VCONSTANT, dwRegister + i));
			m_CurrentState.m_VertexShaderConstants[dwRegister + i] = *(((D3DXVECTOR4*)pConstantData) + i);
		}
	}

	// SetPixelShaderConstant
	virtual void SavePixelShaderConstant(DWORD dwRegister,CONST void* pConstantData,DWORD dwConstantCount)
	{
		DWORD i;
		for (i = 0; i < dwConstantCount; i++)
		{
			assert((dwRegister + i) < NVSTATEMANAGER_MAX_VCONSTANTS);

			m_CopyState.m_PixelShaderConstants[dwRegister + i] = *(((D3DXVECTOR4*)pConstantData) + i);
		}
		SetPixelShaderConstant(dwRegister, pConstantData, dwConstantCount);
	}

	virtual void RestorePixelShaderConstant(DWORD dwRegister, DWORD dwConstantCount)
	{
		SetPixelShaderConstant(dwRegister, &m_CopyState.m_PixelShaderConstants[dwRegister], dwConstantCount);
	}

	virtual void SetPixelShaderConstant(DWORD dwRegister,CONST void* pConstantData,DWORD dwConstantCount)
	{
		DWORD i;

		assert(pConstantData);
		assert(dwRegister < NVSTATEMANAGER_MAX_PCONSTANTS);

		if (!m_bForce)
		{
			for (i = 0; i < dwConstantCount; i++)
			{
				assert((dwRegister + i) < NVSTATEMANAGER_MAX_PCONSTANTS);
				if (m_CurrentState.m_PixelShaderConstants[dwRegister + i] != *(((D3DXVECTOR4*)pConstantData + i)))
					break;
			}

			if (i == dwConstantCount)
				return;
		}

		// Set the renderstate and remember it.
		for (i = 0; i < dwConstantCount; i++)
		{
			assert((dwRegister + i) < NVSTATEMANAGER_MAX_VCONSTANTS);
			m_DirtyStates.push_back(NVStateID(STATE_PCONSTANT, dwRegister + i));
			m_CurrentState.m_PixelShaderConstants[dwRegister + i] = *(((D3DXVECTOR4*)pConstantData) + i);
		}
	}

	void SaveStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData,UINT Stride)
	{
		// Check that we have set this up before, if not, the default is this.
		m_CopyState.m_StreamData[StreamNumber] = m_CurrentState.m_StreamData[StreamNumber];
		SetStreamSource(StreamNumber, pStreamData, Stride);
	}

	void RestoreStreamSource(UINT StreamNumber)
	{
		SetStreamSource(StreamNumber, m_CopyState.m_StreamData[StreamNumber].m_pStreamData, m_CopyState.m_StreamData[StreamNumber].m_Stride);
	}

	void SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData,UINT Stride)
	{
		if (!m_bForce && m_CurrentState.m_StreamData[StreamNumber] == NVStreamData(pStreamData, Stride))
		{
			return;
		}
		m_DirtyStates.push_back(NVStateID(STATE_STREAM, (DWORD)StreamNumber));
		m_CurrentState.m_StreamData[StreamNumber] = NVStreamData(pStreamData, Stride);
	}

	void SaveIndices(LPDIRECT3DINDEXBUFFER8 pIndexData, UINT BaseVertexIndex)
	{
		m_CopyState.m_IndexData = m_CurrentState.m_IndexData;
		SetIndices(pIndexData, BaseVertexIndex);
	}

	void RestoreIndices()
	{
		SetIndices(m_CopyState.m_IndexData.m_pIndexData, m_CopyState.m_IndexData.m_BaseVertexIndex);
	}

    void SetIndices(LPDIRECT3DINDEXBUFFER8 pIndexData,UINT BaseVertexIndex)
	{
		if (!m_bForce && m_CurrentState.m_IndexData == NVIndexData(pIndexData, BaseVertexIndex))
		{
			return;
		}

		m_DirtyStates.push_back(NVStateID(STATE_INDEX));
		m_CurrentState.m_IndexData = NVIndexData(pIndexData, BaseVertexIndex);
	}

#ifdef _DEBUG
	void Debug_DumpFlushStates(DWORD DebugLevel)
	{
		tStateID::const_iterator itrDirtyStates = m_DirtyStates.begin();
		while (itrDirtyStates != m_DirtyStates.end())
		{
			const NVStateID& ThisState = *itrDirtyStates;
			switch(ThisState.m_Type)
			{
				case STATE_RENDER:
					DISPDBG(DebugLevel, "RENDERSTATE:" << ThisState.m_RenderStateType);
					break;
				case STATE_TEXTURE:
					DISPDBG(DebugLevel, "TEXTURE:" << ThisState.m_dwValue0);
					break;
				case STATE_TEXTURESTAGE:
					DISPDBG(DebugLevel, "TEXTURESTAGESTATE:" << ThisState.m_dwStage << "," << ThisState.m_TextureStageStateType);
					break;
				case STATE_VSHADER:
					DISPDBG(DebugLevel, "VERTEXSHADER:" << ThisState.m_dwValue0);
					break;
				case STATE_PSHADER:
					DISPDBG(DebugLevel, "PIXELSHADER:" << ThisState.m_dwValue0);
					break;
				case STATE_TRANSFORM:
					DISPDBG(DebugLevel, "TRANSFORM:" << ThisState.m_TransformStateType);
					break;
				case STATE_VCONSTANT:
					DISPDBG(DebugLevel, "VERTEXSHADERCONSTANT:" << ThisState.m_dwValue0);
					break;
				case STATE_PCONSTANT:
					DISPDBG(DebugLevel, "PIXELSHADERCONSTANT:" << ThisState.m_dwValue0);
					break;
				case STATE_STREAM:
					DISPDBG(DebugLevel, "STREAM:" << ThisState.m_dwValue0);
					break;
				case STATE_INDEX:
					DISPDBG(DebugLevel, "INDEX");
					break;
			}
			
			itrDirtyStates++;
		}
	}
#define DEBUG_DUMPFLUSHSTATES(a) Debug_DumpFlushStates(a)
#else
#define DEBUG_DUMPFLUSHSTATES(a)
#endif

	void FlushVertexShader()
	{
		CHECK_D3DAPI(m_pD3DDev->SetVertexShader(m_CurrentState.m_dwVertexShader));
		m_ChipState.m_dwVertexShader = m_CurrentState.m_dwVertexShader;
	}

	void FlushPixelShader()
	{
		CHECK_D3DAPI(m_pD3DDev->SetPixelShader(m_CurrentState.m_dwPixelShader));
		m_ChipState.m_dwPixelShader = m_CurrentState.m_dwPixelShader;
	}

	void FlushState()
	{
		DWORD dwWrittenStates = 0;

		DISPDBG(10, "Flushing " << m_DirtyStates.size() << " States");
		
		DEBUG_DUMPFLUSHSTATES(10);

		tStateID::const_iterator itrDirtyStates = m_DirtyStates.begin();
		while (itrDirtyStates != m_DirtyStates.end())
		{
			const NVStateID& ThisState = *itrDirtyStates;
			switch(ThisState.m_Type)
			{
				case STATE_MATERIAL:
				{
					dwWrittenStates++;
					m_ChipState.m_D3DMaterial = m_CurrentState.m_D3DMaterial;
					CHECK_D3DAPI(m_pD3DDev->SetMaterial(&m_CurrentState.m_D3DMaterial));
				}
				break;

				case STATE_RENDER:
				{
					if (m_bForce || (m_ChipState.m_RenderStates[ThisState.m_RenderStateType] != m_CurrentState.m_RenderStates[ThisState.m_RenderStateType]))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetRenderState(ThisState.m_RenderStateType, m_CurrentState.m_RenderStates[ThisState.m_RenderStateType]));
						m_ChipState.m_RenderStates[ThisState.m_RenderStateType] = m_CurrentState.m_RenderStates[ThisState.m_RenderStateType];
					}
				}
				break;

				case STATE_TEXTURE:
				{
					if (m_bForce || (m_ChipState.m_Textures[ThisState.m_dwValue0] != m_CurrentState.m_Textures[ThisState.m_dwValue0]))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetTexture(ThisState.m_dwValue0, m_CurrentState.m_Textures[ThisState.m_dwValue0]));
						m_ChipState.m_Textures[ThisState.m_dwValue0] = m_CurrentState.m_Textures[ThisState.m_dwValue0];
					}
				}
				break;

				case STATE_TEXTURESTAGE:
				{
					if (m_bForce || (m_ChipState.m_TextureStates[ThisState.m_dwStage][ThisState.m_TextureStageStateType] != m_CurrentState.m_TextureStates[ThisState.m_dwStage][ThisState.m_TextureStageStateType]))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetTextureStageState(ThisState.m_dwStage, ThisState.m_TextureStageStateType, m_CurrentState.m_TextureStates[ThisState.m_dwStage][ThisState.m_TextureStageStateType]));
						m_ChipState.m_TextureStates[ThisState.m_dwStage][ThisState.m_TextureStageStateType] = m_CurrentState.m_TextureStates[ThisState.m_dwStage][ThisState.m_TextureStageStateType];
					}
				}
				break;

				case STATE_VSHADER:
				{
					if (m_bForce || (m_ChipState.m_dwVertexShader != m_CurrentState.m_dwVertexShader))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetVertexShader(m_CurrentState.m_dwVertexShader));
						m_ChipState.m_dwVertexShader = m_CurrentState.m_dwVertexShader;
					}
				}
				break;

				case STATE_PSHADER:
				{
					if (m_bForce || (m_ChipState.m_dwPixelShader != m_CurrentState.m_dwPixelShader))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetPixelShader(m_CurrentState.m_dwPixelShader));
						m_ChipState.m_dwPixelShader = m_CurrentState.m_dwPixelShader;
					}
				}
				break;

				// Always set this one
				case STATE_TRANSFORM:
				{
					dwWrittenStates++;
					CHECK_D3DAPI(m_pD3DDev->SetTransform(ThisState.m_TransformStateType, &m_CurrentState.m_Matrices[ThisState.m_TransformStateType]));
					m_ChipState.m_Matrices[ThisState.m_TransformStateType] = m_CurrentState.m_Matrices[ThisState.m_TransformStateType];
				}
				break;
					
				case STATE_VCONSTANT:
				{
					if (m_bForce || (m_ChipState.m_VertexShaderConstants[ThisState.m_dwValue0] != m_CurrentState.m_VertexShaderConstants[ThisState.m_dwValue0]))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetVertexShaderConstant(ThisState.m_dwValue0, m_CurrentState.m_VertexShaderConstants[ThisState.m_dwValue0], 1));
						m_ChipState.m_VertexShaderConstants[ThisState.m_dwValue0] = m_CurrentState.m_VertexShaderConstants[ThisState.m_dwValue0];
					}
				}
				break;

				case STATE_PCONSTANT:
				{
					if (m_bForce || (m_ChipState.m_PixelShaderConstants[ThisState.m_dwValue0] != m_CurrentState.m_PixelShaderConstants[ThisState.m_dwValue0]))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetPixelShaderConstant(ThisState.m_dwValue0, m_CurrentState.m_PixelShaderConstants[ThisState.m_dwValue0], 1));
						m_ChipState.m_PixelShaderConstants[ThisState.m_dwValue0] = m_CurrentState.m_PixelShaderConstants[ThisState.m_dwValue0];
					}
				}
				break;
				
				case STATE_STREAM:
				{
					if (m_bForce || (!(m_ChipState.m_StreamData[ThisState.m_dwValue0] == m_CurrentState.m_StreamData[ThisState.m_dwValue0])))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetStreamSource(ThisState.m_dwValue0, m_CurrentState.m_StreamData[ThisState.m_dwValue0].m_pStreamData, m_CurrentState.m_StreamData[ThisState.m_dwValue0].m_Stride));
						m_ChipState.m_StreamData[ThisState.m_dwValue0] = m_CurrentState.m_StreamData[ThisState.m_dwValue0];
					}
				}
				break;

				case STATE_INDEX:
				{
					if (m_bForce || (!(m_ChipState.m_IndexData == m_CurrentState.m_IndexData)))
					{
						dwWrittenStates++;
						CHECK_D3DAPI(m_pD3DDev->SetIndices(m_CurrentState.m_IndexData.m_pIndexData, m_CurrentState.m_IndexData.m_BaseVertexIndex));
						m_ChipState.m_IndexData = m_CurrentState.m_IndexData;
					}
				}
				break;
			}
			itrDirtyStates++;
		}

		m_DirtyStates.clear();
		DISPDBG(10, "..." << dwWrittenStates << " Actually sent");

	}

    HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
	{
		FlushState();
		return (m_pD3DDev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount));
	}

	HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount)
	{
		FlushState();
		return (m_pD3DDev->DrawIndexedPrimitive(PrimitiveType, minIndex, NumVertices, startIndex, primCount));
	}


private:
	NVStateManagerState m_ChipState;
	NVStateManagerState m_CurrentState;
	NVStateManagerState m_CopyState;
	tStateID m_DirtyStates;
	bool m_bForce;

	LPDIRECT3DDEVICE8 m_pD3DDev;

};

#define NVSTATEMANAGER (NVStateManager::GetSingleton())


}; //nv_objects

#endif __NVSTATEMANAGER_H