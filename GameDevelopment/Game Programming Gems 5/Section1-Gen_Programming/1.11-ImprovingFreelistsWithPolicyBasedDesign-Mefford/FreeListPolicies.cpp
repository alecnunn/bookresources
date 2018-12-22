#include "FreeListPolicies.h"

//NOTE: This file needs to be included in your project only if you plan on
// using the SharedChunkAllocationPolicy.  It contains the implementation of
// gems::detail::SharedFreeList

namespace gems
{
	namespace detail
	{
		SharedFreeList* SharedFreeList::pInstance = 0; 
		SharedFreeList::FreeList SharedFreeList::sharedLists[SharedFreeList::numLists];
		std::vector< void* > SharedFreeList::chunks;

		void* SharedFreeList::FreeList::Allocate()
		{
			if( !pHead )
				return 0;

			++numBlocksInUse;
			if( numBlocksInUse > peakBlocksInUse )
				peakBlocksInUse = numBlocksInUse;

			void* pReturn = pHead;
			pHead = pHead->pNext;
			return pReturn;
		}

		void SharedFreeList::FreeList::Deallocate( void* pBlock )
		{
			--numBlocksInUse;
			PushBlock( pBlock );
		}

		void SharedFreeList::FreeList::PushBlock( void* pBlock )
		{
			FreeBlock* pNewHead = reinterpret_cast< FreeBlock* >( pBlock );
			pNewHead->pNext = pHead;
			pHead = pNewHead;
		}

		void SharedFreeList::FreeList::Reset()
		{
			pHead = 0;
			numBlocksInUse = 0;
			peakBlocksInUse = 0;
			numBlockAllocated = 0;
		}

		SharedFreeList::FreeList* SharedFreeList::GetSharedListFromSize( unsigned int size )
		{
			assert( size != 0 );
			assert( size < maxBytes );

			unsigned int index = (size-1) / listSpacing;
			assert( index < SharedFreeList::numLists );

			return sharedLists + index;
		}

		unsigned int SharedFreeList::GetAlignmentFromSize( unsigned int size )
		{
			assert( size != 0 );
			
			// Round up the size to the nearest listSpacing
			unsigned int roundedSize = ( size + (listSpacing+1) ) & (~(listSpacing-1));

			// define minimum and maximum alignment (64 as max alignment is somewhat arbitrary)
			const unsigned int minAlign = listSpacing;
			const unsigned int maxAlign = (listSpacing > 64) ? listSpacing : 64;

			for( unsigned int align = minAlign; align < maxAlign; ++align )
			{
				if( roundedSize & align )
					return (align < minAlign) ? minAlign : align;
			}

			return maxAlign;
		}

		void* SharedFreeList::Allocate( unsigned int size )
		{
			FreeList* pSharedList = GetSharedListFromSize( size );
			return pSharedList->Allocate();
		}

		void SharedFreeList::Deallocate( void* pBlock, unsigned int size )
		{
			FreeList* pSharedList = GetSharedListFromSize( size );
			pSharedList->Deallocate( pBlock );
		}

		void SharedFreeList::Grow( unsigned int numBlocks, unsigned int size )
		{
			FreeList* pFreeList = GetSharedListFromSize( size );

			// Note that to be safe, we must align the chunk allocations to the nearest power
			// of two for the given list size.  I.e. 16 byte classes may or may not need to be
			// aligned to 16 bytes, so to be safe, all chunks capable of handling 16 byte classes
			// must be aligned to 16 bytes.  Similarly 24 byte chunks must be aligned
			// to 8 bytes, etc. etc.
			char* pChunk = (char*)AlignedMalloc( numBlocks*size, GetAlignmentFromSize(size) );

			for( unsigned int ix = 0; ix < numBlocks; ++ix )
			{
				pFreeList->PushBlock( pChunk+ ix*size );
			}

			pFreeList->numBlockAllocated += numBlocks;
			chunks.push_back( pChunk );
		}

		void SharedFreeList::FreeAll()
		{
			// Free all the memory
			for( std::vector< void* >::iterator it = chunks.begin(); it != chunks.end(); ++it )
			{
				AlignedFree( *it );
			}

			// Empty all the free lists
			for( int ix = 0; ix < numLists; ++ix )
				sharedLists[ix].Reset();
		}

		unsigned int SharedFreeList::GetNumBlocksAllocated( unsigned int size )
		{
			return GetSharedListFromSize( size )->numBlockAllocated;
		}

		unsigned int SharedFreeList::GetNumBlocksInUse( unsigned int size )
		{
			return GetSharedListFromSize( size )->numBlocksInUse;
		}

		unsigned int SharedFreeList::GetPeakNumberOfBlocksInUse( unsigned int size )
		{
			return GetSharedListFromSize( size )->peakBlocksInUse;
		}
	}
}
