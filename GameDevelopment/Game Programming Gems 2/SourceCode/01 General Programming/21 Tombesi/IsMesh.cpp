/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
bool IsMesh(INode *pNode)
{
	if(pNode == NULL)return false;
	ObjectState os = pNode->EvalWorldState(0); 
	if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
		return true;
	return false;
}

bool IsBone(INode *pNode)
{
	if(pNode == NULL)return false;
	ObjectState os = pNode->EvalWorldState(0);
	if (!os.obj) return false;

	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0))
		return true;

	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;
	
	Control *cont = pNode->GetTMController();
	//other Biped parts
	if(	cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||
		//Biped root "Bip01"
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID
	) return true;
	return false;
}

