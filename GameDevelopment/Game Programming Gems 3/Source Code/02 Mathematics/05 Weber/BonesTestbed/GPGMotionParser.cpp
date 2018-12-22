
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



#define	GPGMOTIONPARSER_DEBUG	FALSE
#define GPG_MP_PROGRESSGRAIN	20




/******************************************************************************
GPGMotionParser::GPGMotionParser(void): m_frame_parsetable(*this)

******************************************************************************/
GPGMotionParser::GPGMotionParser(void): m_frame_parsetable(*this)
	{
	}



/******************************************************************************
void GPGMotionParser::TransferMotionData(GPGMotion &motion)

******************************************************************************/
void GPGMotionParser::TransferMotionData(GPGMotion &motion)
	{
	GPGParseTable parsetable(*this);
	long size=(GetParserMode()==OUTPUT)?
								motion.GetTracks().GetNumberElements(): 1;

	const char *motionname=NULL;
	if(GetParserMode()==OUTPUT)
		motionname=motion.GetName().Raw8();

	parsetable.Expect(
			(GPGTransferFunction)&GPGMotionParser::TransferMotion,
			&motion,"Motion",motionname,size);

	TransferBlock(parsetable);

#if GPGMOTIONPARSER_DEBUG
	if(GetParserMode()==INPUT)
		{
		long m,tracks=motion.GetTracks().GetNumberElements();

		GPGLOG("Motion has %d tracks:\n",tracks);
		for(m=0;m<tracks;m++)
			{
			GPGLOG(" %d \"%s\" %d frames\n",
							m,motion.GetTrackConst(m).GetNameConst(),
							motion.GetTrackConst(m).GetNumberElements());
			}
		}
#endif
	}



/******************************************************************************
void GPGMotionParser::TransferMotion(GPGParseEntry &entry)

******************************************************************************/
void GPGMotionParser::TransferMotion(GPGParseEntry &entry)
	{
	GPGMotion *motion=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&motion);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(motion);

	long size=entry.GetSize();
	I32 number=size;
	U32 skip=0;
	const char *tracklabel= (GetParserMode()==OUTPUT)? "track": NULL;

	if(GetParserMode()==INPUT)
		motion->SetName(entry.GetName());

	GPGParseTable parsetable(*this);

	parsetable.Expect(
			&GPGParser::TransferSkip,
			&skip,"skip","length",1,32);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&number,"int","entries",1,32);
	parsetable.ExpectArray(
			(GPGTransferFunction)&GPGMotionParser::TransferTrack,
			&(motion->GetTracks()),"Track",tracklabel,size,0,"entries");

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());
	}



/******************************************************************************
void GPGMotionParser::TransferTrack(GPGParseEntry &entry)

	WARNING does not sort frames, assumes correct order in file

******************************************************************************/
void GPGMotionParser::TransferTrack(GPGParseEntry &entry)
	{
	GPGKeyTrack *track=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&track);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(track);

	long index=entry.GetElementCount();
	if( !(index%GPG_MP_PROGRESSGRAIN) )
		{
		if(GetParserMode()==OUTPUT)
			sprintf(progresstext,"Save Motion Track %d",index);
		else
			sprintf(progresstext,"Load Motion Track %d",index);
		GPGCharacter::StateProgress(progresstext);
		}

	GPGList<GPGKeyFrame> *frames=track;
	long size=(GetParserMode()==OUTPUT)? frames->GetNumberElements(): 1;

	I32 number=size;
	const char *framelabel= (GetParserMode()==OUTPUT)? "frame": NULL;

	char name[GPGPARSE_NAMELENGTH];
	if(GetParserMode()==OUTPUT)
		strcpy(name,track->GetNameConst());

	GPGParseTable parsetable(*this);

	parsetable.Expect(
			&GPGParser::TransferString,
			(U8 *)name,"string","name",GPGPARSE_NAMELENGTH);
	parsetable.Expect(
			&GPGParser::TransferI32s,
			&number,"int","entries",1,32);
	parsetable.ExpectList(
			(GPGTransferFunction)&GPGMotionParser::TransferFrame,
			frames,"Frame",framelabel,size,0,"entries");

	m_frame_parsetable.Expect(
			&GPGParser::TransferF32s,
			NULL,"float","time",1,32);
	m_frame_parsetable.Expect(
			&GPGParser::TransferVector3,
			NULL,"vector3","displacement",1,32,"",3);
	m_frame_parsetable.Expect(
			&GPGParser::TransferQuaternion,
			NULL,"quaternion","rotation",1,16,"",4);

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
	WhenOutput_Indent(entry.GetIndention());

	if(GetParserMode()==INPUT)
		track->SetName(name);
	}



/******************************************************************************
void GPGMotionParser::TransferFrame(GPGParseEntry &entry)

******************************************************************************/
void GPGMotionParser::TransferFrame(GPGParseEntry &entry)
	{
	GPGKeyFrame *frame=NULL;
	GPGRESULT result=entry.GetInstance().CopyPointerTo(&frame);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(frame);

#if FALSE
	GPGParseTable parsetable(*this);
	parsetable.SetNewlining(true);

	parsetable.Expect(
			&GPGParser::TransferF32s,
			&(frame->GetTime()),"float","time",1,32);
	parsetable.Expect(
			&GPGParser::TransferVector3,
			&(frame->GetLocation()),"vector3","displacement",1,32,"",3);
	parsetable.Expect(
			&GPGParser::TransferQuaternion,
			&(frame->GetRotation()),"quaternion","rotation",1,16,"",4);

	WhenOutput_Append("\n");
	TransferBlock(parsetable);
#else
	m_frame_parsetable.RememberOrder();
	m_frame_parsetable.AlterPointer(0,&(frame->Time()));
	m_frame_parsetable.AlterPointer(1,&(frame->Location()));
	m_frame_parsetable.AlterPointer(2,&(frame->Rotation()));

	WhenOutput_Append("\n");
	TransferBlock(m_frame_parsetable);
#endif
	WhenOutput_Indent(entry.GetIndention());
	}

#endif /* GPG_USE_HPARSER */
