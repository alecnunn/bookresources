/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
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
