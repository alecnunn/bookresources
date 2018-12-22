//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    IteratorList.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
//	The IteratorList is a double linked list that uses an implicit itorator.
//
//	This data structure has all the benefits of a list, and adds O(1) indexing for
//	sequential iterations.
//----------------------------------------------------------------------------------------------

#ifndef __ITERATORLIST_H
#define __ITERATORLIST_H

//----------------------------------------------------------------------------------------------
// CListContainer: An entry in the list; it contains forward and backward links
//----------------------------------------------------------------------------------------------

template <class TData> 
class CListContainer {
public:
	TData			m_data;
	void *			m_pNext;
	void *			m_pPrevious;
};


//----------------------------------------------------------------------------------------------
// CIterator: A list iterator
//----------------------------------------------------------------------------------------------

template <class TData>
class CIterator {
public:
	// constructor method
								CIterator() { m_pContainer = NULL; }

public:
	// iterator setup and maintenance routines
	void						AttachToList(CListContainer <TData> * pContainer, int iIndex = 0);
	void						SetIndex(int iIndex) { m_index = iIndex; }
	int 						GetIndex() const { return m_index; }

public:
	// iterator manipulation
	BOOL						Next();
	BOOL						Previous();
	CListContainer <TData> *	GetContainer() const;
	
protected:
	CListContainer <TData> *	m_pContainer;
	int							m_index;
};


//----------------------------------------------------------------------------------------------
// CIteratorList: List template with implicit iterator
//----------------------------------------------------------------------------------------------

template <class TData>
class CIteratorList {
public: 
	// construction & destruction methods
								CIteratorList();
								~CIteratorList();

public:
	// list manipulation methods
	BOOL						AddHead(TData data);
	BOOL						AddTail(TData data);
	BOOL						InsertAt(int iIndex, TData data);
	void						RemoveHead();
	void						RemoveTail();
	void						RemoveAt(int iIndex);	
	void						RemoveAll();

public:
	// overloaded operators
    const TData					operator[](int iIndex) const;
    TData &						operator[](int iIndex);

public:
	// get methods
	inline TData &				GetHead() const { assert(m_nSize > 0); return m_pHead->m_data; }
	inline TData &				GetTail() const { assert(m_nSize > 0); return m_pTail->m_data; }
	int							GetSize() const { return m_nSize; }

protected:
	CListContainer <TData> *	GetContainer(int iIndex);
	CListContainer <TData> *	MoveIterator(CIterator <TData> * pIterator, int iTargetIndex);

protected:
	CListContainer <TData> *	m_pHead;
	CListContainer <TData> *	m_pTail;
	CIterator <TData>			m_itLastAccessPosition;
	int							m_nSize;
};

//----------------------------------------------------------------------------------------------
// AttachToList(): attaches the iterator to a certain container in the list
//----------------------------------------------------------------------------------------------

template <class TData>
void CIterator<TData>::AttachToList(CListContainer <TData> * pContainer, int iIndex) {
	assert( pContainer != NULL );
	m_pContainer = pContainer;
	m_index = iIndex;
}

//----------------------------------------------------------------------------------------------
// Next(): moves the iterator to the next node in the list
//----------------------------------------------------------------------------------------------

template <class TData>
BOOL CIterator<TData>::Next() {
	assert( m_pContainer != NULL );
	if (m_pContainer->m_pNext != NULL) {
		m_pContainer = (CListContainer <TData> *) m_pContainer->m_pNext;
		m_index++;
		return TRUE;
	}
	return FALSE;	
}

//----------------------------------------------------------------------------------------------
// Previous(): moves the iterator to the previous node in the list
//----------------------------------------------------------------------------------------------

template <class TData>
BOOL CIterator<TData>::Previous() {
	assert( m_pContainer != NULL );
	if (m_pContainer->m_pPrevious != NULL) {
		m_pContainer = (CListContainer <TData> *) m_pContainer->m_pPrevious;
		m_index--;
		return TRUE;
	}
	return FALSE;	
}

//----------------------------------------------------------------------------------------------
// GetContainer(): returns the container that is currently selected by the iterator
//----------------------------------------------------------------------------------------------

template <class TData>
CListContainer <TData> * CIterator<TData>::GetContainer() const {
	assert( m_pContainer != NULL ); 
	return m_pContainer;
}

//----------------------------------------------------------------------------------------------
// CIteratorList(): constructor
//----------------------------------------------------------------------------------------------

template <class TData>
CIteratorList<TData>::CIteratorList() {
	m_pHead = NULL;
	m_pTail = NULL;
	m_nSize = 0;
}

//----------------------------------------------------------------------------------------------
// ~CIteratorList(): destructor
//----------------------------------------------------------------------------------------------

template <class TData>
CIteratorList<TData>::~CIteratorList() {
	RemoveAll();
}

//----------------------------------------------------------------------------------------------
// AddHead(): adds an element to the head of the list
//----------------------------------------------------------------------------------------------

template <class TData>
BOOL CIteratorList<TData>::AddHead(TData data) {
	return InsertAt(0, data);
}

//----------------------------------------------------------------------------------------------
// AddTail(): adds an element to the tail of the list
//----------------------------------------------------------------------------------------------

template <class TData>
BOOL CIteratorList<TData>::AddTail(TData data) {
	return InsertAt(m_nSize, data);
}

//----------------------------------------------------------------------------------------------
// InsertAt(): inserts an element at the specified position in the list
//----------------------------------------------------------------------------------------------

template <class TData>
BOOL CIteratorList<TData>::InsertAt(int iIndex, TData data) {
	assert( iIndex >= 0 && iIndex <= m_nSize);

	// create new container
	CListContainer <TData> * pContainer = new CListContainer <TData>;
	if (pContainer == NULL) {
		return FALSE;
	}
	pContainer->m_data = data;

	if (iIndex < m_nSize) {
		// insert item in list 
		CListContainer <TData> * pReplace;
		pReplace = GetContainer(iIndex);

		pContainer->m_pNext = (void *)pReplace;
		pContainer->m_pPrevious = pReplace->m_pPrevious;
		pReplace->m_pPrevious = (void *)pContainer;

		if (pContainer->m_pPrevious != NULL) {
			CListContainer <TData> * pPrevious;
			pPrevious = (CListContainer <TData> *) (pContainer->m_pPrevious);
			pPrevious->m_pNext = (void *)pContainer;
		} else {
			m_pHead = pContainer;
		}
	} else {
		// add item to end of list
		pContainer->m_pNext = NULL;
		pContainer->m_pPrevious = (void *)m_pTail;
		if (m_pTail != NULL) {
			m_pTail->m_pNext = (void *)pContainer;
		} else {
			m_pHead = pContainer;
		}
		m_pTail = pContainer;
	}

	// move lap iterator
	m_itLastAccessPosition.AttachToList(pContainer, iIndex);

	m_nSize++;
	return TRUE;
}

//----------------------------------------------------------------------------------------------
// RemoveHead(): removes the element at the head of the list
//----------------------------------------------------------------------------------------------

template <class TData>
void CIteratorList<TData>::RemoveHead() {
	RemoveAt(0);
}

//----------------------------------------------------------------------------------------------
// RemoveTail(): removes the element at the tail of the list
//----------------------------------------------------------------------------------------------

template <class TData>
void CIteratorList<TData>::RemoveTail() {
	RemoveAt(m_nSize-1);
}

//----------------------------------------------------------------------------------------------
// RemoveHead(): removes the element at the specified position in the list
//----------------------------------------------------------------------------------------------

template <class TData>
void CIteratorList<TData>::RemoveAt(int iIndex) {
	assert( iIndex >= 0 && iIndex < m_nSize);

	CListContainer <TData> * pRemove, * pNext, * pPrevious;
	pRemove = GetContainer(iIndex);
	pNext = (CListContainer <TData> *) (pRemove->m_pNext);
	pPrevious = (CListContainer <TData> *) (pRemove->m_pPrevious);

	if (pPrevious != NULL) {
		pPrevious->m_pNext = (void *)pNext;
	} else {
		m_pHead = pNext;
	}

	if (pNext != NULL) {
		pNext->m_pPrevious = (void *)pPrevious;
	} else {
		m_pTail = pPrevious;
	}

	// release container
	delete pRemove;

	// update lap iterator index if necessary
	int iLapIndex = m_itLastAccessPosition.GetIndex();
	if (iIndex <= iLapIndex) {
		m_itLastAccessPosition.SetIndex(iLapIndex+1);
	}

	m_nSize--;	
}

//----------------------------------------------------------------------------------------------
// RemoveAll(): removes all elements from the list
//----------------------------------------------------------------------------------------------

template <class TData>
void CIteratorList<TData>::RemoveAll() {
	while (m_pHead != NULL) {
		CListContainer <TData> * pNext = (CListContainer <TData> *) m_pHead->m_pNext;
		delete m_pHead;
		m_pHead = pNext;
	}
	m_pTail = NULL;
	m_nSize = 0;
}

//----------------------------------------------------------------------------------------------
// GetContainer(): returns the container at the specified position in the list
//----------------------------------------------------------------------------------------------

template <class TData>
CListContainer <TData> * CIteratorList<TData>::GetContainer(int iIndex) {
	CIterator <TData> itHead, itTail;
	int distHead, distTail, distLap;
	
	itHead.AttachToList(m_pHead, 0);
	itTail.AttachToList(m_pTail, m_nSize-1);

	distHead = iIndex;
	distTail = m_nSize - 1 - iIndex;
	distLap = abs(iIndex - m_itLastAccessPosition.GetIndex());

	if (distHead < distTail) {
		if (distHead < distLap) {
			// distHead is the nearest to target distance
			m_itLastAccessPosition = itHead;
			return MoveIterator(&itHead, iIndex);
		} else {
			// distLap is the nearest to target distance
			return MoveIterator(&m_itLastAccessPosition, iIndex);
		}
	} else {
		if (distTail < distLap) {
			// distTail is the nearest to target distance
			m_itLastAccessPosition = itTail;
			return MoveIterator(&itTail, iIndex);
		} else {
			// distLap is the nearest to target distance
			return MoveIterator(&m_itLastAccessPosition, iIndex);
		}
	}
}

//----------------------------------------------------------------------------------------------
// operator[](): returns the element at the specified position in the list (constant)
//----------------------------------------------------------------------------------------------

template <class TData>
const TData	CIteratorList<TData>::operator[](int iIndex) const {
	assert(iIndex >= 0 && iIndex < m_nSize);

	CListContainer <TData> * pContainer;
	pContainer = GetContainer(iIndex);
	return pContainer->m_data;
}

//----------------------------------------------------------------------------------------------
// operator[](): returns the element at the specified position in the list (reference)
//----------------------------------------------------------------------------------------------

template <class TData>
TData &	CIteratorList<TData>::operator[](int iIndex) {
	assert(iIndex >= 0 && iIndex < m_nSize);

	CListContainer <TData> * pContainer;
	pContainer = GetContainer(iIndex);
	return pContainer->m_data;
}

//----------------------------------------------------------------------------------------------
// MoveIterator(): Moves an iterator to the specified target index
//----------------------------------------------------------------------------------------------

template <class TData>
CListContainer <TData> * CIteratorList<TData>::MoveIterator(CIterator <TData> * pIterator, int iTargetIndex) {
	if (pIterator->GetIndex() < iTargetIndex) {
		while (pIterator->GetIndex() != iTargetIndex) pIterator->Next();
	} else {
		while (pIterator->GetIndex() != iTargetIndex) pIterator->Previous();
	}
	return pIterator->GetContainer();
}

//----------------------------------------------------------------------------------------------
#endif // __ITERATORLIST_H