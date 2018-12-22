
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

#ifndef __GPGDeque_h__
#define __GPGDeque_h__


#define GPGDQ_RESETVACANT	FALSE



/**************************************************************************//** 
	Double-Ended Queue implemented as circular queue on a dynamic array.

	WARNING minimal optimization

	PopHead() and PopTail() discard the popped entry,
	so read it BEFORE you pop it.

	The queue does not do strong bounds checking.
	Do not try an index >= GetNumberQueued().

	The queue stores actual objects, if you want to queue pointers,
	then you should template on a pointer to the object's class.

	NOTE To properly reset reused items on the queue, the derived deque should
	provide a `ResetElement(T &element)'.

	NOTE Since arbitrary non-reference counted classes are accepted in the
	template, `T &Pop*()' is not supported since the object would be gone
	after the pop.

	Example:
		GPGDeque<float>	fdeque;
		fdeque.PushTail()=1.2f;
		fdeque.PushTail()=3.4f;
		float head=fdeque.ElementFromHead(0);		-> head = 1.2f
		fdeque.PopTail();
*//***************************************************************************/
template<class T> class GPGDeque: private GPGArray<T>
	{
	public:
					GPGDeque(void)
						{
						m_head=0;
						m_tail=0;
						m_elementsQueued=0;
						ResizeToAtLeast(m_elementsAllocated);
						};

virtual	void	Clear(U32 preallocation=0)
						{
						GPGArray<T>::Clear(preallocation);
						m_head=0;
						m_tail=0;
						m_elementsQueued=0;
						};

virtual	void	ResetElement(T &element)		{ };

		T		&PushHead(void);
		bool	PopHead(void);

		T		&PushTail(void);
		bool	PopTail(void);

		U32		GetNumberQueued(void) const	{ return m_elementsQueued; };

		T		&ElementFromHead(U32 index);
const	T		&ElementFromHeadConst(U32 index) const;
		T		&ElementFromTail(U32 index);
const	T		&ElementFromTailConst(U32 index) const;

	private:
		void	Expand(void);
		U32		CorrectIndex(I32 index) const;

		I32		m_head;
		I32		m_tail;
		U32		m_elementsQueued;
	};

template<class T> GPGINLINE T &GPGDeque<T>::PushHead(void)
	{
	Expand();

	if(m_elementsQueued>1)
		m_head++;
	if(m_head>=(I32)m_elementsAllocated)
		m_head-=m_elementsAllocated;

//	(*this)[m_head].Reset();
	ResetElement((*this)[m_head]);

	return (*this)[m_head];
	}

template<class T> GPGINLINE bool GPGDeque<T>::PopHead(void)
	{
	if(m_elementsQueued<1)
		return false;

	if(--m_elementsQueued)
		{
		m_head--;
		if(m_head<0)
			m_head+=m_elementsAllocated;
		}

	return true;
	}

template<class T> GPGINLINE T &GPGDeque<T>::PushTail(void)
	{
	Expand();

	if(m_elementsQueued>1)
		m_tail--;
	if(m_tail<0)
		m_tail+=m_elementsAllocated;

//	(*this)[m_tail].Reset();
	ResetElement((*this)[m_tail]);

	return (*this)[m_tail];
	}

template<class T> GPGINLINE bool GPGDeque<T>::PopTail(void)
	{
	if(m_elementsQueued<1)
		return false;

	if(--m_elementsQueued)
		{
		m_tail++;
		if(m_tail>=(I32)m_elementsAllocated)
			m_tail-=m_elementsAllocated;
		}

	return true;
	}

template<class T> GPGINLINE T &GPGDeque<T>::ElementFromHead(U32 index)
	{
	GPGASSERT(index<m_elementsQueued);
	return GetElement(CorrectIndex((I32)(m_head-index)));
	}

template<class T> GPGINLINE const T
					&GPGDeque<T>::ElementFromHeadConst(U32 index) const
	{
	GPGASSERT(index<m_elementsQueued);
	return GetElementConst(CorrectIndex((I32)(m_head-index)));
	}

template<class T> GPGINLINE T &GPGDeque<T>::ElementFromTail(U32 index)
	{
	GPGASSERT(index<m_elementsQueued);
	return GetElement(CorrectIndex((I32)(index+m_tail)));
	}

template<class T> GPGINLINE const T
					&GPGDeque<T>::ElementFromTailConst(U32 index) const
	{
	GPGASSERT(index<m_elementsQueued);
	return GetElementConst(CorrectIndex((I32)(index+m_tail)));
	}

template<class T> GPGINLINE void GPGDeque<T>::Expand(void)
	{
	m_elementsQueued++;
	if(m_elementsQueued>this->GetNumberElements())
		{
		U32 oldAllocated=m_elementsAllocated;

		ResizeToAtLeast(m_elementsQueued);
		ResizeToAtLeast(m_elementsAllocated);

		//* move high entries
		U32 m,range=oldAllocated-m_tail;
		for(m=1;m<range+1;m++)
			*m_array[CorrectIndex(m_elementsAllocated-m)]=
									*m_array[CorrectIndex(oldAllocated-m)];

		U32 move=m_elementsAllocated-oldAllocated;
		if(m_head>m_tail)
			m_head+=move;
		if(oldAllocated)
			m_tail+=move;

#if GPGDQ_RESETVACANT
		for(m=0;m<move;m++)
//			m_array[CorrectIndex(m_tail-1-m)]->Reset();
			ResetElement(m_array[CorrectIndex(m_tail-1-m)]);
#endif
		}
	}

template<class T> GPGINLINE U32 GPGDeque<T>::CorrectIndex(I32 index) const
	{
	if(index<0)
		index+=m_elementsAllocated;
	if(index>=(I32)m_elementsAllocated)
		index-=m_elementsAllocated;
	return (U32)index;
	}

#endif /* __GPGDeque_h__ */
