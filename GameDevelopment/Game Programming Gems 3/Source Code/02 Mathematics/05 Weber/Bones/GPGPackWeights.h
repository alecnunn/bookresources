
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

#ifndef __GPGPackWeights_h__
#define __GPGPackWeights_h__


#define	GPGPW_CHECK_READ_OVERRUN	_DEBUG
#define	GPGPW_CHECK_WRITE_OVERRUN	_DEBUG
#define	GPGPW_ASSERT_ON_OVERRUN		FALSE	// ignored if not checking

#ifdef __ICL
#include <xmmintrin.h>
#endif

#ifdef MACINTOSH
//////////////////////////////////////////////////////////////////////////
//	Here is the defining of the control word for the dst.				//
//	1 vector(s) worth of data											//
//	1 block to fetch													//
//	0 bytes between blocks												//
//////////////////////////////////////////////////////////////////////////
const int kPrefetchControlWord = (1 << 24) | (1 << 16) | 0;
#endif

/******************************************************************************
class GPGPackVertex

******************************************************************************/
class GPGPackVertex
	{
	public:
					GPGPackVertex(void)				{ };

		void		SetVertexIndex(U16 set)			{ m_vertexid=set; };
		U16			GetVertexIndex(void) const		{ return m_vertexid; };

		void		SetNumWeights(U8 set)			{ m_weights=set; };
		void		IncNumWeights(void)				{ m_weights++; };
		U8			GetNumWeights(void) const		{ return m_weights; };

		F32			*Offset(void)					{ return m_offset; };
const	F32			*OffsetConst(void) const		{ return m_offset; };

		F32			*NormalOffset(void)				{ return m_normaloffset; };
const	F32			*NormalOffsetConst(void) const	{ return m_normaloffset; };
	private:

		GPGGeomIndex	m_vertexid;
		F32				m_offset[3];
		F32				m_normaloffset[3];
		U8				m_weights;
	};



/******************************************************************************
class GPGPackBoneWeight

******************************************************************************/
class GPGPackBoneWeight
	{
	public:
					GPGPackBoneWeight(void)			{ };

		void		SetBoneIndex(U16 set)			{ m_boneid=set; };
		U16			GetBoneIndex(void) const		{ return m_boneid; };

		void		SetBoneWeight(F32 set)			{ m_weight=set; };
		F32			GetBoneWeight(void) const		{ return m_weight; };

	private:

		F32		m_weight;
		U16		m_boneid;
	};



/******************************************************************************
class GPGPackWeights

******************************************************************************/
class GPGPackWeights
	{
	public:
					GPGPackWeights(void)
						{
						m_size=0;
						m_verticesAllocated=0;
						m_weightsAllocated=0;
						m_verticesWritten=0;
						m_buffer=NULL;
						m_currentread=NULL;
						m_currentwrite=NULL;
						m_wroteweights=true;
						};

virtual				~GPGPackWeights(void)
						{
						if(m_buffer)
							GPGDeallocate(m_buffer);
						};

		void		Allocate(U32 vertices,U32 weights)
						{
						if(m_buffer)
							GPGDeallocate(m_buffer);

						m_verticesAllocated=vertices;
						m_weightsAllocated=weights;
						m_size=vertices*sizeof(GPGPackVertex)+
										weights*sizeof(GPGPackBoneWeight);
						m_buffer=(U8 *)GPGAllocate(m_size);
						RewindForRead();
						RewindForWrite();
						};

		U32					GetNumberWeightsAllocated(void) const
												{ return m_weightsAllocated; };
		U32					GetNumberVerticesConst(void) const
												{ return m_verticesWritten; };

		void				RewindForRead(void)	{ m_currentread=m_buffer; };
		void				RewindForWrite(void)
								{
								m_currentwrite=m_buffer;
								m_verticesWritten=0;
								m_wroteweights=true;
								};


#ifdef __ICL
		// both prefetches are assuming we're on the vertex at the moment
		void PrefetchNextPackVertex() 
		{
			_mm_prefetch(m_currentread, _MM_HINT_T0);
		}

		GPGPackBoneWeight*	PrefetchNextPackWeight() 
		{
			_mm_prefetch(((U8 *) m_currentread)+sizeof(GPGPackVertex), _MM_HINT_T0);
			return (m_currentread + sizeof(GPGPackVertex));
		}
#else
#ifdef MACINTOSH
		void	PrefetchNextPackVertex() 
		{
			// perform data stream touch (dst) - on channel 0
			vec_dst( m_currentread, kPrefetchControlWord, 0 );
		}

		void PrefetchNextPackWeight() 
		{
			// perform data stream touch (dst) - on channel 1
			vec_dst( (long *)(((U8 *)m_currentread) + sizeof(GPGPackVertex)), kPrefetchControlWord, 1 );
		}
#endif
#endif // __ICL

		U32					GetReadOffset(void)
								{ return (U32)(m_currentread-m_buffer); };

const	GPGPackVertex		*NextPackVertexForRead(void)
								{ return NextPackVertexForReplace(); };
		GPGPackVertex		*NextPackVertexForReplace(void)
								{
								GPGASSERT(m_currentread);
#if GPGPW_CHECK_READ_OVERRUN
								if((U32)(m_currentread-m_buffer)>=m_size)
									{
									if(m_size)
										GPGLOG("NextPackVertex*() read overrun "
											"%d+%d bytes\n",
										(U32)(m_currentread-m_buffer),
										(U32)(m_currentread-m_buffer-m_size));
#if	GPGPW_ASSERT_ON_OVERRUN
									GPGASSERT(FALSE);
#endif
									return NULL;
									}
								if(m_currentread>=m_currentwrite)
									{
									if(m_size)
										GPGLOG("NextPackVertex*() read"
											" unwritten "
											"data plus %d bytes\n",
											(U32)(m_currentread-m_buffer));
#if	GPGPW_ASSERT_ON_OVERRUN
									GPGASSERT(FALSE);
#endif
									return NULL;
									}
#endif
								GPGPackVertex *result=
												(GPGPackVertex *)m_currentread;
								m_currentread+=sizeof(GPGPackVertex);
								return result;
								};
const	GPGPackBoneWeight	*NextPackBoneWeightForRead(void)
								{ return NextPackBoneWeightForReplace(); };
		GPGPackBoneWeight	*NextPackBoneWeightForReplace(void)
								{
								GPGASSERT(m_currentread);
#if GPGPW_CHECK_READ_OVERRUN
								if((U32)(m_currentread-m_buffer)>=m_size)
									{
									GPGLOG("NextPackWeight*() read overrun "
										"plus %d bytes\n",
										(U32)(m_currentread-m_buffer));
#if	GPGPW_ASSERT_ON_OVERRUN
									GPGASSERT(FALSE);
#endif
									return NULL;
									}
								if(m_currentread>=m_currentwrite)
									{
									GPGLOG("NextPackWeight*() read unwritten "
										"data plus %d bytes\n",
										(U32)(m_currentread-m_buffer));
#if	GPGPW_ASSERT_ON_OVERRUN
									GPGASSERT(FALSE);
#endif
									return NULL;
									}
#endif
								GPGPackBoneWeight *result=
											(GPGPackBoneWeight *)m_currentread;
								m_currentread+=sizeof(GPGPackBoneWeight);
								return result;
								};

		GPGPackVertex		*NextPackVertexForWrite(void)
								{
								GPGASSERT(m_currentwrite);
								GPGASSERT(m_wroteweights);
#if GPGPW_CHECK_READ_OVERRUN
								if((U32)(m_currentwrite-m_buffer)>=m_size)
									{
									GPGLOG("NextPackVertex*() write overrun "
										"plus %d bytes\n",
										(U32)(m_currentread-m_buffer));
#if	GPGPW_ASSERT_ON_OVERRUN
									GPGASSERT(FALSE);
#endif
									return NULL;
									}
#endif
								GPGPackVertex *result=
												(GPGPackVertex *)m_currentwrite;
								m_currentwrite+=sizeof(GPGPackVertex);
								m_verticesWritten++;
								m_wroteweights=false;
								return result;
								};
		GPGPackBoneWeight	*NextPackBoneWeightForWrite(void)
								{
								GPGASSERT(m_currentwrite);
#if GPGPW_CHECK_READ_OVERRUN
								if((U32)(m_currentwrite-m_buffer)>=m_size)
									{
									GPGLOG("NextPackWeight*() write overrun "
										"plus %d bytes\n",
										(U32)(m_currentread-m_buffer));
#if	GPGPW_ASSERT_ON_OVERRUN
									GPGASSERT(FALSE);
#endif
									return NULL;
									}
#endif
								GPGPackBoneWeight *result=
											(GPGPackBoneWeight *)m_currentwrite;
								m_currentwrite+=sizeof(GPGPackBoneWeight);
								m_wroteweights=true;
								return result;
								};

		void					Log(void);

	private:

		U8	*m_currentread;
		U8	*m_currentwrite;
		U8	*m_buffer;
		U32	m_size;
		U32	m_verticesAllocated;
		U32	m_weightsAllocated;
		U32	m_verticesWritten;
		bool m_wroteweights;
	};

#endif /* __GPGPackWeights_h__ */
