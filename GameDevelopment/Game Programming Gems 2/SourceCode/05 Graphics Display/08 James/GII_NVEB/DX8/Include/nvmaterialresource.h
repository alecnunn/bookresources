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
#include "nvvalue.h"

namespace nv_objects
{

// Data for a material.  Should try to maintain no more than 1 copy
// of each material type.
// Constains a list of named constants which will be loaded in the constant store when this material
// is activated
class NVMaterialResource : public NVBaseResource
{
public:
	NVMaterialResource()
		: NVBaseResource(NVRT_MATERIAL)
	{
	}

	virtual ~NVMaterialResource()
	{
	}

	virtual bool IsDisposed() const { return false; }
	virtual bool Dispose() { return false; }
	virtual bool Invoke() { return true; }
	virtual bool IsDeviceObject() const { return false; }

	static NVMaterialResource* GetMaterialResourcePtr(NVResourceRef Material)
	{
		NVMaterialResource* pMatResource = static_cast<NVMaterialResource*>(Material.GetPointer());
		NVASSERT(pMatResource && pMatResource->GetResourceType() == NVRT_MATERIAL, "Not a material resource");
		return pMatResource;
	}

	// Add a constant to this material
	NVValue* AddValue(const std::string& strName, const NVValue& NewValue)
	{
		NVValue* pValue = FindValue(strName);
		if (pValue)
		{
			*pValue = NewValue;
			return pValue;
		}

		std::pair<tNVValueMap::iterator, bool> Ret = m_Values.insert(tNVValueMap::value_type(strName, NewValue));
		if (Ret.second == true)
			return &((Ret.first)->second);
		
		return NULL;
	}

	// Find a constant in this material
	NVValue* FindValue(const std::string& strName)
	{
		tNVValueMap::iterator itrValue = m_Values.find(strName);
		if (itrValue != m_Values.end())
			return &itrValue->second;

		return NULL;
	}

	static D3DMATERIAL8 GetD3DMaterial(NVMaterialResource* pMaterial) 
	{
		D3DMATERIAL8 Material;
		D3DXVECTOR4 Diffuse;
		D3DXVECTOR4 Specular;
		D3DXVECTOR4 Ambient;
		D3DXVECTOR4 Emissive;
		D3DXVECTOR4 Power;

		NVValue* pValue = pMaterial->FindValue("diffuse");
		if (pValue)
		{
			Diffuse = pValue->GetVECTOR4();
		}
		else
		{
			Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		pValue = pMaterial->FindValue("specular");
		if (pValue)
		{
			Specular = pValue->GetVECTOR4();
		}
		else
		{
			Specular = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		pValue = pMaterial->FindValue("ambient");
		if (pValue)
		{
			Ambient = pValue->GetVECTOR4();
		}
		else
		{
			Ambient = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		pValue = pMaterial->FindValue("power");
		if (pValue)
		{
			Power = pValue->GetVECTOR4();
		}
		else
		{
			Power = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		pValue = pMaterial->FindValue("emissive");
		if (pValue)
		{
			Emissive = pValue->GetVECTOR4();
		}
		else
		{
			Emissive = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		Material.Diffuse = *(D3DCOLORVALUE*)&Diffuse;
		Material.Specular = *(D3DCOLORVALUE*)&Specular;
		Material.Ambient = *(D3DCOLORVALUE*)&Ambient;
		Material.Emissive = *(D3DCOLORVALUE*)&Emissive;
		Material.Power = Power.x;

		return Material;
	}

private:
	// Material properties
	tNVValueMap m_Values;
};




}; // namespace nv_objects;

#endif // __NVMATERIALRESOURCE_H

