///////////////////////////////////////////////////////////////////////////////
//
//  Main.cpp
//
//  Examples and testing code for Custom STL Allocators
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

#include "common.h"
#include "windows.h"

#include <memory>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <complex>
#include <algorithm>

#include "MallocAlloc.h"
#include "Win32Alloc.h"
#include "StaticAlloc.h"
#include "StackAlloc.h"
#include "HeapAlloc.h"

///////////////////////////////////////////////////////////////////////////////
//
// VERIFY message

void ErrorMsg( const char* pMsg, const char* pFile, int nLine )
{
    char msg[2048];
    sprintf( msg, "%s\nFile:%s Line:%d", pMsg, pFile, nLine );
    MessageBox( NULL, msg, "Error", MB_OK );
    DebugBreak();
}

namespace // anonymous
{

using namespace std;
using namespace CustomAlloc;

const int MAX_PERF = 100; // increase for finer performance evaluation

///////////////////////////////////////////////////////////////////////////////
//
// Simple class for testing with containers

class MyClass
{
public:

    MyClass()
    :
        m_A( 1 ),
        m_B( 2.3f ),
        m_C( 4.5, 6.7 ),
        m_D( "MyClass" )
    {
    }

    MyClass( const MyClass& s )
    :
        m_A( s.m_A ),
        m_B( s.m_B ),
        m_C( s.m_C ),
        m_D( s.m_D )
    {
    }

    MyClass& operator=( const MyClass& s )
    {
        // (see Exceptional C++, Herb Sutter, Item 12 )
        MyClass t( s );
        swap( m_A, t.m_A );
        swap( m_B, t.m_B );
        swap( m_C, t.m_C );
        m_D.swap( t.m_D );
        return *this;
    }

    bool operator==( const MyClass& s ) const
    {
        return( m_A == s.m_A );
    }

    bool operator<( const MyClass& s ) const
    {
        return( m_A < s.m_A );
    }

private:

    // Some "interesting" data
    int             m_A;
    float           m_B;
    complex<double> m_C;
    string          m_D;

};

///////////////////////////////////////////////////////////////////////////////
//
// TestAlloc basic testing

template <typename T, class Alloc>
void TestAlloc( Alloc a, const T& t )
{
    // Test ctors
    Alloc b;
    Alloc c(a);
    b = c;

    // Test utility functions
    T x(t);
    Alloc::pointer p = a.address( x );
    VERIFY( p );
    Alloc::const_pointer cp = a.address( t );
    VERIFY( cp );
    Alloc::size_type s = a.max_size();
    VERIFY( s );

    // 1 object
    Alloc::size_type n = 1;
    p = a.allocate( n, NULL );
    VERIFY( p );
    a.construct( p, t );
    VERIFY( *p == t );
    a.destroy( p );
    a.deallocate( p, n );

    // 10 objects
    n = 10;
    p = a.allocate( n, NULL );
    VERIFY( p );
    for( Alloc::size_type i = 0; i < n; ++i )
    {
        a.construct( p+i, t );
        VERIFY( *(p+i) == t );
    }
    for( Alloc::size_type j = 0; j < n; ++j )
        a.destroy( p+j );
    a.deallocate( p, n );

    // Container testing
    // The printf forces the compiler to believe that the contents of
    // the containers are important, even in release mode.

    // vector
    {
    vector<T, Alloc> v(a);
    v.push_back( t );
    printf( "", v );
    }

    // deque
    {
    deque<T, Alloc> d(a);
    d.push_back( t );
    printf( "", d );
    }

    // list
    {
    list<T, Alloc> ls(a);
    ls.push_back( t );
    printf( "", ls );
    }

    // map
    {
    typedef map<int, T, less<int>, Alloc> MapT;
    typedef pair<int, T> Pair;
    MapT m( less<int>(), a );
    m.insert( Pair(1, t) );
    printf( "", m );
    }

    // multimap
    {
    typedef multimap< int, T, less<int>, Alloc > MultiMapT;
    typedef pair<int, T> Pair;
    MultiMapT mm( less<int>(), a );
    mm.insert( Pair( 1, t ) );
    printf( "", mm );
    }

    // set
    {
    typedef set< T, less<T>, Alloc> SetT;
    SetT st( less<T>(), a );
    st.insert( t );
    printf( "", st );
    }

    // multiset
    {
    typedef multiset< T, less<T>, Alloc> MultiSetT;
    MultiSetT ms( less<T>(), a );
    ms.insert( t );
    printf( "", ms );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Test string allocator

template <typename T, class Alloc>
void TestStrAlloc( const Alloc& a, const T& t, const T* s )
{
    basic_string< T, char_traits<T>, Alloc > str(a);
    str.assign( s );
    printf( "", str );
}

///////////////////////////////////////////////////////////////////////////////
//
// Performance testing

template <typename T, class Alloc>
longlong TestPerf( const Alloc& a, const T& t )
{
    longlong nStart = 0;
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nStart ) );

    // Rather than testing a.allocate() and a.deallocate() directly,
    // we do some real-world container insertion and deletion

    // vector
    {
    vector<T, Alloc> v(a);
    for( size_t i = 0; i < MAX_PERF; ++i )
        v.push_back( t );
    printf( "", v );
    }

    // list
    {
    list<T, Alloc> ls(a);
    for( size_t i = 0; i < MAX_PERF; ++i )
        ls.push_back( t );
    printf( "", ls );
    }

    // map
    {
    typedef map<int, T, less<int>, Alloc> MapT;
    typedef pair<int, T> Pair;
    MapT m( less<int>(), a );
    for( size_t i = 0; i < MAX_PERF; ++i )
        m.insert( Pair(1, t) );
    printf( "", m );
    }

    longlong nEnd = 0;
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nEnd ) );
    return nEnd - nStart;
}

///////////////////////////////////////////////////////////////////////////////
//
// Performance test string alloc

template <typename T, class Alloc>
longlong TestPerfStr( const Alloc& a, const T& t, const T* s )
{
    longlong nStart = 0;
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nStart ) );

    for( int i = 0; i < MAX_PERF; ++i )
    {
        basic_string< T, char_traits<T>, Alloc > str( a );
        str.assign( s );
        printf( "", s );
    }

    longlong nEnd = 0;
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nEnd ) );
    return nEnd - nStart;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test default allocator

void TestDefaultAllocator()
{
    TestAlloc( allocator<int>(), 1 );
    TestAlloc( allocator<double>(), 1.1 );
    TestAlloc( allocator<string>(), string( "abc" ) );
    TestAlloc( allocator<MyClass>(), MyClass() );

    TestStrAlloc( allocator<char>(), 'a', "abc" );
    TestStrAlloc( allocator<wchar_t>(), L'a', L"abc" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test MallocAlloc

void TestMallocAlloc()
{
    TestAlloc( MallocAlloc<int>(), 1 );
    TestAlloc( MallocAlloc<double>(), 1.1 );
    TestAlloc( MallocAlloc<string>(), string( "abc" ) );
    TestAlloc( MallocAlloc<MyClass>(), MyClass() );

    TestStrAlloc( MallocAlloc<char>(), 'a', "abc" );
    TestStrAlloc( MallocAlloc<wchar_t>(), L'a', L"abc" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test Win32Alloc

void TestWin32Alloc()
{
    TestAlloc( Win32Alloc<int>(), 1 );
    TestAlloc( Win32Alloc<double>(), 1.1 );
    TestAlloc( Win32Alloc<string>(), string( "abc" ) );
    TestAlloc( Win32Alloc<MyClass>(), MyClass() );

    TestStrAlloc( Win32Alloc<char>(), 'a', "abc" );
    TestStrAlloc( Win32Alloc<wchar_t>(), L'a', L"abc" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test StaticAlloc

void TestStaticAlloc()
{
    TestAlloc( StaticAlloc<int, 10000>(), 1 );
    TestAlloc( StaticAlloc<double, 10000>(), 1.1 );
    TestAlloc( StaticAlloc<string, 20000>(), string( "abc" ) );
    TestAlloc( StaticAlloc<MyClass, 50000>(), MyClass() );

    TestStrAlloc( StaticAlloc<char, 10000>(), 'a', "abc" );
    TestStrAlloc( StaticAlloc<wchar_t, 10000>(), L'a', L"abc" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test StackAlloc

void TestStackAlloc()
{
    const size_t nBytes = 40 * 1024;
    unsigned char stack[ nBytes ]; // 40 K stack data

    TestAlloc( StackAlloc<int>( stack, nBytes ), 1 );
    TestAlloc( StackAlloc<double>( stack, nBytes ), 1.1 );
    TestAlloc( StackAlloc<string>( stack, nBytes ), string( "abc" ) );
    TestAlloc( StackAlloc<MyClass>( stack, nBytes ), MyClass() );

    TestStrAlloc( StackAlloc<char>( stack, nBytes ), 'a', "abc" );
    TestStrAlloc( StackAlloc<wchar_t>( stack, nBytes ), L'a', L"abc" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test HeapAlloc

void TestHeapAlloc()
{
    TestAlloc( CustomAlloc::HeapAlloc<int>(), 1 );
    TestAlloc( CustomAlloc::HeapAlloc<double>(), 1.1 );
    TestAlloc( CustomAlloc::HeapAlloc<string>(), string( "abc" ) );
    TestAlloc( CustomAlloc::HeapAlloc<MyClass>(), MyClass() );

    TestStrAlloc( CustomAlloc::HeapAlloc<char>(), 'a', "abc" );
    TestStrAlloc( CustomAlloc::HeapAlloc<wchar_t>(), L'a', L"abc" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test performance of default allocator

longlong TestPerfDefault()
{
    longlong nTicks = 0;
    nTicks += TestPerf( allocator<int>(), 1 );
    nTicks += TestPerf( allocator<double>(), 1.1 );
    nTicks += TestPerf( allocator<string>(), string( "abc" ) );
    nTicks += TestPerf( allocator<MyClass>(), MyClass() );

    nTicks += TestPerfStr( allocator<char>(), 'a', "abc" );
    nTicks += TestPerfStr( allocator<wchar_t>(), L'a', L"abc" );
    return nTicks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test performance of MallocAlloc

longlong TestPerfMalloc()
{
    longlong nTicks = 0;
    nTicks += TestPerf( MallocAlloc<int>(), 1 );
    nTicks += TestPerf( MallocAlloc<double>(), 1.1 );
    nTicks += TestPerf( MallocAlloc<string>(), string( "abc" ) );
    nTicks += TestPerf( MallocAlloc<MyClass>(), MyClass() );

    nTicks += TestPerfStr( MallocAlloc<char>(), 'a', "abc" );
    nTicks += TestPerfStr( MallocAlloc<wchar_t>(), L'a', L"abc" );
    return nTicks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test performance of Win32Alloc

longlong TestPerfWin32()
{
    longlong nTicks = 0;
    nTicks += TestPerf( Win32Alloc<int>(), 1 );
    nTicks += TestPerf( Win32Alloc<double>(), 1.1 );
    nTicks += TestPerf( Win32Alloc<string>(), string( "abc" ) );
    nTicks += TestPerf( Win32Alloc<MyClass>(), MyClass() );

    nTicks += TestPerfStr( Win32Alloc<char>(), 'a', "abc" );
    nTicks += TestPerfStr( Win32Alloc<wchar_t>(), L'a', L"abc" );
    return nTicks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test performance of StaticAlloc

longlong TestPerfStatic()
{
    longlong nTicks = 0;
    nTicks += TestPerf( StaticAlloc<int, 20000>(), 1 );
    nTicks += TestPerf( StaticAlloc<double, 20000>(), 1.1 );
    nTicks += TestPerf( StaticAlloc<string, 200000>(), string( "abc" ) );
    nTicks += TestPerf( StaticAlloc<MyClass, 400000>(), MyClass() );

    nTicks += TestPerfStr( StaticAlloc<char, 10000>(), 'a', "abc" );
    nTicks += TestPerfStr( StaticAlloc<wchar_t, 10000>(), L'a', L"abc" );
    return nTicks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test performance of StackAlloc

longlong TestPerfStack()
{
    const size_t nBytes = 500 * 1024;
    unsigned char stack[ nBytes ]; // 500K stack data

    longlong nTicks = 0;
    nTicks += TestPerf( StackAlloc<int>( stack, nBytes ), 1 );
    nTicks += TestPerf( StackAlloc<double>( stack, nBytes ), 1.1 );
    nTicks += TestPerf( StackAlloc<string>( stack, nBytes ), string( "abc" ) );
    nTicks += TestPerf( StackAlloc<MyClass>( stack, nBytes ), MyClass() );

    nTicks += TestPerfStr( StackAlloc<char>( stack, nBytes ), 'a', "abc" );
    nTicks += TestPerfStr( StackAlloc<wchar_t>( stack, nBytes ), L'a', L"abc" );
    return nTicks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test performance of HeapAlloc

longlong TestPerfHeap()
{
    longlong nTicks = 0;
    nTicks += TestPerf( CustomAlloc::HeapAlloc<int>(), 1 );
    nTicks += TestPerf( CustomAlloc::HeapAlloc<double>(), 1.1 );
    nTicks += TestPerf( CustomAlloc::HeapAlloc<string>(), string( "abc" ) );
    nTicks += TestPerf( CustomAlloc::HeapAlloc<MyClass>(), MyClass() );

    nTicks += TestPerfStr( CustomAlloc::HeapAlloc<char>(), 'a', "abc" );
    nTicks += TestPerfStr( CustomAlloc::HeapAlloc<wchar_t>(), L'a', L"abc" );
    return nTicks;
}

///////////////////////////////////////////////////////////////////////////////
//
// Test example code

void TestExampleCode()
{
    typedef void* pointer;
    typedef unsigned int size_type;
    size_type n = 1;

    HANDLE h = GetProcessHeap();
    DWORD nBytes = n * sizeof(int);
    VOID* p = ::HeapAlloc( h, 0, nBytes );
    if( p == NULL )
        throw bad_alloc();
    pointer x = pointer(p);
    USED(x);

    assert( p != NULL );
    h = GetProcessHeap();
    BOOL bSuccess = HeapFree( h, 0, p );
    assert( bSuccess );
    USED( bSuccess );

    int j;
    size_t i;

    longlong nStart = 0;
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nStart ) );
    for( j = 0; j < MAX_PERF; ++j )
    {
        list<int> lst;
        for( i = 0; i < 10; ++i )
            lst.push_back( i );
        lst.reverse();
        lst.clear();
        printf( "", lst );
    }
    longlong nEnd = 0;
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nEnd ) );
    longlong nDefault = nEnd - nStart;

    const size_t nStackSize = 1024;
    unsigned char Stack[nStackSize];
    StackAlloc<int> sa( Stack, nStackSize );

    QueryPerformanceCounter( (LARGE_INTEGER*)( &nStart ) );
    for( j = 0; j < MAX_PERF; ++j )
    {
        //list<int, StackAlloc<int> > StackList( csa );
        list<int> StackList;
        for( i = 0; i < 10; ++ i )
            StackList.push_back( i );
        StackList.reverse();
        StackList.clear();
        printf( "", StackList );
    }
    QueryPerformanceCounter( (LARGE_INTEGER*)( &nEnd ) );
    longlong nStack = nEnd - nStart;

    longlong nFreq = 0;
    QueryPerformanceFrequency( (LARGE_INTEGER*)( &nFreq ) );

    double fFreq    = double( nFreq );
    double fDefault = double( nDefault ) / fFreq;
    double fStack   = double( nStack   ) / fFreq;

    printf( "Default: %f mS\n"
            "Stack  : %f mS\n\n",
            fDefault, fStack );
}

///////////////////////////////////////////////////////////////////////////////
//
// Test allocator performance

void TestPerformance()
{
    // Simple performance test of each allocator
    longlong nDefault = TestPerfDefault();
    longlong nMalloc  = TestPerfMalloc();
    longlong nWin32   = TestPerfWin32();
    longlong nStatic  = TestPerfStatic();
    longlong nStack   = TestPerfStack();
    longlong nHeap    = TestPerfHeap();

    longlong nFreq = 0;
    QueryPerformanceFrequency( (LARGE_INTEGER*)( &nFreq ) );

    double fFreq    = double( nFreq );
    double fDefault = double( nDefault ) / fFreq;
    double fMalloc  = double( nMalloc  ) / fFreq;
    double fWin32   = double( nWin32   ) / fFreq;
    double fStatic  = double( nStatic  ) / fFreq;
    double fStack   = double( nStack   ) / fFreq;
    double fHeap    = double( nHeap    ) / fFreq;

    printf( "Default: %f mS\n"
            "Malloc : %f mS\n"
            "Win32  : %f mS\n"
            "Static : %f mS\n"
            "Stack  : %f mS\n"
            "Heap   : %f mS\n",
            fDefault, fMalloc, fWin32, fStatic, fStack, fHeap );
}

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////////////
//
// Main

int __cdecl main()
{
    TestDefaultAllocator();
    TestMallocAlloc();
    TestWin32Alloc();
    TestStaticAlloc();
    TestStackAlloc();
    TestHeapAlloc();

    TestExampleCode();
    TestPerformance();
	return 0;
}
