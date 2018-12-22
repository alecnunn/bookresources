
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

#ifndef __GPGBackBones_h__
#define __GPGBackBones_h__


// first to make sure it's independent
#include "GPGBones.h"

#include "GPGBase.h"


//#define GPG_R1_HACKS


//* comment out to remove parsing code
#ifndef GPG_R1_HACKS
#define GPG_USE_HPARSER

#include "Bones/GPGInstant.h"
#include "Bones/GPGKeyFrame.h"
#include "Bones/GPGKeyTrack.h"
#include "Bones/GPGKeyTrackArray.h"
#include "Bones/GPGMotion.h"
#endif

#ifdef GPG_USE_HPARSER
#include "GPGHParser.h"
#include "GPGFileIO.h"
#endif

class GPGCharacter;
class GPGBoneNode;
class GPGBoneNodeList;

#include "GPGMatrix3x4.h"
#include "GPGCylinder.h"

#include "GPGIndent.h"
#include "GPGLong3.h"
#include "GPGMotionReader.h"

#include "GPGMeshVertexMap.h"
#include "GPGMeshInterface.h"
#include "GPGBoneLinks.h"
#include "GPGCoreNode.h"
#include "GPGConstraints.h"
#include "GPGBoneNode.h"
#include "GPGBoneCache.h"
#include "GPGBoneCacheArray.h"
//#include "GPGDefaultMesh.h"
#include "GPGBoneNodeList.h"
#include "GPGVertexWeight.h"
#include "GPGVertexWeights.h"
#include "GPGPackWeights.h"
#include "GPGSkin.h"
#include "GPGIKModes.h"
#include "GPGBoneContext.h"
#include "GPGCharacter.h"

#include "../InfoRenderOGL/GPGInfoRenderOGL.h"

//* General Non-Intrusive Data-Specific Parsers
#ifdef GPG_USE_HPARSER
#include "../BonesTestbed/GPGParseBoneMap.h"
#include "../BonesTestbed/GPGParseBoneMapList.h"
#include "../BonesTestbed/GPGBoneParser.h"
#include "../BonesTestbed/GPGMotionParser.h"

//* File Parsers
#include "../BonesTestbed/GPGASH_BoneParser.h"
#include "../BonesTestbed/GPGASH_MotionParser.h"
#endif

//* GPGMesh Interface
#include "GPGMeshGroup_Impl.h"
#include "GPGMeshGroup_Character.h"

//* Bones/Motion Managers
#include "GPGBonesManagerImpl.h"
#include "GPGMotionMixerImpl.h"
#include "GPGMotionManagerImpl.h"

#endif /* __GPGBackBones_h__ */
