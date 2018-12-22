/**********************************************************************
 	MODULE: importMRC
	
	DESCRIPTION: This module is useful to put MRC data form file to an 
				 in memory C++ Object.
	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "stdafx.h"
#include <string.h>
#include "MRC.h"
#include "..\MRCexport\MRCformat2.h"

//===================================================================================================
static bool LoadSkinData (FILE* stream, MRC_t* MRCdata);
static bool LoadMeshData (FILE* stream, MRC_t* MRCdata);
static bool LoadBoneData(FILE* stream, bone_t* Bone, int keyCnt);

//===================================================================================================
//===================================================================================================
MRC_t* APIENTRY MRCLoad (FILE* MRCfile)
{	
	bool ok=false;
	
	//Version check
	rewind(MRCfile);
	unsigned long version;
	fread(&version,sizeof(unsigned long),1,MRCfile);

	if(version != mrc_version)
	{
		fclose(MRCfile);
		return 0;
	}
	
	//data allocation
	MRC_t* MRCdata = new MRC_t;

	//data load
	if (LoadSkinData(MRCfile, MRCdata))	ok=true;		//correct read done

	if (ok)	return MRCdata;

	else {//TODO: proper deallocation
		delete MRCdata;
		return NULL;
	}
}
//===================================================================================================
//===================================================================================================


//=======================================================================================
static bool LoadSkinData (FILE* stream, MRC_t* MRCdata) {
	//object start
	long startObj=ftell(stream);

	//load mesh data
	if (!LoadMeshData (stream,MRCdata)) 
		return false;
		
	MRCobject_hdr oHdr;
	fread(&oHdr,sizeof(MRCobject_hdr),1,stream);
	//animation time
	MRCdata->time_span = oHdr.animLast;	
	//animation samples count
	MRCdata->key_cnt = oHdr.keyCnt;
	//bone count
	MRCdata->bone_cnt = oHdr.boneCnt;	

	//not only mesh
	if (oHdr.boneCnt)
	{
		//positioning to bone start
		fseek(stream, oHdr.boneOfs, SEEK_SET);
		
		MRCdata->Bones = new bone_t[MRCdata->bone_cnt];

		int keyCnt = oHdr.keyCnt;
		
		for(int i = 0; i < MRCdata->bone_cnt; i++)
		{//read bone data
			if (! LoadBoneData(stream, &(MRCdata->Bones[i]), keyCnt) )
				return false;
		}
	}
	//return to object start
	fseek(stream, startObj, SEEK_SET);

	return true;
}

//=======================================================================================
static bool LoadMeshData (FILE* stream, MRC_t* MRCdata) {
	
	//object start
	long startObj=ftell(stream);

	//object header data
	MRCobject_hdr oHdr;
	fread(&oHdr,sizeof(MRCobject_hdr),1,stream);
	
	MRCmesh_hdr mHdr;
	fread(&mHdr,sizeof(MRCmesh_hdr),1,stream);

	//mesh sizes
	MRCdata->vert_cnt = mHdr.vertCnt;
    MRCdata->norm_cnt = mHdr.normCnt;
	MRCdata->face_cnt = mHdr.faceCnt;
	
	//loading mesh data
	MRCdata->verts = new vertex_t[MRCdata->vert_cnt];
	fseek(stream, mHdr.vertOfs, SEEK_SET);
	fread(MRCdata->verts, sizeof(vertex_t), MRCdata->vert_cnt, stream);

	MRCdata->norms = new vertex_t[ MRCdata->norm_cnt];
	fseek(stream, mHdr.normOfs, SEEK_SET);
	fread(MRCdata->norms , sizeof(vertex_t), MRCdata->norm_cnt, stream);

	MRCdata->faces = new msh_face_t[MRCdata->face_cnt];
	fseek(stream, mHdr.faceOfs, SEEK_SET);
	fread(MRCdata->faces , sizeof(msh_face_t), MRCdata->face_cnt, stream);
	
	//return to object start
	fseek(stream, startObj, SEEK_SET);

	return true;
}

//=======================================================================================
static bool LoadBoneData(FILE* stream, bone_t* Bone, int keyCnt)
{
	MRCbone_hdr bHdr;

	///bone structure data
	fread(&bHdr,sizeof(MRCbone_hdr),1,stream);
	memcpy(Bone->inverse_bone_TM, bHdr.inverseOrientationTM, sizeof(float[16]));
	Bone->parent_idx = bHdr.parentIdx;			// -1 if is root
	Bone->child_cnt = bHdr.childCnt;
	if (!Bone->child_cnt) Bone->childs=NULL;	// no childs if is leaf
	else {
		Bone->childs = new int[Bone->child_cnt];
		
		fread(Bone->childs, sizeof(int), Bone->child_cnt, stream);
	}
	
	//bone vertex weight data:	
	Bone->vert_cnt = bHdr.vertexCnt;
	if (!Bone->vert_cnt) Bone->vert_weight=NULL;
	else {
		fseek(stream,bHdr.boneWeightsOfs,SEEK_SET);
		Bone->vert_weight = new vert_weight_t[Bone->vert_cnt];
		fread(Bone->vert_weight, sizeof(vert_weight_t), Bone->vert_cnt, stream);
	}

	//bone animation data:
	if (!keyCnt) Bone->key = NULL;
	else {
		fseek(stream, bHdr.boneKeysOfs,SEEK_SET);
		Bone->key = new key_t[keyCnt];
		fread(Bone->key, sizeof(key_t), keyCnt, stream);
	}
	return true;
}
