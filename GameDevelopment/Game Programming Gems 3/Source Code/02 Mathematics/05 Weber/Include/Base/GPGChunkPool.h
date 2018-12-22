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

#ifndef __GPGChunkPool_h__
#define __GPGChunkPool_h__

/** This is an alternative allocator to malloc/free, GPGAllocate/GPGDeallocate,
and GPGUnitAllocator.  This implementation makes different tradeoffs than the
other alternatives.  GPGAllocate/GPGDeallocate is basicly the same as
malloc/free which is the basic general purpose heap memory management
mechanism.  GPGUnitAllocator assumes fixed size chunks of memory and caches
freed chunks which are tracked via a free list.  GPGChunkPool uses
GPGAllocate to allocate memory */
class GPGChunkPool
{
	public:
		GPGChunkPool();
		GPGChunkPool(size_t max);
virtual	~GPGChunkPool(void);

		void	*Allocate(size_t s);
		void	Deallocate(void* pMemory, size_t s);
	private:
		U32		m_max;
		void	***m_nfList;
		U32		*m_nfIndex;
		U32		*m_nfAlloc;
		size_t	m_offset;
};

GPGINLINE void *GPGChunkPool::Allocate(size_t size)
{
	size_t s = size - m_offset;
	if(s < m_max && m_nfIndex[s] > 0)
	{
		m_nfIndex[s]--;
		return m_nfList[s][m_nfIndex[s]];
	}
	else
	{
		return GPGAllocate(s);
	}
}

GPGINLINE void GPGChunkPool::Deallocate(void* pMemory, size_t size)
{
	size_t s = size - m_offset;
	if(s < m_max)
	{
		if(m_nfIndex[s] >= m_nfAlloc[s])
		{
			if(!m_nfAlloc[s])
			{
				m_nfAlloc[s] = 4;
				m_nfList[s] =
					(void **)GPGAllocate(m_nfAlloc[s] * sizeof(void *));
			}
			else
			{
				m_nfAlloc[s] *= 2;
				m_nfList[s] = 
					(void **)GPGReallocate(m_nfList[s]
					,m_nfAlloc[s] * sizeof(void *));
			}
			GPGASSERT(m_nfList[s]);
			if(!m_nfList[s])
			{
				return;
			}
		}
		m_nfList[s][m_nfIndex[s]] = pMemory;
		m_nfIndex[s]++;
	}
	else
	{
		GPGDeallocate(pMemory);
	}
}


#endif /* __GPGChunkPool_h__ */
