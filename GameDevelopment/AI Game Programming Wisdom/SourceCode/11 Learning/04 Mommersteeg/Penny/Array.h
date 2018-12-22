//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Array.h
//----------------------------------------------------------------------------------------------

#ifndef __ARRAY_H_
#define __ARRAY_H_

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------------------------
// CArray: A simple, dynamically allocated array template; similar to the one used in MFC.
//----------------------------------------------------------------------------------------------

template <class TData>
class CArray
{
public:					
						CArray();
						~CArray();

	inline TData&		operator[](long index) { return Item(index); }

public:

	long				Add(TData ti);
	TData				RemoveAt(long index);
	void				Remove(TData ti);
	void				RemoveAll();
	TData&				Item(long index);
	void				SetSize(long size);
	long				GetSize() { return m_nSize; }
	CArray <TData> *	Clone();

protected:

	TData *				m_pItems;
	long				m_nSize;
};


//----------------------------------------------------------------------------------------------
// CArray: Constructor
//----------------------------------------------------------------------------------------------

template <class TData> 
CArray<TData>::CArray()
{
	m_pItems = NULL;
	m_nSize = 0;
}

//----------------------------------------------------------------------------------------------
// ~CArray: Destructor
//----------------------------------------------------------------------------------------------

template <class TData>
CArray<TData>::~CArray()
{
	if (m_nSize>0) {
		RemoveAll();
	}
}

//----------------------------------------------------------------------------------------------
// Add(): Adds a new item to the array; expands the array
//----------------------------------------------------------------------------------------------

template <class TData>
long CArray<TData>::Add(TData ti)
{
	TData * pItems = new TData[m_nSize+1];
	memcpy(pItems,m_pItems,sizeof(TData)*m_nSize);
	pItems[m_nSize] = ti;
	delete [] m_pItems;
	m_pItems = pItems;
	return m_nSize++;
}

//----------------------------------------------------------------------------------------------
// RemoveAt(): Removes item at specified index from the array; array is shrinked
//----------------------------------------------------------------------------------------------

template <class TData>
TData CArray<TData>::RemoveAt(long index)
{
	assert( index >= 0 && index < m_nSize );

	TData val = Item(index);
	TData * pItems = new TData[m_nSize-1];
	memcpy(pItems, m_pItems, sizeof(TData)*index);
	memcpy(pItems+index,m_pItems+index+1, sizeof(TData)*(m_nSize-index-1));
	delete [] m_pItems;
	m_pItems = pItems;
	m_nSize--;
	return val;
}

//----------------------------------------------------------------------------------------------
// Remove(): Removes the specified item from the array; array is shrinked
//----------------------------------------------------------------------------------------------

template <class TData>
void CArray<TData>::Remove(TData ti)
{
	long i = 0;	
	while (m_pItems[i] != ti && i<m_nSize) {
		i++;
	}
	if (i != m_nSize) {
		RemoveAt(i);
	}
}

//----------------------------------------------------------------------------------------------
// RemoveAll(): Removes all items from the array; array is emptied
//----------------------------------------------------------------------------------------------

template <class TData>
void CArray<TData>::RemoveAll()
{
	if (m_nSize>0)
	{
		delete [] m_pItems;
		m_pItems = NULL;
		m_nSize = 0;
	}
}

//----------------------------------------------------------------------------------------------
// Item(): Item at specified index is returned
//----------------------------------------------------------------------------------------------

template <class TData>
TData& CArray<TData>::Item(long index)
{
	assert( index >= 0 && index <= m_nSize );
	return m_pItems[index];
}

//----------------------------------------------------------------------------------------------
// SetSize(): Sets the size of the array; remains array contents (if they still fit)
//----------------------------------------------------------------------------------------------

template <class TData>
void CArray<TData>::SetSize(long size)
{
	TData * pItems;
	pItems = new TData[size];

	if (size > m_nSize)
	{
		memcpy(pItems,m_pItems,sizeof(TData)*m_nSize);
	}
	else 
	{
		memcpy(pItems,m_pItems,sizeof(TData)*size);
	}

	delete [] m_pItems;
	m_pItems = pItems;
	m_nSize = size;
}

//----------------------------------------------------------------------------------------------
// Clone(): Duplicates the array
//----------------------------------------------------------------------------------------------

template <class TData>
CArray<TData>* CArray<TData>::Clone() 
{
	CArray<TData>* copy = new CArray<TData>;
	copy.m_pItems = new TData[m_nSize];
	memcpy(copy.m_pItems, m_pItems, sizeof(TData)*m_nSize);
	copy.m_nSize = m_nSize;
	return copy;
}

//----------------------------------------------------------------------------------------------
#endif // __ARRAY_H_
