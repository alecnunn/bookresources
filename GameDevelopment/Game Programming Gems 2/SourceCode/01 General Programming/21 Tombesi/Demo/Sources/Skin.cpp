/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "skin.h"

//========================================================================================
//////////////////////////////////////////////////////////
_Skin::_Skin() :RootBone()
{
	Color[0]=1.0;	Color[1]=0.5;	Color[2]=1.0;
	FaceCnt = NormCnt = VertCnt = 0;
	TempVert = Vert = NULL;
	Face = NULL;
	Norm = NULL;
}

//////////////////////////////////////////////////////////
// build the object from the MRCdata pointer
//////////////////////////////////////////////////////////
_Skin::_Skin(const MRC_t* MRCdata) : RootBone(MRCdata) 
{	
	Color[0]=1.0;	Color[1]=0.5;	Color[2]=1.0;

	FaceCnt = MRCdata->face_cnt;
	NormCnt = MRCdata->norm_cnt;
	VertCnt = MRCdata->vert_cnt;
	
	if (NormCnt) {
		//TODO: for now just one normal per vertex (NO SMOOTHING GROOPS)
		Norm = new vertex_t[NormCnt];
		memcpy(Norm,MRCdata->norms,NormCnt*sizeof(vertex_t));
	}
	else Norm = NULL;

	if (VertCnt) {
		Vert = new vertex_t[VertCnt];
		TempVert = new vertex_t[VertCnt];
		memcpy(Vert,MRCdata->verts,VertCnt*sizeof(vertex_t));
	}
	else Vert = NULL;

	if (FaceCnt) {
		//TODO: for now just one normal per vertex (NO SMOOTHING GROOPS)
		Face = new faceIdx[FaceCnt];
		for (int i=0;i<FaceCnt;++i)
			memcpy(Face[i],MRCdata->faces[i].v,3*sizeof(int));
	}
	else Face = NULL;

	memset(TempVert,0, sizeof(vertex_t)*VertCnt);
	UpdateTempData(RootBone);
}

//////////////////////////////////////////////////////////
_Skin::~_Skin() {
	if (Face) delete [] Face;
	if (Vert) delete [] Vert;
	if (Norm) delete [] Norm;
}

//////////////////////////////////////////////////////////
_Skin::UpdateTempData( _Bone &Bone) {

	//Deform mesh data
	vertex_t temp1,temp2;
	for (int j=0;j<Bone.VertexCount;++j)
	{
		const int &idx=Bone.BoneWeights[j].Index;
		MatXVect(temp1, Bone.InverseOrient_TM, Vert[idx]);
		MatXVect(temp2, Bone.BoneAnim_TM, temp1);

		VectMul(temp2,temp2,Bone.BoneWeights[j].Weight);
		
		VectAdd(TempVert[idx],TempVert[idx],temp2);
	}
	
	for (int i=0;i<Bone.BoneChildsCount;++i)
	{
		UpdateTempData (*Bone.BoneChilds[i]);		
	}	
}

//////////////////////////////////////////////////////////
_Skin::Update(ulong mSecGlobal) {
	
	//update bone transformation
	RootBone.Update(mSecGlobal);
	
	//reset temp mesh data
	memset(TempVert,0, sizeof(vertex_t)*VertCnt);

	//Deform mesh data
	UpdateTempData(RootBone);
}
//========================================================================================

