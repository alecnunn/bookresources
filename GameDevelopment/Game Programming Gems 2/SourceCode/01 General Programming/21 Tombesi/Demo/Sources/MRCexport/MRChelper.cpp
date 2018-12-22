/**********************************************************************
 	MODULE: MRCexport
	
	DESCRIPTION: This module is the plugin for MAX 3 to export in MRC format
	
	Version:		1.22			

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "utils.h"
#include "MRChelper.h"
#include "extra\Bipexp.h"
#include "extra\Phyexp.h"
#include "extra\ISkin.h"

// ============================================================================
bool IsMesh(INode *pNode)
{
	if(pNode == NULL)
		return false;
	ObjectState os = pNode->EvalWorldState(0); 
	if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
		return true;

	return false;
}

// ============================================================================
//test if is a bone or a biped controller
bool IsBone(INode *pNode)
{
	if(pNode == NULL)
		return false;

	ObjectState os = pNode->EvalWorldState(0);

	if (!os.obj) return false;

	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0))
		return true;

	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;

	Control *cont = pNode->GetTMController();
	if(cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||		//others biped parts
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID			//biped root "Bip01"
		
	) return true;

	return false;
}

/*
// ============================================================================
bool IsBipedBone(INode *pNode)
{
	if(pNode == NULL || pNode->IsRootNode())
		return false;

	Control *cont = pNode->GetTMController();
	if(cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID
	) return true;

	return false;
}
*/
// ============================================================================
// Get the number of direct child bones of a node
int GetChildBoneCount(INode *pNode)
{
	int count = 0;

	for(int i = 0; i < pNode->NumberOfChildren(); i++)
	{
		if(IsBone(pNode->GetChildNode(i)))
			count++;
	}
	return count;
}

// ============================================================================
// how many bones in the scene? Pass it the rootNode
int CountBones(INode* pNode)
{
	int count = 0;
	if(IsBone(pNode))
		count += 1;

	for(int i = 0; i < pNode->NumberOfChildren(); i++)
		count += CountBones(pNode->GetChildNode(i));
	return count;
}

// ============================================================================
// ============================================================================
// used by GetBoneByIndex
static bool BuildIter (INode* pnode, INode** const Iterator, int& currIdx) {
	
	if(IsBone(pnode)){
		Iterator[currIdx++] = pnode;
	}

	for(int i = 0; i < pnode->NumberOfChildren(); i++) {
		BuildIter (pnode->GetChildNode(i),Iterator,currIdx);
	}

	return true;
}

// ============================================================================
// Get bone pointer from an index, this should get passed the root node
INode* GetBoneByIndex(INode* const pRoot, int index) {
	INode* bone = NULL;
	const int bone_cnt = CountBones(pRoot);
	
	if (index>=bone_cnt) 
		return NULL;

	INode** const Iterator = new INode* [bone_cnt];
	int currIdx=0;
	
		BuildIter(pRoot,Iterator,currIdx);
	
	assert(currIdx==bone_cnt);

	bone = Iterator[index];
	
	assert (GetBoneIndex(pRoot,bone)==index);

	delete [] Iterator;
		
	assert (IsBone(bone));
	return bone;
}
// ============================================================================

// ============================================================================
// Recursive iterator to get a bone index, used with GetBoneIndex
int RecursiveGetBoneIndex(INode *pRoot, INode *pNodeTest, int &boneCount)
{
	int boneIdx = -1;

	if(IsBone(pRoot))
	{
		boneIdx = boneCount;
		boneCount++;

		if(pRoot == pNodeTest)
			return boneIdx;
	}

	// recurse child nodes
	for(int i = 0; i < pRoot->NumberOfChildren(); i++)
	{
		int boneIdx = RecursiveGetBoneIndex(pRoot->GetChildNode(i), pNodeTest, boneCount);
		if(boneIdx >= 0)
			return boneIdx;
	}

	return -1;
}

// ============================================================================
// Get an index from a node pointer
int GetBoneIndex(INode *pRoot, INode *pNode)
{
	if(!IsBone(pNode))
		return -1;

	int boneCount = 0;
	return RecursiveGetBoneIndex(pRoot, pNode, boneCount);
}


// ============================================================================
Matrix3 GetBoneTM(INode *pNode, TimeValue t)
{
	Matrix3 tm(1);

		tm = pNode->GetNodeTM(t);

	tm.NoScale();
	return tm;
}
// ============================================================================


// ============================================================================
static Modifier* GetPhysiqueMod(INode *pNode)
{
	Object *pObj = pNode->GetObjectRef();
	if(!pObj) return NULL;

	// Is derived object ?
	while(pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		// Yes -> Cast.
		IDerivedObject *pDerivedObj = static_cast<IDerivedObject*>(pObj);

		// Iterate over all entries of the modifier stack.
		int ModStackIndex = 0;
		while(ModStackIndex < pDerivedObj->NumModifiers())
		{
			// Get current modifier.
			Modifier* pMod = pDerivedObj->GetModifier(ModStackIndex);

			// Is this Physique ?
			if(pMod->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
				return pMod;

			// Next modifier stack entry.
			ModStackIndex++;

		}
		pObj = pDerivedObj->GetObjRef();
	}
	// Not found.
	return NULL;
}

// ============================================================================
static Modifier* GetSkinMod(INode *pNode)
{
	Object *pObj = pNode->GetObjectRef();
	if(!pObj) return NULL;
	
	// Is derived object ?
	while(pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		// Yes -> Cast.
		IDerivedObject *pDerObj = static_cast<IDerivedObject*>(pObj);

		// Iterate over all entries of the modifier stack.
		int ModStackIndex = 0;
		while (ModStackIndex < pDerObj->NumModifiers())
		{
			// Get current modifier.
			Modifier* mod = pDerObj->GetModifier(ModStackIndex);
			
			// Is this Skin ?
			if(mod->ClassID() == SKIN_CLASSID)
			{
				// Yes -> Exit.
				return mod;
			}
			
			// Next modifier stack entry.
			ModStackIndex++;
		}
		pObj = pDerObj->GetObjRef();
	}
	// Not found.
	return NULL;
}

// ============================================================================
bool ProcessBoneWeights (INode *pNode,INode *pRoot, BoneData_t* BD)
{
	if ((!pNode)||(!IsMesh(pNode))) return false;
			 
	
	Modifier *pMod = GetPhysiqueMod(pNode);
	if(!pMod)
	{
/*		pMod = GetSkinMod(pNode);
		if ( (!pMod) ||
			 (!GetSkinWeights(pNode,pRoot, pMod, BD)) )
*/			return false;
	}
	else
			GetPhysiqueWeights(pNode, pRoot, pMod, BD);

	return true;
}


// ============================================================================
int ProcessBoneStruct(INode *pNode, INode *pRoot,int parentIdx, BoneData_t* BD)
{
	//test if the node is a bone		
	if (IsBone(pNode)) 
	{	
		int currIdx = GetBoneIndex (pRoot, pNode);

		assert(-1!=currIdx);
		
		//set bone header data
		MRCbone_hdr &bHdr = BD[currIdx].Hdr;
		// get the bones inverse base matrix at time 0		
		Matrix3 tm = GetBoneTM(pNode, 0);
		tm.Invert();
		MAXtoGL(tm, bHdr.inverseOrientationTM);
		bHdr.parentIdx = parentIdx;
		bHdr.childCnt = GetChildBoneCount(pNode);

		//set child's index vector
		if(bHdr.childCnt > 0)
		{
			BD[currIdx].childsVect.reserve(bHdr.childCnt);
			for (int i=0; i<pNode->NumberOfChildren(); ++i)
			{
				int chIdx = ProcessBoneStruct(pNode->GetChildNode(i),pRoot,currIdx, BD);		
				if (0<=chIdx)
					BD[currIdx].childsVect.push_back(chIdx);
			}
		}
		assert (BD[currIdx].childsVect.size() == BD[currIdx].Hdr.childCnt);
		return currIdx;
	}
	else 
	{
		for (int i=0; i<pNode->NumberOfChildren(); ++i)
			ProcessBoneStruct(pNode->GetChildNode(i),pRoot,-1, BD);
		return -1;
	}
}	

// ============================================================================
int ProcessBoneAnim (INode *pRoot, Interval range, ULONG sampleD, BoneData_t* BD)
{
	int keyCnt=0;
	int totbones = CountBones(pRoot);
	const ULONG start = TicksToMilliSec(range.Start());
	const ULONG end   = TicksToMilliSec(range.End());
		
	if (!totbones) return 0;

	// cycle throught every bone
	for (int idx=0; idx<totbones; ++idx) {
		// get bone node pointer
		INode* pBone = GetBoneByIndex(pRoot, idx);
			assert (IsBone(pBone));

		ULONG msec=0;
		//BD[idx].keysVect.reserve(end/sampleD);
		// grab keys for this bone
		for(msec = start; msec < end+sampleD; msec += sampleD)
		{	
			MRCbonekey_hdr KeyHdr;
			memset(&KeyHdr, 0, sizeof(MRCbonekey_hdr));
			
			//grab key time (in millisecs)
			if(msec > end) KeyHdr.time = end;
			else KeyHdr.time = msec;

			//grab bone animation matrix at time t
			Matrix3 tm;
			TimeValue t;
			t = MilliSecToTicks(msec);
			tm = GetBoneTM(pBone, t);		
			MAXtoGL(tm);

			// grab translation
			Point3 pos = tm.GetTrans();		
			KeyHdr.pos[0] = pos.x;
			KeyHdr.pos[1] = pos.y;
			KeyHdr.pos[2] = pos.z;

			// grab rotation union quaternion
			Quat quat(tm);
			quat.Normalize();
			KeyHdr.quat[0] = quat.x;
			KeyHdr.quat[1] = quat.y;
			KeyHdr.quat[2] = quat.z;
			KeyHdr.quat[3] = quat.w;

			BD[idx].keysVect.push_back(KeyHdr);
		}
	}
	keyCnt = BD[0].keysVect.size();
	return keyCnt;
}


// ============================================================================
// ============================================================================
bool GetPhysiqueWeights(INode *pNode, INode *pRoot, Modifier *pMod, BoneData_t *BD)
{
	// create a Physique Export Interface for the given Physique Modifier
	IPhysiqueExport *phyInterface = (IPhysiqueExport*)pMod->GetInterface(I_PHYINTERFACE);
	if(phyInterface)
	{
		// create a ModContext Export Interface for the specific node of the Physique Modifier
		IPhyContextExport *modContextInt = (IPhyContextExport*)phyInterface->GetContextInterface(pNode);
		// needed by vertex interface (only Rigid one supported for now)
		modContextInt->ConvertToRigid(TRUE);
		// more than a single bone per vertex
		modContextInt->AllowBlending(TRUE);

		if(modContextInt)
		{
			int totalVtx = modContextInt->GetNumberVertices();
			for(int i = 0; i < totalVtx; i++)
			{
				//get i-th vertex
				IPhyVertexExport *vtxInterface = (IPhyVertexExport*)modContextInt->GetVertexInterface(i);
				if(vtxInterface)
				{
					int vtxType = vtxInterface->GetVertexType();
					if(vtxType == RIGID_TYPE)
					{
						//get bone
						INode *boneNode = ((IPhyRigidVertex*)vtxInterface)->GetNode();
						int boneIdx = GetBoneIndex(pRoot, boneNode);

						//Build vertex data
						MRCweight_hdr wdata;
						wdata.vertIdx = i;
						wdata.weight = 1.0f;
						//Insert into proper bonedata
						BD[boneIdx].weightsVect.push_back(wdata);
						//update vertexWeightCnt for that bone
						BD[boneIdx].Hdr.vertexCnt = BD[boneIdx].weightsVect.size();
					}
					else if(vtxType == RIGID_BLENDED_TYPE)
					{
						IPhyBlendedRigidVertex *vtxBlendedInt = (IPhyBlendedRigidVertex*)vtxInterface;
						for(int j = 0; j < vtxBlendedInt->GetNumberNodes(); j++)
						{
							//get j-th bone
							INode *boneNode = vtxBlendedInt->GetNode(j);
							//whose index is boneIdx
							int boneIdx = GetBoneIndex(pRoot, boneNode);
	
							//Build vertex data
							MRCweight_hdr wdata;
							wdata.vertIdx = i;
							wdata.weight = vtxBlendedInt->GetWeight(j);
							
							//check vertex existence for this bone
							bool notfound = true;
							for (int v=0; notfound && v<BD[boneIdx].weightsVect.size();v++)
							{
								//update found vertex weight data for that bone
								if (BD[boneIdx].weightsVect[v].vertIdx == wdata.vertIdx)
									{
										BD[boneIdx].weightsVect[v].weight += wdata.weight;
										notfound = false;
									}
							}

							if (notfound)
							{
								//Add a new vertex weight data into proper bonedata
								BD[boneIdx].weightsVect.push_back(wdata);
								//update vertexWeightCnt for that bone
								BD[boneIdx].Hdr.vertexCnt = BD[boneIdx].weightsVect.size();
							}
						}
					}
				}
			}
			phyInterface->ReleaseContextInterface(modContextInt);
		}
		pMod->ReleaseInterface(I_PHYINTERFACE, phyInterface);
	}
	return true;
}

// ============================================================================
// ============================================================================
// export to MRC file every BoneData entry
bool ExportBoneData(BoneData_t *BD,int boneCnt, int keyCnt, FILE *out)
{	
// write weights in bone major format
	//i-th bone
	for(int i = 0; i < boneCnt; i++)
	{
		long BoneHdrPos = ftell(out);

		fseek(out, sizeof(MRCbone_hdr), SEEK_CUR);
		
		// write the child bone index list
		for (int j=0; j<BD[i].Hdr.childCnt; j++)
		{			
			fwrite(&BD[i].childsVect[j], sizeof(int), 1, out);
		}			
		BD[i].Hdr.boneWeightsOfs = ftell(out);

		//BoneWeight data export
		int weightCnt = BD[i].Hdr.vertexCnt;
		
		//write data for j.th vertex who belongs to i-th bone
		for( j = 0; j < weightCnt; j++)
		{
			MRCweight_hdr &vertWeight = BD[i].weightsVect[j];
			fwrite(&vertWeight, sizeof(MRCweight_hdr), 1, out);
		}
		BD[i].Hdr.boneKeysOfs = ftell(out);
				
		//BoneANIMATION data export
		for( j = 0; j < keyCnt; j++)
		{
			MRCbonekey_hdr &boneKey = BD[i].keysVect[j];
			fwrite(&boneKey, sizeof(MRCbonekey_hdr), 1, out);
		}

		long endPos = ftell(out);
		// write down bone header
		fseek(out, BoneHdrPos, SEEK_SET);
		fwrite(&BD[i].Hdr, sizeof(MRCbone_hdr), 1, out);
		fseek(out, endPos, SEEK_SET);
	}
	return true;
}

// ============================================================================
// dummy view used internally by WriteMesh
class NullView : public View {
public:
	Point2 ViewToScreen(Point3 p)
	{ 
		return Point2(p.x,p.y); 
	}

	NullView() {
		worldToView.IdentityMatrix();
		screenW=800.0f; screenH = 600.0f;
	}

};

static NullView nullView;
// ============================================================================
// get and export mesh data to file
bool ExportMesh (INode* pNode, FILE *out)
{
	if ((!pNode)||(!IsMesh(pNode))) return false;

	MRCmesh_hdr mHdr;
	memset(&mHdr, 0, sizeof(MRCmesh_hdr));

	Matrix3 tm = pNode->GetObjectTM(0);
	ObjectState os = pNode->EvalWorldState(0);
	int needDelete;
	Mesh& mesh = *(( (GeomObject*) os.obj)->GetRenderMesh(0,pNode,nullView,needDelete));
	
	long startPos = ftell(out);
	fseek(out, sizeof(MRCmesh_hdr), SEEK_CUR);

	// write the mesh vertices
	mHdr.vertCnt = mesh.getNumVerts();
	mHdr.vertOfs = ftell(out);
	for(int i = 0; i < mHdr.vertCnt; i++)
	{
		Point3 pnt = mesh.getVert(i) * tm;			//premultiply in MAX
		MAXtoGL(pnt);								//useful coordinate transformation
		fwrite(&pnt.x, sizeof(float), 3, out);
	}

	// write vertex normals
	mesh.buildNormals();
	mHdr.normCnt = mesh.getNumVerts();
	mHdr.normOfs = ftell(out);
	for(i = 0; i < mHdr.normCnt; i++)
	{ 
		// normals are taken from a unique smoothing group
		Point3 norm = Normalize(mesh.getNormal(i));
		MAXtoGL(norm);
		fwrite(&norm.x, sizeof(float), 3, out);
	}

	// build and write faces
	mHdr.faceCnt = mesh.getNumFaces();
	mHdr.faceOfs = ftell(out);
	for(i = 0; i < mHdr.faceCnt; i++)
	{
		MRCface_hdr fHdr;
		memset(&fHdr, 0, sizeof(MRCface_hdr));

		fHdr.vert[0] = mesh.faces[i].v[0];
		fHdr.vert[1] = mesh.faces[i].v[1];
		fHdr.vert[2] = mesh.faces[i].v[2];
		// TODO: insert normals
		fwrite(&fHdr, sizeof(MRCface_hdr), 1, out);
	}

	long endPos = ftell(out);
	fseek(out, startPos, SEEK_SET);
	fwrite(&mHdr, sizeof(MRCmesh_hdr), 1, out);
	fseek(out, endPos, SEEK_SET);
	return true;
}
// ============================================================================
// ============================================================================