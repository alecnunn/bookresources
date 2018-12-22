/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
Modifier* GetPhysiqueMod(INode *pNode)
{
	Object *pObj = pNode->GetObjectRef();
	if(!pObj) return NULL;

	// Is it a derived object?
	while(pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		// Yes -> Cast
		IDerivedObject *pDerivedObj = static_cast<IDerivedObject*>(pObj);

		// Iterate over all entries of the modifier stack
		int ModStackIndex = 0;
		while(ModStackIndex < pDerivedObj->NumModifiers())
		{
			// Get current modifier
			Modifier* pMod = pDerivedObj->GetModifier(ModStackIndex);

			// Is this Physique?
			if(pMod->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
				return pMod;

			// Next modifier stack entry
			ModStackIndex++;

		}
		pObj = pDerivedObj->GetObjRef();
	}
	// Not found
	return NULL;
}
