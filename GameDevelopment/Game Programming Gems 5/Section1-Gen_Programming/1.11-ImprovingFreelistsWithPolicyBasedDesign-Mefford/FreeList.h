#ifndef FREELIST_H__
#define FREELIST_H__

// Several default policies are included in FreeListPolicies.h 
// They are included separately simply to keep the code clean and separated
#include "FreeListPolicies.h"

namespace gems
{

template< typename T,
          class AllocationPolicy = PlacementNewLinkedList<T>,
		  class GrowthPolicy = DoubleGrowthSize<16> >
class FreeList
	: public AllocationPolicy
	, public GrowthPolicy
{
public:
	FreeList();
	explicit FreeList( const GrowthPolicy& gp );
	~FreeList();

	T* Allocate();
	void Free( T* pObject );

	// These template member functions enable support for the passage of arguments to 
	// the Create policy member function.  In the DefaultAllocationPolicy, they are used
	// as arguments to the constructor, allowing objects with arbitrary constructors to be
	// allocated out of the freelist properly
	template< typename A1 > T* Allocate( A1 a1 );
	template< typename A1, typename A2 > T* Allocate( A1 a1, A2 a2 );
	template< typename A1, typename A2, typename A3 > T* Allocate( A1 a1, A2 a2, A3 a3 );
	template< typename A1, typename A2, typename A3, typename A4 > T* Allocate( A1 a1, A2 a2, A3 a3, A4 a4 );

private:
	FreeList( const FreeList& );
	FreeList& operator=( const FreeList& );

	void* RetrieveFreeItem();
};

template< typename T, 
	class AllocationPolicy, 
	class GrowthPolicy >
FreeList< T, AllocationPolicy, GrowthPolicy >::FreeList()
{
	unsigned int numElementsToPrealloc = GrowthPolicy::GetNumberToPreallocate();
	if( numElementsToPrealloc > 0 )
		AllocationPolicy::Grow( numElementsToPrealloc );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
FreeList< T, AllocationPolicy, GrowthPolicy >::FreeList( const GrowthPolicy& gp )
	: GrowthPolicy( gp )
{
	unsigned int numElementsToPrealloc = GrowthPolicy::GetNumberToPreallocate();
	if( numElementsToPrealloc > 0 )
		AllocationPolicy::Grow( numElementsToPrealloc );
}

template< typename T, 
	class AllocationPolicy, 
	class GrowthPolicy >
FreeList< T, AllocationPolicy, GrowthPolicy >::~FreeList()
{
}

template< typename T, 
	class AllocationPolicy, 
	class GrowthPolicy >
T* FreeList< T, AllocationPolicy, GrowthPolicy >::Allocate()
{
	void* pItem = RetrieveFreeItem();
	if( !pItem )
		return 0;
	else
		return AllocationPolicy::Create( pItem );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
void FreeList< T, AllocationPolicy, GrowthPolicy >::Free( T* pObject )
{
	if( !pObject )
		return;

	AllocationPolicy::Destroy( pObject );
	AllocationPolicy::Push( pObject );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
template< typename A1 >
T* FreeList< T, AllocationPolicy, GrowthPolicy >::Allocate( A1 a1 )
{
	void* pItem = RetrieveFreeItem();
	if( !pItem )
		return 0;
	else
		return AllocationPolicy::Create( pItem, a1 );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
template< typename A1, typename A2 >
T* FreeList< T, AllocationPolicy, GrowthPolicy >::Allocate( A1 a1, A2 a2 )
{
	void* pItem = RetrieveFreeItem();
	if( !pItem )
		return 0;
	else
		return AllocationPolicy::Create( pItem, a1, a2 );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
template< typename A1, typename A2, typename A3 >
T* FreeList< T, AllocationPolicy, GrowthPolicy >::Allocate( A1 a1, A2 a2, A3 a3 )
{
	void* pItem = RetrieveFreeItem();
	if( !pItem )
		return 0;
	else
		return AllocationPolicy::Create( pItem, a1, a2, a3 );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
template< typename A1, typename A2, typename A3, typename A4 >
T* FreeList< T, AllocationPolicy, GrowthPolicy >::Allocate( A1 a1, A2 a2, A3 a3, A4 a4 )
{
	void* pItem = RetrieveFreeItem();
	if( !pItem )
		return 0;
	else
		return AllocationPolicy::Create( pItem, a1, a2, a3, a4 );
}

template< typename T,
	class AllocationPolicy,
	class GrowthPolicy >
void* FreeList< T, AllocationPolicy, GrowthPolicy >::RetrieveFreeItem()
{
	void* pItem = AllocationPolicy::Pop();
	if( !pItem )
	{
		unsigned int growSize = GrowthPolicy::GetNumberToGrow( AllocationPolicy::GetNumBlocksAllocated() );
		if( growSize != 0 )
		{
			AllocationPolicy::Grow( growSize );
			pItem = AllocationPolicy::Pop();
		}
	}

	return pItem;
}

}

#endif //FREELIST_H__