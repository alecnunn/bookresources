/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVMATERIALRESOURCE_H
#define __NVMATERIALRESOURCE_H

#include "nvresourcemanager.h"
#include "nvstatemanager.h"
#include "nvtextureresource.h"

namespace nv_objects
{

static const DWORD NV_MAX_TEXTURES = 8;
static const DWORD NV_MAX_INDICES_IN_BUFFER = 0xFFFF;
// A class which holds a constant.
// A constant is a vec4 and a location in memory
static const DWORD NV_NO_CONSTANT = (DWORD)-1;
class NVConstant
{
public:
	NVConstant(const D3DXVECTOR4& Constant = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), DWORD dwLocation = NV_NO_CONSTANT)
		: m_Constant(Constant),
		m_dwLocation(dwLocation)
	{}

	void SetLocation(DWORD dwLocation) { m_dwLocation = dwLocation; }
	void SetConstant(const D3DXVECTOR4& Const) { m_Constant = Const; }
	void SetConstant(const D3DXVECTOR3& Const) { m_Constant.x = Const.x; m_Constant.y = Const.y; m_Constant.z = Const.z; }
	DWORD GetLocation() const { return m_dwLocation; }
	const D3DXVECTOR4& GetConstant() const { return m_Constant; }

	D3DXVECTOR4 m_Constant;
	DWORD m_dwLocation;
};
typedef std::map<const std::string, NVConstant> tConstantMap;

// Texture stage states for each stage
class NVTextureStageState
{
public:
	NVTextureStageState(DWORD dwValue)
		: m_dwValue(dwValue)
	{}

	DWORD GetValue() const { return m_dwValue; }
	void SetValue(DWORD dwValue) { m_dwValue = dwValue; }
	
	DWORD m_dwValue;
};
typedef std::map<D3DTEXTURESTAGESTATETYPE, NVTextureStageState> tTextureStageStateMap;

// Renderstates for each stage
class NVRenderState
{
public:
	NVRenderState(DWORD dwValue)
		: m_dwValue(dwValue)
	{}

	DWORD GetValue() const { return m_dwValue; }
	void SetValue(DWORD dwValue) { m_dwValue = dwValue; }
	
	DWORD m_dwValue;
};
typedef std::map<D3DRENDERSTATETYPE, NVRenderState> tRenderStateMap;

// Data for a material.  Should try to maintain no more than 1 copy
// of each material type.
// Constains a list of named constants which will be loaded in the constant store when this material
// is activated
static const DWORD NV_INVALID_STATE = (DWORD)-1;
class NVMaterialResource : public NVBaseResource
{
public:
	NVMaterialResource()
		: NVBaseResource(NVRT_MATERIAL),
		m_dwVertexShader(0),
		m_dwPixelShader(0),
		m_bD3DMaterial(false)
	{
		for (DWORD i = 0; i < NV_MAX_TEXTURES; i++)
		{
			m_TextureResourceID[i] = NVINVALID_RESOURCEID;
			m_TextureResourceOriginalID[i] = NVINVALID_RESOURCEID;
		}
	}

	virtual ~NVMaterialResource()
	{
		DWORD i;

		// Restore all the textures
		for (i = 0; i < NV_MAX_TEXTURES; i++)
		{
			RestoreTexture(i);
		}

		m_Constants.clear();
	}

	virtual bool IsDisposed() const { return false; }
	virtual bool Dispose() { return false; }
	virtual bool Invoke() { return true; }
	virtual bool IsDeviceObject() const { return false; }

	static NVMaterialResource* GetMaterialResource(NVResourceID MaterialID)
	{
		return static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource(MaterialID));
	}
	
	// Add a constant to this material
	NVConstant* AddConstant(const std::string& strName, const D3DXVECTOR4& Constant = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), DWORD dwLocation = NV_NO_CONSTANT)
	{
		NVConstant* pConstant = FindConstant(strName);
		if (pConstant)
		{
			pConstant->SetConstant(Constant);
			pConstant->SetLocation(dwLocation);
			return pConstant;
		}

		std::pair<tConstantMap::iterator, bool> Ret = m_Constants.insert(tConstantMap::value_type(strName, NVConstant(Constant, dwLocation)));
		if (Ret.second == true)
			return &((Ret.first)->second);
		return NULL;

	}

	// Find a constant in this material
	NVConstant* FindConstant(const std::string& strName)
	{
		tConstantMap::iterator itrConstant = m_Constants.find(strName);
		if (itrConstant != m_Constants.end())
			return &itrConstant->second;

		return NULL;
	}

	// Add a texturestagestate to this material
	NVTextureStageState* AddTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue)
	{
		NVTextureStageState* pState = FindTextureStageState(dwStage, Type);
		if (pState)
		{
			pState->SetValue(dwValue);
			return pState;
		}

		std::pair<tTextureStageStateMap::iterator, bool> Ret = m_TextureStageStates[dwStage].insert(tTextureStageStateMap::value_type(Type, NVTextureStageState(dwValue)));
		if (Ret.second == true)
			return &((Ret.first)->second);
		return NULL;
	}

	// Find a texturestagestate in this material
	NVTextureStageState* FindTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type)
	{
		tTextureStageStateMap::iterator itrStates = m_TextureStageStates[dwStage].find(Type);
		if (itrStates != m_TextureStageStates[dwStage].end())
			return &itrStates->second;

		return NULL;
	}

	// Add a renderstate to this material
	NVRenderState* AddRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue)
	{
		NVRenderState* pState = FindRenderState(Type);
		if (pState)
		{
			pState->SetValue(dwValue);
			return pState;
		}

		std::pair<tRenderStateMap::iterator, bool> Ret = m_RenderStates.insert(tRenderStateMap::value_type(Type, NVRenderState(dwValue)));
		if (Ret.second == true)
			return &((Ret.first)->second);
		return NULL;
	}

	// Find a texturestagestate in this material
	NVRenderState* FindRenderState(D3DRENDERSTATETYPE Type)
	{
		tRenderStateMap::iterator itrStates = m_RenderStates.find(Type);
		if (itrStates != m_RenderStates.end())
			return &itrStates->second;

		return NULL;
	}

	// Add a renderstate to this material
	D3DMATERIAL8* AddD3DMaterial(const D3DMATERIAL8* pMaterial)
	{
		m_bD3DMaterial = true;
		m_D3DMaterial = *pMaterial;
		return &m_D3DMaterial;
	}

	// Find a texturestagestate in this material
	D3DMATERIAL8* FindD3DMaterial()
	{
		if (m_bD3DMaterial)
			return &m_D3DMaterial;

		return NULL;
	}

	void AddOriginalTexture(DWORD dwTexNum, NVResourceID TextureID)
	{
		m_TextureResourceOriginalID[dwTexNum] = TextureID;
		AddTexture(dwTexNum, TextureID);
	}

	void AddTexture(DWORD dwTexNum, NVResourceID TextureID) 
	{
		// Add this new one to the material
		m_TextureResourceID[dwTexNum] = TextureID; 
	}

	const NVResourceID GetTextureID(DWORD dwTexNum) 
	{
		return m_TextureResourceID[dwTexNum]; 
	}

	void RestoreTexture(DWORD dwTexNum) 
	{
		AddTexture(dwTexNum, m_TextureResourceOriginalID[dwTexNum]);
	}
	
	void AddVertexShader(DWORD dwVertexShader) { m_dwVertexShader = dwVertexShader; }
	void AddPixelShader(DWORD dwPixelShader) { m_dwPixelShader = dwPixelShader; }

	DWORD GetVertexShader() const { return m_dwVertexShader; }
	DWORD GetPixelShader() const { return m_dwPixelShader;	}
	
	// Function to apply this material to the device.  Will save away state for later
	bool SaveMaterial()
	{
		DWORD i;
		
		// Setup Material constants
		tConstantMap::const_iterator itrConstants = m_Constants.begin();
		while (itrConstants != m_Constants.end())
		{
			const NVConstant& Constant = itrConstants->second;
			if (Constant.GetLocation() != NV_NO_CONSTANT)
			{
				NVSTATEMANAGER.SaveVertexShaderConstant(Constant.GetLocation(), &Constant.GetConstant(), 1);
			}

			itrConstants++;
		};

		// Setup Textures
		for (i = 0; i < NV_MAX_TEXTURES; i++)
		{
			if (m_TextureResourceID[i] != NVINVALID_RESOURCEID)
			{
				LPDIRECT3DBASETEXTURE8 pTexture = NVTextureResource::GetTexture(m_TextureResourceID[i]);
				if (pTexture)
				{
					NVSTATEMANAGER.SaveTexture(i, pTexture);
				}
				else
				{
					NVSTATEMANAGER.SaveTexture(i, NULL);
				}
			}
			else
			{
				NVSTATEMANAGER.SaveTexture(i, NULL);
			}

			// Set the texture stage states
			tTextureStageStateMap::const_iterator itrStageStates = m_TextureStageStates[i].begin();
			while (itrStageStates != m_TextureStageStates[i].end())
			{
				const NVTextureStageState& State = itrStageStates->second;
				NVSTATEMANAGER.SaveTextureStageState(i, itrStageStates->first, State.GetValue());
				
				itrStageStates++;
			};

		}

		// Set the render states
		tRenderStateMap::const_iterator itrRenderStates = m_RenderStates.begin();
		while (itrRenderStates != m_RenderStates.end())
		{
			const NVRenderState& State = itrRenderStates->second;
			D3DRENDERSTATETYPE Type = itrRenderStates->first;
			NVSTATEMANAGER.SaveRenderState(Type, State.GetValue());
			
			itrRenderStates++;
		};

		if (m_bD3DMaterial)
		{
			NVSTATEMANAGER.SaveMaterial(&m_D3DMaterial);
		}

		// Setup Shaders
		NVASSERT(GetVertexShader(), "No vertex shader");
		NVSTATEMANAGER.SaveVertexShader(GetVertexShader());
		NVSTATEMANAGER.SavePixelShader(GetPixelShader());

		return true;
	}

	// Function to restore the previous material
	bool RestoreMaterial()
	{
		DWORD i;

		// Restore renderstates
		tConstantMap::const_iterator itrConstants = m_Constants.begin();
		while (itrConstants != m_Constants.end())
		{
			const NVConstant& Constant = itrConstants->second;
			if (Constant.GetLocation() != NV_NO_CONSTANT)
			{
				NVSTATEMANAGER.RestoreVertexShaderConstant(Constant.GetLocation(), 1);
			}

			itrConstants++;
		};

		// Restore textures
		for (i = 0; i < NV_MAX_TEXTURES; i++)
		{
			NVSTATEMANAGER.RestoreTexture(i);

			// Set the texture stage states
			tTextureStageStateMap::const_iterator itrStageStates = m_TextureStageStates[i].begin();
			while (itrStageStates != m_TextureStageStates[i].end())
			{
				const NVTextureStageState& State = itrStageStates->second;
				NVSTATEMANAGER.RestoreTextureStageState(i, itrStageStates->first);
				
				itrStageStates++;
			};

		}

		// Restore renderstates
		tRenderStateMap::const_iterator itrRenderStates = m_RenderStates.begin();
		while (itrRenderStates != m_RenderStates.end())
		{
			const NVRenderState& State = itrRenderStates->second;
			D3DRENDERSTATETYPE Type = itrRenderStates->first;
			NVSTATEMANAGER.RestoreRenderState(Type);
			
			itrRenderStates++;
		};

		if (m_bD3DMaterial)
		{
			NVSTATEMANAGER.RestoreMaterial();
		}

		// Restore shaders
		NVSTATEMANAGER.RestoreVertexShader();
		NVSTATEMANAGER.RestorePixelShader();

		return true;
	}

	// Material properties
	tConstantMap m_Constants;
	tTextureStageStateMap m_TextureStageStates[NV_MAX_TEXTURES];
	tRenderStateMap m_RenderStates;
	D3DMATERIAL8 m_D3DMaterial;
	bool m_bD3DMaterial;

	DWORD m_dwVertexShader;
	DWORD m_dwPixelShader;
	
	NVResourceID m_TextureResourceID[NV_MAX_TEXTURES];
	NVResourceID m_TextureResourceOriginalID[NV_MAX_TEXTURES];
};

}; // namespace nv_objects;

#endif // __NVMATERIALRESOURCE_H

