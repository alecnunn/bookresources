/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "Bones.h"

//========================================================================================
static int SearchRootIdx(const MRC_t* MRCdata);

//////////////////////////////////////////////////////////
_RootBone::_RootBone() {
	AnimEndTime=0;
	AnimRelTime=0;
}

//////////////////////////////////////////////////////////
//	make the root bone from MRCdata pointer  
//	recursively builds the hierarchycal full structure
_RootBone::_RootBone(const MRC_t* MRCdata) : _Bone( MRCdata, SearchRootIdx (MRCdata) )
{
	AnimEndTime = MRCdata->time_span;
	AnimRelTime = 0;
}

//////////////////////////////////////////////////////////
//	update animation time and transformation
void _RootBone::Update(ulong mSecGlobal)
{
	setAnimRelTime(mSecGlobal);
	
	glPushMatrix();					//save current matrix
	glLoadIdentity();				//initialize as an identity matrix

	UpdateBoneAnim_TM (AnimRelTime);

	glPopMatrix();					// restore current matrix
}
//========================================================================================



//========================================================================================
//////////////////////////////////////////////////////////
_Bone::_Bone() {
	VertexCount=BoneChildsCount=0;
	
	Identity(InverseOrient_TM);
	Identity(BoneAnim_TM);
	BoneChilds=NULL;
	BoneWeights=NULL;
}

//////////////////////////////////////////////////////////
//	make a bone from MRCdata pointer and a Bone index
//	recursively builds the hierarchycal structure under this bone
_Bone::_Bone(const MRC_t* MRCdata,const int bone_idx) {
	//index check (DEBUG)
	if (!((bone_idx>=0)&&(bone_idx<MRCdata->bone_cnt)))
	{
		VertexCount=BoneChildsCount=0;
		Identity(InverseOrient_TM);
		Identity(BoneAnim_TM);
		BoneChilds=NULL;
		BoneWeights=NULL;
		return;	
	}
	
//this bone data
	bone_t& MRCbone = MRCdata->Bones[bone_idx];
	
	//influenced verts
	VertexCount = MRCbone.vert_cnt;
	if (0==VertexCount) BoneWeights=NULL;
	else 
	{
		BoneWeights = new _BoneWeights[VertexCount];
			assert(sizeof(_BoneWeights)==sizeof(vert_weight_t));
		memcpy(BoneWeights, MRCbone.vert_weight, VertexCount*sizeof(_BoneWeights));
	}

	//bone matrix data
	CpyMat(InverseOrient_TM, MRCbone.inverse_bone_TM);
	Identity(BoneAnim_TM);

//Debug	
	MatrixInv(BoneAnim_TM,MRCbone.inverse_bone_TM);
//

	//bone animation data
		assert (sizeof(key_t)==sizeof(_Key));
	BoneKeys.KeysCount = MRCdata->key_cnt;
	if (BoneKeys.KeysCount) 
	{
		BoneKeys.Keys = new _Key[BoneKeys.KeysCount];
		memcpy(BoneKeys.Keys, MRCbone.key, BoneKeys.KeysCount*sizeof(_Key));
	}
	
	//bone hierarchy structure
	BoneChildsCount = MRCbone.child_cnt;
	if (0==BoneChildsCount) 
	{	//is a leaf

		BoneChilds = NULL;
	}
	else
	{	//is not a leaf
		BoneChilds = new (_Bone*[BoneChildsCount]);
		for (int i=0;i<BoneChildsCount;i++)
		{//recursive hierarchy structure construction
			BoneChilds[i] = new _Bone(MRCdata, MRCbone.childs[i]);	//recursive call
		}
	}
}

//////////////////////////////////////////////////////////
_Bone::~_Bone() {
	if (BoneChilds) delete[] BoneChilds;
	if (BoneWeights) delete[] BoneWeights;
}

//////////////////////////////////////////////////////////
void _Bone::UpdateBoneAnim_TM(const ulong mSec)
{
	if (!BoneKeys.KeysCount) return;
	//update BoneAnim_TM
	//glPushMatrix();
	InterpolateTransf(mSec);
	//glPopMatrix();

	//recurse into Bone's childs
	if (0==BoneChildsCount) return;

	for (int i=0; i < BoneChildsCount; ++i)
	{
		BoneChilds[i]->UpdateBoneAnim_TM(mSec);
	}
}

//////////////////////////////////////////////////////////
//you must have an OpenGl context enabled to use this method
inline void _Bone::InterpolateTransf(const ulong mSec)
{
	vect pos;
	quat quat;

	ulong lastTime = BoneKeys.Keys[BoneKeys.KeysCount-1].mSec;


	{//LINEAR INTERPOLATION BETWEEN TRANSFORMATION

		// get transformation index by time
		int tempIdx = BoneKeys.GetIndexKey(mSec);
	
		// Get keys
		_Key KeyStart = BoneKeys.Keys[tempIdx];
		_Key KeyEnd = BoneKeys.Keys[tempIdx+1];

		// Position interpolation
		float t = (mSec - KeyStart.mSec) / (float)(KeyEnd.mSec - KeyStart.mSec);
		vect dir;
		VectDelta(dir, KeyEnd.pos, KeyStart.pos);
		VectMul(dir, dir, t);
		VectAdd(pos, KeyStart.pos, dir);

		// Quaternion interpolation
		Slerp(KeyStart.quat, KeyEnd.quat, t, quat);

//debug: for no quaternion interpolation
		//CpyQuat(quat, KeyStart.quat);
///
	}

	// SET MATRIX BoneAnim_TM using OpenGL transformations
	matrix temp;
	QuatToMatrix(quat, temp);

	glPushMatrix();
		
		glTranslatef(pos[0], pos[1], pos[2]);
		glMultMatrixf(temp);

		glGetFloatv(GL_MODELVIEW_MATRIX , BoneAnim_TM);

	glPopMatrix();
}
//========================================================================================





//========================================================================================
_BoneKeys::_BoneKeys() {
	KeysCount=KeysCount=0;
	Keys=NULL;
}

_BoneKeys::~_BoneKeys() {
	if (Keys) delete[] Keys;
}

//////////////////////////////////////////////////////////
//fast get the key's index by time
int _BoneKeys::GetIndexKey(const ulong mSec)
{
	if (mSec==0) return 0;

	const int lastIdx = KeysCount-1;
	ulong lastTime = Keys[lastIdx].mSec;
	if (mSec >= lastTime) return (lastIdx);

	lastTime = Keys[lastIdx-1].mSec;
	if (mSec >= lastTime) return (lastIdx-1);
	
	int tmpIndex = ((mSec * (lastIdx-1)) / lastTime) ;

	assert ( (Keys[tmpIndex].mSec<=mSec)&&(mSec<Keys[tmpIndex+1].mSec) );
	
	return tmpIndex;
}
//========================================================================================




//========================================================================================
//////////////////////////////////////////////////////////
// utility function to get root bone idx
static int SearchRootIdx(const MRC_t* MRCdata) 
{
	for (int i=0; i< MRCdata->bone_cnt; i++)
	{
		if (-1 == MRCdata->Bones[i].parent_idx)
			return i;	//i: bone root index found
	}
	return -1;		//root not found
}
//========================================================================================