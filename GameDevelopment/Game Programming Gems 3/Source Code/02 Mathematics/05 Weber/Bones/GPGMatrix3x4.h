
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

#ifndef __GPGMatrix3x4_h__
#define __GPGMatrix3x4_h__

#ifdef MACINTOSH
#include "XG4Simulator.h"

typedef union
{
	__vector float 	vecArray;
	float 		 	floatArray[4];
} VectorFloatElement;

#ifndef isQuadAligned
#define isQuadAligned( p ) (((unsigned long)(p) & 0x0000000F) == 0)
#endif

#ifndef isLongAligned
#define isLongAligned( p ) (((unsigned long)(p) & 0x00000003) == 0)
#endif
#endif

//* if bones are scaled, this MUST be activated to get unit normals
#define GPGM34_UNSCALE	FALSE

/******************************************************************************
class GPGMatrix3x4

	shearless version of GPGMatrix4x4

******************************************************************************/
class GPGMatrix3x4
	{
	public:
						GPGMatrix3x4(void)
							{
							// no default clear
							};

						GPGMatrix3x4(const GPGMatrix4x4 &operand)
							{
							operator=(operand);
							};

		GPGMatrix3x4	&operator=(const GPGMatrix4x4 &operand);

		void			TransformVertexAndNormal(const F32 scale,
											const F32 *const vector_in,
											F32 *const vector_out,
											const F32 *const normal_in,
											F32 *const normal_out) const;
		void			CumulativeTransformVertexAndNormal(const F32 scale,
											const F32 *const vector_in,
											F32 *const vector_out,
											const F32 *const normal_in,
											F32 *const normal_out) const;
#ifdef MACINTOSH
		void 			AltivecTransformVertexAndNormal(const F32 scale,
											const F32 *const vector_in,
											F32 *const vector_out,
											const F32 *const normal_in,
											F32 *const normal_out) const;

		void 			AltivecCumulativeTransformVertexAndNormal( const F32 scale,
											const F32 *const vector_in,
											F32 *const vector_out,
											const F32 *const normal_in,
											F32 *const normal_out) const;
#endif

		GPGString		Out(bool verbose=false) const;

	private:

		F32				m_data[12];
#if GPGM34_UNSCALE
		F32				m_norm[9];
#endif
	};


GPGINLINE GPGMatrix3x4 &GPGMatrix3x4::operator=(const GPGMatrix4x4 &operand)
	{
	const F32 *mat=operand.RawConst();

	m_data[0]=mat[0];
	m_data[1]=mat[4];
	m_data[2]=mat[8];
	m_data[3]=mat[12];

	m_data[4]=mat[1];
	m_data[5]=mat[5];
	m_data[6]=mat[9];
	m_data[7]=mat[13];

	m_data[8]=mat[2];
	m_data[9]=mat[6];
	m_data[10]=mat[10];
	m_data[11]=mat[14];

#if GPGM34_UNSCALE
	//* Is this valid and is it the fastest way to remove scale?
	GPGQuaternion quat=operand;
	GPGMatrix4x4 result=quat;
	const F32 *mat2=result.RawConst();

	m_norm[0]=mat2[0];
	m_norm[1]=mat2[4];
	m_norm[2]=mat2[8];

	m_norm[3]=mat2[1];
	m_norm[4]=mat2[5];
	m_norm[5]=mat2[9];

	m_norm[6]=mat2[2];
	m_norm[7]=mat2[6];
	m_norm[8]=mat2[10];
#endif

	return *this;
	}

	//*						`Center of the World'
	//*	for bones animation, this is primary action of the core loop
// NOTE limited 3x3+3 non-shear multiplication, not 4x4 full multiplication
GPGINLINE void GPGMatrix3x4::TransformVertexAndNormal(const F32 scale,
					const F32 *const vi,F32 *const vout,
					const F32 *const ni,F32 *const nout) const
	{
	// unrolled
#if GPGM34_UNSCALE
	vout[0]=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]=scale*(ni[0]*m_norm[0]+ni[1]*m_norm[1]+ni[2]*m_norm[2]);
	vout[1]=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]=scale*(ni[0]*m_norm[3]+ni[1]*m_norm[4]+ni[2]*m_norm[5]);
	vout[2]=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+m_data[11]);
	nout[2]=scale*(ni[0]*m_norm[6]+ni[1]*m_norm[7]+ni[2]*m_norm[8]);
#else
	vout[0]=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]=scale*(ni[0]*m_data[0]+ni[1]*m_data[1]+ni[2]*m_data[2]);
	vout[1]=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]=scale*(ni[0]*m_data[4]+ni[1]*m_data[5]+ni[2]*m_data[6]);
	vout[2]=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+m_data[11]);
	nout[2]=scale*(ni[0]*m_data[8]+ni[1]*m_data[9]+ni[2]*m_data[10]);
#endif
	}
GPGINLINE void GPGMatrix3x4::CumulativeTransformVertexAndNormal(
					const F32 scale,
					const F32 *const vi,F32 *const vout,
					const F32 *const ni,F32 *const nout) const
	{
	// unrolled
#if GPGM34_UNSCALE
	vout[0]+=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]+=scale*(ni[0]*m_norm[0]+ni[1]*m_norm[1]+ni[2]*m_norm[2]);
	vout[1]+=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]+=scale*(ni[0]*m_norm[3]+ni[1]*m_norm[4]+ni[2]*m_norm[5]);
	vout[2]+=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+
																	m_data[11]);
	nout[2]+=scale*(ni[0]*m_norm[6]+ni[1]*m_norm[7]+ni[2]*m_norm[8]);
#else
	vout[0]+=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]+=scale*(ni[0]*m_data[0]+ni[1]*m_data[1]+ni[2]*m_data[2]);
	vout[1]+=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]+=scale*(ni[0]*m_data[4]+ni[1]*m_data[5]+ni[2]*m_data[6]);
	vout[2]+=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+
																	m_data[11]);
	nout[2]+=scale*(ni[0]*m_data[8]+ni[1]*m_data[9]+ni[2]*m_data[10]);
#endif
	}


#endif /* __GPGMatrix3x4_h__ */

// EOF
