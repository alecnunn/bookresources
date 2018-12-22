/*********************************************************************NVMH2****
Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "nvmaterialresource.h"

namespace nv_objects
{

NVMaterialResource* NVValue::m_pMaterialDestination = NULL;

NVValue NVValue::operator* (const NVValue& rhs) const
{		
	switch(m_Type)
	{
		case NVV_VECTOR4:
		{
			D3DXVECTOR4 Result;
			switch(rhs.m_Type)
			{
				case NVV_VECTOR4:
					Result = VectorModulate(m_Vector4, rhs.m_Vector4);
					break;
				case NVV_VECTOR3:
					Result = VectorModulate(m_Vector4, D3DXVECTOR4(rhs.m_Vector3.x, rhs.m_Vector3.y, rhs.m_Vector3.z, 1.0f));
					break;
				case NVV_VECTOR2:
					Result = VectorModulate(m_Vector4, D3DXVECTOR4(rhs.m_Vector2.x, rhs.m_Vector2.y, 1.0f, 1.0f));
					break;
				case NVV_DWORD:
					Result = m_Vector4 * rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_Vector4 * rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_VECTOR4, &Result);
		}
		break;
		
		case NVV_VECTOR3:
		{
			D3DXVECTOR3 Result;
			switch(rhs.m_Type)
			{
				case NVV_VECTOR4:
					Result = VectorModulate(m_Vector3, D3DXVECTOR3(rhs.m_Vector4.x, rhs.m_Vector4.y, rhs.m_Vector4.z));
					break;
				case NVV_VECTOR3:
					Result = VectorModulate(m_Vector3, rhs.m_Vector3);
					break;
				case NVV_VECTOR2:
					Result = VectorModulate(m_Vector3, D3DXVECTOR3(rhs.m_Vector2.x, rhs.m_Vector2.y, 1.0f));
					break;
				case NVV_DWORD:
					Result = m_Vector3 * rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_Vector3 * rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_VECTOR3, &Result);
		}
		break;
		
		case NVV_VECTOR2:
		{
			D3DXVECTOR2 Result;
			switch(rhs.m_Type)
			{
				case NVV_VECTOR4:
					Result = VectorModulate(m_Vector2, D3DXVECTOR2(rhs.m_Vector4.x, rhs.m_Vector4.y));
					break;
				case NVV_VECTOR3:
					Result = VectorModulate(m_Vector2, D3DXVECTOR2(rhs.m_Vector3.x, rhs.m_Vector3.y));
					break;
				case NVV_VECTOR2:
					Result = VectorModulate(m_Vector2, rhs.m_Vector2);
					break;
				case NVV_DWORD:
					Result = m_Vector2 * rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_Vector2 * rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_VECTOR2, &Result);
		}
		break;
		
		case NVV_DWORD:
		{
			DWORD Result;
			switch(rhs.m_Type)
			{
				case NVV_DWORD:
					Result = m_dwValue * rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_dwValue * rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_DWORD, &Result);
		}
		break;
		
		case NVV_FLOAT:
		{
			float Result;
			switch(rhs.m_Type)
			{
				case NVV_DWORD:
					Result = m_fValue * rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_fValue * rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_FLOAT, &Result);
		}
		break;
		
		default:
			NVASSERT(0, "Unknown value!");
			break;
	}
	return NVValue(NVV_DWORD);
}

NVValue NVValue::operator+ (const NVValue& rhs) const
{		
	switch(m_Type)
	{
		case NVV_VECTOR4:
		{
			D3DXVECTOR4 Result;
			switch(rhs.m_Type)
			{
				case NVV_VECTOR4:
					Result = m_Vector4 + rhs.m_Vector4;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_VECTOR4, &Result);
		}
		break;
		
		case NVV_VECTOR3:
		{
			D3DXVECTOR3 Result;
			switch(rhs.m_Type)
			{
				case NVV_VECTOR3:
					Result = m_Vector3 + rhs.m_Vector3;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_VECTOR3, &Result);
		}
		break;

		case NVV_VECTOR2:
		{
			D3DXVECTOR2 Result;
			switch(rhs.m_Type)
			{
				case NVV_VECTOR2:
					Result = m_Vector2 + rhs.m_Vector2;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_VECTOR2, &Result);
		}
		break;

		case NVV_DWORD:
		{
			DWORD Result;
			switch(rhs.m_Type)
			{
				case NVV_DWORD:
					Result = m_dwValue + rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_dwValue + rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_DWORD, &Result);
		}
		break;
		
		case NVV_FLOAT:
		{
			float Result;
			switch(rhs.m_Type)
			{
				case NVV_DWORD:
					Result = m_fValue + rhs.m_dwValue;
					break;
				case NVV_FLOAT:
					Result = m_fValue + rhs.m_fValue;
					break;
				default:
					NVASSERT(0, "Can't multiply these values!");
					break;
			}
			return NVValue(NVV_FLOAT, &Result);
		}
		break;

		default:
		{
			NVASSERT(0, "Unknown value!");
		}
		break;
	}
	return NVValue(NVV_DWORD);
}

NVValue NVValue::ApplyLink() const
{
	if (m_Action == NVVL_DEFAULT)
		return *this;

	NVValue* pTarget = m_pMaterialDestination->FindValue(m_strValueNameLink);
	
	// If there is no target, return the current value.
	if (!pTarget)
	{
		return *this;
	}

	switch(m_Action)
	{
		case NVVL_SELECT_MATERIAL:
			return *pTarget;
			break;
		case NVVL_MULTIPLY_MATERIAL:
			return (*this * *pTarget);
			break;
		case NVVL_ADD_MATERIAL:
			return (*this + *pTarget);
			break;
		default:
			NVASSERT(0, "Unknown action!");
			break;
	}
	return NVValue(NVV_DWORD);
}

}