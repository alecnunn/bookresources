
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

#ifndef __GPGVertexIndexer_h__
#define __GPGVertexIndexer_h__


/******************************************************************************
class GPGVertexIndexer: public GPGVertexIter

	only accesses first texture layer

******************************************************************************/
class GPGVertexIndexer: public GPGVertexIter
	{
	public:
		void		MoveToIndex(U32 index)
						{
						m_indexptr=(U8 *)Index(index);
						GPGASSERT(m_indexptr);
						};

		GPGVector3	*GetPosition(void)
						{
						return (GPGVector3*)(m_indexptr+m_offsets
															.positionOffset);
						}
		void		SetPosition(const float *data)
						{
						*((GPGVector3*)(m_indexptr+m_offsets.positionOffset))
																		=data;
						}

		GPGVector3	*GetNormal(void)
						{
						return (GPGVector3*)(m_indexptr+m_offsets.normalOffset);
						}
		void		SetNormal(const float *data)
						{
						*((GPGVector3*)(m_indexptr+m_offsets.normalOffset))
																		=data;
						};

		GPGVector2	*GetTexCoord(void)
						{
						return (GPGVector2*)(m_indexptr+
													m_offsets.textureOffset);
						}
		void		SetTexCoord(const float *data)
						{
						*((GPGVector3*)(m_indexptr+m_offsets.textureOffset))
																		=data;
						};

					//* convenience
		void		SetVertexAndNormal(long index,const float vert[3],
														const float normal[3])
						{
						MoveToIndex(index);
						SetPosition(vert);
						SetNormal(normal);
						}


	private:
		U8			*m_indexptr;
	};

#endif /* __GPGVertexIndexer_h__ */
