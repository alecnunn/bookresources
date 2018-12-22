/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
		 A Value class used by the renderstyle resources and the material resource.
		 Facilitates easy links between the two.

		 At the moment the NVValue is responsible for the 'links' to the next nvvalue.  
		 I'll be changing that when I have more time to enable more complex operations.
		 The idea will be to have an NVValueCalculator which can take an abitrary number
		 of arguments/operations and perform them as needed.

  - cmaughan@nvidia.com

      
        
******************************************************************************/
#ifndef __NVVALUE_H
#define __NVVALUE_H

namespace nv_objects
{

// Simple reoutines to do vector modulation - not present in the basic operators
__inline D3DXVECTOR4 VectorModulate(const D3DXVECTOR4& Vec1, const D3DXVECTOR4& Vec2)
{
	return D3DXVECTOR4(Vec1.x * Vec2.x, Vec1.y * Vec2.y, Vec1.z * Vec2.z, Vec1.w * Vec1.w);
}

__inline D3DXVECTOR3 VectorModulate(const D3DXVECTOR3& Vec1, const D3DXVECTOR3& Vec2)
{
	return D3DXVECTOR3(Vec1.x * Vec2.x, Vec1.y * Vec2.y, Vec1.z * Vec2.z);
}

__inline D3DXVECTOR2 VectorModulate(const D3DXVECTOR2& Vec1, const D3DXVECTOR2& Vec2)
{
	return D3DXVECTOR2(Vec1.x * Vec2.x, Vec1.y * Vec2.y);
}

typedef enum tagNVValueType
{
	NVV_UNKNOWN,
	NVV_VECTOR4,
	NVV_VECTOR3,
	NVV_VECTOR2,
	NVV_DWORD,
	NVV_FLOAT,
	NVV_RESOURCE,
	NVV_D3DMATERIAL
} NVValueType;

typedef enum tagNVValueLinkAction
{
	NVVL_DEFAULT,			// Use the current value
	NVVL_SELECT_MATERIAL,	// Select the material value
	NVVL_MULTIPLY_MATERIAL,	// Multiply the currect value by the target value
	NVVL_ADD_MATERIAL		// Add the current value to the target value
} NVValueLinkAction;


class NVMaterialResource;

class NVValue
{
public:
	~NVValue()
	{};

	NVValue()
		: m_Type(NVV_UNKNOWN),
		m_Action(NVVL_DEFAULT),
		m_strValueNameLink("")
	{}

	NVValue(const NVValue& rhs)
	{
		*this = rhs;
	}

	// User parameterized type
	NVValue(NVValueType Type, const void* pValue = NULL, NVValueLinkAction Action = NVVL_DEFAULT, const char* pszValueNameLink = NULL)
		: m_Type(Type),
		m_Action(Action)
	{
		if (pszValueNameLink)
			m_strValueNameLink = pszValueNameLink;

		switch(m_Type)
		{
			case NVV_UNKNOWN:
				break;
			case NVV_VECTOR3:
				if (pValue)
					m_Vector3 = *(D3DXVECTOR3*)pValue;
				else
					m_Vector3 = D3DXVECTOR3();
				break;
			case NVV_VECTOR4:
				if (pValue)
					m_Vector4 = *(D3DXVECTOR4*)pValue;
				else
					m_Vector4 = D3DXVECTOR4();
				break;
			case NVV_VECTOR2:
				if (pValue)
					m_Vector2 = *(D3DXVECTOR2*)pValue;
				else
					m_Vector2 = D3DXVECTOR2();
				break;
			case NVV_DWORD:
				if (pValue)
					m_dwValue = *(DWORD*)pValue;
				else
					m_dwValue = 0;
				break;
			case NVV_D3DMATERIAL:
				if (pValue)
				{
					memcpy(&m_D3DMaterial, pValue, sizeof (D3DMATERIAL8));
				}
				else
				{
					ZeroMemory(&m_D3DMaterial, sizeof(D3DMATERIAL8));
				}
				break;
			case NVV_RESOURCE:
				if (pValue)
					m_Resource = *(NVResourceRef*)pValue;
				else
					m_Resource = NVResourceRef(NULL);
				break;
			case NVV_FLOAT:
				if (pValue)
					m_fValue = *(float*)pValue;
				else
					m_fValue = 0.0f;
				break;
		}

	};

	// Functions to get values back
	DWORD GetDWORD() const
	{
		NVValue NewValue = ApplyLink();
		NVASSERT(NewValue.GetType() == NVV_DWORD, "ERROR: Type is not a DWORD");
		return NewValue.m_dwValue;
	}

	NVResourceRef GetRESOURCE() const
	{
		NVValue NewValue = ApplyLink();
		NVASSERT(NewValue.GetType() == NVV_RESOURCE, "ERROR: Type is not a resource");
		return NewValue.m_Resource;
	}

	D3DXVECTOR4 GetVECTOR4() const
	{
		NVValue NewValue = ApplyLink();
		switch(NewValue.m_Type)
		{
			case NVV_VECTOR3:
				return D3DXVECTOR4(NewValue.m_Vector3.x, NewValue.m_Vector3.y, NewValue.m_Vector3.z, 1.0f);
				break;
			case NVV_VECTOR2:
				return D3DXVECTOR4(NewValue.m_Vector2.x, NewValue.m_Vector2.y, 1.0f, 1.0f);
				break;
			case NVV_VECTOR4:
				return NewValue.m_Vector4;
				break;
			default:
				NVASSERT(0, "Not Correct Value Type!");
				break;
		}
		return D3DXVECTOR4();
	}

	D3DMATERIAL8 GetD3DMATERIAL() const
	{
		NVValue NewValue = ApplyLink();
		NVASSERT(NewValue.GetType() == NVV_D3DMATERIAL, "ERROR: Type is not a material");
		return NewValue.m_D3DMaterial;
	}

	// Applies this value's link and returns a new value
	NVValue ApplyLink() const;

	// Operators to define interactions with other NVValue's
	NVValue operator* (const NVValue& rhs) const;
	NVValue operator+ (const NVValue& rhs) const;

	const NVValueType GetType() const { return m_Type; }

	// The destination material is used to find link targets
	static void SetMaterialDestination(NVMaterialResource* pMaterial) 
	{
		m_pMaterialDestination = pMaterial;
	}
 
	static NVMaterialResource* GetMaterialDestination() 
	{
		return m_pMaterialDestination; 
	}


private:
	static NVMaterialResource* m_pMaterialDestination;

	static NVResourceRef m_CurrentMaterial;
	NVValueType m_Type;
	NVValueLinkAction m_Action;
	std::string m_strValueNameLink;
	D3DXVECTOR4 m_Vector4;
	D3DXVECTOR3 m_Vector3;
	D3DXVECTOR2 m_Vector2;
	NVResourceRef m_Resource;
	union
	{
		D3DMATERIAL8 m_D3DMaterial;
		DWORD m_dwValue;
		float m_fValue;
	};
};
typedef std::map<const std::string, NVValue> tNVValueMap;


}; // namespace nv_objects;

#endif // __NVVALUE_H

