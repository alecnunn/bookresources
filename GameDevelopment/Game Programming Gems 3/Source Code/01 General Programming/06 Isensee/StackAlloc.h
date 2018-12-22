///////////////////////////////////////////////////////////////////////////////
//
//  StackAlloc.h
//
//  Stack-based allocator. Designed to use stack-based data passed as a parameter
//  to the allocator ctor. Does not "free" the memory. Assumes that if the
//  allocator is copied, stack memory is cleared and new allocations begin
//  at the bottom of the stack again.
//
//  Copyright © 2002 Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined( STACK_ALLOCATOR_H )
#define STACK_ALLOCATOR_H

#include "Common.h"
#include <memory>
#include <limits>

namespace CustomAlloc
{

///////////////////////////////////////////////////////////////////////////////
//
// StackAlloc

template <typename T>
class StackAlloc
{
public:

    // Typedefs
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    // Constructors
    StackAlloc() throw()
    :
        mpStack( NULL ),
        mBytesAllocated( 0 ),
        mMaxBytes( 0 )
    {
    }

    StackAlloc( unsigned char* pStack, size_t nMaxBytes ) throw()
    :
        mpStack( pStack ),
        mBytesAllocated( 0 ),
        mMaxBytes( nMaxBytes )
    {
    }

    StackAlloc( const StackAlloc& sa ) throw()
    :
        mpStack( sa.mpStack ),
        mBytesAllocated( 0 ),
        mMaxBytes( sa.mMaxBytes )
    {
        // Copying the "stack" resets mBytesAllocated to zero
    }

#if _MSC_VER >= 1400 // VC 7 can't handle template members
    template <typename U>
    StackAlloc( const StackAlloc<U>& sa ) throw()
    :
        mpStack( sa.mpStack ),
        mBytesAllocated( 0 ),
        mMaxBytes( sa.mMaxBytes )
    {
        // Copying the "stack" resets mBytesAllocated to zero
    }
#endif

    StackAlloc& operator=( const StackAlloc& sa )
    {
        // Copying the "stack" resets mBytesAllocated to zero
        mpStack = sa.mpStack;
        mBytesAllocated = 0;
        mMaxBytes = sa.mMaxBytes;
        return *this;
    }

    // Destructor
    ~StackAlloc() throw()
    {
    }

    // Utility functions
    pointer address( reference r ) const
    {
        return &r;
    }

    const_pointer address( const_reference c ) const
    {
        return &c;
    }

    size_type max_size() const
    {
        return std::numeric_limits<size_t>::max() / sizeof(T);
    }

    // In-place construction
    void construct( pointer p, const_reference c )
    {
        // placement new operator
        new( reinterpret_cast<void*>(p) ) T(c);
    }

    // In-place destruction
    void destroy( pointer p )
    {
        // call destructor directly
        (p)->~T();
    }

    // Rebind to allocators of other types
    template <typename U>
    struct rebind
    {
        typedef StackAlloc<U> other;
    };

    // Allocate raw memory
    pointer allocate( size_type n, const void* = NULL )
    {
        void* pRaw = mpStack + mBytesAllocated;
        mBytesAllocated += ( n * sizeof(T) );

        if( mBytesAllocated+1 > mMaxBytes )
            throw std::bad_alloc();

        return pointer(pRaw);
    }

    // Free raw memory.
    // Note that C++ standard defines this function as
    // deallocate( pointer p, size_type). Because Visual C++ 6.0
    // compiler doesn't support template rebind, Dinkumware uses
    // void* hack.
    void deallocate( void*, size_type )
    {
        // No need to free stack memory
    }

    // Non-standard Dinkumware hack for Visual C++ 6.0 compiler.
    // VC 6 doesn't support template rebind.
    char* _Charalloc( size_type n )
    {
        return reinterpret_cast<char*>( allocate( n, NULL ) );
    }

    // Required for global comparison functions
    unsigned char* GetStack() const
    {
        return mpStack;
    }

private:

    unsigned char* mpStack;
    size_t         mBytesAllocated;
    size_t         mMaxBytes;

}; // end of StackAlloc

// Comparison
template <typename T1>
bool operator==( const StackAlloc<T1>& lhs, const StackAlloc<T1>& rhs) throw()
{
    return lhs.GetStack() == rhs.GetStack();
}

template <typename T1>
bool operator!=( const StackAlloc<T1>& lhs, const StackAlloc<T1>& rhs) throw()
{
    return lhs.GetStack() != rhs.GetStack();
}

} // end namespace CustomAlloc

#endif // STACK_ALLOCATOR_H

///////////////////////////////////////////////////////////////////////////////
