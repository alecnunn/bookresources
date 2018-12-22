
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

#ifndef __GPGVertexWeight_h__
#define __GPGVertexWeight_h__




/******************************************************************************
class GPGVertexWeight

******************************************************************************/
class GPGVertexWeight
	{
	public:
					GPGVertexWeight(void)	{ Reset(); };

		void		Reset(void)
						{
						m_boneid=0;
						m_meshid=0;
						m_vertexid=0;
						m_weight=0.0f;
						m_offset.Reset();
						m_normaloffset.Reset();
						};

		GPGVertexWeight	&operator=(const GPGVertexWeight &operand)
						{
						CopyFrom(operand);
						return *this;
						};

		void		CopyFrom(const GPGVertexWeight &other)
						{
						m_boneid=other.m_boneid;
						m_meshid=other.m_meshid;
						m_vertexid=other.m_vertexid;
						m_weight=other.m_weight;
						m_offset=other.m_offset;
						m_normaloffset=other.m_normaloffset;
						};

		void		SetBoneIndex(I32 set)			{ m_boneid=set; };
		I32			GetBoneIndex(void) const		{ return m_boneid; };
		I32			&BoneIndex(void)				{ return m_boneid; };

		void		SetMeshIndex(I32 set)			{ m_meshid=set; };
		I32			GetMeshIndex(void) const		{ return m_meshid; };
		I32			&MeshIndex(void)				{ return m_meshid; };

		void		SetVertexIndex(I32 set)			{ m_vertexid=set; };
		I32			GetVertexIndex(void) const		{ return m_vertexid; };
		I32			&VertexIndex(void)				{ return m_vertexid; };

		void		SetBoneWeight(F32 set)			{ m_weight=set; };
		F32			GetBoneWeight(void) const		{ return m_weight; };
		F32			&BoneWeight(void)				{ return m_weight; };

		GPGVector3	&Offset(void)					{ return m_offset; };
const	GPGVector3	&OffsetConst(void) const		{ return m_offset; };

		GPGVector3	&NormalOffset(void)				{ return m_normaloffset; };
const	GPGVector3	&NormalOffsetConst(void) const
													{ return m_normaloffset; };

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif

		GPGString	Out(bool verbose=false) const;

	private:
		I32				m_meshid;
		I32				m_boneid;
		I32				m_vertexid;
		F32				m_weight;
		GPGVector3		m_offset;
		GPGVector3		m_normaloffset;	// cache only one
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGVertexWeight &wbone);
#endif

#endif /* __GPGVertexWeight_h__ */
