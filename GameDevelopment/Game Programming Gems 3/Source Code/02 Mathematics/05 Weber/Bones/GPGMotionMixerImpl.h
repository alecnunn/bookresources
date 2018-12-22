
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




/******************************************************************************
class GPGMotionMixerImpl: public GPGMotionMixerInterface

******************************************************************************/
class GPGMotionMixerImpl: public GPGMotionMixerInterface
	{
	public:
					GPGMotionMixerImpl(void);
virtual				~GPGMotionMixerImpl(void);

virtual GPGRESULT	Reset(void);

virtual GPGRESULT	AssociateWithMotionManager(GPGMotionManagerInterface *set);
					/// alternative to pick motions without a motion manager
virtual GPGRESULT	UseCustomMotionArray(long length,GPGMotion *motion);

virtual GPGRESULT	AssociateWithBonesManager(GPGBonesManagerInterface *set);
virtual GPGRESULT	MapFullMotionToCharacter(long motionid);
virtual GPGRESULT	SubMapMotionToCharacter(long motionid,long fromboneid);
virtual GPGRESULT	MapMotionTrackToBone(long motionid,GPGString trackname,
																long boneid);

virtual GPGRESULT	GetMotionTimeLimits(float *min,float *max);

virtual GPGRESULT	SetPositionExplicitly(long boneid,
												const GPGVector3 *location,
												const GPGQuaternion *rotation,
												const GPGVector3 *scale);
virtual GPGRESULT	GetBonePosition(long boneid,GPGVector3 *location,
												GPGQuaternion *rotation,
												GPGVector3 *scale);
virtual GPGRESULT	GetAbsoluteBoneMatrix(long boneid,GPGMatrix4x4 *pMatrix);
virtual GPGRESULT	ActivateIK(long boneid,bool on);

virtual	GPGRESULT	ApplyMotion(F32 time,bool toblend);
virtual	GPGRESULT	ApplyIK(void);
virtual GPGRESULT	GetMotionReader(long boneid,GPGMotionReader **reader);

virtual GPGRESULT	IncrementTimeOffset(F32 deltaOffset)
								{ m_timeOffset+=deltaOffset; return GPG_OK; };
virtual GPGRESULT	SetTimeOffset(F32 offset)
								{ m_timeOffset=offset; return GPG_OK; };
virtual GPGRESULT	GetTimeOffset(F32 *offset) const
								{ *offset=m_timeOffset; return GPG_OK; };
virtual	GPGRESULT	CalcBonePositionAtTime(long boneid,F32 time,
								GPGVector3 *location,GPGQuaternion *rotation,
								GPGVector3 *scale);


//* not exposed:

		GPGRESULT	VerifyCharacter(GPGVoidStar vset);

	private:

static	BOOL		MapBone(GPGCoreNode &node,GPGVoidStar state);

		GPGMotion	*LookupMotion(long motionid);
		void		ResizeReaderArray(long size);

									// use manager or custom, not both
		GPGMotionManagerInterface	*m_motionmanager;
		F32							m_timeOffset;

									// custom array
		U32							m_customlength;
		GPGMotion					*m_motionarray;

		GPGCharacter				*m_character;
		GPGArray<GPGMotionReader>	m_readerarray;
	};
