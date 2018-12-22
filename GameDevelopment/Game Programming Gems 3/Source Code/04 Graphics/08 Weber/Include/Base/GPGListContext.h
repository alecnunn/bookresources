
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

#ifndef __GPGListContext_h__
#define __GPGListContext_h__


//* verify pointer usage with lists
#define	GPGLIST_STORE_CHECKPOINTER	TRUE

#define GPGLIST_CHECKPOINTER	(defined(_DEBUG) && GPGLIST_STORE_CHECKPOINTER)


/******************************************************************************
class GPGListContext

	Do not access members except through GPGList

	GPGListContext is similar to an iterator except that it does not contain
	a pointer to an GPGList<>.  All access to an GPGList<> is done directly
	through GPGList<> member functions with an GPGListContext argument.
	This means that
		1) GPGListContext is generic and does not need to be templated.
		2) The context is very lightweight and has no template instantiation.
		3) No special permissions are needed for context to list accesses.
		4) Your code always explicitly says which list it is accessing.

******************************************************************************/
class GPGListContext
	{
	public:
						GPGListContext(void)
							{
							Init();
							};

						GPGListContext(GPGListContext &operand)
							{
							Init();
							SetCurrent(operand.GetCurrent());
							SetAtTail(operand.GetAtTail());
							};

		void			Init(void)
							{
							m_current=NULL;
							m_at_tail=false;
#if GPGLIST_CHECKPOINTER
							m_pCoreList=NULL;
#endif
							};

virtual					~GPGListContext(void)
							{
							if(m_current)
								m_current->DecReferences();
							};

		GPGListContext	&operator=(GPGListContext &operand)
							{
							SetCurrent(operand.GetCurrent());
							SetAtTail(operand.GetAtTail());
							return *this;
							};

		GPGListNode		*GetCurrent(void)
							{
							CheckValid();
							return m_current;
							};
		void			SetCurrent(GPGListNode *set)
							{
							if(m_current)
								m_current->DecReferences();
							if(set)
								{
								GPGASSERT(set->GetValid());
								set->IncReferences();
								}
							m_current=set;
							};

		void			SetAtTail(bool set)					{ m_at_tail=set; };
		bool			GetAtTail(void)
							{
//							CheckValid();
							return m_at_tail;
							};


	private:
		void			CheckValid(void)
							{
							// if m_current is fine
							if(!m_current || m_current->GetValid())
								return;

							// find heir
							GPGListNode *heir=m_current;
							while(heir && !heir->GetValid())
								{
								heir=heir->GetHeir();
								}

							// if good heir, copy
							if(heir && heir->GetValid())
								{
//								printf("heir 0x%x\n",heir);
								SetCurrent(heir);
								SetAtTail(false);
								}
							// reset, reset
							else
								{
								SetCurrent(NULL);
								SetAtTail(false);
								}
							};

		GPGListNode		*m_current;
		bool			m_at_tail;

#if	GPGLIST_CHECKPOINTER
	public:
		void			SetCorePointer(const GPGCoreList *pSet)
													{ m_pCoreList=pSet; };
		bool			CheckCorePointer(const GPGCoreList *pCheck) const
							{
							if(m_pCoreList && m_pCoreList!=pCheck)
								GPGLOG("GPGListContext::CheckCorePointer"
										"(0x%x) does not match 0x%x\n",
								(unsigned int)pCheck,(unsigned int)m_pCoreList);
							GPGASSERT(!m_pCoreList || m_pCoreList==pCheck);
							return (!m_pCoreList || m_pCoreList==pCheck);
							};
#endif
#if	GPGLIST_STORE_CHECKPOINTER
	private:
		const GPGCoreList	*m_pCoreList;
#endif
	};



#endif /* __GPGListContext_h__ */
