//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SlidingWindow.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
//	A sliding window is a cyclic array of a fixed size (dynamically allocated).
//	
//	It is ideal for monitoring sequences. The window is shifted by one position 
//	whenever a new element is added (i.e. each element at index i is moved to index i-1).
//	This is efficiently done by overloading the index operator.
//
//	Note:
//	With SetSize(), you set the size of the window. With GetSize(), you retrieve 
//	the number of elements in the window. The window is initially empty after SetSize(),
//	and no elements are shifted as long as the window is not entirely filled.
//----------------------------------------------------------------------------------------------

#ifndef __SLIDINGWINDOW_H
#define __SLIDINGWINDOW_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------------------------
// CSlidingWindow: a simple but efficient sliding window
//----------------------------------------------------------------------------------------------

template <class TData>
class CSlidingWindow {
public:
	// construction & destruction methods
						CSlidingWindow();
						CSlidingWindow(int nSize);
						~CSlidingWindow();

	// array manipulation methods
	TData *				Add(TData elt);
	int					SetSize(int nSize);
	int 				GetSize();
	void				Clear();

	// content manipulation methods
	const TData 		operator[](int iIndex) const;
	TData &				operator[](int iIndex);

protected:	
	int					m_nSize;
	int					m_iEnd;
	int					m_iNext;
	TData *				m_pElements;
	TData				m_Dropped;
};

//----------------------------------------------------------------------------------------------
// CSlidingWindow(): Constructor
//----------------------------------------------------------------------------------------------

template <class TData>
CSlidingWindow<TData>::CSlidingWindow() {	
	m_pElements = NULL;
	m_nSize = 0;	
	m_iNext = 0;
	m_iEnd = 0;
}

//----------------------------------------------------------------------------------------------
// CSlidingWindow(): Constructor 2
//----------------------------------------------------------------------------------------------

template <class TData>
CSlidingWindow<TData>::CSlidingWindow(int nSize) {	
	m_pElements = new TData[nSize];
	m_nSize = m_pElements == NULL ? 0 : nSize;
	m_iNext = 0;
	m_iEnd = 0;
}

//----------------------------------------------------------------------------------------------
// ~CSlidingWindow(): Destructor
//----------------------------------------------------------------------------------------------

template <class TData>
CSlidingWindow<TData>::~CSlidingWindow() {
	Clear();
}

//----------------------------------------------------------------------------------------------
// Add(): Adds a new element to the window
//----------------------------------------------------------------------------------------------

template <class TData>
TData * CSlidingWindow<TData>::Add(TData elt) {
	assert( m_nSize > 0 );

	if (m_iEnd < m_nSize) {
		m_pElements[m_iNext] = elt;
		m_iEnd ++;
		m_iNext ++;
		return NULL;
	} else {
		int index = m_iNext % m_nSize;
		m_Dropped = m_pElements[index];
		m_pElements[index] = elt;
		m_iNext = index + 1;
		return &m_Dropped;
	}
}

//----------------------------------------------------------------------------------------------
// SetSize(): Sets the size of the window
//----------------------------------------------------------------------------------------------

template <class TData>
int CSlidingWindow<TData>::SetSize(int nSize) {
	Clear();

	m_pElements = new TData[nSize];
	m_nSize = m_pElements == NULL ? 0 : nSize;
	m_iNext = 0;
	m_iEnd = 0;

	return m_nSize;
}

//----------------------------------------------------------------------------------------------
// GetSize(): Returns the *actual* number of elements in the window
//----------------------------------------------------------------------------------------------

template <class TData>
int CSlidingWindow<TData>::GetSize() {
	return m_iEnd;
}

//----------------------------------------------------------------------------------------------
// Clear(): Resets the window (not only removes the elements)
//----------------------------------------------------------------------------------------------

template <class TData>
void CSlidingWindow<TData>::Clear() {
	if (m_pElements != NULL) {
		delete [] m_pElements;
		m_pElements = NULL;
		m_nSize = 0;
		m_iNext = 0;
		m_iEnd = 0;
	}
}

//----------------------------------------------------------------------------------------------
// operator[](): returns the element at the specified index in the window (constant)
//----------------------------------------------------------------------------------------------

template <class TData>
const TData CSlidingWindow<TData>::operator[](int iIndex) const {
	assert(0 <= iIndex && iIndex < m_iEnd);

	if (m_iEnd < m_nSize) {		
		return m_pElements[iIndex];
	} else {
		return m_pElements[(m_iNext + iIndex) % m_nSize];
	}
}

//----------------------------------------------------------------------------------------------
// operator[](): returns the element at the specified index in the window (reference)
//----------------------------------------------------------------------------------------------

template <class TData>
TData & CSlidingWindow<TData>::operator[](int iIndex) {
	assert(0 <= iIndex && iIndex < m_iEnd);

	if (m_iEnd < m_nSize) {
		return m_pElements[iIndex];
	} else {
		return m_pElements[(m_iNext + iIndex) % m_nSize];
	}
}

//----------------------------------------------------------------------------------------------
#endif // __SLIDINGWINDOW_H