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

#ifndef __ListTemplates_h__
#define __ListTemplates_h__

#include <fstream.h>


template <class NODE, class INDEX> class CArray
{
protected:
	INDEX m_nElements;			// The number of elements in the array
	INDEX m_nLockedElements;	// The number of locked elements in the array
	unsigned char *m_pFlags;	// An array of element status flags
	NODE *m_pBuffer;			// The array of elements

public:
	enum { Locked = 0x80, Dirty = 0x40 };

	// Constructor/destructor methods
	CArray(INDEX nElements=0)
	{
		m_nElements = 0;
		if(nElements)
			Init(nElements);
	}
	~CArray()		{ Cleanup(); }

	// Init and Cleanup methods
	void Init(INDEX nElements)
	{
		Cleanup();
		m_nElements = nElements;
		m_nLockedElements = 0;
		m_pFlags = new unsigned char[m_nElements];
		memset(m_pFlags, 0, m_nElements);
		m_pBuffer = new NODE[m_nElements];
	}
	void Cleanup()
	{
		if(m_nElements)
		{
			delete []m_pBuffer;
			delete m_pFlags;
			m_nElements = 0;
		}
	}

	// Info methods
	INDEX GetElementCount()				{ return m_nElements; }
	INDEX GetElementSize()				{ return sizeof(NODE); }
	INDEX GetArraySize()				{ return GetElementCount() * GetElementSize(); }
	INDEX GetLockedElementCount()		{ return m_nLockedElements; }
	INDEX GetFreeElementCount()			{ return GetElementCount() - GetLockedElementCount(); }

	// Status flag methods
	unsigned char GetFlags(INDEX n)					{ return m_pFlags[n]; }
	void SetFlags(INDEX n, unsigned char nFlags)	{ m_pFlags[n] |= nFlags; }
	void ClearFlags(INDEX n, unsigned char nFlags)	{ m_pFlags[n] &= ~nFlags; }

	// Array manipulation methods
	NODE *GetBuffer()					{ return m_pBuffer; }
	NODE *operator[](INDEX n)			{ return &m_pBuffer[n]; }
};


template <class NODE, class INDEX> class CStackedArray : public CArray<NODE, INDEX>
{
protected:
	INDEX *m_pStack;			// A stack of free element indices

public:
	// Constructor/destructor methods
	CStackedArray(INDEX nElements=0)
	{
		m_nElements = 0;
		if(nElements)
			Init(nElements);
	}
	~CStackedArray()		{ Cleanup(); }

	// Init and Cleanup methods
	void Init(INDEX nElements)
	{
		Cleanup();
		CArray<NODE, INDEX>::Init(nElements);
		m_pStack = new INDEX[m_nElements];
		for(INDEX i=0; i<m_nElements; i++)
			m_pStack[i] = i;
	}
	void Cleanup()
	{
		if(m_nElements)
			delete m_pStack;
		CArray<NODE, INDEX>::Cleanup();
	}

	// Array manipulation methods
	INDEX LockElement()
	{
		INDEX nElement = (INDEX)-1;
		if(m_nLockedElements < m_nElements)
		{
			nElement = m_pStack[m_nLockedElements++];
			m_pFlags[nElement] = Locked;
		}
		return nElement;
	}
	void UnlockElement(INDEX n)
	{
		m_pFlags[n] = 0;
		m_pStack[--m_nLockedElements] = n;
	}

	INDEX GetStackIndex(INDEX n)		{ return m_pStack[n]; }
};

template <class NODE, class INDEX> class CPackedArray : public CArray<NODE, INDEX>
{
protected:
	INDEX m_nLowestUnused;		// The index of the lowest unused element in the array

public:
	// Constructor/destructor methods
	CPackedArray(INDEX nElements=0)
	{
		m_nElements = 0;
		if(nElements)
			Init(nElements);
	}
	~CPackedArray()		{ Cleanup(); }

	// Init and Cleanup methods
	void Init(INDEX nElements)
	{
		Cleanup();
		CArray<NODE, INDEX>::Init(nElements);
		m_nLowestUnused = 0;
	}
	void Cleanup()
	{
	}

	// Array manipulation methods
	INDEX LockElement()
	{
		INDEX nElement = (INDEX)-1;
		if(m_nLowestUnused < m_nElements)
		{
			nElement = m_nLowestUnused++;
			m_pFlags[nElement] |= Locked;
			m_nLockedElements++;
			while(m_nLowestUnused < m_nElements && (m_pFlags[m_nLowestUnused] & Locked))
				m_nLowestUnused++;
		}
		return nElement;
	}
	void UnlockElement(INDEX n)
	{
		m_pFlags[n] &= ~Locked;
		m_nLockedElements--;
		m_nLowestUnused = n < m_nLowestUnused ? n : m_nLowestUnused;
	}
};



/******************************************************************************
* Class: CListNode
*******************************************************************************
* Deriving from this class turns your class into a node that can be used in a 
* doubly-linked list. It was designed to work with CLinkedList (defined below)
* in such a way that objects can insert and remove themselves from the list.
* It is only templatized so it can use pointers of your class's type to keep
* you from having to cast all returned pointers.
Example:
#define CMyObjectList CLinkedList<CMyObject>
class CMyObject : public CListNode<CMyObject> { ... };
******************************************************************************/
template <class A> class CListNode
{
public:
	A *m_pNext;			// Points to next item in the list
	A *m_pPrevious;		// Points to previous item in the list

public:
	// Constructors/destructors
	CListNode()					{ m_pNext = m_pPrevious = NULL; }
	~CListNode()				{ Remove(); }
	static void InitList(CListNode<A> &nHead, CListNode<A> &nTail)
	{
		nHead.m_pNext = (A *)&nTail;
		nTail.m_pPrevious = (A *)&nHead;
	}

	// Members to get the next and previous pointers (must use Insert/Remove members to set them)
	A *GetNext()				{ return m_pNext; }
	A *GetPrevious()			{ return m_pPrevious; }

	// Functions for searching the list, override these functions to use
	DWORD GetID()				{ return 0; }
	const char *GetName()		{ return ""; }
	void Write(ostream &os)		{ os.write((char *)this, sizeof(A)); }
	bool Read(istream &is)
	{
		is.read((char *)this, sizeof(A));
		m_pNext = m_pPrevious = NULL;
		return (!is ? false : true);
	}

	// Members to insert into, remove from, and check status in a list
	bool IsInList()				{ return (m_pNext != NULL && m_pPrevious != NULL); }
	void InsertBefore(A *pNode)
	{
		if(pNode && !IsInList())
		{
			m_pNext = pNode;
			m_pPrevious = pNode->m_pPrevious;
			pNode->m_pPrevious->m_pNext = (A *)this;
			pNode->m_pPrevious = (A *)this;
		}
	}
	void InsertAfter(A *pNode)
	{
		if(pNode && !IsInList())
		{
			m_pPrevious = pNode;
			m_pNext = pNode->m_pNext;
			pNode->m_pNext->m_pPrevious = (A *)this;
			pNode->m_pNext = (A *)this;
		}
	}
	void Remove()
	{
		if(m_pPrevious)
			m_pPrevious->m_pNext = m_pNext;
		if(m_pNext)
			m_pNext->m_pPrevious = m_pPrevious;
		m_pNext = m_pPrevious = NULL;
	}

	// Members to find items in the list by ID, name, or index
	A *FindID(DWORD dwID)
	{
		for(A *pNode = (A *)this; pNode->IsInList(); pNode=pNode->GetNext())
		{
			if(dwID == pNode->GetID())
				return pNode;
		}
		return NULL;
	}
	A *FindName(const char *pszName)
	{
		for(A *pNode = (A *)this; pNode->IsInList(); pNode=pNode->GetNext())
		{
			if(stricmp(pszName, pNode->GetName()) == 0)
				return pNode;
		}
		return NULL;
	}
	A *FindIndex(int nIndex)
	{
		A *pNode = (A *)this;
		for(int i=0; pNode->IsInList() && i<nIndex; i++)
			pNode = pNode->GetNext();
		return pNode->IsInList() ? pNode : NULL;
	}
};

/*******************************************************************************
* Class: CLinkedList
********************************************************************************
* This class implements a doubly-linked list that allows the objects within it
* to insert and remove themselves without access to the actual list (to insert,
* a pointer to an item already in the list is needed). To accomplish this,
* static head and tail nodes must be kept instead of pointers, and no count
* variable can used (because the objects have no way to update them). The head
* and tail nodes point to each other when the list is empty. See CListNode above
* for more information.
Important Note:
The static head and tail nodes mark the end of the list, not NULL. Use
CListNode::IsInList() to see when you've reached the end, like this:
for(CObject *p = list.GetHead(); p->IsInList(); p = p->GetNext()) { ... }
*******************************************************************************/
template <class A> class CLinkedList
{
protected:
	CListNode<A> m_nHead;	// Points to head item
	CListNode<A> m_nTail;	// Points to tail item

public:
	// Constructors/destructors
	CLinkedList()		{ CListNode<A>::InitList(m_nHead, m_nTail); }
	~CLinkedList()		{ RemoveAll(); }

	// Members to get the head and tail items of the list
	A *GetHead()		{ return m_nHead.GetNext(); }
	A *GetTail()		{ return m_nTail.GetPrevious(); }

	// Members to add items to the list
	void AddHead(A *p)					{ p->InsertAfter((A *)&m_nHead); }
	void AddTail(A *p)					{ p->InsertBefore((A *)&m_nTail); }
	void InsertBefore(A *p, A *pNode)	{ p->InsertBefore(pNode); }
	void InsertAfter(A *p, A *pNode)	{ p->InsertAfter(pNode); }

	// Members to remove items from the list
	A *Remove(A *p)		{ p->Remove(); return p; }
	A *RemoveHead()		{ return Remove(GetHead()); }
	A *RemoveTail()		{ return Remove(GetTail()); }
	void RemoveAll()	{ while(GetHead()->IsInList()) delete GetHead(); }

	// Members to get the size of the list
	bool IsEmpty()		{ return !GetHead()->IsInList(); }
	int GetCount()
	{
		register A *pNode = GetHead();
		for(int i=0; pNode->IsInList(); i++)
			pNode = pNode->GetNext();
		return i;
	}

	// Members to find items in the list by ID, name, or index
	A *FindID(DWORD dwID, A *pNode=NULL)
	{
		if(!pNode)
			pNode = GetHead();
		return pNode->FindID(dwID);
	}
	A *FindName(const char *pszName, A *pNode=NULL)
	{
		if(!pNode)
			pNode = GetHead();
		return pNode->FindName(pszName);
	}
	A *FindIndex(int nIndex, A *pNode=NULL)
	{
		if(!pNode)
			pNode = GetHead();
		return pNode->FindIndex(nIndex);
	}

	bool Read(istream &is)
	{
		int n;
		is.read((char *)&n, sizeof(int));
		if(!is)
			return false;
		for(int i=0; i<n; i++)
		{
			A *pNode = new A;
			if(!pNode->Read(is))
				return false;
			AddTail(pNode);
		}
		return true;
	}
	void Write(ostream &os)
	{
		int n = GetCount();
		os.write((char *)&n, sizeof(int));
		for(A *pNode=GetHead(); pNode->IsInList(); pNode=pNode->GetNext())
			pNode->Write(os);
	}
};

/******************************************************************************
* Class: CTreeNode
*******************************************************************************
* Deriving from this class turns your class into a node to use in a tree of the
* specified order. It is templatized simply so it can use pointers of your
* class's type, keeping you from havig to cast pointers you get back.
* Example:
* class CMyObject : public CTreeNode<CMyObject, 4> { ... };
******************************************************************************/
template <class A, int nOrder> class CTreeNode
{
protected:
	A *m_pParent;
	A *m_pChild[nOrder];

public:
	CTreeNode(A *pParent=NULL)
	{
		m_pParent = pParent;
		for(register int i=0; i<nOrder; i++)
			m_pChild[i] = NULL;
	}
	~CTreeNode()
	{
		for(register int i=0; i<nOrder; i++)
		{
			if(m_pParent && m_pParent->m_pChild[i] == this)
				m_pParent->SetChild(i, NULL);
			if(m_pChild[i])
				delete m_pChild[i];
		}
	}

	A *GetParent()				{ return m_pParent; }
	void SetParent(A *p)		{ m_pParent = p; }
	A *GetChild(int n)			{ return m_pChild[n]; }
	void SetChild(int n, A *p)	{ m_pChild[n] = p; }
};

/******************************************************************************
* Class: CAVLTreeNode
*******************************************************************************
* Deriving from this class turns your class into a node to use in an AVL tree.
* It was designed to work with CAVLTree (defined below).
* Example:
* #define CObjectTree CAVLTree<CObject>
* class CObject : public CAVLTreeNode<CObject> { ... };
******************************************************************************/
template <class A> class CAVLTreeNode : public CTreeNode<A, 2>
{
protected:
	short m_nBalance;

protected:
	// Rotates pRoot left or right. Used to balance the tree. Returns true when the new root's parent needs to change its balance.
	// (To visualize what it's doing for either direction, replace nDirection with Left and nOtherDir with Right or vice-versa.)
	static bool RotateOnce(A *pRoot, int nDirection)
	{
		int nOtherDir = (nDirection == Left) ? Right : Left;			// Get the other direction
		A *pParent = pRoot->m_pParent;
		int nSide = (pParent->m_pChild[Left] == pRoot) ? Left : Right;

		A *pNewRoot = pRoot->m_pChild[nOtherDir];						// Move nOtherDir child into root's current position (to move root in nDirection)
		pParent->m_pChild[nSide] = pNewRoot;							// (Make parent point to new root)
		pNewRoot->m_pParent = pParent;									// (Don't forget to change its parent)

		pRoot->m_pChild[nOtherDir] = pNewRoot->m_pChild[nDirection];	// Move new root's nDirection child to old root's nOtherDir
		if(pRoot->m_pChild[nOtherDir])
			pRoot->m_pChild[nOtherDir]->m_pParent = pRoot;				// (Don't forget to change its parent)
		
		pNewRoot->m_pChild[nDirection] = pRoot;							// Move old root down to the nDirection
		pRoot->m_pParent = pNewRoot;									// (Don't forget to change its parent)

		// Update balances and return true if new root's parent's balance has changed
		bool bRetVal = (pNewRoot->m_nBalance == 0) ? false : true;
		pRoot->m_nBalance = -(nDirection ? ++(pNewRoot->m_nBalance) : --(pNewRoot->m_nBalance));
		return bRetVal;
	}
	
	// Rotates pRoot left or right twice (by rotating child in opposite direction first). Used to balance the tree. Returns true when the new root's parent needs to change its balance.
	// (To visualize what it's doing for either direction, replace nDirection with Left and nOtherDir with Right or vice-versa.)
	static bool RotateTwice(A *pRoot, int nDirection)
	{
		int nOtherDir = (nDirection == Left) ? Right : Left;			// Get the other direction
		A *pParent = pRoot->m_pParent;
		int nSide = (pParent->m_pChild[Left] == pRoot) ? Left : Right;

		A *pNode = pRoot->m_pChild[nOtherDir];							// nOtherDir child doesn't move (but it gets a new parent)
		A *pNewRoot = pNode->m_pChild[nDirection];						// Move its nDirection child into root's current position (to move root in nDirection)
		pParent->m_pChild[nSide] = pNewRoot;							// (Make parent point to new root)
		pNewRoot->m_pParent = pParent;									// (Don't forget to change its parent)

		pRoot->m_pChild[nOtherDir] = pNewRoot->m_pChild[nDirection];	// Move new root's nDirection child to old root's nOtherDir
		if(pRoot->m_pChild[nOtherDir])
			pRoot->m_pChild[nOtherDir]->m_pParent = pRoot;				// (Don't forget to change its parent)

		pNode->m_pChild[nDirection] = pNewRoot->m_pChild[nOtherDir];	// Move new root's nOtherDir child to the node that's not moving
		if(pNode->m_pChild[nDirection])
			pNode->m_pChild[nDirection]->m_pParent = pNode;				// (Don't forget to change its parent)

		pNewRoot->m_pChild[nDirection] = pRoot;							// Move old root down to the nDirection
		pRoot->m_pParent = pNewRoot;									// (Don't forget to change its parent)
		pNewRoot->m_pChild[nOtherDir] = pNode;							// Put the node that didn't move under new root
		pNode->m_pParent = pNewRoot;									// (Don't forget to change its parent)

		// Update balances and return true
		pNewRoot->m_pChild[0]->m_nBalance = -max(pNewRoot->m_nBalance, 0);
		pNewRoot->m_pChild[1]->m_nBalance = -min(pNewRoot->m_nBalance, 0);
		pNewRoot->m_nBalance = 0;
		return true;
	}

	// Rebalances a single node. Returns true when the balance of pRoot's parent has been changed.
	static bool ReBalance(A *pRoot) 
	{
		bool bHeightChange = false;
		if(pRoot->m_nBalance < -1)
			bHeightChange = (pRoot->m_pChild[0]->m_nBalance == 1) ? RotateTwice(pRoot, Right) : RotateOnce(pRoot, Right);
		else if(pRoot->m_nBalance > 1)
			bHeightChange = (pRoot->m_pChild[1]->m_nBalance == -1) ? RotateTwice(pRoot, Left) : RotateOnce(pRoot, Left);
		return bHeightChange;
	}

	// Moves a locked node down in the tree down to where it can be safely deleted.
	static void MoveDown(A *pNode)
	{
		// Get pNode's parent and the side that points to pNode
		A *pParent = pNode->m_pParent;
		int nSide = (pParent->m_pChild[Left] == pNode) ? Left : Right;

		// Find the next item in the sort order (right one, then left all the way down)
		A *pSwap = pNode->m_pChild[Right];
		while(pSwap->m_pChild[Left])
			pSwap = pSwap->m_pChild[Left];

		// Swap the parent and right child pointers of the two nodes (must be handled differently if pNode is pSwap's parent)
		A *pTemp = pSwap->m_pChild[Right];
		if(pSwap->m_pParent == pNode)
		{
			pNode->m_pParent = pSwap;
			pSwap->m_pChild[Right] = pNode;
		}
		else
		{
			pNode->m_pParent = pSwap->m_pParent;
			pNode->m_pParent->m_pChild[(pNode->m_pParent->m_pChild[Left] == pSwap) ? Left : Right] = pNode;
			pSwap->m_pChild[Right] = pNode->m_pChild[Right];
			pSwap->m_pChild[Right]->m_pParent = pSwap;
		}
		pNode->m_pChild[Right] = pTemp;
		if(pNode->m_pChild[Right])
			pNode->m_pChild[Right]->m_pParent = pNode;
		pSwap->m_pParent = pParent;
		pParent->m_pChild[nSide] = pSwap;

		// Swap the left child of the two nodes (easy because pNode's left child is never NULL and pSwap's left child is always NULL)
		pSwap->m_pChild[Left] = pNode->m_pChild[Left];
		pSwap->m_pChild[Left]->m_pParent = pSwap;
		pNode->m_pChild[Left] = NULL;

		// Swap the balance factors of the two nodes
		short nTemp = pNode->m_nBalance;
		pNode->m_nBalance = pSwap->m_nBalance;
		pSwap->m_nBalance = nTemp;
	}

public:
	enum { Left, Right };
	CAVLTreeNode() : CTreeNode<A, 2>()	{ m_nBalance = 0; }
	bool IsInTree()						{ return (m_pParent != NULL); }
	int GetBalance()					{ return m_nBalance; }

	// Inserts the current node into an AVL tree which has a root of pRoot
	// pRoot is actually a placeholder node for the root, the tree actually starts in its left subtree
	// Returns true for success, false for failure
	bool Insert(A *pRoot)
	{
		A *pParent = pRoot;
		A *pNode = pRoot->m_pChild[Left];
		int nCompare = -1;
		while(pNode)
		{
			nCompare = pNode->Compare((A *)this);
			if(!nCompare)
				return false;
			pParent = pNode;
			pNode = pNode->m_pChild[(nCompare > 0) ? Right : Left];
		}
		pNode = (A *)this;
		pNode->m_pParent = pParent;
		pParent->m_pChild[(nCompare > 0) ? Right : Left] = pNode;

		while(pParent->IsInTree())
		{
			pNode = pParent;
			pParent = pNode->m_pParent;
			pNode->m_nBalance += nCompare;
			nCompare = (pParent->m_pChild[Left] == pNode) ? -1 : 1;
			if(!pNode->m_nBalance || ReBalance(pNode))
				break;
		}
		return true;
	}

	// Removes the current node from an AVL tree
	// Returns true for success, false for failure
	bool Remove()
	{
		if(m_pChild[Left] && m_pChild[Right])
			MoveDown((A *)this);

		int nDirection = (m_pChild[Left] != NULL) ? Left : Right;
		A *pParent = m_pParent;
		A *pNode = m_pChild[nDirection];
		m_pParent = NULL;
		m_pChild[nDirection] = NULL;
		int nCompare = (pParent->m_pChild[Left] == (A *)this) ? -1 : 1;
		pParent->m_pChild[(nCompare > 0) ? Right : Left] = pNode;
		if(pNode)
			pNode->m_pParent = pParent;

		while(pParent->IsInTree())
		{
			pNode = pParent;
			pParent = pNode->m_pParent;
			pNode->m_nBalance -= nCompare;
			nCompare = (pParent->m_pChild[Left] == pNode) ? -1 : 1;
			if(pNode->m_nBalance && !ReBalance(pNode))
				break;
		}
		return true;
	}
};


/*******************************************************************************
* Class: CAVLTree
********************************************************************************
* This class implements an AVL tree.
*******************************************************************************/
template <class A, class KeyType> class CAVLTree
{
protected:
	CAVLTreeNode<A> m_nRoot;
	int m_nCount;

	void RecursiveCount(A *pNode)
	{
		if(!pNode)
			return;
		m_nCount++;
		RecursiveCount(pNode->GetChild(0));
		RecursiveCount(pNode->GetChild(1));
	}

public:
	CAVLTree()				{ m_nCount = 0; }
	bool IsEmpty()			{ return (GetRoot() == NULL); }
	A *GetRoot()			{ return m_nRoot.GetChild(0); }
	bool Insert(A *pNode)	{ return pNode->Insert((A *)&m_nRoot); }
	bool Remove(A *pNode)	{ return pNode->Remove(); }

	int GetCount()
	{
		m_nCount = 0;
		RecursiveCount(GetRoot());
		return m_nCount;
	}
	int GetMaxDepth()
	{
		A *pNode = GetRoot();
		for(int nDepth=0; pNode; nDepth++)
			pNode = pNode->GetChild(pNode->GetBalance() <= 0 ? 0 : 1);
		return nDepth;
	}

	A *FindKey(KeyType tKey)
	{
		int nCompare;
		A *pNode = GetRoot();
		while(pNode && (nCompare = pNode->Compare(tKey)) != 0)
			pNode = (nCompare > 0) ? pNode->GetChild(1) : pNode->GetChild(0);
		return pNode;
	}
	A *Remove(KeyType tKey)
	{
		A *pNode = FindKey(tKey);
		if(pNode)
			pNode->Remove();
		return pNode;
	}
};

#endif	// __ListTemplates_h__
