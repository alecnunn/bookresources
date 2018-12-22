// Some standard useful classes, templates, etc.


#ifndef TOMSLIB_INCLUDED
#define TOMSLIB_INCLUDED


#include <assert.h>
#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>


#include "stdtypes.h"




// What a hack!!!!!
// "Cures" the scope of for() variables in the MS compiler.
#define for if(0){}else for





#ifdef _DEBUG

//#define ASSERT(exp) { assert(exp); __assume(exp); }
#define ASSERT(exp) { if (!(exp)){_asm { int 3 };} }

inline TRACE ( char *fmt, ... )
{
	va_list vlist;

	char pchTemp[1000];

	va_start ( vlist, fmt );
	vsprintf ( pchTemp, fmt, vlist );
	OutputDebugString ( pchTemp );
	va_end ( vlist );
}

#else //#ifdef DEBUG

#define ASSERT sizeof
#define TRACE sizeof

#endif //#else //#ifdef DEBUG


// Always happens, whether in release or debug mode.
#define RELEASEASSERT(exp) { if (!(exp)){_asm { int 3 };} }



static void DumpFunction ( char *pchFormat, ... )
{
	va_list va_ptr;
	char pchTemp[256];

	va_start ( va_ptr, pchFormat );

	vsprintf ( pchTemp, pchFormat, va_ptr );
	OutputDebugString ( pchTemp );

	va_end ( va_ptr );
}

#if 1
#define DUMP(args) DumpFunction args
#else
#define DUMP(args) sizeof args
#endif


// Couldn't do it with classes and templates,
// have to do it the old fashioned way.

// Defines a link
#define DlinkDefine(classname,linkname)\
	classname *linkname##pNextItem;\
	classname *linkname##pPrevItem


// Defines the class methods for the link.
#define DlinkMethods(classname,linkname)																		\
	void linkname##Init ( void ) { linkname##pNextItem = NULL; linkname##pPrevItem = NULL; }					\
																												\
	/* Find the next in the list. */																			\
	classname *linkname##Next ( void )																			\
	{																											\
		ASSERT ( ( linkname##pPrevItem == NULL ) || ( linkname##pPrevItem->linkname##pNextItem == this ) );		\
		ASSERT ( ( linkname##pNextItem == NULL ) || ( linkname##pNextItem->linkname##pPrevItem == this ) );		\
		return linkname##pNextItem;																				\
	}																											\
																												\
	/* Find the previous in the list. */																		\
	classname *linkname##Prev ( void )																			\
	{																											\
		ASSERT ( ( linkname##pPrevItem == NULL ) || ( linkname##pPrevItem->linkname##pNextItem == this ) );		\
		ASSERT ( ( linkname##pNextItem == NULL ) || ( linkname##pNextItem->linkname##pPrevItem == this ) );		\
		return linkname##pPrevItem;																				\
	}																											\
																												\
	/* Remove this item from the list. */																		\
	/* Returns the next item. */																				\
	classname *linkname##Del ( void )																			\
	{																											\
		classname *pNext = linkname##pNextItem;																	\
		classname *pPrev = linkname##pPrevItem;																	\
		ASSERT ( ( pPrev == NULL ) || ( pPrev->linkname##pNextItem == this ) );									\
		ASSERT ( ( pNext == NULL ) || ( pNext->linkname##pPrevItem == this ) );									\
		if ( pPrev != NULL )																					\
		{																										\
			pPrev->linkname##pNextItem = pNext;																	\
		}																										\
		if ( pNext != NULL )																					\
		{																										\
			pNext->linkname##pPrevItem = pPrev;																	\
		}																										\
		linkname##pNextItem = NULL;																				\
		linkname##pPrevItem = NULL;																				\
		return pNext;																							\
	}																											\
																												\
	/* Remove this item from the list. */																		\
	/* Assumes that this is the first item in the list for speed (asserted). */									\
	/* Returns the next item. */																				\
	classname *linkname##DelScan ( void )																		\
	{																											\
		ASSERT ( linkname##pPrevItem == NULL );																	\
		classname *pNext = linkname##pNextItem;																	\
		ASSERT ( ( pNext == NULL ) || ( pNext->linkname##pPrevItem == this ) );									\
		if ( pNext != NULL )																					\
		{																										\
			pNext->linkname##pPrevItem = NULL;																	\
		}																										\
		linkname##pNextItem = NULL;																				\
		linkname##pPrevItem = NULL;																				\
		return pNext;																							\
	}																											\
																												\
	/* Deletes the item, given a pointer to the first item in the list. */										\
	/* Modifies the point as necessary. */																		\
	/* Returns the next item. */																				\
	/* Use like   pThing->DelFromFirst ( &pFirst ); */															\
	classname *linkname##DelFromFirst ( classname **ppFirst )													\
	{																											\
		ASSERT ( ppFirst != NULL );																				\
		ASSERT ( *ppFirst != NULL );																			\
		if ( *ppFirst == this )																					\
		{																										\
			ASSERT ( linkname##pPrevItem == NULL );																\
			/* First in the list. */																			\
			*ppFirst = linkname##Del();																			\
			return *ppFirst;																					\
		}																										\
		else																									\
		{																										\
			/* DLink lists already know their previous item. */													\
			return ( linkname##Del() );																			\
		}																										\
	}																											\
																												\
	/* Add the item after the given item. */																	\
	/* Return the new next item. */																				\
	classname *linkname##AddAfter ( classname *pPrev )															\
	{																											\
		ASSERT ( linkname##pNextItem == NULL );																	\
		ASSERT ( linkname##pPrevItem == NULL );																	\
		ASSERT ( pPrev != NULL );																				\
		linkname##pNextItem = pPrev->linkname##pNextItem;														\
		linkname##pPrevItem = pPrev;																			\
		if ( linkname##pNextItem != NULL )																		\
		{																										\
			linkname##pNextItem->linkname##pPrevItem = this;													\
		}																										\
		pPrev->linkname##pNextItem = this;																		\
		return ( linkname##pNextItem );																			\
	}																											\
																												\
	/* Add the item after the given item. */																	\
	/* Assumes this is the last item in the list for speed (asserted) */										\
	void linkname##AddAfterEnd ( classname *pPrev )																\
	{																											\
		ASSERT ( linkname##pNextItem == NULL );																	\
		ASSERT ( linkname##pPrevItem == NULL );																	\
		ASSERT ( pPrev != NULL );																				\
		ASSERT ( pPrev->linkname##pNextItem == NULL );															\
		linkname##pNextItem = NULL;																				\
		linkname##pPrevItem = pPrev;																			\
		pPrev->linkname##pNextItem = this;																		\
	}																											\
																												\
	/* Add before given item. */																				\
	/* Returns the new previous item. */																		\
	classname *linkname##AddBefore ( classname *pNext )															\
	{																											\
		ASSERT ( linkname##pNextItem == NULL );																	\
		ASSERT ( linkname##pPrevItem == NULL );																	\
		ASSERT ( pNext != NULL );																				\
		linkname##pNextItem = pNext;																			\
		linkname##pPrevItem = pNext->linkname##pPrevItem;														\
		if ( linkname##pPrevItem != NULL )																		\
		{																										\
			linkname##pPrevItem->linkname##pNextItem = this;													\
		}																										\
		pNext->linkname##pPrevItem = this;																		\
		return ( linkname##pPrevItem );																			\
	}																											\
																												\
	/* Add the item before the given item. */																	\
	/* Assumes this is the first item in the list for speed (asserted) */										\
	void linkname##AddBeforeStart ( classname *pNext )															\
	{																											\
		ASSERT ( linkname##pNextItem == NULL );																	\
		ASSERT ( linkname##pPrevItem == NULL );																	\
		ASSERT ( pNext != NULL );																				\
		ASSERT ( pNext->linkname##pPrevItem == NULL );															\
		linkname##pPrevItem = NULL;																				\
		linkname##pNextItem = pNext;																			\
		pNext->linkname##pPrevItem = this;																		\
	}																											\
																												\
	/* Find the last item in the list. */																		\
	classname *linkname##FindLast ( void )																		\
	{																											\
		classname *pCurr = this;																				\
		ASSERT ( pCurr != NULL );																				\
		while ( pCurr->linkname##pNextItem != NULL )															\
		{																										\
			ASSERT ( ( pCurr->linkname##pPrevItem == NULL ) || ( pCurr->linkname##pPrevItem->linkname##pNextItem == pCurr ) );	\
			ASSERT ( ( pCurr->linkname##pNextItem == NULL ) || ( pCurr->linkname##pNextItem->linkname##pPrevItem == pCurr ) );	\
			pCurr = pCurr->linkname##pNextItem;																	\
		}																										\
		return pCurr;																							\
	}																											\
																												\
	/* Find the first item in the list. */																		\
	classname *linkname##FindFirst ( void )																		\
	{																											\
		classname *pCurr = this;																				\
		ASSERT ( pCurr != NULL );																				\
		while ( pCurr->linkname##pPrevItem != NULL )															\
		{																										\
			ASSERT ( ( pCurr->linkname##pPrevItem == NULL ) || ( pCurr->linkname##pPrevItem->linkname##pNextItem == pCurr ) );	\
			ASSERT ( ( pCurr->linkname##pNextItem == NULL ) || ( pCurr->linkname##pNextItem->linkname##pPrevItem == pCurr ) );	\
			pCurr = pCurr->linkname##pPrevItem;																	\
		}																										\
		return pCurr;																							\
	}																											\
																												\
	/* Consistency check - checks the current list is sound. */													\
	void linkname##ConsistencyCheck ( void )																	\
	{																											\
		classname *pCurr = this;																						\
		/* Scan backwards. */																					\
		ASSERT ( pCurr != NULL );																				\
		while ( pCurr->linkname##pPrevItem != NULL )															\
		{																										\
			ASSERT ( ( pCurr->linkname##pPrevItem == NULL ) || ( pCurr->linkname##pPrevItem->linkname##pNextItem == pCurr ) );				\
			ASSERT ( ( pCurr->linkname##pNextItem == NULL ) || ( pCurr->linkname##pNextItem->linkname##pPrevItem == pCurr ) );				\
			pCurr = pCurr->linkname##pPrevItem;																	\
		}																										\
		ASSERT ( ( pCurr->linkname##pPrevItem == NULL ) || ( pCurr->linkname##pPrevItem->linkname##pNextItem == pCurr ) );					\
		ASSERT ( ( pCurr->linkname##pNextItem == NULL ) || ( pCurr->linkname##pNextItem->linkname##pPrevItem == pCurr ) );					\
																												\
		/* Scan forwards. */																					\
		pCurr = this;																							\
		ASSERT ( pCurr != NULL );																				\
		while ( pCurr->linkname##pNextItem != NULL )															\
		{																										\
			ASSERT ( ( pCurr->linkname##pPrevItem == NULL ) || ( pCurr->linkname##pPrevItem->linkname##pNextItem == pCurr ) );				\
			ASSERT ( ( pCurr->linkname##pNextItem == NULL ) || ( pCurr->linkname##pNextItem->linkname##pPrevItem == pCurr ) );				\
			pCurr = pCurr->linkname##pNextItem;																	\
		}																										\
		ASSERT ( ( pCurr->linkname##pPrevItem == NULL ) || ( pCurr->linkname##pPrevItem->linkname##pNextItem == pCurr ) );					\
		ASSERT ( ( pCurr->linkname##pNextItem == NULL ) || ( pCurr->linkname##pNextItem->linkname##pPrevItem == pCurr ) );					\
	}																											\
																												\
	/* Adds the item to the end of this list */																	\
	void linkname##AddToEnd ( classname *pItem )																\
	{																											\
		ASSERT ( pItem != NULL );																				\
		linkname##AddAfterEnd ( pItem->linkname##FindLast() );													\
	}																											\
																												\
	/* Adds the item the the start of this list */																\
	void linkname##AddToStart ( classname *pItem )																\
	{																											\
		ASSERT ( pItem != NULL );																				\
		linkname##AddBeforeStart ( pItem->linkname##FindFirst() );												\
	}																											\
																												\
	/* Check that all the pointer are NULL, ready to be deleted */												\
	bool linkname##CheckNull ( void )																			\
	{																											\
		return ( ( linkname##pNextItem == NULL ) && ( linkname##pPrevItem == NULL ) );							\
	}																											\
																												\
	/* Calls delete() on all the objects in this list and deletes it */											\
	void linkname##DelWholeList ( void )																		\
	{																											\
		classname *pCur = linkname##FindFirst();																\
		while ( pCur != NULL )																					\
		{																										\
			classname *pNext = pCur->linkname##DelScan();														\
			delete ( pCur );																					\
			pCur = pNext;																						\
		}																										\
	}																											\







static int CountBitsIn ( unsigned int dwInput )
{
	int iCount = 0;
	unsigned int dwTemp = dwInput;
	while ( dwTemp != 0 )
	{
		dwTemp = ( dwTemp & ( dwTemp - 1 ) );
		iCount++;
	}
	return ( iCount );
}


// Swap things.
template <class S> void Swap ( S &a, S &b )
{
	S temp = a;
	a = b;
	b = temp;
}


// Count number of items in a static array, e.g. strings, etc.
// So for: DWORD dwArray[20], NumItemsIn ( dwArray ) = 20
template <class T> int NumItemsIn ( T &thing )
{
	return ( sizeof(thing) / sizeof (*thing) );
}


// Safe version of strncpy - always puts a trailing 0 on the end.
template <class C> C *SafeStrncpy ( C *dest, const C *src, size_t number )
{
	strncpy ( dest, src, number );
	dest[number-1] = 0;
	return ( dest );
}



// A moderately good "close enough" thing.
inline bool CloseEnough ( float f1, float f2, float epsilon = 0.0001f )
{
	if ( fabsf ( f1 ) > fabsf ( f2 ) )
	{
		return ( fabsf ( f1 - f2 ) <= fabsf ( epsilon * f1 ) + epsilon );
	}
	else
	{
		return ( fabsf ( f1 - f2 ) <= fabsf ( epsilon * f2 ) + epsilon );
	}
}




// "Values" to pass to templates.
// Thanks to Jan for this perversion!
class Yes
{
public:
	enum { bVal = TRUE };
};

class No
{
public:
	enum { bVal = FALSE };
};




// Slow - checks the heap every time.
#define CHECK_HEAP 0

// A binary heap. Actually stores pointers to type THING, not the THINGs themselves.
template <class THING, class SORT> class BinaryHeap
{

private:
	struct Blob
	{
		THING				*pThing;
		SORT				Sort;
	};

	Blob					*pBlobArray;
	int						iCurrentSize;
	int						iAllocatedSize;

	int						iCurrentPos;

#ifdef DEBUG
	// A flag that says using FindNext, RemoveNext and RemoveCurrent are OK to call.
	bool					bFindNextValid;
#endif


public:

	BinaryHeap ( void )
	{
		pBlobArray = NULL;
		iCurrentSize = 0;
		iAllocatedSize = 0;
#ifdef DEBUG
		bFindNextValid = FALSE;
#endif
		iCurrentPos = 0;
	}

	~BinaryHeap ( void )
	{
		if ( pBlobArray != NULL )
		{
			delete[] pBlobArray;
			pBlobArray = NULL;
		}
		iCurrentSize = 0;
		iAllocatedSize = 0;
	}


	void Check ( void )
	{
#if CHECK_HEAP
#ifdef DEBUG
		for ( int iCurPos = iCurrentSize-1; iCurPos >= 2; iCurPos-- )
		{
			ASSERT ( pBlobArray[iCurPos].Sort <= pBlobArray[iCurPos>>1].Sort );
			ASSERT ( pBlobArray[iCurPos].pThing != NULL );
		}
#endif
#endif
	}

	void Add ( THING *pThisThing, SORT ThisSort )
	{
		Check();

#ifdef DEBUG
		bFindNextValid = FALSE;
#endif
		iCurrentPos = 0;

		if ( iCurrentSize <= 1 )
		{
			iCurrentSize = 1;
		}

		// Make sure it's big enough.
		if ( iAllocatedSize <= iCurrentSize )
		{
			// Add 50% to the allocated size.
			iAllocatedSize = ( iAllocatedSize >> 1 ) + iAllocatedSize;
			// And then a bit more.
			iAllocatedSize += 32;
			// And round up to 1k array size.
			iAllocatedSize = ( iAllocatedSize + 1023 ) & ~1023;

			Blob *pOldBlobArray = pBlobArray;
			pBlobArray = new Blob[iAllocatedSize];
			ASSERT ( pBlobArray != NULL );
			if ( pOldBlobArray != NULL )
			{
				memcpy ( pBlobArray, pOldBlobArray, ( iCurrentSize + 1 ) * sizeof ( Blob ) );
				delete[] pOldBlobArray;
			}
			Check();
		}

		// And add the item.
		iCurrentPos = iCurrentSize;
		while ( ( iCurrentPos > 1 ) && ( pBlobArray[iCurrentPos >> 1].Sort < ThisSort ) )
		{
			pBlobArray[iCurrentPos] = pBlobArray[iCurrentPos >> 1];
			iCurrentPos >>= 1;
		}
		pBlobArray[iCurrentPos].Sort = ThisSort;
		pBlobArray[iCurrentPos].pThing = pThisThing;

		iCurrentSize++;

		Check();
	}


	THING *FindFirst ( void )
	{
		if ( iCurrentSize > 1 )
		{
			ASSERT ( pBlobArray != NULL );
#ifdef DEBUG
			bFindNextValid = TRUE;
#endif
			iCurrentPos = 1;
			return pBlobArray[1].pThing;
		}
		else
		{
#ifdef DEBUG
			bFindNextValid = FALSE;
#endif
			iCurrentPos = 0;
			return NULL;
		}
	}

	// Must have called FindFirst first.
	// THIS DOES NOT TRAVERSE IN SORTED ORDER!
	THING *FindNextUnsorted ( void )
	{
#ifdef DEBUG
		ASSERT ( bFindNextValid );
#endif
		if ( iCurrentPos >= iCurrentSize - 1 )
		{
			// Reached the end.
			return NULL;
		}
		else
		{
			iCurrentPos++;

			return pBlobArray[iCurrentPos].pThing;
		}
	}

	// Must have called FindFirst/FindNext first.
	SORT GetCurrentSort ( void )
	{
#ifdef DEBUG
		ASSERT ( bFindNextValid );
#endif
		ASSERT ( iCurrentPos < iCurrentSize );
		ASSERT ( pBlobArray != NULL );
		return pBlobArray[iCurrentPos].Sort;
	}

	// Must have called FindFirst/FindNext first.
	THING *RemoveCurrent ( void )
	{
#ifdef DEBUG
		ASSERT ( bFindNextValid );
		bFindNextValid = FALSE;
#endif
		if ( iCurrentPos < ( iCurrentSize - 1 ) )
		{
			ASSERT ( pBlobArray != NULL );
			THING *pThing = pBlobArray[iCurrentPos].pThing;

			SORT MovedSort = pBlobArray[iCurrentSize-1].Sort;


			// First bubble this item up the list until
			// the parent is greater or equal to the last item in the heap.
			while ( ( iCurrentPos > 1 ) &&
					( pBlobArray[iCurrentPos>>1].Sort < MovedSort ) )
			{
				pBlobArray[iCurrentPos] = pBlobArray[iCurrentPos>>1];
				iCurrentPos = iCurrentPos >> 1;
			}



			// Then delete it, and replace it by the last in the heap,
			// then bubble that item down the heap again.
			iCurrentSize--;

			// And bubble the last item back down the tree.
			while ( (iCurrentPos<<1) < iCurrentSize )
			{
				if ( ( MovedSort >= pBlobArray[(iCurrentPos<<1)+0].Sort ) &&
					 ( ( ((iCurrentPos<<1)+1) >= iCurrentSize ) ||
					   ( MovedSort >= pBlobArray[(iCurrentPos<<1)+1].Sort ) ) )
				{
					// Yep - fits here.
					break;
				}
				else
				{
					// Find the bigger of the two, and move it up.
					if ( ( ((iCurrentPos<<1)+1) < iCurrentSize ) &&
						 ( pBlobArray[(iCurrentPos<<1)+0].Sort < pBlobArray[(iCurrentPos<<1)+1].Sort ) )
					{
						pBlobArray[iCurrentPos] = pBlobArray[(iCurrentPos<<1)+1];
						iCurrentPos = (iCurrentPos<<1)+1;
					}
					else
					{
						pBlobArray[iCurrentPos] = pBlobArray[(iCurrentPos<<1)+0];
						iCurrentPos = (iCurrentPos<<1)+0;
					}
				}
			}

			// Fits here.
			pBlobArray[iCurrentPos] = pBlobArray[iCurrentSize];
			pBlobArray[iCurrentSize].pThing = NULL;

			Check();

			return pThing;
		}
		else if ( iCurrentPos == iCurrentSize - 1 )
		{
			// This is already the last item - that was easy!
			iCurrentSize--;
			THING *pThing = pBlobArray[iCurrentPos].pThing;
			return pThing;
		}
		else
		{
			return NULL;
		}
	}

	// Must have called FindFirst first.
	THING *RemoveNext ( void )
	{
#ifdef DEBUG
		ASSERT ( bFindNextValid );
#endif
		iCurrentPos++;
		return RemoveCurrent();
	}

	THING *RemoveFirst ( void )
	{
#ifdef DEBUG
		// Keep the assert happy.
		bFindNextValid = TRUE;
#endif
		iCurrentPos = 1;
		return RemoveCurrent();
	}

	// Number of items in the heap.
	int SizeOfHeap ( void )
	{
		return ( iCurrentSize - 1 );
	}

};





// An arbitrary-sized list template class.
// Designed to hold _unsorted_ data, but only RemoveItem()
// actually disturbs the order, so you can use it for general arrays
// if you don't use that function.
template <class T>
class ArbitraryList
{
	T		*pT;				// The list.
	int		iSize;				// The current size of the list.
	int		iReservedSize;		// The current reserved size of the list.


public:

	// Constructor, with optional initial size setting.
	ArbitraryList ( int iInitialSize = 0 )
	{
		pT = NULL;
		iSize = 0;
		iReservedSize = 0;
		if ( iInitialSize > 0 )
		{
			SizeTo ( iInitialSize );
		}
	}

	// Destructor.
	~ArbitraryList ( void )
	{
		if ( pT == NULL )
		{
			ASSERT ( iReservedSize == 0 );
			ASSERT ( iSize == 0 );
		}
		else
		{
			ASSERT ( iReservedSize > 0 );
			ASSERT ( iSize >= 0 );
			delete[] pT;
			pT = NULL;
		}
	}

	// Returns the pointer to the given item.
	T *Item ( const int iItem )
	{
		ASSERT ( iItem < iSize );
		return ( &pT[iItem] );
	}

	const T *ConstItem ( const int iItem ) const
	{
		ASSERT ( iItem < iSize );
		return ( &pT[iItem] );
	}

	// Or use the [] operator.
	T &operator [] ( const int iItem )
	{
		return *Item ( iItem );
	}

	const T &operator [] ( const int iItem ) const 
	{
		return *ConstItem ( iItem );
	}

	// Returns the pointer to the first item.
	T *Ptr ( void )
	{
		return ( pT );
	}

	// Returns the size of the list
	int Size ( void ) const
	{
		return iSize;
	}

	// Grows or shrinks the list to this number of items.
	// Preserves existing items.
	// Items that fall off the end of a shrink may vanish.
	// Returns the pointer to the first item.
	// Set bBinAtZero to false if you don't want the memory to be freed,
	// even though the size is 0. This speeds things up for a list that is
	// frequently used, even though it's zeroed between uses.
	T *SizeTo ( int iNum, bool bBinAtZero = true )
	{
		ASSERT ( iNum >= 0 );
		iSize = iNum;
		if ( iNum <= iReservedSize )
		{
			if ( iNum == 0 )
			{
				// Shrunk to 0 - bin the memory.
				delete[] pT;
				pT = NULL;
				iReservedSize = 0;
			}
			else
			{
				ASSERT ( pT != NULL );
			}
			return ( pT );
		}
		else
		{
			// Need to grow. Grow by 50% more than
			// needed to avoid constant regrows.
			int iNewSize = ( iNum * 3 ) >> 1;
			if ( pT == NULL )
			{
				ASSERT ( iReservedSize == 0 );
				pT = new T [iNewSize];
			}
			else
			{
				ASSERT ( iReservedSize != 0 );

				T *pOldT = pT;
				pT = new T[iNewSize];
				for ( int i = 0; i < iReservedSize; i++ )
				{
					pT[i] = pOldT[i];
				}
				delete[] pOldT;
			}
			ASSERT ( pT != NULL );
			iReservedSize = iNewSize;
			return ( pT );
		}
	}

	// Adds one item to the list and returns a pointer to that new item.
	T *AddItem ( void )
	{
		SizeTo ( iSize + 1 );
		return ( &pT[iSize-1] );
	}

	// Frees all the array memory. Just a fancy way of saying SizeTo(0).
	void FreeMem ( void )
	{
		SizeTo ( 0, true );
	}

	// Removes the given item number by copying the last item
	// to that position and shrinking the list.
	// NOTE! Code relies on this behaviour - don't change it.
	void RemoveItem ( int iItemNumber )
	{
		ASSERT ( iItemNumber < iSize );
		pT[iItemNumber] = pT[iSize-1];
		SizeTo ( iSize - 1 );
	}

	// Copy the specified data into the list.
	void CopyFrom ( int iFirstItem, T *p, int iNumItems )
	{
		for ( int i = 0; i < iNumItems; i++ )
		{
			*(Item ( i + iFirstItem ) ) = p[i];
		}
	}

	// A copy from another arbitrary list of the same type.
	void CopyFrom ( int iFirstItem, ArbitraryList<T> &other, int iFirstOtherItem, int iNumItems )
	{
		for ( int i = 0; i < iNumItems; i++ )
		{
			*(Item ( i + iFirstItem ) ) = *(other.Item ( i + iFirstOtherItem ) );
		}
	}


	// Copy constructor.
	ArbitraryList ( const ArbitraryList<T> &other )
	{
		int iNumItems = other.Size();

		pT = NULL;
		iSize = 0;
		iReservedSize = 0;
		if ( iInitialSize > 0 )
		{
			SizeTo ( iNumItems );
		}
		for ( int i = 0; i < iNumItems; i++ )
		{
			*(Item(i) ) = *(other.Item(i) );
		}
	}


};






#define MIN(a,b) ( (a) > (b) ? (b) : (a) )
#define MAX(a,b) ( (a) < (b) ? (b) : (a) )





// "tweaks"
struct Tweak;


// The root the tree will hang off.
extern Tweak TweakRoot;
// The currently selected one.
extern Tweak *ptweakCurrent;


// The base class
struct Tweak
{
	const char *pchName;
	const char *pchDescription;
	Tweak *ptweakParent;	// Parent.
	Tweak *ptweakChild;		// First child.
	Tweak *ptweakNext;		// Next sibling.

	bool	bChildrenShown;

	enum Type
	{
		TweakType_Node,		// A node of the tree, not actually a value.
		TweakType_Float,	// A float
		TweakType_Int		// An int
	} type;					// We don't want no steenkin' RTTI.


	Tweak ( void )
	{
		pchName = "";
		pchDescription = "";
		ptweakParent = NULL;
		ptweakChild = NULL;
		ptweakNext = NULL;

		bChildrenShown = false;

		type = TweakType_Node;
	}

	virtual Type GetType ( void ) { return TweakType_Node; }

	virtual void Inc ( void ) { ASSERT ( FALSE ); }
	virtual void Dec ( void ) { ASSERT ( FALSE ); }
	virtual void FineInc ( void ) { ASSERT ( FALSE ); }
	virtual void FineDec ( void ) { ASSERT ( FALSE ); }
	virtual void Neg ( void ) { ASSERT ( FALSE ); }
	virtual void Default ( void ) { ASSERT ( FALSE ); }

	virtual float GetFloat ( void ) { ASSERT ( FALSE ); return 0.0f; }
	virtual int GetInt ( void ) { ASSERT ( FALSE ); return 0; }

	Tweak *NextShown ( void )
	{
		Tweak *ptweakTheNext = NULL;
		if ( ( type == Tweak::TweakType_Node ) && bChildrenShown )
		{
			ptweakTheNext = ptweakChild;
		}
		else
		{
			ptweakTheNext = ptweakNext;
		}
		Tweak *ptweakThis = this;
		while ( ptweakTheNext == NULL )
		{
			// Run out of nexts - try the parent's next.
			if ( ptweakThis->ptweakParent == NULL )
			{
				// Hit the bottom - there is no next.
				return NULL;
			}
			ptweakThis = ptweakThis->ptweakParent;
			ptweakTheNext = ptweakThis->ptweakNext;
		}
		return ptweakTheNext;
	}

private:
	// stuff not allowed.
	Tweak (const Tweak &other) {ASSERT(false);}
	void operator = (Tweak &other) {ASSERT(false);}

};


// The root.
extern Tweak *pUnsortedTweakRoot;


struct TweakFloat : Tweak
{
	float val;			// Current value.
	float def;			// Default value.

	// NOTE! pchNewName and pchNewDescription are not owned by this, they're static.
	TweakFloat ( const char *pchNewName, const char *pchNewDescription, float fDefaultValue )
	{
		// This just adds this in any order.
		// They will be sorted into trees later.
		type = TweakType_Float;
		pchName = pchNewName;
		if ( pchNewDescription == NULL )
		{
			// Don't want NULLs all over the place.
			pchDescription = "";
		}
		else
		{
			pchDescription = pchNewDescription;
		}
		ptweakParent = NULL;
		ptweakChild = NULL;
		ptweakNext = pUnsortedTweakRoot;
		pUnsortedTweakRoot = this;

		def = fDefaultValue;
		val = fDefaultValue;

		bChildrenShown = false;
	}
	
	virtual Type GetType ( void )
	{
		return TweakType_Float;
	}

	virtual float GetFloat ( void )
	{
		return val;
	}
	virtual int GetInt ( void )
	{
		return (int)val;
	}
	virtual void Inc ( void )
	{
		val *= 1.1f;
	}
	virtual void Dec ( void )
	{
		val /= 1.1f;
	}
	virtual void Neg ( void )
	{
		val = -val;
	}
	virtual void FineInc ( void )
	{
		val *= 1.01f;
	}
	virtual void FineDec ( void )
	{
		val /= 1.01f;
	}
	virtual void Default ( void )
	{
		val = def;
	}

	// And now actually getting/setting the info.
	operator float ( void ) const
	{
		return val;
	}
	void operator= ( const float &newval )
	{
		val = newval;
	}

};

struct TweakInt : Tweak
{
	int val;			// Current value.
	int def;			// Default value.

	// NOTE! pchNewName and pchNewDescription are not owned by this, they're static.
	TweakInt ( char *pchNewName, char *pchNewDescription, int iDefaultValue )
	{
		// This just adds this in any order.
		// They will be sorted into trees later.
		type = TweakType_Int;
		pchName = pchNewName;
		if ( pchNewDescription == NULL )
		{
			// Don't want NULLs all over the place.
			pchDescription = "";
		}
		else
		{
			pchDescription = pchNewDescription;
		}
		ptweakParent = NULL;
		ptweakChild = NULL;
		ptweakNext = pUnsortedTweakRoot;
		pUnsortedTweakRoot = this;

		def = iDefaultValue;
		val = iDefaultValue;

		bChildrenShown = false;
	}
	
	virtual Type GetType ( void )
	{
		return TweakType_Int;
	}

	virtual float GetFloat ( void )
	{
		return (float)val;
	}
	virtual int GetInt ( void )
	{
		return val;
	}
	virtual void Inc ( void )
	{
		if ( ( val >= 0 ) && ( val < 16 ) )
		{
			val++;
		}
		else
		{
			val += val >> 4;
		}
	}
	virtual void Dec ( void )
	{
		if ( ( val >= 0 ) && ( val < 16 ) )
		{
			val--;
		}
		else
		{
			val -= val >> 4;
		}
	}
	virtual void Neg ( void )
	{
		val = -val;
	}
	virtual void FineInc ( void )
	{
		if ( val >= 0 )
		{
			val++;
		}
		else
		{
			val--;
		}
	}
	virtual void FineDec ( void )
	{
		if ( val >= 0 )
		{
			val--;
		}
		else
		{
			val++;
		}
	}
	virtual void Default ( void )
	{
		val = def;
	}

	// And now actually getting/setting the info.
	operator int ( void ) const
	{
		return val;
	}
	void operator= ( const int &newval )
	{
		val = newval;
	}

};



// Call this sometime after all tweaks have been added - sorts them into a tree.
void SortTweaksIntoAPrettyTree ( void );

void TweakNext ( void );
void TweakPrev ( void );
void TweakOpen ( void );
void TweakClose ( void );
void TweakInc ( void );
void TweakDec ( void );
void TweakIncSlow ( void );
void TweakDecSlow ( void );
void TweakDefault ( void );
void TweakNeg ( void );

#define TWEAK_INT(name,descript,value) TweakInt name ( #name, descript, value )
#define TWEAK_FLOAT(name,descript,value) TweakFloat name ( #name, descript, value )
#define EXTERN_TWEAK_INT(name) extern TweakInt name;
#define EXTERN_TWEAK_FLOAT(name) extern TweakFloat name;

#ifdef _DEBUG
#define TWEAK_INT_DEBUG(name,descript,value) TweakInt name ( #name, descript, value )
#define TWEAK_FLOAT_DEBUG(name,descript,value) TweakFloat name ( #name, descript, value )
#else
#define TWEAK_INT_DEBUG(name,descript,value) const int name = value;
#define TWEAK_FLOAT_DEBUG(name,descript,value) const float name = value;
#endif


#endif //#ifndef TOMSLIB_INCLUDED




























