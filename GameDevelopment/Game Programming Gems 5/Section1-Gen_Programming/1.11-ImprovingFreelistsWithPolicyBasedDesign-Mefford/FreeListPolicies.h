#ifndef FREELISTPOLICIES_H__
#define FREELISTPOLICIES_H__

#include <vector>
#include <algorithm>
#include <assert.h>

#include <malloc.h>

namespace gems
{

namespace detail
{
	// These may need to be redefined for other platforms.  They are necessary so that
	// memory may be allocated aligned in such a manner that it can be later used by 
	// classes with certain alignment requirements.  These #defines will work with VC7 or
	// VC6 with the processor pack installed
	#define AlignOf( T ) __alignof( T )
	#define AlignedMalloc( size, alignment ) _aligned_malloc( size, alignment )
	#define AlignedFree( pMemory ) _aligned_free( pMemory )

	// Static Check is basically a compile time assert.  Expr is a compile time integral
	// expression.  If it evaluates to 0 during compilation, msg, will show up in a compile error
	#define STATIC_CHECK( expr, msg ) \
	typedef char ERROR_##msg[1][(expr)]

	// A helper class that policies can derive from if they want calls to Create
	// to wind up calling the object's constructor through placement new and 
	// Destroy to call the object's destructor
	template< typename T > class PlacementNewCreationHelper
	{
	public:
		static T* Create( void* pMemory )
		{
			return new( pMemory ) T;
		}

		template< typename A1 > 
			static T* Create( void* pMemory, A1 a1 )
		{
			return new( pMemory ) T(a1);
		}

		template< typename A1, typename A2 > 
			static T* Create( void* pMemory, A1 a1, A2 a2 )
		{
			return new( pMemory ) T(a1, a2);
		}

		template< typename A1, typename A2, typename A3 > 
			static T* Create( void* pMemory, A1 a1, A2 a2, A3 a3 )
		{
			return new( pMemory ) T(a1, a2, a3);
		}

		template< typename A1, typename A2, typename A3, typename A4 > 
			static T* Create( void* pMemory, A1 a1, A2 a2, A3 a3, A4 a4 )
		{
			return new( pMemory ) T(a1, a2, a3, a4);
		}

		static void Destroy( T* pItem )
		{
			pItem->~T();
		}
	};

	// Simple helper class each policy can use by deriving from to provide 
	// the GetNumBlocksAllocated interface necessary for Allocation Policies
	class AllocationTracker
	{
	public:
		AllocationTracker() 
			: numBlocksAllocated(0)
			, numBlocksInUse(0)
			, peakBlocksInUse(0)
		{
		}

		unsigned int GetNumBlocksAllocated() const
		{
			return numBlocksAllocated;
		}

		unsigned int GetNumberOfBlocksInUse() const
		{
			return numBlocksInUse;
		}

		unsigned int GetPeakNumberOfBlocksInUse() const
		{
			return peakBlocksInUse;
		}

	protected:
		void IncrementNumBlocksInUse()
		{
			++numBlocksInUse;
			if( numBlocksInUse > peakBlocksInUse )
				peakBlocksInUse = numBlocksInUse;
		}

		void DecrementNumBlocksInUse()
		{
			--numBlocksInUse;
		}

		void ResetBlocksInUse()
		{
			numBlocksInUse = 0;
		}

		unsigned int numBlocksAllocated;

	private:
		unsigned int numBlocksInUse;
		unsigned int peakBlocksInUse;
	};

	// This class is used by SharedChunkPolicy, and is, itself, effectively an array of
	// simple linked lists.  Note, that it only supports allocating objects <= maxBytes
	// in size, and each allocation is rounded up to the nearest listSpacing size.  
	// These numbers can be easily tweaked below.  
	class SharedFreeList
	{
	public:
		static const int maxBytes = 128;
		static const int listSpacing = 8;
		static const int numLists = maxBytes / listSpacing;

		static void* Allocate( unsigned int size );
		static void  Deallocate( void* pBlock, unsigned int size );

		static void  Grow( unsigned int numBlocks, unsigned int size );

		static void FreeAll();

		static unsigned int GetNumBlocksAllocated( unsigned int size );
		static unsigned int GetNumBlocksInUse( unsigned int size );
		static unsigned int GetPeakNumberOfBlocksInUse( unsigned int size );

	private:
		SharedFreeList();
		~SharedFreeList();

		struct FreeList
		{
			FreeList() : pHead(0), numBlockAllocated(0),
				numBlocksInUse(0), peakBlocksInUse(0)
			{}

			void* Allocate();
			void  Deallocate( void* pBlock );

			void  PushBlock( void* pBlock );
			void  Reset();

			struct FreeBlock
			{
				FreeBlock* pNext;
			};

			FreeBlock* pHead;
			unsigned int numBlockAllocated;
			unsigned int numBlocksInUse;
			unsigned int peakBlocksInUse;
		};

		static FreeList* GetSharedListFromSize( unsigned int size );
		static unsigned int GetAlignmentFromSize( unsigned int size );

		static SharedFreeList* pInstance;

		static FreeList sharedLists[numLists];
		static std::vector< void* > chunks;
	};

} // namespace detail

// Some basic growth policies
template< int NumToPrealloc, int NumToGrow, int MaxAllocated > struct ConstantGrowthPolicy
{
	static unsigned int GetNumberToPreallocate()
	{
		return NumToPrealloc;
	}

	static unsigned int GetNumberToGrow( unsigned int numAllocated )
	{
		if( numAllocated + NumToGrow > MaxAllocated )
			return 0;
		else
			return NumToGrow;
	}
};

template< typename T, int BlockSize > struct ConstantChunkSizeGrowthPolicy
{
	static const int numberToGrow = (BlockSize / sizeof(T) > 8) ? (BlockSize / sizeof(T)) : 8;
	static unsigned int GetNumberToPreallocate()
	{
		return 0;
	}

	static unsigned int GetNumberToGrow( unsigned int unused )
	{
		return numberToGrow;
	}
};

template< int InitialSize > struct DoubleGrowthSize
{
	static unsigned int GetNumberToPreallocate()
	{
		return 0;
	}

	static unsigned int GetNumberToGrow( unsigned int numAllocated )
	{
		if( numAllocated == 0 )
			return InitialSize;
		else
			return numAllocated;
	}
};


// This allocation policy is arguably the simplest, and probably the most useful and robust
//
// When a grow request is processed, an uninitialized chunk of memory is allocated 
// to exactly hold the new blocks.  Each new uninitialized block is then cast to a FreeBlock, 
// which is a simple link list node, and added to the link list of free blocks.  
// This effectively hijacks the first sizeof( FreeBlock* ) of each block to point to the next free block.
//
// When an object is allocated, the head block is taken from the linked list and a new T is 
// constructed with placement new.  When an object is freed, the destructor is called and 
// then it is cast back to a FreeBlock item and put back into the list of free blocks
//
// This gives lightning fast allocation and deallocation performance while supporting objects
// with arbitrarily complex destructors and constructors.  It also has no per-block overhead (and only 
// 4 bytes of overhead per chunk)
//
template< typename T > class PlacementNewLinkedList
	: public detail::PlacementNewCreationHelper< T >
	, public detail::AllocationTracker
{
public:
	PlacementNewLinkedList() : pFreeBlocks( 0 )
	{
	}

protected:

	~PlacementNewLinkedList()
	{
		// free up our chunks of memory
		// NOTE:  if there are outstanding allocations, this policy won't call their destructors
		// which could lead to resource leaks
		for( std::vector< void* >::iterator it = chunks.begin(); it != chunks.end(); ++it )
		{
			AlignedFree( *it );
		}
	}

	void Push( void* pItem )
	{
		FreeBlock* pNewHead = reinterpret_cast< FreeBlock* > ( pItem );
		pNewHead->pNext = pFreeBlocks;
		pFreeBlocks = pNewHead;
		DecrementNumBlocksInUse();
	}

	void* Pop()
	{
		if( !pFreeBlocks )
			return 0;

		IncrementNumBlocksInUse();
		void* pReturn = pFreeBlocks;
		pFreeBlocks = pFreeBlocks->pNext;
		return pReturn;
	}

	void Grow( unsigned int numBlocks )
	{
		char* pNewChunk = (char*)AlignedMalloc( numBlocks * sizeof(T), AlignOf(T) );
		numBlocksAllocated += numBlocks;

		for( unsigned int ix = 0; ix < numBlocks; ++ix )
		{
			IncrementNumBlocksInUse();	// Make this call to offset the decrement inside push
			Push( pNewChunk + ix*sizeof(T) );
		}
		chunks.push_back( pNewChunk );
	}

private:
	struct FreeBlock
	{
		FreeBlock *pNext;
	};

	FreeBlock* pFreeBlocks;
	std::vector< void* > chunks;

	// Verify at compile time that T is large enough to safely hold a FreeList*
	// NOTE, there are ways around this--pad blocks to be at least sizeof(FreeList*) or use byte offsets instead of pointers
	STATIC_CHECK( sizeof(T) >= sizeof(FreeBlock*), PlacementNewLinkedList_Requires_sizeof_T_greater_equal_sizeof_pointer );
};

// This policy mimics the implementation of the freelist in Game Programming Gems 4.
// Notably, it constructs blocks of T by actually calling new T[], and does nothing
// on Create or Destroy.  This might be a benefit if the constructor call is relatively heavyweight
// and not strictly necessary, or possibly for freelists of POD data structures.
//
// Since the objects are not reconstructed using placement new on each allocation,
// the trick of hijacking part of each object to link to succeeding objects can not be relied on
// to work since this may overwrite an object's vtable, leading to some very ugly crashes.
// So a separate list of free pointers is maintained as a stack.  This unfortunately adds 
// sizof( void* ) overhead to each object allocated using this policy. 
//
// Because the aim of freelists is roughly to replace heap allocation using new, I strongly
// recommend using an implementation more similar to PlacementNewLinkedList unless you have good
// reason not to.  Especially since that policy requires no additional per-object memory overhead.
//
// This is still a useful lesson to show how powerful a policy design can be.  The
// freelist from GPG4 can simply be thought of as one implementation in a family of freelist
// implementations supported by this policy based freelist.
//
template< typename T > class ConstructOnceStack
	: public detail::AllocationTracker
{
public:
	ConstructOnceStack() {}

	// The FreeAll class returns all outstanding elements to the FreeList.  This might be called at the end of 
	// a level or something.  It does not free the underlying memory.  This is the same behavior as the 
	// FreeAll function from GPG4.
	// NOTE:  This is an example of a policy class exposing additional functionality to client classes.  This
	// is possible because the freelist publicly derives from the AllocationPolicy.
	void FreeAll()
	{
		freeObjects.clear();
		ResetBlocksInUse();

		for( std::vector< Chunk >::iterator it = chunks.begin(); it != chunks.end(); ++it )
		{
			for( unsigned int ix = 0; ix < it->numObjects; ++ix )
			{
				IncrementNumBlocksInUse();	// Make this call to offset the decrement inside push
				Push( it->pObjects + ix );
			}
		}
	}

	// Reset frees all the memory of this allocation policy and resets it to an empty freelist.
	// This is another demonstration of a policy class exposing additional functionality.
	void Reset()
	{
		freeObjects.clear();
		ResetBlocksInUse();

		for( std::vector< Chunk >::iterator it = chunks.begin(); it != chunks.end(); ++it )
		{
			delete [] (it->pObjects);
		}

		chunks.clear();
		numBlocksAllocated = 0;
	}

protected:
	~ConstructOnceStack()
	{
		Reset();
	}

	static T* Create( void* pMemory )
	{
		return reinterpret_cast< T* >( pMemory );
	}

	// The templated Create functions aren't defined here, which means you will get a compile 
	// error if you try to call Allocate with arguments on a free list using this allocation policy 
	// This is probably a useful signal to the programmer that those arguments will be ignored
	// but a valid option would be to simply define them to simply ignore arguments and return pMemory as above

	static void Destroy( T* pItem )
	{
	}

	void Push( void* pItem )
	{
		DecrementNumBlocksInUse();
		freeObjects.push_back( pItem );
	}

	void* Pop()
	{
		if( !freeObjects.size() )
			return 0;
		else
		{
			IncrementNumBlocksInUse();
			void* pReturn = freeObjects.back();
			freeObjects.pop_back();
			return pReturn;
		}
	}

	void Grow( unsigned int numObjects )
	{
		Chunk newChunk;
		newChunk.pObjects = new T[numObjects];
		newChunk.numObjects = numObjects;

		for( unsigned int ix = 0; ix < newChunk.numObjects; ++ix )
		{
			IncrementNumBlocksInUse();	// Make this call to offset the decrement inside push
			Push( newChunk.pObjects + ix );
		}
		chunks.push_back( newChunk );

		numBlocksAllocated += numObjects;
	}

private:

	struct Chunk
	{
		T* pObjects;
		unsigned int numObjects;
	};

	std::vector< Chunk > chunks;
	std::vector< void* > freeObjects;
};


// The CompactableChunkPolicy is a novel free list policy that can actually reclaim
// unused memory in certain circumstances under certain allocation patterns.
//
// The policy maintains a list of Chunks that it allocates when a request to Grow is processed.
// Each Chunk maintains a link list of free blocks.  When an allocation is requested, a free block
// from one of the chunks is returned (and constructed with placement new).  When a deallocation
// occurs, the block is returned to the appropriate chunk.  If the policy detects that two chunks
// are completely unused, then it will actually return the memory of the larger of the two chunks to the heap.
// This may be useful if you have many freelists in operation, which may have bursts of usage followed 
// by relatively little usage, and it is important that every freelist is not consuming peak memory
// at all times.
//
// Notthing is free though, and it should be noted that this policy is not as fast as the above two.
// Most notably, in a worst case, Free performs a linear search through all allocated chunks to find
// the chunk a given block was allocated from. It is likely still significantly faster than most 
// general heap allocators.
//
// This policy works best when your allocation pattern is roughly many allocations followed by many deallocations
// in roughly reverse order from the allocations.
template< typename T > class CompactableChunkPolicy
	: public detail::PlacementNewCreationHelper< T >
	, public detail::AllocationTracker
{
public:
	CompactableChunkPolicy()
		: allocChunk( 0 )
		, deallocChunk( 0 )
	{
	}

protected:
	~CompactableChunkPolicy()
	{
		for( Chunks::iterator it = chunks.begin(); it != chunks.end(); ++it )
		{
			it->Release();
		}
	}

	void Push( void* pItem )
	{
		assert(!chunks.empty());
		assert(&chunks.front() <= deallocChunk);
		assert(&chunks.back() >= deallocChunk);

		deallocChunk  = VicinityFind(pItem);
		assert(deallocChunk);

		DoDeallocate(pItem);
		DecrementNumBlocksInUse();
	}

	void* Pop()
	{
		if( allocChunk == 0 || allocChunk->blocksAvailable == 0 )
		{
			// Try to find a valid chunk with an available block
			Chunks::iterator it = chunks.begin();
			for(;; ++it)
			{
				if( it == chunks.end() )
				{
					break;
				}
				if( it->blocksAvailable > 0 )
				{
					allocChunk = &*it;
					break;
				}
			}
		}

		if( allocChunk == 0 || allocChunk->blocksAvailable == 0 )
			return 0;
		else
		{
			IncrementNumBlocksInUse();
			return allocChunk->Allocate();
		}
	}

	void Grow( unsigned int numBlocks )
	{
		assert( allocChunk == 0 || allocChunk->blocksAvailable == 0 );

		Chunk newChunk;
		newChunk.Init(numBlocks);
		chunks.push_back( newChunk );
		allocChunk = &chunks.front();
		deallocChunk = &chunks.back();

		numBlocksAllocated += numBlocks;
	}

private:

	struct Chunk
	{
		void Init(unsigned int blocks)
		{
			assert( blocks > 0 );

			pData = (unsigned char*)AlignedMalloc( sizeof(T) * blocks, AlignOf(T) );
			assert( pData );

			numBlocks = blocks;
			blocksAvailable = blocks;
			pFirst = 0;

			// Link each block inside this chunk (do so in reverse order to slightly improve
			// locality of reference for elements allocated in order)
			for( int i = blocks-1; i >= 0; --i )
			{
				*((void**)(pData + i*sizeof(T))) = pFirst;
				pFirst = pData + i*sizeof(T);
			}
		}
		
		void* Allocate()
		{
			if( !blocksAvailable )
				return 0;

			assert( pFirst != 0 );

			void* pResult = pFirst;
			pFirst = *(void**)pResult;

			--blocksAvailable;
			return pResult;
		}

		void Deallocate( void* pBlock )
		{
			*(void**)pBlock = pFirst;
			pFirst = pBlock;

			++blocksAvailable;
			assert( blocksAvailable <= numBlocks );
		}

		void Release()
		{
			AlignedFree( pData );
		}

		unsigned char* pData;
		void* pFirst;
		unsigned int blocksAvailable;
		unsigned int numBlocks;
	};

	typedef std::vector< Chunk > Chunks;

	Chunks chunks;
	Chunk* allocChunk;
	Chunk* deallocChunk;

	// This implementation of CompactableBlockPolicy has the same limiation as PlacementNewLinkedList
	// See Modern C++ Design for an implementation that removes this restriction (while limiting chunks
	// to contain no more than 256 blocks), as well as motivation for optimizing 1 byte allocations.
	STATIC_CHECK( sizeof(T) >= sizeof(void*), ConpactableBlockPolicy_Requires_sizeof_T_greater_equal_sizeof_pointer );

	Chunk* VicinityFind( void* p )
	{
		assert(!chunks.empty());
		assert(deallocChunk);

		Chunk* lo = deallocChunk;
		Chunk* hi = deallocChunk + 1;
		Chunk* loBound = &chunks.front();
		Chunk* hiBound = &chunks.back() + 1;

		// Special case: deallocChunk_ is the last in the array
		if (hi == hiBound) hi = 0;

		for (;;)
		{
			if (lo)
			{
				if (p >= lo->pData && p < lo->pData + lo->numBlocks * sizeof(T))
				{
					return lo;
				}
				if (lo == loBound) lo = 0;
				else --lo;
			}

			if (hi)
			{
				if (p >= hi->pData && p < hi->pData + hi->numBlocks * sizeof(T))
				{
					return hi;
				}
				if (++hi == hiBound) hi = 0;
			}
		}
		assert(false);
		return 0;
	}

	void DoDeallocate( void* p )
	{
		assert(deallocChunk->pData <= p);
		assert(deallocChunk->pData + deallocChunk->numBlocks * sizeof(T) > p);

		// call into the chunk, will adjust the inner list but won't release memory
		deallocChunk->Deallocate(p);

		if (deallocChunk->blocksAvailable == deallocChunk->numBlocks)
		{
			// deallocChunk_ is completely free, should we release it? 
			Chunk& lastChunk = chunks.back();

			if (&lastChunk == deallocChunk)
			{
				// check if we have two last chunks empty

				if (chunks.size() > 1 && 
					deallocChunk[-1].blocksAvailable == deallocChunk[-1].numBlocks)
				{
					// Two free chunks, discard the last one
					// Make sure we discard the one with the largest number of blocks
					if( deallocChunk[-1].numBlocks > deallocChunk->numBlocks )
					{
						std::swap( *deallocChunk, deallocChunk[-1] );
					}
					numBlocksAllocated -= lastChunk.blocksAvailable;
					lastChunk.Release();
					chunks.pop_back();
					allocChunk = deallocChunk = &chunks.front();
				}
				return;
			}

			if (lastChunk.blocksAvailable == lastChunk.numBlocks)
			{
				// Two free blocks, discard one
				// Make sure we discard the one with the largest number of blocks
				if( deallocChunk->numBlocks > lastChunk.numBlocks )
				{
					std::swap( *deallocChunk, lastChunk );
				}
				numBlocksAllocated -= lastChunk.blocksAvailable;
				lastChunk.Release();
				chunks.pop_back();

				std::swap( *deallocChunk, chunks.back() );
				allocChunk = deallocChunk = &chunks.front();
			}
			else
			{
				// move the empty chunk to the end
				std::swap(*deallocChunk, lastChunk);
				allocChunk = &chunks.back();
			}
		}
	}
};

// SharedChunkPolicy is an allocation policy that actually defers its allocations to 
// a global shared free list.
//
// This means that two freelists allocating objects that are identical (or nearly identical) 
// in size will actually share chunks and blocks of memory.  If an application uses
// a significant number of freelists, this can eliminate much of the excess memory stored
// in freelists waiting to be allocated while still giving you virtually all of the benefits of
// freelist allocation.  The main tradeoff you will be making by using this policy is that
// allocations within the same freelist are likely to be located at random areas of memory,
// eliminating the locality of reference benefits that individual freelists can give you.
//
// It should be noted that STLPort's default allocator behaves remarkably similar to this
// allocation policy
template< typename T > class SharedChunkPolicy
	: public detail::PlacementNewCreationHelper< T >
{
public:

	unsigned int GetNumBlocksAllocated() const
	{
		return detail::SharedFreeList::GetNumBlocksAllocated( sizeof(T) );
	}

	unsigned int GetNumberOfBlocksInUse() const
	{
		return detail::SharedFreeList::GetNumBlocksInUse( sizeof(T) );
	}

	unsigned int GetPeakNumberOfBlocksInUse() const
	{
		return detail::SharedFreeList::GetPeakNumberOfBlocksInUse( sizeof(T) );
	}

protected:
	~SharedChunkPolicy()
	{ }

	void Push( void* pItem )
	{
		detail::SharedFreeList::Deallocate( pItem, sizeof(T) );
	}

	void* Pop()
	{
		return detail::SharedFreeList::Allocate( sizeof(T) );
	}

	void Grow( unsigned int numBlocks )
	{
		detail::SharedFreeList::Grow( numBlocks, sizeof(T) );
	}

	// This class only works on objects that fit inside one of SharedFreeList's internal lists
	STATIC_CHECK( sizeof(T) <= detail::SharedFreeList::maxBytes, 
		SharedChunkPolicy_requires_sizeof_T_not_greater_than_max_SharedFreeList_size );
};

} // namespace gems

#endif //FREELISTPOLICIES_H__