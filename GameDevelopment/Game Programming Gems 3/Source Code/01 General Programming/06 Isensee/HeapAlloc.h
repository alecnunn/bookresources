///////////////////////////////////////////////////////////////////////////////
//
//  HeapAlloc.h
//
//  Heap-based allocator. Uses a unique heap per allocator per type T. Based
//  on Win32 HeapCreate and HeapAlloc. Could be easily customized to not
//  serialize heap access for single-threaded programs (see HeapCreate
//  HEAP_NO_SERIALIZE option).
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

#if !defined( HEAP_ALLOCATOR_H )
#define HEAP_ALLOCATOR_H

#include "Common.h"
#include "windows.h"
#include <memory>
#include <limits>

namespace CustomAlloc
{

///////////////////////////////////////////////////////////////////////////////
//
// HeapAlloc

template <typename T>
class HeapAlloc
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
    HeapAlloc() throw()
    :
        mHeap( NULL )
    {
        // For various types of heaps, including single-threaded and limited
        // heaps, see HeapCreate() parameters.
        mHeap = HeapCreate( 0, 0, 0 );
    }

    HeapAlloc( const HeapAlloc& ) throw()
    :
        mHeap( NULL )
    {
        mHeap = HeapCreate( 0, 0, 0 );
    }

#if _MSC_VER >= 1300 // VC 7 can't handle template members
    template <typename U>
    HeapAlloc( const HeapAlloc<U>& ) throw()
    :
        mHeap( NULL )
    {
        mHeap = HeapCreate( 0, 0, 0 );
    }
#endif

    HeapAlloc& operator=( const HeapAlloc& ha )
    {
        Destroy();
        mHeap = HeapCreate( 0, 0, 0 );
        return *this;
    }

    // Destructor
    ~HeapAlloc() throw()
    {
        Destroy();
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
        typedef HeapAlloc<U> other;
    };

    // Allocate raw memory
    pointer allocate( size_type n, const void* = NULL )
    {
        assert( mHeap != NULL );
        DWORD nBytes = n * sizeof(T);
        VOID* p = ::HeapAlloc( mHeap, 0, nBytes );
        if( p == NULL )
            throw std::bad_alloc();
        return pointer(p);
    }

    // Free raw memory.
    // Note that C++ standard defines this function as
    // deallocate( pointer p, size_type). Because Visual C++ 6.0
    // compiler doesn't support template rebind, Dinkumware uses
    // void* hack.
    void deallocate( void* p, size_type )
    {
        // assert( p != NULL );
        // The standard states that p must not be NULL. However, some
        // STL implementations fail this requirement, so the check must
        // be made here.
        if( p == NULL )
            return;
        assert( mHeap != NULL );
        BOOL bSuccess = HeapFree( mHeap, 0, p );
        assert( bSuccess );
        USED( bSuccess );
    }

    // Non-standard Dinkumware hack for Visual C++ 6.0 compiler.
    // VC 6 doesn't support template rebind.
    char* _Charalloc( size_type n )
    {
        return reinterpret_cast<char*>( allocate( n, NULL ) );
    }

private:

    void Destroy()
    {
        BOOL bSuccess = HeapDestroy( mHeap );
        assert( bSuccess );
        USED( bSuccess );
    }

private:

    HANDLE mHeap;

}; // end of HeapAlloc

// Comparison
template <typename T1>
bool operator==( const HeapAlloc<T1>& lhs, const HeapAlloc<T1>& rhs) throw()
{
    return lhs.mHeap == rhs.mHeap;
}

template <typename T1>
bool operator!=( const HeapAlloc<T1>& lhs, const HeapAlloc<T1>& rhs) throw()
{
    return lhs.mHeap != rhs.mHeap;
}

} // end namespace CustomAlloc

#endif // HEAP_ALLOCATOR_H

///////////////////////////////////////////////////////////////////////////////
