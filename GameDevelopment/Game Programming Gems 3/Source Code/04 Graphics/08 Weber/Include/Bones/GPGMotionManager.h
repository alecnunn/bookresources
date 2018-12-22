
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

#ifndef __GPGMotionManager_h__
#define __GPGMotionManager_h__


class GPGMotion;


/******************************************************************************
class GPGMotionManagerInterface

	Each GPGMotionMixer draws from one GPGMotionManager.

	There can probably just be one master motion manager,
	but it might be useful to group independent motion sets into
	different managers.

	NOTE The motions are stored in an array indexed by the motionid.
	Try to keep your requested id's small so that the array is not
	terribly sparse.

******************************************************************************/
class GPGMotionManagerInterface
	{
	public:
					/// reset manager as though just instantiated
virtual GPGRESULT	Reset(void)												=0;

					/** Load motion in file into index given by requestid.
						If motion exists at that index and replace==false,
						motion is loaded into a different index returned
						through resultid. */
virtual	GPGRESULT	LoadMotion(const char *filename,bool replace,
											long requestid,long *resultid)	=0;

					/// remove motion at index from memory
virtual	GPGRESULT	UnloadMotion(long motionid)								=0;

//	internal:
					/** intended for internal access only
						For given index, gets the actually GPGMotion structure
						and name.   You must be able to declare an GPGMotion
						in order to extract the data.  Arguments motion and
						sourcename are output only.
						They can be NULL (do not get).  If the motion was
						loaded, sourcename should be the filename. */
virtual	GPGRESULT	GetMotion(long motionid,GPGMotion **motion,
													GPGString *sourcename)	=0;
	};



extern "C"
{
#ifdef GPGBONESDLL

// Class factory definitions for run-time linking of the
// technology though a DLL interface.
#define FN_GPGCreateMotionManager		"GPGCreateMotionManager"
typedef GPGMotionManagerInterface* (*PF_GPGCreateMotionManager)();

#define FN_GPGDeleteMotionManager		"GPGDeleteMotionManager"
typedef void (*PF_GPGDeleteMotionManager)(GPGMotionManagerInterface* pManager);

#else

// Class factory methods for static linking of the
// technology through a library.
GPGMotionManagerInterface* GPGCreateMotionManager();
void GPGDeleteMotionManager(GPGMotionManagerInterface* pManager);

#endif
}

#endif /* __GPGMotionManager_h__ */
