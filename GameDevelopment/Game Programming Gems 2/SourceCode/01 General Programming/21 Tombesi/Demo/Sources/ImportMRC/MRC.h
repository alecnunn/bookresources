/**********************************************************************
 	MODULE: importMRC
	
	DESCRIPTION: This module is useful to put MRC data form file to an 
				 in memory C++ Object.
	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once

#include <stdio.h>
//=========================================================================================
//			MRC file format INTERFACE
//			external data structures
///////////////////////////////////////////////////////////////////////////////////////////
//=========================================================================================

#if defined(__cplusplus)
extern "C" {
#endif

//===================================================================================================
#pragma pack(push,1)

/*	weight data  */
struct vert_weight_t
{
	unsigned int vert_idx;
	float weight;
};

/* Key data */
struct key_t
{
	unsigned long time;
	
	float pos[3];
	float quat[4];
};

#pragma pack(pop)

//=========================================================================================
/*	Bones  */
struct bone_t
{
	int parent_idx;					//bone parent index (-1 if is root)
	int child_cnt;
	int* childs;					//direct bone child indexes
	float inverse_bone_TM [16];		//bone's inverse orientation matrix

	int vert_cnt;					//number of vertex influenced
	vert_weight_t* vert_weight;		//bone to vertex influence

	//animation data
	key_t* key;						//bone animation

	bone_t();
	~bone_t();

};

//=========================================================================================
typedef float vertex_t[3];

struct msh_face_t
{
	int v[3];  // vertex indices
	int n[3];  // normal indices
};

//=========================================================================================
// MRC object
struct MRC_t
{
	//animation data
	unsigned long time_span;	//total animation time in millisecs
	int key_cnt;
	
	//skeleton data
	int bone_cnt;				//total number of bones
	bone_t* Bones;				//sequence of all bones

	//mesh data:
	int norm_cnt;
	vertex_t* norms;

	int vert_cnt;
	vertex_t* verts;

	int face_cnt;
	msh_face_t* faces;

	//living functions
	MRC_t();
	~MRC_t();

};
//=========================================================================================
//NOTE: time is in millisecs
//=========================================================================================
//===================================================================================================



//===================================================================================================
//===================================================================================================
MRC_t* APIENTRY MRCLoad (FILE* MRCfile);
//===================================================================================================
//===================================================================================================


#if defined(__cplusplus)
};
#endif



