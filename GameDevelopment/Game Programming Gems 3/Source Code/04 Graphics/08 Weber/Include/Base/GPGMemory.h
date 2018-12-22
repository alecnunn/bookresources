/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/
//
//	GPGMemory.h
//
//	DESCRIPTION
//		This module defines a memory abstraction layer.  It's used to funnel
//	memory related services (allocation, deallocation and reallocation)
//	through a common point that can be controlled for tracking or porting
//	purposes.  It also overloads the default C++ new, new[], delete and
//	delete[] operators so that the memory abstraction layer is used.
//
//	NOTES
//
//	TODO clarify NULL and 0 argument handling
//		delete must accept NULL
//			free should also, but it may not be supported everywhere
//		it is not clear if new of array size 0 is absolutely permitted
//			malloc(0) should be valid, but it may not be supported everywhere
//		These issues should be clarified and implemented.


#ifndef GPGMemory_h
#define GPGMemory_h


#include <stdlib.h>
#include <new.h>

#include "Base/GPGDataTypes.h"
#include "Base/GPGDebug.h"
#include "Base/GPGResult.h"
#include "PlatformAbstraction/GPGDynamicMacro.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef void*	( GPGAllocateFunction )( size_t byteCount );
typedef void	( GPGDeallocateFunction )( void* pMemory );
typedef void*	( GPGReallocateFunction )( void* pMemory, size_t byteCount );

/* declare GPGHandle's as an opaque type */
typedef struct _GPGHandle *GPGHandle;

typedef GPGHandle	( GPGHandleNewFunction		)( size_t byteCount );
typedef void		( GPGHandleDisposeFunction	)( GPGHandle *pHandle );
typedef size_t		( GPGHandleGetSizeFunction	)( GPGHandle handle );
typedef int			( GPGHandleSetSizeFunction	)( GPGHandle handle, size_t byteCount );
typedef void *		( GPGHandleLockFunction		)( GPGHandle handle );
typedef void *		( GPGHandleUnlockFunction	)( GPGHandle handle );

#ifdef __cplusplus
}
#endif


#ifdef GPG_COMPONENT_LIBRARY
GPG_DL_IMPORT	extern GPGAllocateFunction		*gs_pAllocateFunction;
GPG_DL_IMPORT	extern GPGDeallocateFunction	*gs_pDeallocateFunction;
GPG_DL_IMPORT	extern GPGReallocateFunction	*gs_pReallocateFunction;
#else
GPG_DL_EXPORT	extern GPGAllocateFunction		*gs_pAllocateFunction;
GPG_DL_EXPORT	extern GPGDeallocateFunction	*gs_pDeallocateFunction;
GPG_DL_EXPORT	extern GPGReallocateFunction	*gs_pReallocateFunction;
#endif
extern GPGHandleNewFunction		*gs_pHandleNewFunc;
extern GPGHandleDisposeFunction	*gs_pHandleDisposeFunc;
extern GPGHandleGetSizeFunction	*gs_pHandleGetSizeFunc;
extern GPGHandleSetSizeFunction	*gs_pHandleSetSizeFunc;
extern GPGHandleLockFunction	*gs_pHandleLockFunc;
extern GPGHandleUnlockFunction	*gs_pHandleUnlockFunc;


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef DEBUG_ALLOC

void *GPGDebugAllocate(size_t byteCount, const char *pFile, long line);
void GPGDebugDeallocate(void *p);
void *GPGDebugReallocate(void *p, size_t byteCount, const char *pFile, long line);
void GPGDebugAllocLog(void);

#endif

//---------------------------------------------------------------------------
//	GPGGetMemoryFunctions
//
//	This function is used to get pointers to the three main memory functions.
//	Specify NULL for any function pointer that is not desired.  GPG_OK is
//	always returned.
//---------------------------------------------------------------------------

GPGRESULT GPGGetMemoryFunctions( GPGAllocateFunction**		ppAllocateFunction,
								 GPGDeallocateFunction**	ppDeallocateFunction,
								 GPGReallocateFunction**	ppReallocateFunction );

//---------------------------------------------------------------------------
//	GPGSetMemoryFunctions
//
//	This function is used to replace the three main memory functions.
//	Specify NULL for all three function pointers to restore the default
//	functions.  Upon success, GPG_OK is returned.  Otherwise,
//	GPG_E_INVALID_POINTER is returned.
//---------------------------------------------------------------------------

GPGRESULT GPGSetMemoryFunctions( GPGAllocateFunction*	pAllocateFunction,
								 GPGDeallocateFunction*	pDeallocateFunction,
								 GPGReallocateFunction*	pReallocateFunction );

//---------------------------------------------------------------------------
//	GPGAllocate
//
//	This function is used to allocate a block of memory.  If successful, a
//	pointer to the memory block is returned.  Otherwise, enough memory isn't
//	available and NULL is returned.
//---------------------------------------------------------------------------

#ifdef DEBUG_ALLOC

#define GPGAllocate( byteCount ) GPGDebugAllocate( byteCount, __FILE__, __LINE__ )

#else

inline
void* GPGAllocate( size_t byteCount )
{
	return gs_pAllocateFunction( byteCount );
}

#endif

//---------------------------------------------------------------------------
//	GPGDeallocate
//
//	This function is used to deallocate a block of memory previously
//	allocated with the GPGDeallocate function.
//---------------------------------------------------------------------------

#ifdef DEBUG_ALLOC

#define GPGDeallocate( pMemory ) GPGDebugDeallocate( pMemory )

#else

inline
void GPGDeallocate( void* pMemory )
{
	GPGASSERT( gs_pDeallocateFunction );

	gs_pDeallocateFunction( pMemory );
}

#endif

//---------------------------------------------------------------------------
//	GPGReallocate
//
//	This function is used to reallocate a block of memory previously
//	allocated by the GPGAllocate function using the new byte count.  If
//	successful, a pointer to the reallocated memory block is returned.  It
//	may not be the same pointer passed to the function, however the contents
//	of the memory block will mirror the original memory block (they will be
//	truncated if a smaller block is requested or undefined after the original
//	data ends if a larger block is requested).  Otherwise, enough memory
//	isn't available and NULL is returned.
//---------------------------------------------------------------------------

#ifdef DEBUG_ALLOC

#define GPGReallocate( pMemory, byteCount ) GPGDebugReallocate( pMemory, byteCount, __FILE__, __LINE__ )

#else

inline
void* GPGReallocate( void* pMemory, size_t byteCount )
{
	return gs_pReallocateFunction( pMemory, byteCount );
}

#endif

//---------------------------------------------------------------------------
//	GPGSetHandleFunctions
//
//  The default implementation of handles is to return pointers allocated
//  by GPGAllocate() ... so a lock is just casting the pointer from the handle
//  to a (void *), etc ... this allows you write code using the handle
//  allocator even if one isn't available.
//
//  You use this function to switch to a more meaningful implementation ...
//  for instance, the 3D Xtra will redirect to IMoaHandle (not that any of
//  the GPG code will know the difference).
//
//	You can restore the original vector of functions by invoking this with
//	a NULL pointer.
//
//	Upon success, GPG_OK is returned.  Otherwise,
//	GPG_E_INVALID_POINTER is returned.
//---------------------------------------------------------------------------

GPGRESULT GPGSetHandleFunctions(
	GPGHandleNewFunction		*pHandleNew,
	GPGHandleDisposeFunction	*pHandleDisposeFunc,
	GPGHandleGetSizeFunction	*pHandleGetSizeFunc,
	GPGHandleSetSizeFunction	*pHandleSetSizeFunc,
	GPGHandleLockFunction		*pHandleLockFunc,
	GPGHandleUnlockFunction		*pHandleUnlockFunc
);

//---------------------------------------------------------------------------
//	GPGHandleNew
//
//	This function is used to allocate a block of memory.  If successful, a non-null
//	GPGHandle is return. This handle can be GPGHandleLock()'ed to return a pointer
//  to the actual memory block. This returns NULL for failure.
//---------------------------------------------------------------------------

inline
GPGHandle GPGHandleNew( size_t byteCount )
{
	GPGASSERT( gs_pHandleNewFunc );

	return gs_pHandleNewFunc( byteCount );
}

//---------------------------------------------------------------------------
//	GPGHandleDispose
//
//	This function releases a handle allocated by GPGHandleNew(). You pass in
//  the address of the handle so it can be zero-ed out (to prevent further
//  use) and it's okay to pass a pointer to null so do this :
//
//	GPGHandleDispose(&handle);
//
//  instead of :
//
//  if (handle) /* pointless, as GPGHandleDispose() will check */
//	{
//		GPGHandleDispose(&handle);
//		handle = NULL; /* already null at this point */
//	}
//---------------------------------------------------------------------------

inline
void GPGHandleDispose( GPGHandle *pHandle )
{
	GPGASSERT( gs_pHandleDisposeFunc );

	gs_pHandleDisposeFunc( pHandle );
}

//---------------------------------------------------------------------------
//	GPGHandleGetSize
//
//	This function returns the size of the handle allocated by GPGHandleNew().
//---------------------------------------------------------------------------

inline
size_t GPGHandleGetSize( GPGHandle handle )
{
	GPGASSERT( gs_pHandleGetSizeFunc );

	return gs_pHandleGetSizeFunc( handle );
}

//---------------------------------------------------------------------------
//	GPGHandleSetSize
//
//	This function resizes a handle allocated by GPGHandleNew().
//---------------------------------------------------------------------------

inline
int GPGHandleSetSize( GPGHandle handle, size_t byteCount )
{
	GPGASSERT( gs_pHandleSetSizeFunc );

	return gs_pHandleSetSizeFunc( handle, byteCount );
}

//---------------------------------------------------------------------------
//	GPGHandleLock
//
//	This function locks the handle. It returns a pointer to the memory block.
//---------------------------------------------------------------------------

inline
void *GPGHandleLock( GPGHandle handle )
{
	GPGASSERT( gs_pHandleLockFunc );

	return gs_pHandleLockFunc( handle );
}

//---------------------------------------------------------------------------
//	GPGHandleUnlock
//
//	This function unlocks a block locked by GPGHandleLock(). After unlocking,
//	the pointer returned by GPGHandleLock() is (potentially) invalid, and should
//  be discarded ... to this end, 
//
//   ptr = GPGHandleLock( handle );
//	  ... ; do something with ptr
//   ptr = GPGHandleUnlock( handle );
//
//	is a useful way to write your code, as GPGHandleUnlock() always returns NULL.
//---------------------------------------------------------------------------

inline
void *GPGHandleUnlock( GPGHandle handle )
{
	GPGASSERT( gs_pHandleUnlockFunc );

	return gs_pHandleUnlockFunc( handle );
}


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/*-------------------------------------------------------------------------
	Template class CGPGHandleLocker

	An attempt to help people convert pointer based code to handles. For
	instance, 

		long * g_pArray;

		void Allocate() { g_pArray = new long[kArraySize]; }
		long Element(int index) { return g_pArray[index]; }
		void Release() { delete [] g_pArray; }

	could be re-written

		typedef CGPGHandleLocker<long> LockedLongHandle;

		GPGHandle g_hArray;

		void Allocate() { g_hArray = GPGHandleNew( sizeof(long) * kArraySize ); }
		long Element(int index) { LockedLongHandle ptr(g_hArray); return ptr[index]; }
		void Release() { GPGHandleDispose(&g_pArray); }

	Of course, you could always write :

		long Element(int index)
		{
			long * ptr = (long *)GPGHandleLock( g_hArray );
			long result = ptr[index];
			ptr = (long *)GPGHandleUnlock(g_hArray);
			return result;
		}

	NOTE : This code assumes locking a valid, non-null handle can never fail.

--------------------------------------------------------------------------*/


template <class Tpointer> class CGPGHandleLocker
{
	public:
		CGPGHandleLocker( GPGHandle& handleToLock ) :
			mHandle( handleToLock ),
			mMemory( NULL )
		{
			// lock handle
			if( mHandle )
				mMemory = (Tpointer)GPGHandleLock( mHandle );
		}
			
		~CGPGHandleLocker()
		{
			if( mMemory )
				mMemory = (Tpointer)GPGHandleUnlock( mHandle );
		}
		
		size_t GetSize() const { return GPGHandleGetSize( mHandle ); }
 		operator Tpointer () const	{ return mMemory; }
		
	protected:
		GPGHandle&	mHandle;
		Tpointer	mMemory;
};

#endif // __cplusplus

#endif


