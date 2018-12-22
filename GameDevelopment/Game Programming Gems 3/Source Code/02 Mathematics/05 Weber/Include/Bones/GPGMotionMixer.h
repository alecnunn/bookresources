
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

#ifndef __GPGMotionMixer_h__
#define __GPGMotionMixer_h__


class GPGBonesManagerInterface;
class GPGVector3;
class GPGQuaternion;
class GPGMotionReader;



/**************************************************************************//**
class GPGMotionMixerInterface

	NOTE this is a motion mixer, not a motion blender.
	It selects from multiple tracks and/or IK.
	It does not blend from one motion to another through time.

	Each GPGCharacter can have multiple GPGMotionMixers from which it selects.
	Each mixer uses a single GPGMotionManager and is used for a specific
	character.  You must Reset() before associating a different motionmanager
	of bonesmanager.

	Multiple mixers can draw from the same GPGMotionManager.
	Multiple mixers can be applied same GPGBonesManager.
	A specific GPGMotionMixer can only be used on a single GPGMotionManager
	and a single GPGBonesManager.

*//***************************************************************************/
class GPGMotionMixerInterface
	{
	public:
					/// `initialization'

					/// reset manager as though just instantiated
virtual	GPGRESULT	Reset(void)												=0;
					/// choose the single GPGMotionManager for this mixer
virtual	GPGRESULT	AssociateWithMotionManager(
										GPGMotionManagerInterface *set)		=0;
					/// choose the single GPGBonesManager for this mixer
virtual	GPGRESULT	AssociateWithBonesManager(GPGBonesManagerInterface *set)=0;
					/// `setup'

					/// map the entire indexed motion into mixer,
					/// assuming matching track and bone names
virtual	GPGRESULT	MapFullMotionToCharacter(long motionid)					=0;
					/// map a sub-hierarchy starting from a specified bone
virtual GPGRESULT	SubMapMotionToCharacter(long motionid,long fromboneid)	=0;
					/// map a specific named track to a specific indexed bone 
virtual	GPGRESULT	MapMotionTrackToBone(long motionid,
										GPGString trackname,long boneid)	=0;
					/// set specific indexed bone to a explicit displacement
					/// and rotation
virtual	GPGRESULT	SetPositionExplicitly(long boneid,
										const GPGVector3 *location,
										const GPGQuaternion *rotation=NULL,
										const GPGVector3 *scale=NULL)		=0;
					/// get min and max time of all currently mapped tracks
virtual GPGRESULT	GetMotionTimeLimits(float *min,float *max)				=0;
					/// turn on///off IK for specific indexed bone
virtual	GPGRESULT	ActivateIK(long boneid,bool on)							=0;

virtual GPGRESULT	IncrementTimeOffset(F32 deltaOffset)					=0;
virtual GPGRESULT	SetTimeOffset(F32 offset)								=0;
virtual GPGRESULT	GetTimeOffset(F32 *offset) const						=0;

					/// `per frame update'

					/// get current displacement and rotation of specific
					/// indexed bone
virtual	GPGRESULT	GetBonePosition(long boneid,GPGVector3 *location,
												GPGQuaternion *rotation=NULL,
												GPGVector3 *scale=NULL)		=0;
					/// get model-relative rotation of last deformation
virtual GPGRESULT	GetAbsoluteBoneMatrix(long boneid,GPGMatrix4x4 *matrix)	=0;
//*	internal:
					/// intended for internal access only
					/// Get a bone's position at a specified time
virtual	GPGRESULT	CalcBonePositionAtTime(long boneid,F32 time,
												GPGVector3 *location,
												GPGQuaternion *rotation=NULL,
												GPGVector3 *scale=NULL)		=0;

					/// intended for internal access only
					/// apply motion in all mapped tracks at specified time
virtual GPGRESULT	ApplyMotion(F32 time,bool toblend)						=0;
					/// intended for internal access only
					/// apply IK to all IK-activated bones
virtual GPGRESULT	ApplyIK(void)											=0;
					/// intended for internal access only
					/// for a given boneid, get the asscoiated GPGMotionReader
					/// You must be able to declare an GPGMotionReader 
					/// in order to extract the data.
virtual GPGRESULT	GetMotionReader(long boneid,GPGMotionReader **reader)	=0;
	};



extern "C"
{
#ifdef GPGBONESDLL

/// Class factory definitions for run-time linking of the
/// technology though a DLL interface.
#define FN_GPGCreateMotionMixer			"GPGCreateMotionMixer"
typedef GPGMotionMixerInterface* (*PF_GPGCreateMotionMixer)();

#define FN_GPGDeleteMotionMixer			"GPGDeleteMotionMixer"
typedef void (*PF_GPGDeleteMotionMixer)(GPGMotionMixerInterface* pManager);

#else

/// Class factory methods for static linking of the
/// technology through a library.
GPGMotionMixerInterface* GPGCreateMotionMixer();
void GPGDeleteMotionMixer(GPGMotionMixerInterface* pManager);

#endif
}

#endif /// __GPGMotionMixer_h__ *///
