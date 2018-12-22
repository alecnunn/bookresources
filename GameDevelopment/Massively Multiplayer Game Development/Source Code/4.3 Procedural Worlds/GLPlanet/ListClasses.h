/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

class CPtrListNode
{
protected:
	CPtrListNode *m_pNext;
	CPtrListNode *m_pPrevious;
	void *m_pData;

public:
	CPtrListNode(void *pData=NULL)
	{
		m_pNext = m_pPrevious = NULL;
		m_pData = pData;
	}
	~CPtrListNode()
	{
		Remove();
	}

	static void InitList(CPtrListNode &nHead, CPtrListNode &nTail)
	{
		nHead.m_pNext = &nTail;
		nTail.m_pPrevious = &nHead;
	}

	CPtrListNode *GetNext()			{ return m_pNext; }
	CPtrListNode *GetPrevious()		{ return m_pPrevious; }
	void *GetData()				{ return m_pData; }
	bool IsInList()				{ return (m_pNext != NULL && m_pPrevious != NULL); }

	void InsertBefore(CPtrListNode *pNode)
	{
		if(pNode && !IsInList())
		{
			m_pNext = pNode;
			m_pPrevious = pNode->m_pPrevious;
			pNode->m_pPrevious->m_pNext = this;
			pNode->m_pPrevious = this;
		}
	}
	void InsertBefore(void *pData)
	{
		CPtrListNode *pNode = new CPtrListNode(pData);
		pNode->InsertBefore(this);
	}
	void InsertAfter(CPtrListNode *pNode)
	{
		if(pNode && !IsInList())
		{
			m_pPrevious = pNode;
			m_pNext = pNode->m_pNext;
			pNode->m_pNext->m_pPrevious = this;
			pNode->m_pNext = this;
		}
	}
	void InsertAfter(void *pData)
	{
		CPtrListNode *pNode = new CPtrListNode(pData);
		pNode->InsertAfter(this);
	}
	void Remove()
	{
		if(m_pPrevious)
			m_pPrevious->m_pNext = m_pNext;
		if(m_pNext)
			m_pNext->m_pPrevious = m_pPrevious;
		m_pNext = m_pPrevious = NULL;
	}
};

class CPtrList
{
protected:
	CPtrListNode m_nHead;	// Points to head item
	CPtrListNode m_nTail;	// Points to tail item

public:
	// Constructors/destructors
	CPtrList()			{ CPtrListNode::InitList(m_nHead, m_nTail); }
	~CPtrList()			{ RemoveAll(); }

	// Members to get the head and tail nodes of the list
	CPtrListNode *GetHead()	{ return m_nHead.GetNext(); }
	CPtrListNode *GetTail()	{ return m_nTail.GetPrevious(); }

	// Members to add items to the list
	void AddHead(void *pData)
	{
		CPtrListNode *p = new CPtrListNode(pData);
		p->InsertAfter(&m_nHead);
	}
	void AddTail(void *pData)
	{
		CPtrListNode *p = new CPtrListNode(pData);
		p->InsertBefore(&m_nTail);
	}

	// Members to remove items from the list
	void *RemoveHead()
	{
		CPtrListNode *p = GetHead();
		void *pData = p->GetData();
		delete p;
		return pData;
	}
	void *RemoveTail()
	{
		CPtrListNode *p = GetTail();
		void *pData = p->GetData();
		delete p;
		return pData;
	}
	void RemoveAll()	{ while(GetHead()->IsInList()) delete GetHead(); }
};
