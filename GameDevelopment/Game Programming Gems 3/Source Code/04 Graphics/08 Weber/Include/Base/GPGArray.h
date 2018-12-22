
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

#ifndef __GPGArray_h__
#define __GPGArray_h__


class GPGVariant;


#define	GPGARRAY_MIN			4
#define	GPGARRAY_EXPANSION		2.0f	// multiplicative buffer granularity

#define GPGARRAY_UNSAFEMEMORY	(GPG_OSTYPE==GPG_WIN32)


/**************************************************************************//**
	allows parser to access generic array without knowing the element type

	WARNING:	this class must not contain data;
				it must be interchangable with a GPGArray through a void*

*//***************************************************************************/
class GPGCoreArray
{
	public:
// TODO: talk to JW about if this is really necessary. The MS compiler has trouble with multiple GPGMap<GPGString,anything> data members in the system with this. It has something to do with identical comdats (LNK1179) related to generated varianttypes
#ifdef GPG_LISTALLOC
virtual	void		AllocElement(GPGVariant *get)							=0;
virtual	void		AccessElement(U32 index,GPGVariant *get)				=0;
#endif
virtual	GPGString	GetElementName(U32 index)		{ return ""; };
virtual	U32			GetElementSize(U32 index)		{ return 1; };
virtual void		Clear(U32 m_preallocation=0)							=0;
};


/**************************************************************************//**
template<class T> class GPGArray: public GPGCoreArray

	NOTE class T is required have a member:
			T &T::operator=(const T &operand)
		Depending on the compiler, lack thereof might only fail
		if the Collect() member is used.  The const is optional.

	Array will use contiguous backing of length specified in
		constructor or Clear() and thereafter scattered new()'s.
		Use Collect() to make the data contiguous again.

	If the elements contain pointed-to data, it may be very unwise
		to use Collect().

	Mechanisms are in place that should allow the original GPGArray to
	be defined as a static in a Win32 DLL given no preallocation on
	construction (it's always safe in Linux).

	NOTE Elements may be reused without re-construction.
    	To properly reset reused items on the queue, the derived deque should
    	provide a `ResetElement(T &element)'.
*//***************************************************************************/
template<class T> class GPGArray: public GPGCoreArray
{
	public:
					GPGArray(U32 preallocation=0);
virtual				~GPGArray(void);

virtual	void		ResetElement(T &element)								{}

		U32			GetNumberElements(void) const	{ return m_elementsUsed; }

		T			&CreateNewElement(void);

		T			&GetElement(U32 index)
					{	GPGASSERT(index<m_elementsUsed);
						return *m_array[index]; }
const	T			&GetElementConst(U32 index) const
					{	GPGASSERT(index<m_elementsUsed);
						return *m_array[index]; }
#if FALSE
					/** Alternate to GetElement(); goes directly to backing.
					Can only access elements in the contiguous region */
					//* NOTE no noticeable performance gain
		T			&Backing(U32 index)
						{
						GPGASSERT(index<m_prealloc);
						return m_contiguous[index];
						};
const	T			&BackingConst(U32 index) const
						{
						GPGASSERT(index<m_prealloc);
						return m_contiguous[index];
						};
#endif
		T			&operator[](U32 index)	{ return GetElement(index); }
const	T			&operator[](U32 index) const
					{	return GetElementConst(index); }

					/// heavyweight copy (does not automatically Collect())
		void		operator=(const GPGArray<T> &other);

					/// Copy all of other array to end of this one
		void		Append(const GPGArray<T> &other);

#ifdef GPG_LISTALLOC
					// for GPGCoreArray
virtual	void		AllocElement(GPGVariant *get)
					{	*get=&CreateNewElement(); }
virtual	void		AccessElement(U32 index,GPGVariant *get)
					{	*get=&GetElement(index); }
#endif

					// destructs all elements
virtual	void		Clear(U32 preallocation=0);

					// if set<m_elementsUsed, destructs excess elements
		void		ResizeToExactly(U32 set);

					// if required<m_elementsUsed, does nothing (expand only)
		void		ResizeToAtLeast(U32 required);

					// moves all elements into contiguous space
		T			*Collect(void);

		GPGString	Out(bool verbose=false) const;

	protected:
		U32			m_elementsAllocated;
		T			**m_array;

	private:
		void		Construct(U32 index);
		void		Destruct(U32 index);
		void		DestructAll(void);
		void		Preallocate(U32 preallocation);
		void		Resize(U32 needed);

		T			*m_contiguous;
		U32			m_prealloc;
		U32			m_elementsUsed;

		GPGDeallocateFunction	*m_pDeallocate;
};

template<class T>
GPGINLINE GPGArray<T>::GPGArray(U32 preallocation)
{
	m_pDeallocate=NULL;
	m_elementsUsed=0;
	m_elementsAllocated=0;
	m_array=NULL;
	m_prealloc=0;
	m_contiguous=NULL;
	Preallocate(preallocation);
	if(preallocation>0)
		Resize((preallocation>GPGARRAY_MIN)? preallocation: GPGARRAY_MIN);
}

template<class T>
GPGINLINE GPGArray<T>::~GPGArray(void)
{
	/// \todo investigate major threading issue
#if GPGARRAY_UNSAFEMEMORY
	//* temporarily reinstate dellocation function appropriate for our data
	GPGAllocateFunction		*pAllocateFunction;
	GPGDeallocateFunction	*pDeallocateFunction;
	GPGReallocateFunction	*pReallocateFunction;

	GPGGetMemoryFunctions(&pAllocateFunction,&pDeallocateFunction,
													&pReallocateFunction);

	GPGSetMemoryFunctions(pAllocateFunction,m_pDeallocate,
													pReallocateFunction);
#endif

	DestructAll();
	GPGASSERT(m_array==NULL);
	GPGASSERT(m_contiguous==NULL);

#if GPGARRAY_UNSAFEMEMORY
	//* restore current dellocation function
	GPGSetMemoryFunctions(pAllocateFunction,pDeallocateFunction,
													pReallocateFunction);
#endif
}

template<class T>
GPGINLINE T &GPGArray<T>::CreateNewElement(void)
{
	ResizeToAtLeast(m_elementsUsed+1);

	return GetElement(m_elementsUsed-1);
}

template<class T>
GPGINLINE void GPGArray<T>::Clear(U32 m_preallocation)
{
	DestructAll();
	Preallocate(m_preallocation);
	if(m_preallocation>0)
		Resize((m_preallocation>GPGARRAY_MIN)? m_preallocation: GPGARRAY_MIN);
}

template<class T>
GPGINLINE void GPGArray<T>::ResizeToExactly(U32 set)
{
	if(m_elementsAllocated<set)
		ResizeToAtLeast(set);
	else
	{
		U32 m;
		// if m_elementsUsed < set
		for(m=m_elementsUsed;m<set;m++)
			Construct(m);
		// if set < m_elementsUsed
		for(m=set;m<m_elementsUsed;m++)
			Destruct(m);

		m_elementsUsed=set;

		// NOTE doesn't deallocate space for pointers TODO?
	}
}

template<class T>
GPGINLINE void GPGArray<T>::ResizeToAtLeast(U32 required)
{
	if(m_elementsAllocated<required)
		Resize(required);

	U32 m;
	for(m=m_elementsUsed;m<required;m++)
		Construct(m);

	if(m_elementsUsed<required)
		m_elementsUsed=required;
}

template<class T>
GPGINLINE T *GPGArray<T>::Collect(void)
{
	U32 m;
	U32 newsize=m_elementsUsed;
	if(newsize<m_prealloc)
		newsize=m_prealloc;

	T *newbuffer=NULL;
	if(newsize>0)
	{
		newbuffer=new T[newsize];

		// copy old m_contiguous data
		for(m=0;m<m_prealloc;m++)
			newbuffer[m]=m_contiguous[m];

		// copy and destroy scattered data
		for(;m<newsize;m++)
		{
			newbuffer[m]= *(m_array[m]);

			Destruct(m);
		}
	}

	delete[] m_contiguous;
	m_contiguous=newbuffer;
	m_prealloc=newsize;

	// just assigns pointers into m_contiguous
	for(m=0;m<m_prealloc;m++)
		Construct(m);

	return m_contiguous;
}

template<class T>
GPGINLINE void GPGArray<T>::Construct(U32 index)
{
	if(index<m_prealloc)
	{
		m_array[index]=&m_contiguous[index];
		ResetElement(m_contiguous[index]);
	}
	else
		m_array[index]=new T;
}

template<class T>
GPGINLINE void GPGArray<T>::Destruct(U32 index)
{
	if(index>=m_prealloc)
		delete m_array[index];

	m_array[index]=NULL;
}

template<class T>
GPGINLINE void GPGArray<T>::DestructAll(void)
{
	U32 m;
	for(m=m_prealloc;m<m_elementsUsed;m++)
		Destruct(m);

	if(m_array)
	{
		GPGASSERT(m_pDeallocate);
		if(m_pDeallocate)
			m_pDeallocate(m_array);
	}
	m_array=NULL;

	m_elementsUsed=0;
	m_elementsAllocated=0;

	if(m_contiguous)
	{
		delete[] m_contiguous;
		m_contiguous=NULL;
	}
	m_prealloc=0;
}

template<class T>
GPGINLINE void GPGArray<T>::Preallocate(U32 preallocation)
{
	if(m_contiguous)
	{
		delete[] m_contiguous;
		m_contiguous=NULL;
	}

	m_prealloc=preallocation;
	if(m_prealloc>0)
		m_contiguous=new T[m_prealloc];
}

template<class T>
GPGINLINE void GPGArray<T>::Resize(U32 needed)
{
//	U32 last_size=m_elementsAllocated;

	if(m_elementsAllocated<GPGARRAY_MIN || m_elementsAllocated<needed)
	{
		m_elementsAllocated=(U32)(m_elementsAllocated*GPGARRAY_EXPANSION);

		if(m_elementsAllocated<needed)
			m_elementsAllocated=needed;

		if(m_elementsAllocated<GPGARRAY_MIN)
			m_elementsAllocated=GPGARRAY_MIN;

//#ifdef _DEBUG
#if FALSE
		// HACK always copy to new buffer
		T **temp=(T **)GPGAllocate(m_elementsAllocated*sizeof(T*));
		if(m_array)
			{
			memcpy(temp,m_array,last_size*sizeof(T*));
			m_pDeallocate(m_array);
			}
		m_array=temp;
#else
		m_array=(T **)GPGReallocate(m_array,m_elementsAllocated*sizeof(T*));
#endif
		m_pDeallocate=gs_pDeallocateFunction;

		GPGASSERT(m_array);
	}
}


template<class T>
GPGINLINE void GPGArray<T>::operator=(const GPGArray<T> &other)
{
	Clear();
	Append(other);
}


template<class T>
GPGINLINE void GPGArray<T>::Append(const GPGArray<T> &other)
{
	U32 offset=GetNumberElements();
	U32 m,length=other.GetNumberElements();

	ResizeToAtLeast(offset+length);
	for(m=0;m<length;m++)
		GetElement(offset+m)=other.GetElementConst(m);
}

#endif /* __GPGArray_h__ */
