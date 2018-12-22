///////////////////////////////////////////////////////////////////////////////
//
//  Win32Alloc.h
//
//  Win32 allocator. Based on Win32 HeapAlloc function.
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

#if !defined( WIN32_ALLOCATOR_H )
#define WIN32_ALLOCATOR_H

#include "Common.h"
#include "windows.h"
#include <memory>
#include <limits>

namespace CustomAlloc
{

///////////////////////////////////////////////////////////////////////////////
//
// Win32Alloc

template <typename T>
class Win32Alloc
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
    Win32Alloc() throw()
    {
    }

    Win32Alloc( const Win32Alloc& ) throw()
    {
    }

#if _MSC_VER >= 1400 // VC 7 can't handle template members
    template <typename U>
    Win32Alloc( const Win32Alloc<U>& ) throw()
    {
    }
#endif

    Win32Alloc& operator=( const Win32Alloc& )
    {
        return *this;
    }

    // Destructor
    ~Win32Alloc() throw()
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
        typedef Win32Alloc<U> other;
    };

    // Allocate raw memory
    pointer allocate( size_type n, const void* = NULL )
    {
        HANDLE h = GetProcessHeap();
        DWORD nBytes = n * sizeof(T);
        VOID* p = ::HeapAlloc( h, 0, nBytes );
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
        HANDLE h = GetProcessHeap();
        BOOL bSuccess = HeapFree( h, 0, p );
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

    // No data

}; // end of Win32Alloc

// Comparison
template <typename T1, typename T2>
bool operator==( const Win32Alloc<T1>&,
                 const Win32Alloc<T2>& ) throw()
{
    return true;
}

template <typename T1, typename T2>
bool operator!=( const Win32Alloc<T1>&,
                 const Win32Alloc<T2>& ) throw()
{
    return false;
}

} // end namespace CustomAlloc

#endif // WIN32_ALLOCATOR_H

///////////////////////////////////////////////////////////////////////////////
