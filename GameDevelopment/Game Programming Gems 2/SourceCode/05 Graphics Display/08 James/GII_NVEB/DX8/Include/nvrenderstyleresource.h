/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVRENDERSTYLERESOURCE_H
#define __NVRENDERSTYLERESOURCE_H

#include "nvresourcemanager.h"
#include "nvstatemanager.h"
#include "nvtextureresource.h"
#include "nvmaterialresource.h"
#include "nvvalue.h"
#include "nvlink.h"

namespace nv_objects
{

static const DWORD NV_MAX_TEXTURES = 8;
static const DWORD NV_MAX_INDICES_IN_BUFFER = 0xFFFF;

class NVLinkConstantSlot
{
public:
	NVLinkConstantSlot(NVLINK_CONSTANT_ID ConstantID, LONG Offset)
		: m_ConstantID(ConstantID),
		m_Offset(Offset)
	{}

	NVLINK_CONSTANT_ID GetID() const { return m_ConstantID; }
	void SetID(NVLINK_CONSTANT_ID ConstantID) { m_ConstantID = ConstantID; }

	LONG GetOffset() const { return m_Offset; }
	void SetOffset(LONG Offset) { m_Offset = Offset; }

	bool operator < (const NVLinkConstantSlot& rhs) const
	{
		if (m_ConstantID < rhs.m_ConstantID)
			return true;
		else if (m_ConstantID > rhs.m_ConstantID)
			return false;
		else if (m_Offset < rhs.m_Offset)
		 return true;

		return false;
	}

private:
	NVLINK_CONSTANT_ID m_ConstantID;
	LONG m_Offset;
};
typedef std::map<NVLinkConstantSlot, DWORD> tNVLinkConstantSlotMap;
typedef std::map<DWORD, NVValue> tNVConstantMap;
typedef std::map<D3DTEXTURESTAGESTATETYPE, NVValue> tNVTextureStageStateMap;
typedef std::map<D3DRENDERSTATETYPE, NVValue> tNVRenderStateMap;

class NVRenderStylePass
{ 
public:
	NVRenderStylePass()
		: m_dwVertexShader(0),
		m_dwPixelShader(0),
		m_bMaterialValid(false),
		m_D3DMaterial(NVV_D3DMATERIAL)
	{
		for (DWORD i = 0; i < NV_MAX_TEXTURES; i++)
		{
			m_Textures[i] = NVValue(NVV_RESOURCE, NULL);
		}
	};

	virtual ~NVRenderStylePass()
	{
		Clear();
	}

	void Clear()
	{
		for (DWORD i = 0; i < NV_MAX_TEXTURES; i++)
		{
			m_Textures[i] = NVValue(NVV_RESOURCE, NULL);
			m_TextureStageStates[i].clear();
		}
		m_Constants.clear();
		m_RenderStates.clear();
		m_dwVertexShader = 0;
		m_dwPixelShader = 0;
	}

	void SetConstantSlot(NVLINK_CONSTANT_ID ConstantID, LONG lOffset, DWORD dwLocation)
	{
		m_ConstantSlots[NVLinkConstantSlot(ConstantID, lOffset)] = dwLocation;
	}

	DWORD GetConstantSlot(NVLINK_CONSTANT_ID ConstantID, LONG lOffset) const
	{
		tNVLinkConstantSlotMap::const_iterator itrConstantSlots = m_ConstantSlots.find(NVLinkConstantSlot(ConstantID, lOffset));
		
		NVASSERT(itrConstantSlots != m_ConstantSlots.end(), "ERROR: Could not find constant slot!");
		return itrConstantSlots->second;
	}

	// Set from an NVLink constant ID
	void SetConstant(NVLINK_CONSTANT_ID ConstantID, LONG lOffset, const NVValue& Value)
	{

		// Make sure the NVValue is a vector.
		NVASSERT((Value.GetType() == NVV_VECTOR2) || (Value.GetType() == NVV_VECTOR3) || (Value.GetType() == NVV_VECTOR4), 
			"ERROR: Constant must be of type vector!");

		// Should always find it wether or not it has a slot.
		DWORD dwLocation = GetConstantSlot(ConstantID, lOffset);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_Constants[dwLocation] = Value;
		}
	}

	void SetConstant(DWORD dwLocation, const NVValue& Value)
	{

		// Make sure the NVValue is a vector.
		NVASSERT((Value.GetType() == NVV_VECTOR2) || (Value.GetType() == NVV_VECTOR3) || (Value.GetType() == NVV_VECTOR4), 
			"ERROR: Constant must be of type vector!");

		m_Constants[dwLocation] = Value;
	}

	void SetMaterial(const NVValue& D3DMaterial)
	{
		m_D3DMaterial = D3DMaterial;
		m_bMaterialValid = true;
	}

	// Methods for setting states
	void SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, const NVValue& Value)
	{
		m_TextureStageStates[dwStage][Type] = Value;
	}

	void SetRenderState(D3DRENDERSTATETYPE Type, const NVValue& Value)
	{
		m_RenderStates[Type] = Value;
	}

	void SetTexture(DWORD dwTexNum, const NVValue& Value) 
	{
		m_Textures[dwTexNum] = Value;
	}

	// Methods for states not set from NVValues
	void SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue)
	{
		m_TextureStageStates[dwStage][Type] = NVValue(NVV_DWORD, &dwValue);
	}

	void SetRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue)
	{
		m_RenderStates[Type] = NVValue(NVV_DWORD, &dwValue);
	}

	void SetTexture(DWORD dwTexNum, DWORD dwValue) 
	{
			m_Textures[dwTexNum] = NVValue(NVV_DWORD, &dwValue);
	}

	const NVResourceRef GetTexture(DWORD dwTexNum) 
	{
		return m_Textures[dwTexNum].GetRESOURCE(); 
	}

	void SetVertexShader(DWORD dwVertexShader) { m_dwVertexShader = dwVertexShader; }
	void SetPixelShader(DWORD dwPixelShader) { m_dwPixelShader = dwPixelShader; }

	DWORD GetVertexShader() const { return m_dwVertexShader; }
	DWORD GetPixelShader() const { return m_dwPixelShader;	}
	
	// Function to apply this renderstyle to the device.  Will save away state for later
	bool ApplyRenderStyle(NVResourceRef Material)
	{
		DWORD i;

		// Tell the values which material is the current destination.
		// This is the cleanest way I can think of for now.
		NVMaterialResource* pMaterial = NVMaterialResource::GetMaterialResourcePtr(Material.GetPointer());
		NVASSERT(pMaterial, "ERROR: Invalid material!");
		NVValue::SetMaterialDestination(pMaterial);
		
		// Setup Material constants
		tNVConstantMap::const_iterator itrConstants = m_Constants.begin();
		while (itrConstants != m_Constants.end())
		{
			NVSTATEMANAGER.SaveVertexShaderConstant(itrConstants->first, &itrConstants->second.GetVECTOR4(), 1);
			itrConstants++;
		};

		if (m_bMaterialValid)
		{
			NVSTATEMANAGER.SetMaterial(&m_D3DMaterial.GetD3DMATERIAL());
		}

		// Setup Textures
		for (i = 0; i < NV_MAX_TEXTURES; i++)
		{
			if (m_Textures[i].GetRESOURCE() != NULL)
			{
				LPDIRECT3DBASETEXTURE8 pTexture = NVTextureResource::GetTexture(m_Textures[i].GetRESOURCE());
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
			tNVTextureStageStateMap::const_iterator itrStageStates = m_TextureStageStates[i].begin();
			while (itrStageStates != m_TextureStageStates[i].end())
			{
				const NVValue& State = itrStageStates->second;
				NVSTATEMANAGER.SaveTextureStageState(i, itrStageStates->first, State.GetDWORD());
				
				itrStageStates++;
			};

		}

		// Set the render states
		tNVRenderStateMap::const_iterator itrRenderStates = m_RenderStates.begin();
		while (itrRenderStates != m_RenderStates.end())
		{
			const NVValue& State = itrRenderStates->second;
			D3DRENDERSTATETYPE Type = itrRenderStates->first;
			NVSTATEMANAGER.SaveRenderState(Type, State.GetDWORD());
			
			itrRenderStates++;
		};

		// Setup Shaders
		NVASSERT(GetVertexShader(), "No vertex shader");
		NVSTATEMANAGER.SaveVertexShader(GetVertexShader());
		NVSTATEMANAGER.SavePixelShader(GetPixelShader());

		return true;
	}

	// Function to restore the previous renderstyle
	bool RestoreRenderStyle()
	{
		DWORD i;

		// Restore renderstates
		tNVConstantMap::const_iterator itrConstants = m_Constants.begin();
		while (itrConstants != m_Constants.end())
		{
			NVSTATEMANAGER.RestoreVertexShaderConstant(itrConstants->first, 1);
			itrConstants++;
		};

		// Restore textures
		for (i = 0; i < NV_MAX_TEXTURES; i++)
		{
			NVSTATEMANAGER.RestoreTexture(i);

			// Set the texture stage states
			tNVTextureStageStateMap::const_iterator itrStageStates = m_TextureStageStates[i].begin();
			while (itrStageStates != m_TextureStageStates[i].end())
			{
				NVSTATEMANAGER.RestoreTextureStageState(i, itrStageStates->first);
				itrStageStates++;
			};

		}

		// Restore renderstates
		tNVRenderStateMap::const_iterator itrRenderStates = m_RenderStates.begin();
		while (itrRenderStates != m_RenderStates.end())
		{
			NVSTATEMANAGER.RestoreRenderState(itrRenderStates->first);
			itrRenderStates++;
		};

		// Restore shaders
		NVSTATEMANAGER.RestoreVertexShader();
		NVSTATEMANAGER.RestorePixelShader();

		return true;
	}

private:
	// Material properties
	tNVLinkConstantSlotMap m_ConstantSlots;
	tNVConstantMap m_Constants;
	tNVTextureStageStateMap m_TextureStageStates[NV_MAX_TEXTURES];
	tNVRenderStateMap m_RenderStates;
	NVValue m_D3DMaterial;
	bool m_bMaterialValid;

	DWORD m_dwVertexShader;
	DWORD m_dwPixelShader;
	
	NVValue m_Textures[NV_MAX_TEXTURES];
};
typedef std::map<DWORD, NVRenderStylePass*> tPassMap;

// Data for a material.  Should try to maintain no more than 1 copy
// of each material type.
// Constains a list of named constants which will be loaded in the constant store when this material
// is activated
class NVRenderStyleResource : public NVBaseResource
{
public:
	NVRenderStyleResource()
		: NVBaseResource(NVRT_RENDERSTYLE)
	{
	}

	virtual ~NVRenderStyleResource()
	{
		Clear();
	}

	virtual bool IsDisposed() const { return false; }
	virtual bool Dispose() { return false; }
	virtual bool Invoke() { return true; }
	virtual bool IsDeviceObject() const { return false; }

	static NVRenderStyleResource* GetRenderStyleResourcePtr(NVResourceRef RenderStyle)
	{
		NVRenderStyleResource* pResource = static_cast<NVRenderStyleResource*>(RenderStyle.GetPointer());
		NVASSERT(pResource && pResource->GetResourceType() == NVRT_RENDERSTYLE, "Not a renderstyle resource");
		return pResource;
	}

	void Clear()
	{
		tPassMap::iterator itrPasses = m_Passes.begin();
		while (itrPasses != m_Passes.end())
		{
			SAFE_DELETE(itrPasses->second);
			itrPasses++;
		}	
		m_Passes.clear();
	}
		
 	NVRenderStylePass* AddPass(DWORD dwPass)
	{
		m_Passes[dwPass] = new NVRenderStylePass();
		return m_Passes[dwPass];
	}

	void RemovePass(DWORD dwPass)
	{
		tPassMap::iterator itrPass = m_Passes.find(dwPass);
		if (itrPass != m_Passes.end())
		{
			SAFE_DELETE(itrPass->second);
			m_Passes.erase(itrPass);
			return;
		}
	}

	NVRenderStylePass* GetPass(DWORD dwPass) 
	{
		tPassMap::iterator itrPass = m_Passes.find(dwPass);
		if (itrPass != m_Passes.end())
		{
			return itrPass->second;
		}
		return NULL;
	}

	DWORD GetNumPasses() const { return m_Passes.size(); }

private:
	tPassMap m_Passes;
};

}; // namespace nv_objects;

#endif // __NVRENDERSTYLERESOURCE_H

