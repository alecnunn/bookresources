
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



#if GPGALLOW_STREAMS
/******************************************************************************
std::ostream &operator<<(std::ostream &out,const GPGBoneNode &character)

******************************************************************************/
std::ostream &operator<<(std::ostream &out,const GPGBoneNode &character)
	{
	return character.Print(out,0);
	}



/******************************************************************************
std::ostream &GPGBoneNode::Print(std::ostream &out) const

******************************************************************************/
std::ostream &GPGBoneNode::Print(std::ostream &out,long indention) const
	{
	GPGIndent indent(indention);
	GPGIndent indent2(indention+1);

	out<<"\""<<NameConstConst()<<"\" BoneNode\n"<<indent2<<"{\n";

#if GPGPRINT_VERBOSE
	out<<indent2<<"(0x"<<std::hex<<this<<")\n";
#endif

	out<<indent2<<"\"parent\" \""<<Parent()->NameConst()<<"\";\n";
	out<<indent2<<"\"m_length\" float "<<m_length<<";\n";
	out<<indent2<<"@";
	GPGCoreNode::Print(out,indention+1);

	out<<indent2<<"\"positional\" ";
	positional.Print(out,indention+1);

	out<<indent2<<"\"rotational\" ";
	rotational.Print(out,indention+1);

	return out<<indent2<<"}\n";
	}
#endif



/******************************************************************************
GPGString GPGBoneNode::Out(BOOL verbose) const

******************************************************************************/
GPGString GPGBoneNode::Out(BOOL verbose) const
	{
	GPGString result("GPGBoneNode::Out() not implemented");
	return result;
	}



/******************************************************************************
******************************************************************************/
void GPGBoneNode::GPGBoneNodeShare::Reset(void)
	{
	m_autojoint=true;
	m_autolink=true;
	m_boneid=0;
	m_linkboneid= -1;
	m_length=0.0f;
	m_influential=true;
	m_implant_len=0.0f;
	m_implant_rot.MakeIdentity();
	m_implant_lenref=0.0f;
	m_implant_rotref.MakeIdentity();
	m_isbonelink=false;
	}


/******************************************************************************
******************************************************************************/
void GPGBoneNode::ResetBone(void)
	{
	SetIsBone(true);
	m_picked=false;
	m_effected=false;
	m_effector.Reset();
	}



/******************************************************************************
BOOL GPGBoneNode::HasAncestor(GPGCoreNode *node)

******************************************************************************/
BOOL GPGBoneNode::HasAncestor(GPGCoreNode *node)
	{
	if(Parent()==node)
		return true;
	else if(!Parent()->IsBone())
		return false;

	GPGBoneNode *bone=(GPGBoneNode *)Parent();
	return bone->HasAncestor(node);
	}



/******************************************************************************
void GPGBoneNode::CalcImplant(void)

	requires stored transforms

	parent*implant=child -> implant=(parent^-1)*child

******************************************************************************/
void GPGBoneNode::CalcImplant(void)
	{
	GPGCoreNode *parent=Parent();
	if(!parent || !parent->IsBone())
		return;

	const GPGVector3 zero(0.0f,0.0f,0.0f);
	GPGVector3 xaxis;
	xaxis.Set(1.0f,0.0f,0.0f);

#if FALSE
	// orient from parent's view (but biased to parent orientation)
	GPGVector3 child_location,localvector;

	StoredTransform().CopyTranslation(child_location);

	GPGTransform transform=parent->StoredTransform();
	transform.Translate(((GPGBoneNode *)parent)->GetLength(),0.0f,0.0f);
	transform.ReverseTransformVector(child_location,localvector);

	localvector.Normalize();
	m_implant_rot.MakeRotation(xaxis,localvector);
#else
	// orient from child's view
	GPGVector3 parent_local,parent_world;
	if(DisplacementConst().IsApproximately(zero,1e-3f))
		parent_local.Set(-1.0f,0.0f,0.0f);
	else
		parent_local.Set(((GPGBoneNode *)parent)->GetLength(),0.0f,0.0f);

	parent->StoredTransform().TransformVector(parent_local,parent_world);
	StoredTransform().ReverseTransformVector(parent_world,parent_local);

	GPGQuaternion local_rot;
	parent_local.Normalize();
	parent_local.Invert();
	local_rot.MakeRotation(xaxis,parent_local);
	ImplantRotation().Multiply(RotationConst(),local_rot);
#endif

	ImplantLength()=DisplacementConst().CalcMagnitude();
	}



/******************************************************************************
void GPGBoneNode::CalcImplantReference(void)

******************************************************************************/
void GPGBoneNode::CalcImplantReference(void)
	{
	CalcImplant();

	ImplantReferenceRotation()=ImplantRotation();
	ImplantReferenceLength()=ImplantLength();
	}



/******************************************************************************
void GPGBoneNode::CreateEffections(GPGCoreNode *base)

	set effections recursively up to given base node
	if base==NULL, goes all the way to the root-most node

	effections can be made to an unaffected bone;
	they are ignored until reactivated

******************************************************************************/
void GPGBoneNode::CreateEffections(GPGCoreNode *base)
	{
	GPGCoreNode *node=this;

	while(node!=NULL)
		{
		node->Effections().Append(this);

		if(node==base)
			break;

		node=node->Parent();
		}
	}



/******************************************************************************
void GPGBoneNode::CreateBoneLinks(long defaultnumber)

******************************************************************************/
void GPGBoneNode::CreateBoneLinks(long defaultnumber)
	{
	if(!Parent()->IsBone())
		return;

	if(IsAutoLink())
		{
//		BoneLinks().ResetSamples();
		BoneLinks().SetNumberLinks(defaultnumber);

		GPGCylinder &bounds=CylindricalBounds();
		GPGVector3 &jointscale=bounds.GetScale(0);
		float jointsize=0.5f*(jointscale[1]+jointscale[2]);

		float parentlength=((GPGBoneNode *)Parent())->GetLength();
		float childlength=GetLength();
		float implantlength=ImplantLength();

//		float totallength=GetLength()*0.2f+jointsize*1.0f;
		float totallength=GetLength()*0.3f+jointsize*1.5f;

		// limit length to half child AND half parent or implant
#if FALSE
		if(totallength>childlength)
			totallength=childlength;

		if(implantlength>0.0f)
			{
			if(totallength>implantlength)
				totallength=implantlength;
			}
		else if(totallength>parentlength)
			totallength=parentlength;
#endif

		BoneLinks().SetLinkLength(totallength);
		}

	BoneLinks().AllocateArray();

	long m,number=BoneLinks().GetArraySize();
	for(m=0;m<number;m++)
		{
		GPGBoneNode *newnode=new GPGBoneNode(RootCharacter());
		Parent()->AppendChild(newnode);
		BoneLinks().SetLink(m,newnode);
		long newindex=RootCharacter()->GetBoneTableSize();
		RootCharacter()->SetBoneAtIndex(newindex,newnode);

		char buffer[256];
		sprintf(buffer,"%s Link%d",NameConst().Raw8(),m);
		newnode->SetName(buffer);
		newnode->SetBoneIndex(newindex);
		newnode->LinkBoneIndex()=this->GetBoneIndex();
		newnode->SetBoneLink(true);
		}
	}



/******************************************************************************
void GPGBoneNode::CalcBoneLinkReferences(void)

******************************************************************************/
void GPGBoneNode::CalcBoneLinkReferences(void)
	{
	long m,number=BoneLinks().GetArraySize();
	for(m=0;m<number;m++)
		{
		GPGBoneNode *linknode=BoneLinks().GetLink(m);
		linknode->ReferenceDisplacement()=linknode->DisplacementConst();
		linknode->LocalReferenceRotation()=linknode->RotationConst();

		linknode->StoreReferenceTransform(linknode->StoredTransform());
		}
	}


/******************************************************************************
	link locations based on weighting between two paths
		first path follows bone X axes exactly
		second path is a straight line between attachment points
******************************************************************************/
void GPGBoneNode::CalcBoneLinks(void)
	{
	const long number=BoneLinks().GetArraySize();
	if(number<1)
		return;

	const float link_length=BoneLinks().GetLinkLength()/(float)number;

	GPGVector3 &dref=ReferenceDisplacement();
	const GPGVector3 &disp=DisplacementConst();

	/*** NOTE  The implant is a otherwise non-contributing psuedo-parent bone
		needed when the real parent is not directly connected to this child.
		The implant represents where a connected parent might be. */

	// start on implant
	const GPGQuaternion lref=ImplantRotation();

	const GPGQuaternion &lrot=RotationConst();

	GPGTransform transform;
	GPGVector3 fdisp,slide,result;
	slide.Reset();

	GPGVector3 slide0,sliden,slidemid,onbone;
	slide[0]=link_length*(0-(number+2)/2.0f+0.5f);
	transform=lref;
	transform.RotateVector(slide,slide0);

	slide[0]=link_length*((number+2)-1-(number+2)/2.0f+0.5f);
	transform=lrot;
	transform.RotateVector(slide,sliden);

	//* location
	long m;
	float tween,ratio,weight;
	for(m=0;m<number;m++)
		{
		ratio=(number>1)? m/(float)(number-1): 0.0f;	// from 0 to 1
		weight=0.5f-(float)fabs(ratio-0.5f);			// between paths
		tween=(m+1)/(float)(number+1);					// never 0 or 1

		//* account for bone displacement
		fdisp.Subtract(disp,dref);
		fdisp.Scale(tween);
		fdisp.Add(dref);

		transform=(m<number/2)? lref: lrot;

		slide[0]=link_length*(m-number/2.0f+0.5f);
		transform.RotateVector(slide,onbone);

		slidemid.Interpolate(tween,slide0,sliden);
		result.Interpolate(weight,onbone,slidemid);

		GPGBoneNode *linknode=BoneLinks().GetLink(m);
		linknode->SetLength(link_length);
		linknode->Displacement().Add(fdisp,result);
		}

	//* rotation
	GPGVector3 diff,prev,next,xaxis(1.0f,0.0f,0.0f);
	for(m=0;m<number;m++)
		{
		tween=(m+1)/(float)(number+1);
		GPGBoneNode *linknode=BoneLinks().GetLink(m);

		linknode->Rotation().Interpolate(tween,lref,lrot);
		}

	//* computed points are the bone centers, so we have to adjust
	//* since bone positions are stored as the base point
	for(m=0;m<number;m++)
		{
		tween=(m+1)/(float)(number+1);
		GPGBoneNode *linknode=BoneLinks().GetLink(m);

//		tween=powf(tween,1.0f);	// FUTURE explore and parameterize link power
//		linknode->RotationConst().Interpolate(tween,lref,lrot);

		slide[0]= -link_length/2.0f;
		transform=linknode->RotationConst();
		transform.RotateVector(slide,result);
		linknode->Displacement().Add(result);
		}
	}
