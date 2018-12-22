
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/


#include "GPGBackBones.h"



#define GPGKINE_DEBUG			FALSE
#define GPGKINE_CONSTRAIN		TRUE	// FALSE=bypass constraints



GPGDrawLineCB		GPGCharacter::m_drawlineCB=NULL;
GPGProgressCB		GPGCharacter::m_progressCB=NULL;
GPGTimeProgressCB	GPGCharacter::m_timeProgressCB=NULL;



#if GPGALLOW_STREAMS
/******************************************************************************
std::ostream &operator<<(std::ostream &out,const GPGCharacter &character)

******************************************************************************/
std::ostream &operator<<(std::ostream &out,const GPGCharacter &character)
	{
	return character.Print(out,0);
	}



/******************************************************************************
std::ostream &GPGCharacter::Print(std::ostream &out,long indention) const

******************************************************************************/
std::ostream &GPGCharacter::Print(std::ostream &out,long indention) const
	{
	GPGIndent indent(indention);
	GPGIndent indent2(indention+1);

	out<<"\""<<NameConst()<<"\" Character\n"<<indent2<<"{\n";

#if GPGPRINT_VERBOSE
	out<<indent2<<"(0x"<<std::hex<<this<<")\n";
	out<<indent2<<"\"current transform\" ";
	m_transforms.GetCurrentConst().Print(out,indention+1);
#endif

//	out<<indent2<<"@"<<GPGCoreNode::Print(out,indention+1);
	out<<indent2<<"@";
	GPGCoreNode::Print(out,indention+1);

//	out<<indent2<<"\"location\" "<<location<<'\n';
//	out<<"+m_skin: "<<*m_skin;

	out<<indent2<<"}\n";

	PrintDescendents(*this,out,indention);

	return out;
	}



/******************************************************************************
void GPGCharacter::PrintDescendents(const GPGCoreNode &parent,
									std::ostream &out,long indention) const

******************************************************************************/
void GPGCharacter::PrintDescendents(const GPGCoreNode &parent,
									std::ostream &out,long indention) const
	{
	const GPGBoneNodeList &childlist=parent.ChildrenConst();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *node;
	GPGListContext context;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		node->Print(out,indention);

		// recursively to children
		PrintDescendents(*node,out,indention);
		}
	}
#endif



/******************************************************************************
******************************************************************************/
GPGCharacter::GPGCharacter(GPGCharacter *pCloneOf): GPGCoreNode(pCloneOf)
	{
	if(pCloneOf)
		{
		m_pBoneContext=pCloneOf->m_pBoneContext;
		m_pBoneContext->IncReferences();
		Reset();
		CopyBoneHierarchy(pCloneOf,this);
		}
	else
		{
		m_pBoneContext=new GPGBoneContext();
		Reset();
		m_skin->ClearWeights();
		}
	}



/******************************************************************************
******************************************************************************/
GPGCharacter::~GPGCharacter(void)
	{
	RemoveDescendents(*this);

	m_pBoneContext->DecReferences();
	}



/******************************************************************************
void GPGCharacter::Reset(void)

******************************************************************************/
void GPGCharacter::Reset(void)
	{
	GPGCoreNode::Reset();
	m_transforms.GetCurrent().MakeIdentity();

	SetRootCharacter(this);
	RemoveDescendents(*this);

	GPGASSERT(m_pBoneContext);
	m_skin=&m_pBoneContext->Skin();
	GrabSkin();
	SetVertexWeights(&m_skin->GetVertexWeights());
	GetIKModes().Reset();

	m_bonetable.Clear();

	m_testflag=0;
	}



/******************************************************************************
******************************************************************************/
void GPGCharacter::GrabSkin(void)
	{
	m_skin->SetCharacter(this);
	}



/******************************************************************************
******************************************************************************/
void GPGCharacter::CopyBoneHierarchy(GPGCoreNode *pParent,
													GPGCoreNode *pNewParent)
	{
	GPGBoneNodeList &childlist=pParent->Children();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *pNode;
	GPGListContext context;

	childlist.ToHead(context);
	while( (pNode=childlist.PostIncrement(context)) != NULL)
		{
		GPGBoneNode *pNewNode=new GPGBoneNode(this,pNode);
		pNewParent->AppendChild(pNewNode);

		GPGASSERT(pNode->GetBoneIndex()==GetBoneTableSize());

		pNewNode->SetBoneIndex(pNode->GetBoneIndex());
		SetBoneAtIndex(pNewNode->GetBoneIndex(),pNewNode);

		CopyBoneHierarchy(pNode,pNewNode);
		}
	}



/******************************************************************************
GPGString GPGCharacter::Out(bool verbose) const

******************************************************************************/
GPGString GPGCharacter::Out(bool verbose) const
	{
	GPGString message("GPGCharacter::Out() not implemented");
	return message;
	}



/******************************************************************************
GPGCoreNode *GPGCharacter::FindDescendentByName2(GPGCoreNode &parent,
															GPGString name)

******************************************************************************/
GPGCoreNode *GPGCharacter::FindDescendentByName2(GPGCoreNode &parent,
															GPGString name)
	{
	GPGString parentname=parent.NameConst();
	parentname.ForceUppercase();
	if(!parentname.Compare(name))
		return &parent;

	const GPGBoneNodeList &childlist=parent.ChildrenConst();
	if(!childlist.GetNumberElements())
		return NULL;

	GPGCoreNode *found;
	GPGBoneNode *node;
	GPGListContext context;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		// recursively to children
		if((found=FindDescendentByName2(*node,name)))
			return found;
		}

	return NULL;
	}


void GPGCharacter::ForEachNode(long flags,GPGForEachNodeCB callback,
															GPGVoidStar state)
	{
	BOOL stop=false;
	if( !(flags&GPGCHILDFIRST) && callback)
		stop=callback(*this,state);

	if(!stop)
		ForEachNode2(flags,*this,callback,state);

	if( (flags&GPGCHILDFIRST) && callback)
		callback(*this,state);
	}


void GPGCharacter::ForEachNode2(long flags,GPGCoreNode &parent,
								GPGForEachNodeCB callback,GPGVoidStar state)
	{
	GPGBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *node;
	GPGListContext context;
	BOOL stop=false;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		if( !(flags&GPGCHILDFIRST) && callback)
			stop=callback(*node,state);

		//* recursively to children
		if(!stop)
			ForEachNode2(flags,*node,callback,state);

		if( (flags&GPGCHILDFIRST) && callback)
			callback(*node,state);
		}
	}


void GPGCharacter::ForEachNodeDual(GPGForEachNodeCB preCallback,
							GPGForEachNodeCB postCallback,GPGVoidStar state)
	{
	BOOL stop=false;
	if(preCallback)
		stop=preCallback(*this,state);

	if(!stop)
		ForEachNodeDual2(*this,preCallback,postCallback,state);

	if(postCallback)
		postCallback(*this,state);
	}


void GPGCharacter::ForEachNodeDual2(GPGCoreNode &parent,
					GPGForEachNodeCB preCallback,GPGForEachNodeCB postCallback,
					GPGVoidStar state)
	{
	GPGBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *node;
	GPGListContext context;
	BOOL stop=false;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		if(preCallback)
			stop=preCallback(*node,state);

		//* recursively to children
		if(!stop)
			ForEachNodeDual2(*node,preCallback,postCallback,state);

		if(postCallback)
			postCallback(*node,state);
		}
	}



/******************************************************************************
void GPGCharacter::ForEachNodeTransformed(long flags,
						GPGForEachNodeTransformedCB callback,GPGVoidStar state)

	can check GPGCoreNode::IsBone() to see if you can cast callback arg
		from GPGCoreNode to GPGBoneNode

	NOTE if not GPGCHILDFIRST, the callback can legally change the transform
	passed by reference.

******************************************************************************/
void GPGCharacter::ForEachNodeTransformed(long flags,
						GPGForEachNodeTransformedCB callback,GPGVoidStar state)
	{
	BOOL stop=false;

	m_transforms.Push();

	m_transforms.GetCurrent().MakeIdentity();
	m_transforms.GetCurrent().Translate(DisplacementConst());
	m_transforms.GetCurrent().Rotate(RotationConst());
	m_transforms.GetCurrent().Scale(ScaleConst());

	if( !(flags&GPGCHILDFIRST) && callback)
		stop=callback(*this,m_transforms.GetCurrent(),state);

	if(flags&GPGSTORE_XFORM)
		this->StoreTransform(m_transforms.GetCurrent());

	if(!stop)
		ForEachNodeTransformed2(flags,*this,callback,state);

	if( (flags&GPGCHILDFIRST) && callback)
		callback(*this,m_transforms.GetCurrent(),state);

	m_transforms.Pop();
	}



/******************************************************************************
void GPGCharacter::ForEachNodeTransformed2(long flags,GPGCoreNode &parent,
						GPGForEachNodeTransformedCB callback,GPGVoidStar state)

	NOTE scaling DOES propagate to children

******************************************************************************/
void GPGCharacter::ForEachNodeTransformed2(long flags,GPGCoreNode &parent,
						GPGForEachNodeTransformedCB callback,GPGVoidStar state)
	{
	GPGBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *node;
	GPGListContext context;
	BOOL stop=false;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		F32 length=node->GetLength();

//	GPGASSERT(node->ScaleConst().RawConst()[0] == 1.0f);

		m_transforms.Push();

		m_transforms.GetCurrent().Translate(node->DisplacementConst());
		m_transforms.GetCurrent().Rotate(node->RotationConst());
		m_transforms.GetCurrent().Scale(node->ScaleConst());

		if( !(flags&GPGCHILDFIRST) && callback)
			stop=callback(*node,m_transforms.GetCurrent(),state);

		if(flags&GPGSTORE_XFORM)
			node->StoreTransform(m_transforms.GetCurrent());

		m_transforms.GetCurrent().Translate(length,0.0f,0.0f);

		// recursively to children
		if(!stop)
			ForEachNodeTransformed2(flags,*node,callback,state);

		if( (flags&GPGCHILDFIRST) && callback)
			{
			m_transforms.GetCurrent().Translate(-length,0.0f,0.0f);

			callback(*node,m_transforms.GetCurrent(),state);
			}

		m_transforms.Pop();
		}
	}



/******************************************************************************
void GPGCharacter::RemoveDescendents(GPGCoreNode &parent)

******************************************************************************/
void GPGCharacter::RemoveDescendents(GPGCoreNode &parent)
	{
	GPGBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *node;
	GPGListContext context;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		// recursively to children
		RemoveDescendents(*node);

		delete node;
		}

	childlist.Clear();
	}



/******************************************************************************
******************************************************************************/
F32 GPGCharacter::CalcReachingLimit(void)
	{
	F32 max=0.0f;

	ResetToFullReach();
	ForEachNodeTransformed(GPGPARENTFIRST|GPGSTORE_XFORM,NULL);

	//* NOTE using potentially lowered res since higher data may not be loaded
	GetSkin()->DeformMesh(false,false);

	GPGMeshInterface *outmesh=GetSkin()->GetInputMesh();
	GPGASSERT(outmesh);
	GPGVertexIndexer *indexer=outmesh->GetVertexIndexer();
	GPGASSERT(indexer);
	long meshid,meshes=outmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		outmesh->ChooseMeshIndex(meshid);

		long index,vertices=outmesh->GetNumberVertices();
		for(index=0;index<vertices;index++)
			{
			indexer->MoveToIndex(index);
			GPGVector3 *vertex=indexer->GetPosition();
			F32 magnitude=vertex->CalcMagnitude();

			if(max<magnitude)
				max=magnitude;
			}
		}

	ResetToReference();
	ForEachNodeTransformed(GPGPARENTFIRST|GPGSTORE_XFORM,NULL);
	GetSkin()->DeformMesh(false,false);

	GPGLOG("Reaching Limit is %.6G\n",max);
	return max;
	}


void GPGCharacter::RemoveBoneLinks(void)
	{
	ForEachNode(GPGCHILDFIRST,&GPGCharacter::RemoveBoneLinkNode,NULL);
	RebuildBoneTable();
	}


BOOL GPGCharacter::RemoveBoneLinkNode(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	GPGBoneNode &bnode=(GPGBoneNode &)node;

	if(!bnode.IsBoneLink())
		return false;

	GPGCoreNode *parent=node.Parent();
	GPGASSERT(parent);
	parent->RemoveChild(&bnode);
	return false;
	}


void GPGCharacter::BlendBones(F32 weight)
	{
	float weight2=weight;
	if(weight2<0.0f)
		weight2=0.0f;
	if(weight2>1.0f)
		weight2=1.0f;

	ForEachNode(GPGCHILDFIRST,&GPGCharacter::BlendBoneNode,&weight2);
	}


BOOL GPGCharacter::BlendBoneNode(GPGCoreNode &node,GPGVoidStar state)
	{
	if(node.IsBone())
		{
		GPGBoneNode &bnode=(GPGBoneNode &)node;

		if(bnode.IsBoneLink())
			return false;
		}

	F32 *weight=NULL;
	GPGRESULT result=state.CopyPointerTo(&weight);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(weight);

	//* weighted average
	GPGVector3 pos,scale;
	GPGQuaternion rot;

	pos.Interpolate(*weight,node.BlendDisplacementConst(),
												node.DisplacementConst());
	rot.Interpolate(*weight,node.BlendRotationConst(),
												node.RotationConst());
	scale.Interpolate(*weight,node.BlendScaleConst(),
												node.ScaleConst());

	node.Displacement()=pos;
	node.Rotation()=rot;
	node.Scale()=scale;
	return false;
	}


void GPGCharacter::RebuildBoneTable(void)
	{
	ClearBoneTable();
	ForEachNode(GPGCHILDFIRST,&GPGCharacter::AddBoneToTable,this);
	}


BOOL GPGCharacter::AddBoneToTable(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	GPGCharacter *character=NULL;
	GPGRESULT result=state.CopyPointerTo(&character);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(character);

	GPGBoneNode &bnode=(GPGBoneNode &)node;

	character->SetBoneAtIndex(bnode.GetBoneIndex(),&bnode);
	return false;
	}


void GPGCharacter::SetBoneAtIndex(long index,GPGBoneNode *set)
	{
	GPGASSERT(index>=0);

	long m,length=m_bonetable.GetNumberElements();
	m_bonetable.ResizeToAtLeast(index+1);

	for(m=length;m<index;m++)
		m_bonetable[m]=NULL;
	m_bonetable[index]=set;
	}


void GPGCharacter::GenerateBoneLinks(long defaultnumber)
	{
	ForEachNode(GPGCHILDFIRST,&GPGCharacter::CreateLinksOnNode,&defaultnumber);
	GetSkin()->RelinkWeights();
	GetSkin()->FilterWeights();

	RecalcLinks();
	ForEachNodeTransformed(GPGPARENTFIRST|GPGSTORE_XFORM,NULL);
	ForEachNode(GPGCHILDFIRST,&GPGCharacter::CalcLinkReferencesOnNode);

	GetSkin()->ComputeVertexOffsets();
	}


BOOL GPGCharacter::CreateLinksOnNode(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	long *defaultnumber=NULL;
	GPGRESULT result=state.CopyPointerTo(&defaultnumber);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(defaultnumber);

#if TRUE
	// reject anything not on side of body

	const char *buffer=node.Name().Raw8();
	if(strstr(buffer," R ")==NULL && strstr(buffer," L ")==NULL)
		return false;

	if(strstr(buffer,"Clavicle")!=NULL)
		return false;
#endif

	// don't create links on a link
	if(!((GPGBoneNode &)node).IsBoneLink())
		((GPGBoneNode &)node).CreateBoneLinks(*defaultnumber);

	return false;
	}


BOOL GPGCharacter::CalcLinkReferencesOnNode(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	((GPGBoneNode &)node).CalcBoneLinkReferences();
	return false;
	}


void GPGCharacter::RecalcLinks(void)
	{
	ForEachNode(GPGPARENTFIRST,&GPGCharacter::RecalcLinksOnNode);
	}


BOOL GPGCharacter::RecalcLinksOnNode(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	((GPGBoneNode &)node).CalcBoneLinks();
	return false;
	}


void GPGCharacter::StoreReferencePositions(void)
	{
	ForEachNodeTransformed(GPGPARENTFIRST,&GPGCharacter::StoreReferenceForBone);
	}


BOOL GPGCharacter::StoreReferenceForBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	//* store current orientation as the reference
	node.StoreReferenceTransform(transform);
//	node.GlobalReferenceRotation()=transform.QuaternionConst();
	node.LocalReferenceRotation()=node.RotationConst();
	node.ReferenceDisplacement()=node.DisplacementConst();
	return false;
	}



/******************************************************************************
	non-link bones
******************************************************************************/
long GPGCharacter::CountRealBones(void)
	{
	long bones=0;

	ForEachNode(GPGCHILDFIRST,&GPGCharacter::CountRealBone,&bones);

	return bones;
	}


BOOL GPGCharacter::CountRealBone(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	GPGBoneNode &bonenode=(GPGBoneNode &)node;

	if(bonenode.IsBoneLink())
		return false;

	long *bones=NULL;
	GPGRESULT result=state.CopyPointerTo(&bones);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(bones);

	(*bones)++;
	return false;
	}



#define GPGIK_INCREMENTAL	TRUE
void GPGCharacter::ApplyIK(void)
	{
	GPGIKModes &ik_modes=GetIKModes();

	// FUTURE try using a delta<threshhold check instead of fixed number
	// of iterations
	U32 iterations=ik_modes.GetIterations();

	if(!ik_modes.GetIncremental())
		ResetToReference();

	U32 m;
	for(m=0;m<iterations;m++)
		ForEachNodeTransformed(GPGCHILDFIRST|GPGSTORE_XFORM,
											&GPGCharacter::KineBone,&ik_modes);

//	if(ik_modes.GetLimitAngVelocity())
//		ForEachNode(GPGCHILDFIRST,&GPGCharacter::StoreIKData);

	// Is this redundant?
	RecalcLinks();
	}


BOOL GPGCharacter::StoreIKData(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	GPGBoneNode &bonenode=(GPGBoneNode &)node;

	// FIXME
	GPGASSERT(0);
//	bonenode.LastRotation()=bonenode.RotationConst();
	return false;
	}


BOOL GPGCharacter::KineBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	GPGBoneNodeList &effectionlist=node.Effections();

	long effections=effectionlist.GetNumberElements();
	if(!effections)
		return false;

	GPGIKModes *ik_modes=NULL;
	GPGRESULT result=state.CopyPointerTo(&ik_modes);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(ik_modes);

	GPGBoneNode &thisbone=(GPGBoneNode &)node;

	GPGBoneNode *effected_bone;
	GPGListContext context;

	// FUTURE try more flexible weighting
	F32 scalar=1.0f/(float)effections;

	GPGQuaternion change;
	change.MakeIdentity();

	// FUTURE can move calcs for current out of loop

	effectionlist.ToHead(context);
	while( (effected_bone=effectionlist.PostIncrement(context)) != NULL)
		{
		if(!effected_bone->IsEffected())
			continue;

		GPGVector3 effected;

		GPGVector3 &effector=effected_bone->Effector();

		GPGVector3 tip(effected_bone->GetLength(),0.0f,0.0f);
		effected_bone->StoredTransform().TransformVector(tip,effected);

		// everything in world is reverse-transformed to local space
		GPGVector3 current,desired;

		thisbone.StoredTransform().ReverseTransformVector(effected,current);
		thisbone.StoredTransform().ReverseTransformVector(effector,desired);

//#if _DEBUG
#if TRUE
		//* draw some lines for debugging
		GPGVector3 here;
		thisbone.StoredTransform().CopyTranslation(here);

		GPGVector3 world_current,world_desired;
		thisbone.StoredTransform().TransformVector(current,world_current);
		thisbone.StoredTransform().TransformVector(desired,world_desired);

		bool effector_here= (&thisbone==effected_bone);
		GPGCharacter *character=thisbone.RootCharacter();
		character->DrawLine(effector_here? 0: 2,here,world_current);
		character->DrawLine(effector_here? 1: 3,here,world_desired);
#endif

		F32 cur_mag2=current.CalcMagnitudeSquared();
		F32 des_mag2=desired.CalcMagnitudeSquared();

		if(cur_mag2<0.1f || des_mag2<0.1f)
			continue;

		current.Scale(1.0f/GPGSQRT(cur_mag2));
		desired.Scale(1.0f/GPGSQRT(des_mag2));

		GPGQuaternion delta;
		delta.MakeRotation(current,desired);

		// divide influence by number of effections
		delta.ScaleAngle(scalar);

		change.Rotate(delta);
/*
		GPGLOG("%s current %s\n  desired %s\n  delta %s\n  change %s\n",
							thisbone.Name(),
							current.Out(true).Raw(),
							desired.Out(true).Raw(),
							delta.Out(true).Raw(),
							change.Out(true).Raw());
*/
		}

	//* limit angular velocity
	if(ik_modes->GetLimitAngVelocity())
		{
		float radians;
		GPGVector3 axis;

		change.ComputeAngleAxis(radians,axis);

		float maxangvel=ik_modes->GetMaxAngVelocity();
		if(radians>maxangvel)
			{
			radians=maxangvel;
			change.MakeRotation(radians,axis);
			}
		}

	//* apply change
	thisbone.Rotation().Rotate(change);

#if FALSE
	//* TEMP no constraints
	thisbone.RootCharacter()->ForEachNodeTransformed(
												GPGCHILDFIRST|GPGSTORE_XFORM);
	return false;
#endif

	//*				`constrain joint angles'

	GPGConstraints &rot=thisbone.RotationalConstraints();

	bool &world_relative=rot.WorldRelative();

	GPGEuler euler;
	GPGQuaternion inverse,local_delta;
	GPGCoreNode *parent=node.Parent();

#if GPGKINE_DEBUG
	GPGEuler dataeuler;
#endif

	if(world_relative)
		{
		// get global rotation without recomputing m_transforms
		GPGQuaternion global_rot;
		global_rot.Multiply(parent->StoredTransform().QuaternionConst(),
												thisbone.RotationConst());

		GPGQuaternion parent_delta,global_delta,mid;
		inverse.Invert(parent->StoredTransform().QuaternionConst());
		parent_delta.Multiply(parent->GlobalReferenceRotationConst(),inverse);

		mid.Multiply(parent_delta,global_rot);
		inverse.Invert(thisbone.GlobalReferenceRotationConst());
		global_delta.Multiply(mid,inverse);

		euler=global_delta;

#if GPGKINE_DEBUG
		GPGLOG("parent Gref %s\n",
				parent->GlobalReferenceRotationConst().Out(true).Raw());
		dataeuler=parent->GlobalReferenceRotationConst();
		GPGLOG(" euler %s\n",dataeuler.Out(true).Raw());

		GPGLOG("parent Gcur %s\n",
					parent->StoredTransform().QuaternionConst()
					.Out(true).Raw());
		dataeuler=parent->StoredTransform().QuaternionConst();
		GPGLOG(" euler %s\n",dataeuler.Out(true).Raw());

		GPGLOG("child Gref %s\n",
				thisbone.GlobalReferenceRotationConst().Out(true).Raw());
		dataeuler=thisbone.GlobalReferenceRotationConst();
		GPGLOG(" euler %s\n",dataeuler.Out(true).Raw());

		GPGLOG("child Gcur %s\n",global_rot.Out(true).Raw());
		dataeuler=global_rot;
		GPGLOG(" euler %s\n",dataeuler.Out(true).Raw());

		GPGLOG("child Gdel %s\n",global_delta.Out(true).Raw());
		dataeuler=global_delta;
		GPGLOG(" euler %s\n",dataeuler.Out(true).Raw());
#endif
		}
	else
		{
		//* get rotation delta from reference
		inverse.Invert(thisbone.LocalReferenceRotation());
		local_delta.Multiply(inverse,thisbone.RotationConst());

		euler=local_delta;
		}

#if GPGKINE_DEBUG
	GPGLOG("%s Before %s\n",thisbone.Name().Raw8(),euler.Out(true).Raw());
	GPGLOG("min %s\n",rot.Min().Out(true).Raw());
	GPGLOG("max %s\n",rot.Max().Out(true).Raw());
#endif

	//* FUTURE also check alternate interpretation where mag of euler Y >90deg

#if GPGKINE_CONSTRAIN
	GPGEuler eulerRef=thisbone.LocalReferenceRotation();
#if GPGKINE_DEBUG
	GPGLOG("localRef %s\n",thisbone.LocalReferenceRotation().Out(true).Raw());
	GPGLOG("eulerRef %s\n",eulerRef.Out(true).Raw());
	GPGQuaternion testquat;
	euler.ConvertToQuaternion(testquat);
	GPGLOG("testquat %s\n",testquat.Out(true).Raw());
#endif
	//* apply limits
	float eulerY=euler[1];
	float min,max,mindiff,maxdiff;
	int m;
	for(m=0;m<3;m++)
		{
#if GPGKINE_DEBUG
		GPGLOG(" [%d]",m);
#endif

		if(rot.Active()[m] && !rot.Limited()[m])
			{
#if GPGKINE_DEBUG
			GPGLOG(" active-unlimited");
#endif
			continue;
			}

		if(!rot.Active()[m])
			{
#if GPGKINE_DEBUG
			GPGLOG(" inactive");
#endif
//			euler[m]=eulerRef[m];
			euler[m]=0.0f;
			}
		else
			{
			min=rot.Min()[m];
			max=rot.Max()[m];

			// swap if neccessary
			if(min>max)
				{
				min=max;
				max=rot.Min()[m];
				}

			//* if Y near +-90 deg, X and Z are unstable -> use previous
			if(m!=1)
				{
				// FUTURE paramaterize
				const float ymargin=0.05f;	// radians

				float yd90;
				if(eulerY>0.0f)
					yd90=(float)fabs(eulerY-(90.0f*GPGTO_RAD));
				else
					yd90=(float)fabs(eulerY+(90.0f*GPGTO_RAD));
				if(yd90<ymargin)
					{
#if GPGKINE_DEBUG
					GPGLOG(" lock");
#endif
					euler[m]=rot.LastEuler()[m];
					continue;
					}
#if GPGKINE_DEBUG
				else
					GPGLOG(" (%.6G)",yd90);
#endif
				}

			float euler2=euler[m];
			float min2=min;
			while(euler2<=0)
				euler2+=(GPGPI*2.0f);
			while(min2<=0)
				min2+=(GPGPI*2.0f);
			if(euler2<=max || euler2>=min2)
				{
#if GPGKINE_DEBUG
				GPGLOG(" ok");
#endif
				rot.Bias()[m]=2;
				continue;
				}

			mindiff=min-euler[m];
			maxdiff=euler[m]-max;

#if FALSE
			while(mindiff<-GPGPI)
				mindiff+=(GPGPI*2.0f);
			while(mindiff>=GPGPI)
				mindiff-=(GPGPI*2.0f);

			while(maxdiff<-GPGPI)
				maxdiff+=(GPGPI*2.0f);
			while(maxdiff>GPGPI)
				maxdiff-=(GPGPI*2.0f);
#else
			while(mindiff<0)
				mindiff+=(GPGPI*2.0f);
			while(mindiff>=GPGPI)
				mindiff-=(GPGPI*2.0f);

			while(maxdiff<0)
				maxdiff+=(GPGPI*2.0f);
			while(maxdiff>GPGPI)
				maxdiff-=(GPGPI*2.0f);

			mindiff=(float)fabs(mindiff);
			maxdiff=(float)fabs(maxdiff);
#endif

#if TRUE	// GPGIK_INCREMENTAL
			//* use historical bias to avoid alternating incremental solutions
			// this is where a solution far outside the constraints switches
			// between which constraints it is closest to
			// anything over PI is pretty much absolute: no switching
			// until it passes through a valid solution
			const float biasangle=10.0f;

			if(rot.Bias()[m]==0)
				{
				if(maxdiff>0.0f)
					maxdiff+=biasangle;
				}
			else if(rot.Bias()[m]==1)
				{
				if(mindiff>0.0f)
					mindiff+=biasangle;
				}
#endif

#if GPGKINE_DEBUG
			GPGLOG(" %d,%.6G,%.6G",rot.Bias()[m],mindiff,maxdiff);
#endif

			//* figure out which is closest (think circular)
			//  closest is smallest of non-zero values
			if(maxdiff<mindiff)
				{
				euler[m]=max;
				rot.Bias()[m]=1;
				}
			else
				{
				euler[m]=min;
				rot.Bias()[m]=0;
				}
			}
		}
#endif

#if GPGKINE_DEBUG
	GPGLOG("\nAfter %s\n",euler.Out(true).Raw());
#endif

	rot.LastEuler()=euler;

	//* return from delta to reference
	GPGQuaternion constrained;
	if(world_relative)
		{
		// convert world to local
		GPGQuaternion global_delta,global_rot,inverse;

		euler.ConvertToQuaternion(global_delta);

#if TRUE
		GPGQuaternion mid;
		inverse.Invert(parent->GlobalReferenceRotationConst());

/*
		constrained.Multiply(inverse,global_delta);
*/
		mid.Multiply(inverse,global_delta);
		constrained.Multiply(mid,thisbone.GlobalReferenceRotationConst());

#if GPGKINE_DEBUG
		GPGLOG("global_delta %s\n",global_delta.Out(true).Raw());
		dataeuler=global_delta;
		GPGLOG(" euler %s\n",dataeuler.Out(true).Raw());
#endif
		/*
		GPGQuaternion mid;
		mid.Multiply(global_delta,
						parent->StoredTransform().QuaternionConst());
		global_rot.Multiply(mid,thisbone.LocalReferenceRotation());

		inverse.Invert(parent->StoredTransform().QuaternionConst());
		constrained.Multiply(inverse,global_rot);
		*/
#else
		global_rot.Multiply(global_delta,
								thisbone.GlobalReferenceRotationConst());

		inverse.Invert(parent->StoredTransform().QuaternionConst());
		constrained.Multiply(inverse,global_rot);
#endif
		}
	else
		{
		euler.ConvertToQuaternion(local_delta);

		constrained.Multiply(thisbone.LocalReferenceRotation(),local_delta);
		}

	thisbone.Rotation()=constrained;

#if GPGKINE_DEBUG
	GPGLOG("Constrained %s\n",constrained.Out(true).Raw());
	dataeuler=constrained;
	GPGLOG(" euler %s\n\n",dataeuler.Out(true).Raw());
#endif

	//* recompute transforms
	// NOTE don't need to transform all, just this and descendants
	thisbone.RootCharacter()->ForEachNodeTransformed(
												GPGCHILDFIRST|GPGSTORE_XFORM);
	return false;
	}


void GPGCharacter::ResetToReference(void)
	{
	ForEachNode(GPGPARENTFIRST,&GPGCharacter::ResetToReferenceOnNode);
	}


BOOL GPGCharacter::ResetToReferenceOnNode(GPGCoreNode &node,GPGVoidStar state)
	{
	node.Displacement()=node.ReferenceDisplacement();
	node.Rotation()=node.LocalReferenceRotation();
	node.Scale().Set(1.0f,1.0f,1.0f);

	node.BlendDisplacement()=node.ReferenceDisplacement();
	node.BlendRotation()=node.LocalReferenceRotation();
	node.BlendScale().Set(1.0f,1.0f,1.0f);
	return false;
	}


void GPGCharacter::ResetToFullReach(void)
	{
	ForEachNode(GPGPARENTFIRST,&GPGCharacter::ResetToFullReachOnNode);
	}


BOOL GPGCharacter::ResetToFullReachOnNode(GPGCoreNode &node,GPGVoidStar state)
	{
	if(!node.Parent() || node.Parent()==node.RootCharacter())
		node.Displacement().Reset();
	else
		node.Displacement()=node.ReferenceDisplacement();

	node.Rotation().MakeIdentity();
	node.Scale().Set(1.0f,1.0f,1.0f);

	node.BlendDisplacement()=node.Displacement();
	node.BlendRotation().MakeIdentity();
	node.BlendScale().Set(1.0f,1.0f,1.0f);
	return false;
	}


void GPGCharacter::CalcNodeRotationsFromStoredTranslations(bool lookback)
	{
	GPGTransformStack &m_transforms=TransformStack();

	m_transforms.Push();

	m_transforms.GetCurrent().MakeIdentity();
	m_transforms.GetCurrent().Translate(DisplacementConst());
	m_transforms.GetCurrent().Rotate(RotationConst());

	CalcNodeRotations(lookback,*this);

	m_transforms.Pop();
	}


void GPGCharacter::CalcNodeRotations(bool lookback,GPGCoreNode &parent)
	{
	GPGCharacter *character=parent.RootCharacter();
	GPGTransformStack &m_transforms=character->TransformStack();

	GPGVector3 worldvector,localvector;
	GPGVector3 parent_location,child_location,grandchild_location;

	GPGVector3 xaxis(1.0f,0.0f,0.0f);
	GPGVector3 zero(0.0f,0.0f,0.0f);

	GPGBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	GPGBoneNode *node;
	GPGListContext context;
	float parentlength=0.0f;

	if(parent.IsBone())
		{
		node=(GPGBoneNode *)&parent;
		node->StoredTransform().CopyTranslation(parent_location);
		parentlength=node->GetLength();
		}
	else
		parent_location.Reset();

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
		{
		node->StoredTransform().CopyTranslation(child_location);

		if(lookback)
			{
			if(!parent.IsBone())
				{
				node->Displacement()=child_location;
				worldvector.Reset();
				localvector=xaxis;
				}
			else
				{
				worldvector.Subtract(child_location,parent_location);
				m_transforms.GetCurrent().ReverseTransformVector(child_location,
																localvector);
				}
			}
		else
			{
			GPGBoneNode *grandchild=node->Children().GetHead();
			if(grandchild)
				{
				grandchild->StoredTransform()
										.CopyTranslation(grandchild_location);
				worldvector.Subtract(grandchild_location,child_location);
				m_transforms.GetCurrent().ReverseRotateVector(worldvector,
																localvector);
				}
			else
				{
				worldvector.Reset();
				localvector=xaxis;
				}
			}

		localvector.Normalize();

		node->Length()=worldvector.CalcMagnitude();

		if(lookback)
			{
			//* for load ASE: create rotations
			if(node->GetLength()<0.1)
				node->Rotation().MakeIdentity();
			else
				node->Rotation().MakeRotation(xaxis,localvector);
			}
		else
			{
			//* for load ASH: find/create displacements
			m_transforms.GetCurrent().ReverseTransformVector(child_location,
																localvector);
			if(localvector.IsApproximately(zero,0.1f))
				node->Displacement().Reset();
			else
				node->Displacement()=localvector;
			}

		//* do child's new transform
		m_transforms.Push();

		m_transforms.GetCurrent().Translate(node->DisplacementConst());
		m_transforms.GetCurrent().Rotate(node->RotationConst());
		m_transforms.GetCurrent().Translate(node->GetLength(),0.0f,0.0f);

		CalcNodeRotations(lookback,*node);

		m_transforms.Pop();
		}
	}


void GPGCharacter::CalculateImplantReferences(void)
	{
	ForEachNodeTransformed(GPGPARENTFIRST|GPGSTORE_XFORM,
							&GPGCharacter::CalculateImplantReferenceForBone);
	}


BOOL GPGCharacter::CalculateImplantReferenceForBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	GPGBoneNode &bnode=(GPGBoneNode &)node;

	// requires stored m_transforms
	bnode.CalcImplantReference();
	return false;
	}
