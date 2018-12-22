/**********************************************************************
 	MODULE: MRCexport
	
	DESCRIPTION: This module is the plugin for MAX 3 to export in MRC format
	
	Version:		1.22				

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once

#include "std.h"
#include "utils.h"
#include "MRCformat2.h"
#include <vector>

//=========================================================================================
typedef std::vector<MRCweight_hdr> WeightsVect;
typedef std::vector<int> ChildIdxVect;
typedef std::vector<MRCbonekey_hdr> BoneKeysVect;
// ============================================================================
struct BoneData_t 
{
	MRCbone_hdr Hdr;

	ChildIdxVect childsVect;
	WeightsVect weightsVect;
	BoneKeysVect keysVect;

	BoneData_t()
	{
		memset(&Hdr,0,sizeof(MRCbone_hdr));
		childsVect.clear();
		weightsVect.clear();
		keysVect.clear();		
	}
	~BoneData_t()
	{
		childsVect.clear();
		weightsVect.clear();
		keysVect.clear();
	}

};
// ============================================================================



// ============================================================================
bool ExportBoneData(BoneData_t *BD,int boneCnt, int keyCnt, FILE *out);
// ============================================================================
bool ExportMesh (INode* pNode, FILE *out);
// ============================================================================
int ProcessBoneStruct(INode *pNode, INode *pRoot,int parentIdx, BoneData_t* BD);
// ============================================================================
bool ProcessBoneWeights (INode *pNode,INode *pRoot, BoneData_t* BD);
// ============================================================================
int ProcessBoneAnim (INode *pRoot, Interval range, ULONG sampleD, BoneData_t* BD);
// ============================================================================
bool IsMesh(INode *pNode);
bool IsEditTriMesh(INode *pNode);
// ============================================================================
bool IsBone(INode *pNode);
// ============================================================================
int CountBones(INode* pNode);
// ============================================================================
// Get the number of direct child bones of a node
int GetChildBoneCount(INode *pNode);
// ============================================================================
// Get an index from a node pointer
int GetBoneIndex(INode *pRoot, INode *pNodeTest);
// ============================================================================
// Get bone pointer from an index
INode* GetBoneByIndex(INode *pRoot,int index);
// ============================================================================
bool GetPhysiqueWeights(INode *pNode, INode *pRoot, Modifier *pMod, BoneData_t *BD);
// ============================================================================

