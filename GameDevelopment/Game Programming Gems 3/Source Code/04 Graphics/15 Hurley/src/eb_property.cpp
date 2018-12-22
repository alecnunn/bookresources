/*********************************************************************NVMH2****
Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "eb_effect.h"

// Property class helpers
void EBTriggerProperty::Trigger(EBPropertySet* pObj)
{
	assert(m_PropertyType == EBTYPE_TRIGGER_PROP);
	assert(m_dwPropertyFlags & PROPFLAG_CALLBACK);
	pObj->PropertyWritten(this);
}

void EBProperty::WriteValue(EBPropertySet* pObj, const EBValue& Value)
{
	assert(m_PropertyType != EBTYPE_TRIGGER_PROP);
	if (!Value.IsKindOf(m_PropertyType))
	{
		assert(0);
		return;
	}

	Value.Write((BYTE*)pObj + m_dwOffset);
	pObj->PropertyWritten(this);
}

EBValue EBProperty::ReadValue(EBPropertySet* pObj) 
{
	assert(m_PropertyType != EBTYPE_TRIGGER_PROP);

	EBValue Value(m_PropertyType);
	Value.Read((BYTE*)pObj + m_dwOffset);
	pObj->PropertyRead(this);
	return Value;
}
