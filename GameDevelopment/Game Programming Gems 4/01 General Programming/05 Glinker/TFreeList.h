// TFreeList.h - a templated freelist class.
//
// By Paul Glinker
//

#ifndef TFreeList_h
#define TFreeList_h


#include "assertx.h"



template <class FLDataType>


class TFreeList
{
public:

	// Construct a TFreeList with the specified number
	// of objects available for allocation.
	TFreeList(int iNumObjects)
	{
		ASSERT( (iNumObjects > 0) && "Invalid TFreeList size specified." );

		m_pObjectData = new FLDataType[iNumObjects];
		m_ppFreeObjects = new FLDataType*[iNumObjects];

		ASSERT( m_pObjectData && "Not enough memory to allocate object data!" );
		ASSERT( m_ppFreeObjects && "Not enough memory to allocate pointer stack!" );

		m_iNumObjects = iNumObjects;
		m_bFreeOnDestroy = true;

		FreeAll();
	}

	// Constructs a TFreeList with the specified number
	// of objects available for allocation from pre-allocated
	// memory referenced by "pObjectData" and "ppFreeObjects".
	// Note that pObjectData and ppFreeObjects must have
	// "iNumObjects" elements each.
	TFreeList(FLDataType *pObjectData, FLDataType **ppFreeObjects, int iNumObjects)
	{
		ASSERT( (iNumObjects > 0) && "Invalid TFreeList size specified." );

		m_pObjectData = pObjectData;
		m_ppFreeObjects = ppFreeObjects;

		ASSERT( m_pObjectData && "A NULL pointer was passed for the object data!" );
		ASSERT( m_ppFreeObjects && "A NULL pointer was passed for the pointer stack!" );

		m_iNumObjects = iNumObjects;
		m_bFreeOnDestroy = false;

		FreeAll();
	}

	~TFreeList(void)
	{
		// If we have allocated memory,
		// then we must free it.
		if (m_bFreeOnDestroy)
		{
			delete [] m_ppFreeObjects;
			delete [] m_pObjectData;
		}
	}

	// Returns a pointer to a free instance of FLDataType.
	FLDataType *NewInstance(void)
	{
		ASSERT( m_iTop && "Tried to get a new instance but there"
						  "were no free instances available for "
						  "allocation. Consider using GetFree()!" );
			
		return m_ppFreeObjects[--m_iTop];
	}

	// Reclaims the instance referenced by pInstance.
	void FreeInstance(FLDataType *pInstance)
	{
		ASSERT( (pInstance >= &(m_pObjectData[0])) &&
				(pInstance <= &(m_pObjectData[m_iNumObjects-1])) &&
				"Tried to free an object that was"
				"not from this TFreeList" );

		// You might consider putting a debug-only check here to make sure
		// the instance that is being freed isn't already free.

		ASSERT( (m_iTop < m_iNumObjects) && "You have freed at least one"
											"instance more than once." );

		m_ppFreeObjects[m_iTop++] = pInstance;
	}

	// Makes all instances available for allocation.
	void FreeAll(void)
	{
		int iIndex = (m_iNumObjects-1);

		for (m_iTop = 0; m_iTop < m_iNumObjects; m_iTop++)
		{
			m_ppFreeObjects[m_iTop] = &(m_pObjectData[iIndex--]);
		}		
	}

	// Returns the total number of objects
	// managed by this TFreeList.
	int GetSize(void)
	{
		return m_iNumObjects;
	}

	// Returns the number of instances available
	// for allocation.
	int GetFree(void)
	{
		return m_iTop;
	}

private:

	// Points to the array of objects.
	FLDataType *m_pObjectData;

	// The number of objects in m_pObjectData.
	int m_iNumObjects;


	// Points to an array of pointers to free
	// objects within m_pObjectData.  Used as
	// a fixed sized stack.
	FLDataType **m_ppFreeObjects;

	// Keeps track of the first available object in
	// m_ppFreeObjects (the top of the stack).
	int m_iTop;


	// Remembers weather or not to free memory on
	// destruction.
	bool m_bFreeOnDestroy;
};


#endif // TFreeList_h

