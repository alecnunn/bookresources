
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


#include "Bones/GPGBackBones.h"
#ifdef GPG_USE_HPARSER


#define	GPG_BP_PROGRESSGRAIN	1000



/******************************************************************************
GPGBoneParser::GPGBoneParser(void): m_vw_parsetable(*this)

******************************************************************************/
GPGBoneParser::GPGBoneParser(void): m_vw_parsetable(*this)
	{
	}



/******************************************************************************
void GPGBoneParser::TransferBoneData(GPGCharacter &character,bool justweights)

	NOTICE: TransferBoneNode() will receive a (GPGCharacter *),
		not a (GPGBoneNode *) as might be expected.
		The BoneNodes need to allocate themselves and
		attach to the character.

	justweights restricts input to just vertex weights

**********************************t********************************************/
void GPGBoneParser::TransferBoneData(GPGCharacter &character,bool justweights)
	{
#if GPGPARSE_DEBUG
	GPGLOG("TransferBoneData()");
#endif

	const char *charname=NULL;
	long size=1;
	if(GetParserMode()==OUTPUT)
		{
		charname=character.NameConst();
		size=character.GetVertexWeights()->GetNumberElements();
		}

	GPGParseTable parsetable(*this);

	if(!justweights)
		parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferCharacter,
			&character,"Character",charname);

	if(GetParserMode()==INPUT)
		{
		if(!justweights)
			{
			character.ClearBoneTable();

			parsetable.Expect(
				(GPGTransferFunction)&GPGBoneParser::TransferBoneNode,
				&character,"Bone",NULL);
			}

		parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferVertexWeights,
			character.GetVertexWeights(),
			"VertexWeights",NULL);
		}

	TransferBlock(parsetable);

	if(GetParserMode()==OUTPUT)
		{
		// print out all bone nodes independently
		void *state=this;
		character.ForEachNode(GPGPARENTFIRST,
								&GPGBoneParser::OutputBoneNode,state);

		// Output Weights: this could be combined with TransferBlock() above,
		// but file looks better if weights are written after bones
		GPGParseTable parsetable2(*this);
		parsetable2.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferVertexWeights,
			character.GetVertexWeights(),
			"VertexWeights",NULL,size);

		TransferBlock(parsetable2);
		}

	if(GetParserMode()==INPUT && !justweights)
		{
		// get new pointers for names we loaded
		m_bonemaplist.CleanUp(character);

		// use load position as reference
		character.StoreReferencePositions();
		}
	}


BOOL GPGBoneParser::OutputBoneNode(GPGCoreNode &corenode,GPGVoidStar state)
	{
	if(!corenode.IsBone())
		return false;

	GPGBoneNode *bonenode=(GPGBoneNode *)&corenode;

	if(bonenode->IsBoneLink())
		return false;

	GPGBoneParser *parser=NULL;
	GPGRESULT result=state.CopyPointerTo(&parser);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(parser);

	GPGParseTable parsetable(*parser);
	parsetable.Expect(
				(GPGTransferFunction)&GPGBoneParser::TransferBoneNode,
				(void *)bonenode,"Bone",bonenode->NameConst(),1);

	parser->TransferBlock(parsetable);
	return false;
	}



/******************************************************************************
void GPGBoneParser::TransferCharacter(GPGParseEntry &entry)

******************************************************************************/
void GPGBoneParser::TransferCharacter(GPGParseEntry &entry)
	{
#if GPGPARSE_DEBUG
	GPGLOG("TransferCharacter()");
#endif

	GPGCharacter *character=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&character);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(character);

	GPGCoreNode *corenode=(GPGCoreNode *)character;

	if(GetParserMode()==INPUT)
		character->SetName(entry.GetName());

	I32 bones=0;

	long size=1;
	if(GetParserMode()==OUTPUT)
		{
		bones=character->CountRealBones();
		size=corenode->Effections().GetNumberElements();
		}
	I32 effections=size;
	U32 skip=0;

	GPGParseTable parsetable(*this);
	parsetable.SetNewlining(true);

	parsetable.Expect(
			&GPGParser::TransferSkip,
			&skip,"skip","length",1,32);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&bones,"int","bones",1,16);
/*
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&effections,"int","effections",1);
	parsetable.Expect(
		(GPGTransferFunction)&GPGBoneParser::TransferEffection,
		&(corenode->Effections()),"Effection","effection",size);
*/

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());
	}



/******************************************************************************
void GPGBoneParser::TransferEffection(GPGParseEntry &entry)

******************************************************************************/
void GPGBoneParser::TransferEffection(GPGParseEntry &entry)
	{
#if GPGPARSE_DEBUG
	GPGLOG("TransferEffection()");
#endif

	GPGBoneNodeList *bonenodelist=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&bonenodelist);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(bonenodelist);

	char buffer[GPGPARSE_NAMELENGTH];

	GPGParseTable parsetable(*this);
	parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferBoneName,
			buffer,"string","bonename",1);

/*
	parsetable.Expect(
			&GPGParser::TransferF32s,
			???,"float","weight",1,16);
*/

//	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());

	if(GetParserMode()==INPUT)
		{
		GPGBoneNode **pointer=bonenodelist->Append(NULL);
		m_bonemaplist.Add(pointer,buffer);

#if GPGPARSE_DEBUG
		GPGLOG("BoneName=%s^",buffer);
#endif
		}
	}



/******************************************************************************
void GPGBoneParser::InputBoneName(GPGParseEntry &entry)

	more manual than most transfers since not just copying data

	WARNING won't work with binary

******************************************************************************/
void GPGBoneParser::InputBoneName(GPGParseEntry &entry)
	{
#if GPGPARSE_DEBUG
	GPGLOG("InputBoneName()");
#endif

	char *buffer=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&buffer);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(buffer);

	GPGParseTable parsetable(*this);
	if(parsetable.InputBlock())
		{
		InputStringArg(buffer,GPGPARSE_NAMELENGTH,TRUE);
		GetParseFormat()->ConsumeDelimitation();

		// dispose of remaining data
		while(parsetable.InputBlock())
			InputDataBlock(NULL,0);
		}
	}



/******************************************************************************
void GPGBoneParser::TransferVertexWeights(GPGParseEntry &entry)

******************************************************************************/
void GPGBoneParser::TransferVertexWeights(GPGParseEntry &entry)
	{
	GPGVertexWeights *vertexweights=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&vertexweights);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(vertexweights);

	long size=entry.GetSize();
	I32 number=size;
	U32 skip=0;

	GPGParseTable parsetable(*this);
	parsetable.SetNewlining(true);

	parsetable.Expect(
		&GPGParser::TransferSkip,
		&skip,"skip","length",1,32);
	parsetable.Expect(
		&GPGParser::TransferI32s,
		&number,"int","entries",1,32);
	parsetable.ExpectArray(
		(GPGTransferFunction)&GPGBoneParser::TransferVertexWeight,
		vertexweights,"VertexWeight","vertexweight",size,0,"entries");

	m_vw_parsetable.Expect(
		&GPGParser::TransferI32s,
		NULL,"int","boneid",1,16);
	m_vw_parsetable.Expect(
		&GPGParser::TransferI32s,
		NULL,"int","mesh",1,8);
	m_vw_parsetable.Expect(
		&GPGParser::TransferI32s,
		NULL,"int","vertex",1,16);
	m_vw_parsetable.Expect(
		&GPGParser::TransferF32s,
		NULL,"float","weight",1,16);

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	}



/******************************************************************************
void GPGBoneParser::TransferVertexWeight(GPGParseEntry &entry)

	uses lots of space if formatted in XML

******************************************************************************/
void GPGBoneParser::TransferVertexWeight(GPGParseEntry &entry)
	{
	GPGVertexWeight *vertexweight=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&vertexweight);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(vertexweight);

	long index=entry.GetElementCount();
	if( !(index%GPG_BP_PROGRESSGRAIN) )
		{
		if(GetParserMode()==OUTPUT)
			sprintf(progresstext,"Save Weights %d",index);
		else
			sprintf(progresstext,"Load Weights %d",index);
		GPGCharacter::StateProgress(progresstext);
		}

#if TRUE
	GPGParseTable parsetable(*this);

	parsetable.Expect(
		&GPGParser::TransferI32s,
		&vertexweight->BoneIndex(),"int","boneid",1,16);
	parsetable.Expect(
		&GPGParser::TransferI32s,
		&vertexweight->MeshIndex(),"int","mesh",1,8);
	parsetable.Expect(
		&GPGParser::TransferI32s,
		&vertexweight->VertexIndex(),"int","vertex",1,16);
	parsetable.Expect(
		&GPGParser::TransferF32s,
		&vertexweight->BoneWeight(),"float","weight",1,16);

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	}
#else
	m_vw_parsetable.RememberOrder();
	m_vw_parsetable.AlterPointer(0,&vertexweight->BoneIndex());
	m_vw_parsetable.AlterPointer(1,&vertexweight->MeshIndex());
	m_vw_parsetable.AlterPointer(2,&vertexweight->VertexIndex());
	m_vw_parsetable.AlterPointer(3,&vertexweight->BoneWeight());

	WhenOutput_Append("\n");
	TransferBlock(m_vw_parsetable);
	}
#endif



/******************************************************************************
void GPGBoneParser::TransferBoneNode(GPGParseEntry &entry)

******************************************************************************/
void GPGBoneParser::TransferBoneNode(GPGParseEntry &entry)
	{
#if GPGPARSE_DEBUG
	GPGLOG("TransferBoneNode()");
#endif

	GPGCharacter *character=NULL;
	GPGBoneNode *bonenode=NULL;

	I32 parentid;

	if(GetParserMode()==INPUT)
		{
		GPGRESULT result=entry.GetInstance().CopyPointerTo(&character);
		GPGASSERT(result==GPG_OK);
		GPGASSERT(character);

		bonenode=new GPGBoneNode(character);

		bonenode->SetName(entry.GetName());
		}
	else
		{
		GPGRESULT result=entry.GetInstance().CopyPointerTo(&bonenode);
		GPGASSERT(result==GPG_OK);
		GPGASSERT(bonenode);

		character=(GPGCharacter *)bonenode->RootCharacter();

		GPGASSERT(bonenode->Parent());
		if(bonenode->Parent()->IsBone())
			{
			GPGBoneNode *parentnode=(GPGBoneNode *)bonenode->Parent();
			parentid=parentnode->BoneIndex();
			}
		else
			parentid= -1;
		}

	GPGCoreNode *corenode=(GPGCoreNode *)bonenode;

	long size=1;
	if(GetParserMode()==OUTPUT)
		size=corenode->Effections().GetNumberElements();
	I32 effections=size;
	U32 skip=0;

	GPGParseTable parsetable(*this);
	parsetable.SetNewlining(true);

	parsetable.Expect(
			&GPGParser::TransferSkip,
			&skip,"skip","length",1,32);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&(bonenode->BoneIndex()),"int","boneid",1,16);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&parentid,"int","parentid",1,16);
	parsetable.Expect(
			&GPGParser::TransferF32s,
			&(bonenode->Length()),"float","length",1,32);
	parsetable.Expect(
			&GPGParser::TransferVector3,
			&(bonenode->Displacement()),"vector3","displacement",1,32,"",3);
	parsetable.Expect(
			&GPGParser::TransferQuaternion,
			&(bonenode->Rotation()),"quaternion","rotation",1,16,"",4);
	parsetable.Expect(
			&GPGParser::TransferBools,
			&(bonenode->Influential()),"bool","influential",1,8);
	parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferJoint,
			bonenode,"Joint","joint",1);
/*
	parsetable.Expect(
			&GPGParser::TransferBools,
			&(bonenode->Effected()),"bool","effected",1,8);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&effections,"int","effections",1,8);
	parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferEffection,
			&(bonenode->Effections()),"Effection","effection",size);
*/
	parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferConstraints,
			&(bonenode->RotationalConstraints()),"Constraints","rotational");
	parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferConstraints,
			&(bonenode->PositionalConstraints()),"Constraints","positional");
#if FALSE
	parsetable.Expect(
			(GPGTransferFunction)&GPGBoneParser::TransferCoreNode,
			corenode,"CoreNode",NULL);
#endif

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());

/*
if(GetParserMode()==INPUT)
bonenode->Rotation()[0]= -bonenode->Rotation()[0];
*/

	if(GetParserMode()==INPUT)
		{
//		GPGCoreNode *parent=character->FindDescendentByName(parentname);
		GPGCoreNode *parent=character;
		if(parentid>=0)
			parent=character->LookupBoneIndex(parentid);

		character->SetBoneAtIndex(bonenode->BoneIndex(),bonenode);

		if(parent)
			parent->AppendChild(bonenode);
		else
			delete bonenode;
		}
	}



/******************************************************************************
void GPGBoneParser::TransferJoint(GPGParseEntry &entry)

******************************************************************************/
void GPGBoneParser::TransferJoint(GPGParseEntry &entry)
	{
#if GPGPARSE_DEBUG
	GPGLOG("TransferConstraints()");
#endif

	GPGBoneNode *bonenode=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&bonenode);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(bonenode);

	GPGBoneLinks &bonelinks=bonenode->BoneLinks();
	GPGCylinder &cylinder=bonenode->CylindricalBounds();

	GPGParseTable parsetable(*this);
	parsetable.SetNewlining(true);

	parsetable.Expect(
			&GPGParser::TransferI32s,
			&(bonenode->AutoLink()),"bool","autolink",1,8);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&(bonelinks.GetNumberLinks()),"int","numlinks",1,8);
	parsetable.Expect(
			&GPGParser::TransferF32s,
			&(bonelinks.GetLinkLength()),"float","linklength",1,32);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&(bonenode->AutoJoint()),"bool","autojoint",1,8);
	parsetable.Expect(
			&GPGParser::TransferVector3,
			&(cylinder.GetCenter(0)),"vector3","center",2,32,"",3);
	parsetable.Expect(
			&GPGParser::TransferVector3,
			&(cylinder.GetScale(0)),"vector3","scale",2,32,"",3);

	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());
	}



/******************************************************************************
void GPGBoneParser::TransferConstraints(GPGParseEntry &entry)

******************************************************************************/
void GPGBoneParser::TransferConstraints(GPGParseEntry &entry)
	{
#if GPGPARSE_DEBUG
	GPGLOG("TransferConstraints()");
#endif

	GPGConstraints *constraints=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&constraints);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(constraints);

	GPGParseTable parsetable(*this);
	parsetable.SetNewlining(true);

	parsetable.Expect(
			&GPGParser::TransferBools,
			&(constraints->WorldRelative()),"bool","world",1,8);
	parsetable.Expect(
			&GPGParser::TransferBools,
			constraints->Active(),"bool","active",3,8);
	parsetable.Expect(
			&GPGParser::TransferBools,
			constraints->Limited(),"bool","limit",3,8);
	parsetable.Expect(
			&GPGParser::TransferVector3,
//			constraints->Min().Raw(),"float","min",3);
			&(constraints->Min()),"float","min",1,32,"",3);
	parsetable.Expect(
			&GPGParser::TransferVector3,
//			constraints->Max().Raw(),"float","max",3);
			&(constraints->Max()),"float","max",1,32,"",3);

	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());
	}

#endif /* GPG_USE_HPARSER */
