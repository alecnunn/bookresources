///////////////////////////////////////////////////////////////////////////////
//
//  StaticAlloc.h
//
//  Static memory-based allocator. Uses static memory for allocations. Memory
//  is never recovered.
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

#if !defined( STATIC_ALLOCATOR_H )
#define STATIC_ALLOCATOR_H

#include <memory>
#include <limits>

namespace CustomAlloc
{

///////////////////////////////////////////////////////////////////////////////
//
// StaticAlloc

template <typename T, int PoolSize>
class StaticAlloc
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

public:

    // Constructors
    StaticAlloc() throw()
    {
    }

    StaticAlloc( const StaticAlloc& ) throw()
    {
    }

#if _MSC_VER >= 1300 // VC6 can't handle template members
    template <typename U, int PoolSize>
    StaticAlloc( const StaticAlloc<U,PoolSize>& ) throw()
    {
    }
#endif

    StaticAlloc& operator=( const StaticAlloc& )
    {
        return *this;
    }

    // Destructor
    ~StaticAlloc() throw()
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
        typedef StaticAlloc<U, PoolSize> other;
    };

    // Allocate raw memory
    pointer allocate( size_type n, const void* = NULL )
    {
        void* pRaw = mpPool + mBytesAllocated;
        mBytesAllocated += ( n * sizeof(T) );

        if( mBytesAllocated+1 > PoolSize )
            throw std::bad_alloc();

        return pointer(pRaw);
    }

    // Free raw memory.
    // Note that C++ standard defines this function as
    // deallocate( pointer p, size_type). Because Visual C++ 6.0
    // compiler doesn't support template rebind, Dinkumware uses
    // void* hack.
    void deallocate( void* p, size_type )
    {
        // No need to free static memory
    }

    // Non-standard Dinkumware hack for Visual C++ 6.0 compiler.
    // VC 6 doesn't support template rebind.
    char* _Charalloc( size_type nBytes )
    {
        char* pRaw = reinterpret_cast<char*>( mpPool ) + mBytesAllocated;
        mBytesAllocated += nBytes;

        if( mBytesAllocated+1 > PoolSize )
            throw std::bad_alloc();

        return pRaw;
    }

private:

    // Here's the "pool". There is one pool for each PoolSize in a given program
    static unsigned char mpPool[ PoolSize ];
    static size_t        mBytesAllocated;

}; // end of StaticAlloc

// Comparison
template <typename T1, int PoolSize1, typename T2, int PoolSize2>
bool operator==( const StaticAlloc<T1, PoolSize1>&,
                 const StaticAlloc<T2, PoolSize2>& ) throw()
{
    return true;
}

template <typename T1, int PoolSize1, typename T2, int PoolSize2>
bool operator!=( const StaticAlloc<T1, PoolSize1>&,
                 const StaticAlloc<T2, PoolSize2>& ) throw()
{
    return false;
}

// Static definitions
template <typename T, int PoolSize>
unsigned char StaticAlloc<T,PoolSize>::mpPool[ PoolSize ];

template <typename T, int PoolSize>
size_t StaticAlloc<T,PoolSize>::mBytesAllocated;

} // end namespace CustomAlloc

#endif // STATIC_ALLOCATOR_H

///////////////////////////////////////////////////////////////////////////////
