
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

#ifndef __GPGListNode_h__
#define __GPGListNode_h__


class GPGCoreList;

//* 0=none 1=static GPGUnitAllocator 2=static GPGChunkPool 3=local GPGUnitAllocator
#ifndef GPGLIST_USEALLOCATOR
#define	GPGLIST_USEALLOCATOR	3
#endif



/******************************************************************************
class GPGListNode

	m_references are for multiple contexts and one for the list itself
	other nodes don't increment m_references

******************************************************************************/
class GPGListNode
	{
	public:
#if GPGLIST_USEALLOCATOR==3
						GPGListNode(void) { GPGASSERT(0); }
		void			Reset(GPGCoreList *corelist)
							{
							m_pCoreList = corelist;
#else
						GPGListNode(void) { Reset(); };
		void			Reset(void)
							{
#endif
							m_references=1;
							m_valid=true;

							m_pointer=NULL;
							m_previous=NULL;
							m_next=NULL;
							m_heir=NULL;
							};

		void			SetPointer(void *set)			{ m_pointer=set; };
		void			*GetPointer(void) const			{ return m_pointer; };
		void			**GetPointerLocation(void)		{ return &m_pointer;};

		void			SetPrevious(GPGListNode *set)	{ m_previous=set; };
		GPGListNode		*GetPrevious(void) const		{ return m_previous;};

		void			SetNext(GPGListNode *set)		{ m_next=set; };
		GPGListNode		*GetNext(void) const			{ return m_next; };

		void			SetHeir(GPGListNode *set)		{ m_heir=set; };
		GPGListNode		*GetHeir(void) const			{ return m_heir; };

		void			SetValid(bool set)				{ m_valid=set;};
		bool			GetValid(void) const			{ return m_valid; };

		void			Abandon(void)
							{
							// if someone is still pointing at this node,
							// leave a trail to a potentially valid node
							if(m_references>1)
								{
								if(m_next)
									m_heir=m_next;
								else
									m_heir=m_previous;

								if(m_heir)
									m_heir->IncReferences();
								}

							SetValid(false);
							DecReferences();
							};

		void			IncReferences(void)
							{
							m_references++;
							};
		void			DecReferences(void);

		private:
			long			m_references;
			bool			m_valid;
			void			*m_pointer;
#if GPGLIST_USEALLOCATOR==3
			GPGCoreList		*m_pCoreList;
#endif

			GPGListNode		*m_previous;
			GPGListNode		*m_next;
			GPGListNode		*m_heir;
	};



#endif /* __GPGListNode_h__ */
