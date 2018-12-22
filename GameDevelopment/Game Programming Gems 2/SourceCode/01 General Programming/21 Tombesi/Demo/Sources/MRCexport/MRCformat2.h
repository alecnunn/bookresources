/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
#pragma once

//=========================================================================================
//			MRC file format v.2.0.0
//			INTERNAL data structures
///////////////////////////////////////////////////////////////////////////////////////////
//=========================================================================================
#pragma pack(push,1)

// format version
#define MRC_VERSION 201
static const unsigned long mrc_version = (('m') | ('r'<<8) | ('c'<<16) | (MRC_VERSION<<24));

// Object header 
struct MRCobject_hdr
{
	int boneCnt;
	long boneOfs;

	unsigned long animLast;
	unsigned int animFPS;
	int keyCnt;
};

// Mesh header
struct MRCmesh_hdr
{
	int vertCnt;
	long vertOfs;

	int normCnt;
	long normOfs;

	int faceCnt;
	long faceOfs;
};

// Face header
struct MRCface_hdr
{
	int vert[3];
	int norm[3];
};

//	Bone header
struct MRCbone_hdr
{
	int vertexCnt;

	int parentIdx;	//-1 if root
	int childCnt;
	float inverseOrientationTM [16];

	long boneWeightsOfs;
	long boneKeysOfs;
};

//	weight data 
struct MRCweight_hdr
{
	int   vertIdx;
	float weight;
};

//	Bone animation header 
struct MRCbonekey_hdr
{
	unsigned long time;
	
	float pos[3];
	float quat[4];
};
	
#pragma pack(pop)
//=========================================================================================
//NOTE: time is in millisecs
//=========================================================================================