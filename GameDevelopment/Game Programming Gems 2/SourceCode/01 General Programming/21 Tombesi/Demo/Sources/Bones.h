/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once

#include "std.h"
#include "ImportMRC\MRC.h"

//========================================================================================
//========================================================================================
struct _BoneWeights
{
	unsigned int Index;
	float Weight;
};

// ============================================================================
struct _Key
{
public:
	ulong mSec;
	vect pos;
	quat quat;
};
// ============================================================================
struct _BoneKeys
{
	int KeysCount;
	_Key *Keys;

	//=================================
	_BoneKeys();
	~_BoneKeys();
	
	int GetIndexKey(const ulong mSec);	
};

//========================================================================================
class _Bone
{
public:
	int BoneChildsCount;
	_Bone** BoneChilds;			//list of childs for that bone

	int VertexCount;
	_BoneWeights* BoneWeights;	//vertex influenced by this bone
	
	matrix InverseOrient_TM;	//from global to bone's space

	matrix BoneAnim_TM;		//animation matrix

	_BoneKeys BoneKeys;			//animation keys

	//=========================
	_Bone();
	_Bone(const MRC_t* MRCdata,const int bone_idx);
	~_Bone();

protected:	
	void UpdateBoneAnim_TM(const ulong mSec);

private:
	void InterpolateTransf(const ulong mSec);
};

//========================================================================================
class _RootBone: public _Bone
{
public:
	_RootBone();
	_RootBone(const MRC_t* MRCdata);

	void Update(ulong mSecGlobal);
	void Reset()	{ AnimRelTime = 0; }

private:
	ulong AnimEndTime;				//total animation time (in millisecs)
	ulong AnimRelTime;				//current animation time (in millisecs)
	
	//set the anim rlative time given the global time from timer start
	void setAnimRelTime(ulong mSecGlobal)
	{
		AnimRelTime = (mSecGlobal % (AnimEndTime+100));		//at anim end waits for 0.1 sec
//debug
		//AnimRelTime = 0;
		
	}

};
//========================================================================================
