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
//
//	GPGBase
//
//	DESCRIPTION
//    This header file declares all the pieces of the Toolkit infrastructure.
//    Include this header if you want debugging macros, result codes,
//    fundamental datatypes and iterators for doing 3D graphics,
//    and interfaces for mesh, material, neighbor mesh, and utilities.
//    


/******************************************************************************

This library contains general classes related to 3D graphics.
Every class has a header and optional source file named exactly
like the class itself.

The library's primary objective is speed.  Some issues are listed below
where code-safety may be potentially comprimised for this directive.

CONSTRUCTORS

	Many constructors to not initialize their data members since they
	are often immediately fed new values anyhow.
	These classes offer member functions such as Reset(), Set(), or Make*()
	to insert reasonable values.

ARGUMENT CONFLICTS

	Generally, it is not safe to use an instance as an argument
	to it's own member function.  For example,

		GPGQuaternion a,b;
		a.Multiply(a,b);

	does NOT correctly compute a=a*b since the data in a is not put in
	temporary storage for the computations.  You can do this manually with

		GPGQuaternion a,b,c;
		c=a;
		a.Multiply(c,b);

	In many cases, you can find a fast unary version such as

		a.Multiply(b);

******************************************************************************/


#ifndef __GPGBase_h__
#define __GPGBase_h__

#define	GPG_LISTALLOC

#include "Base/GPGDebug.h"
#include "Base/GPGDataTypes.h"
#include "Base/GPGResult.h"
#include "Base/GPGMemory.h"
#include "Base/GPGFastAllocator.h"
#include "Base/GPGUnitAllocator.h"
#include "Base/GPGSharedUnitAllocator.h"
#include "Base/GPGString.h"
#include "Base/GPGArray.h"
#include "Base/GPGDeque.h"
#include "Base/GPGChunkPool.h"
#include "Base/GPGListNode.h"
#include "Base/GPGListContext.h"
#include "Base/GPGList.h"
#include "Base/GPGAllocator.h"
#include "Base/GPGMessage.h"
#include "Base/GPGException.h"
#include "Base/GPGMap.h"
#include "Base/GPGType.h"
#include "Base/GPGVariant.h"
#include "Base/GPGVoidStar.h"
#include "Base/GPGVector3.h"
#include "Base/GPGVector4.h"
#include "Base/GPGVector2.h"
#include "Base/GPGFace.h"
#include "Base/GPGVertexMap.h"
#include "Base/GPGVertexMapGroup.h"
#include "Base/GPGVertexAttributes.h"
#include "Base/GPGIterators.h"
#include "Base/GPGMeshGroup.h"
#include "Base/GPGMesh.h"
#include "Base/GPGMeshInterleaved.h"
#include "Base/GPGMeshInterleavedFactory.h"
#include "Base/GPGTexture.h"
#include "Base/GPGMaterial.h"
#include "Base/GPGInfoRender.h"
#include "Base/GPGVertexIndexer.h"
#include "Base/GPGRegisterNative.h"

#include <stdarg.h>
#include <math.h>

#ifndef GPGALLOW_STREAMS
#define GPGALLOW_STREAMS	FALSE
#endif

#if GPGALLOW_STREAMS
#define GPGPRINT_PRECISION	3
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

// NOTE under VCC, *f() functions don't help, they just map
#ifdef MACINTOSH
#define GPGPOW		pow
#define GPGSIN		sin
#define GPGCOS		cos
#define GPGASIN		asin
#define GPGACOS		acos
#define GPGPOW		pow
#define GPGSQRT		sqrt
#define GPGTAN		tan
#define GPGATAN		atan
#define GPGATAN2	atan2
#else
#define GPGPOW		powf
#define GPGSIN		sinf
#define GPGCOS		cosf
#define GPGASIN		asinf
#define GPGACOS		acosf
#define GPGPOW		powf
#define GPGSQRT		sqrtf
#define GPGTAN		tanf
#define GPGATAN		atanf
#define GPGATAN2	atan2f
#endif

#define GPGPI       (3.1415926f)
#define GPGTO_RAD   (GPGPI/180.0f)

class GPGQuaternion;
#include "Base/GPGMatrix4x4.h"
#include "Base/GPGQuaternion.h"
#include "Base/GPGEuler.h"
#include "Base/GPGTransform.h"
#include "Base/GPGTransformStack.h"

#endif /* __GPGBase_h__ */
