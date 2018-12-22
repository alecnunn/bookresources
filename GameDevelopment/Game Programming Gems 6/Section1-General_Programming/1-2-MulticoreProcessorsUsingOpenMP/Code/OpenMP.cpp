///////////////////////////////////////////////////////////////////////////////
//
//  OpenMP.cpp
//
//  OpenMP examples for Game Programming Gems 6.
//
//  Copyright © 2005 Pete Isensee (PKIsensee@msn.com).
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

#if( _XBOX_VER >= 200 )
    #include <xtl.h>
#else
    #define _WIN32_WINNT 0x0400 // For SetThreadIdealProcessor
    #include <windows.h>
#endif

#include <cassert>
#include <cstdio>
#include <string>
#include <algorithm>
#include <vector>

#include <omp.h> // Standard OpenMP header
#include <malloc.h>

// When _OPENMP defined, compiler is looking for #pragma omp directives
#ifdef _OPENMP 
    #pragma message( "Compiled by an OpenMP-compliant implementation" )
#else
    #pragma message( "Use the /openmp compiler switch" )
#endif

namespace Gems6
{

///////////////////////////////////////////////////////////////////////////////
//
// Constants

// Increase to measure average performance; larger values mean
// longer test runs.
const int gTestRuns = 100;

// Particle system constants
const __int64 gNanosecondsPerParticle = 100;
const int gNumParticles = 100000;

// Collision detection constants
const __int64 gNanosecondsPerSphereIntersect = 100;
const __int64 gNanosecondsPerObjectsIntersect = 10000;
const int gNumObjects = 1000;

const int gSphereIntersectPercent = 10;
const int gObjectsIntersectPercent = 1;

// Qsort constants
const int gQsortNum = 1000000;
const int gMaxStrChars = 16;

// General-purpose constants
const LARGE_INTEGER gZero = { 0, 0 };
const __int64 gNanosecondsPerSecond = 1000000000; // 10^9

// Globals calculated in Initialize()
double gGetNewParticlePosTicks = 0.0;
double gSpheresIntersectTicks = 0.0;
double gObjectsIntersectTicks = 0.0;

///////////////////////////////////////////////////////////////////////////////
//
// Simple timer class

class Timer
{
public:

    Timer()
        : mStart( gZero )
    {
        Restart();
    }

    void Restart()
    {
        QueryPerformanceCounter( &mStart );
    }

    __int64 GetElapsed() const
    {
        LARGE_INTEGER end;
        QueryPerformanceCounter( &end );
        return end.QuadPart - mStart.QuadPart;
    }

private:

    LARGE_INTEGER mStart;

};

///////////////////////////////////////////////////////////////////////////////
//
// Fake particle class for testing

struct Particle
{
    float x;
    float y;
    float z;
};

///////////////////////////////////////////////////////////////////////////////
//
// Fake game object class for testing

struct Object
{
    float a;
    float b;
    float c;
};

///////////////////////////////////////////////////////////////////////////////
//
// Initialize

void Initialize()
{    
    LARGE_INTEGER TicksPerSecond;
    QueryPerformanceFrequency( &TicksPerSecond );
    printf( "QPF: %I64d ticks/sec\n", TicksPerSecond.QuadPart );

    // Max threads available to OpenMP
    printf( "Maximum OpenMP threads: %d\n\n", omp_get_max_threads() );
    
    gGetNewParticlePosTicks = (double)TicksPerSecond.QuadPart / 
                              (double)gNanosecondsPerSecond;
    gGetNewParticlePosTicks *= gNanosecondsPerParticle;

    gSpheresIntersectTicks = (double)TicksPerSecond.QuadPart / 
                             (double)gNanosecondsPerSecond;
    gSpheresIntersectTicks *= gNanosecondsPerSphereIntersect;
    gObjectsIntersectTicks = (double)TicksPerSecond.QuadPart / 
                             (double)gNanosecondsPerSecond;
    gObjectsIntersectTicks *= gNanosecondsPerObjectsIntersect;
}    

///////////////////////////////////////////////////////////////////////////////
//
// UpdateParticles

void UpdateParticles( Particle& particle )
{
    // This function would typically generate a new particle 
    // position based on input values like current velocity, 
    // the value of gravity, and so forth. For testing purposes,
    // this function simply waits in a loop until the specific 
    // period has expired.
    
    Timer timer;
    for( ;; )
    {
        // Do a bit of fake work to make the compiler happy
        particle.x *= particle.y;
        particle.y *= particle.z;
        particle.z *= particle.x;
        
        // Bail out if enough time has elapsed
        if( timer.GetElapsed() > gGetNewParticlePosTicks )
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// SpheresIntersect

bool SpheresIntersect( const Object& a, const Object& b )
{
    // Simulates a sphere intersection function
    Timer timer;
    for( ;; )
    {
        // Bail out if enough time has elapsed
        if( timer.GetElapsed() > gSpheresIntersectTicks )
            break;
        if( &a == &b ) // Use a and b so compiler is happy
            break;
    }
    return( ( rand() % 100 ) < gSphereIntersectPercent );
}

///////////////////////////////////////////////////////////////////////////////
//
// ObjectsIntersect

bool ObjectsIntersect( const Object& a, const Object& b )
{
    // Simulates a finer-grained intersection function
    Timer timer;
    for( ;; )
    {
        // Bail out if enough time has elapsed
        if( timer.GetElapsed() > gObjectsIntersectTicks )
            break;
        if( &a == &b ) // Use a and b so compiler is happy
            break;
    }
    return( ( rand() % 100 ) < gObjectsIntersectPercent );
}

///////////////////////////////////////////////////////////////////////////////
//
// SerialParticleLoop

void SerialParticleLoop( Particle* particle, int numParticles )
{
    for( int i = 0; i < numParticles; ++i )
        UpdateParticles( particle[i] );
}

///////////////////////////////////////////////////////////////////////////////
//
// ParallelParticleLoop

void ParallelParticleLoop( Particle* particle, int numParticles )
{
    #pragma omp parallel for
    for( int i = 0; i < numParticles; ++i )
        UpdateParticles( particle[i] );
}

///////////////////////////////////////////////////////////////////////////////
//
// Data handed off to the thread functions ThreadPoolFn and SlaveParticleFn

struct ThreadData
{
    HANDLE startSlave;      // signaled when slave should begin
    HANDLE slaveDone;       // signaled when slave is done
    Particle* particles;    // Particle array
    int begin;              // starting index into Particle array; inclusive
    int end;                // ending index into Particle array; exclusive
};

///////////////////////////////////////////////////////////////////////////////
//
// Thread team data for native threading test

struct ThreadTeam
{
    int maxThreads;                  // number of hardware threads on team
    std::vector<HANDLE> threads;     // thread handles; size maxThreads
    std::vector<HANDLE> startSlaves; // signaled when slave thread should begin
    std::vector<HANDLE> slavesDone;  // signaled when slave thread is done
    std::vector<ThreadData> data;    // handed off to each thread fn
    Particle* particles;             // Particle array
    int numParticles;                // total number of particles
};

///////////////////////////////////////////////////////////////////////////////
//
// Function that does the real work of updating particles

void SlaveParticleFn( const ThreadData* pThreadData )
{
    // Process one section of particles
    for( int i = pThreadData->begin; i < pThreadData->end; ++i )
        UpdateParticles( pThreadData->particles[i] );
}

///////////////////////////////////////////////////////////////////////////////
//
// There is one of these threads per OpenMP processor. Once started, it
// runs forever. The real work is done by the slave function, which begins
// when startSlave is signaled. Completion of the slave is signaled
// using slaveDone.

unsigned long WINAPI ThreadPoolFn( void* pData )
{
    ThreadData* pThreadData = (ThreadData*)(pData);
    for(;;)
    {
        WaitForSingleObject( pThreadData->startSlave, INFINITE );
        SlaveParticleFn( pThreadData );
        SetEvent( pThreadData->slaveDone );
        Sleep(0); // yield the remaining timeslice
    }
    // return 0;    // never reached
}
    
///////////////////////////////////////////////////////////////////////////////
//
// ThreadTeamInit

void ThreadTeamInit( ThreadTeam& threadTeam, Particle* particles, int numParticles )
{
    // Set up the custom data
    threadTeam.particles = particles;
    threadTeam.numParticles = numParticles;

    // Reserve some memory for thread-specific data
    threadTeam.maxThreads = omp_get_max_threads();
    threadTeam.threads.resize    ( threadTeam.maxThreads );
    threadTeam.startSlaves.resize( threadTeam.maxThreads );
    threadTeam.slavesDone.resize ( threadTeam.maxThreads );
    threadTeam.data.resize       ( threadTeam.maxThreads );

    // Divide up the work among the threads
    int end = 0;
    int segment = threadTeam.numParticles / threadTeam.maxThreads;
    for( int i = 0; i < threadTeam.maxThreads; ++i )
    {
        // Create threads and synchronization events
        threadTeam.threads[i] = CreateThread( NULL, 0, ThreadPoolFn, 
                                              &threadTeam.data[i], 
                                              CREATE_SUSPENDED, NULL );
        threadTeam.startSlaves[i] = CreateEvent( NULL, FALSE, FALSE, NULL );
        threadTeam.slavesDone[i] = CreateEvent( NULL, FALSE, FALSE, NULL );

        // Copy handles needed by the thread functions
        threadTeam.data[i].startSlave = threadTeam.startSlaves[i];
        threadTeam.data[i].slaveDone = threadTeam.slavesDone[i];
        
        // Data needed to do the actual work on a portion of the particle array
        threadTeam.data[i].particles = threadTeam.particles;
        threadTeam.data[i].begin = end;
        threadTeam.data[i].end = end = segment * (i+1);
        
        // Ensure last segment gets any remaining particles, too
        if( i == threadTeam.maxThreads - 1 )
            threadTeam.data[i].end = threadTeam.numParticles;

        // Associate each thread with its own processor
        #if( _XBOX_VER >= 200 )
            XSetThreadProcessor( threadTeam.threads[i], i );
        #else
            SetThreadIdealProcessor( threadTeam.threads[i], i );
        #endif            

        // Start the thread; note that the real work doesn't begin
        // until the startSlaves events are signaled.
        ResumeThread( threadTeam.threads[i] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// NativeParticleLoop

void NativeParticleLoop( const ThreadTeam& threadTeam )
{
    // Kick off the slaves by signaling the events. To avoid interfering with
    // the thread zero (ourself), slaves are started in backward order.
    for( int i = threadTeam.maxThreads-1; i >= 0; --i )
        SetEvent( threadTeam.startSlaves[i] );
        
    // SlaveParticleFn implicitly called on each thread        

    // Wait for all threads on the team to complete
    WaitForMultipleObjects( threadTeam.maxThreads, &threadTeam.slavesDone[0], 
                            TRUE, INFINITE );
}

///////////////////////////////////////////////////////////////////////////////
//
// ThreadTeamCleanup

void ThreadTeamCleanup( ThreadTeam& threadTeam )
{
    for( int i = 0; i < threadTeam.maxThreads; ++i )
    {
        CloseHandle( threadTeam.threads[i] );
        CloseHandle( threadTeam.startSlaves[i] );
        CloseHandle( threadTeam.slavesDone[i] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// SerialCollision

void SerialCollision( Object* obj, int numObjects )
{
    bool anyCollisions = false;
    for( int i = 0; i < numObjects; ++i )
    {
        for( int j = 0; j < i; ++j )
        {
            if( SpheresIntersect( obj[i], obj[j] ) &&
                ObjectsIntersect( obj[i], obj[j] ) )
            {
                anyCollisions = true;
            }
        }
    }
    printf( "", anyCollisions ); // tell compiler anyCollisions is important
}

///////////////////////////////////////////////////////////////////////////////
//
// ParallelCollision

void ParallelCollision( Object* obj, int numObjects )
{
    bool anyCollisions = false;
    #pragma omp parallel for schedule( dynamic, 2 )
    for( int i = 0; i < numObjects; ++i )
    {
        for( int j = 0; j < i; ++j )
        {
            if( SpheresIntersect( obj[i], obj[j] ) &&
                ObjectsIntersect( obj[i], obj[j] ) )
            {
                anyCollisions = true;
            }
        }
    }
    printf( "", anyCollisions ); // tell compiler anyCollisions is important
}

///////////////////////////////////////////////////////////////////////////////
//
// Partition
//
// Standard quicksort algorithm for partitioning an array. Patterned on
// Sedgewick, Algorithms in C++.

template< typename T >
int Partition( T* arr, int lo, int hi )
{
    int i = lo - 1;
    int j = hi;
    T v = arr[ hi ];
    for( ;; )
    {
        while( arr[ ++i ] < v )
            ;
        while( v < arr[ --j ] )
        {
            if( j == lo )
                break;
        }
        if( i >= j )
            break;
        std::swap( arr[i], arr[j] );
    }
    std::swap( arr[i], arr[hi] );
    return i;
}

///////////////////////////////////////////////////////////////////////////////
//
// SerialQsort

template< typename T >
void SerialQsort( T* arr, int lo, int hi )
{
    if( hi <= lo )
        return;
    int n = Partition( arr, lo, hi );
    SerialQsort( arr, lo, n-1 );
    SerialQsort( arr, n+1, hi );
}

///////////////////////////////////////////////////////////////////////////////
//
// ParallelQsort

template< typename T >
void ParallelQsort( T* arr, int lo, int hi )
{
    if( hi <= lo )
        return;
  
    // Partition two levels deep; shipping code would need error
    // checking to validate n-1 and n+1 boundary conditions.
    int n = Partition( arr, lo, hi );
    int m = Partition( arr, lo, n-1 );
    int q = Partition( arr, n+1, hi );
    
    // Divide the work into 4 sections and sort each using
    // non-parallel version above
    #pragma omp parallel sections
    {
        #pragma omp section
        SerialQsort( arr, lo, m-1 );

        #pragma omp section
        SerialQsort( arr, m+1, n-1 );

        #pragma omp section
        SerialQsort( arr, n+1, q-1 );

        #pragma omp section
        SerialQsort( arr, q+1, hi );
    }        
}

///////////////////////////////////////////////////////////////////////////////
//
// CollisionDetection performance measurements

void ParticleSystem()
{
    std::vector< Particle > particle( gNumParticles );

    // Time the serial version
    __int64 serialTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        Timer serialTimer;
        SerialParticleLoop( &particle[0], gNumParticles );
        serialTicks += serialTimer.GetElapsed();
    }        
    printf( "Serial version  : %I64d ticks\n", serialTicks );

    // Time the parallel version
    __int64 parallelTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        Timer parallelTimer;
        ParallelParticleLoop( &particle[0], gNumParticles );
        parallelTicks += parallelTimer.GetElapsed();
    }
    printf( "Parallel version: %I64d ticks\n", parallelTicks );
    printf( " (%fX faster)\n", (DOUBLE)serialTicks / 
                               (DOUBLE)parallelTicks );

    ThreadTeam threadTeam;
    ThreadTeamInit( threadTeam, &particle[0], gNumParticles );

    // Time the native threading version
    __int64 nativeTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        Timer nativeTimer;
        NativeParticleLoop( threadTeam );
        nativeTicks += nativeTimer.GetElapsed();
    }
    printf( "Native version  : %I64d ticks\n", nativeTicks );
    printf( " (%fX faster)\n\n", (DOUBLE)serialTicks / 
                                 (DOUBLE)nativeTicks );
                                 
    ThreadTeamCleanup( threadTeam );
}

///////////////////////////////////////////////////////////////////////////////
//
// CollisionDetection performance measurements

void CollisionDetection()
{
    std::vector< Object > obj( gNumObjects );
    
    __int64 serialTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        srand( i );
        Timer serialTimer;
        SerialCollision( &obj[0], gNumObjects );
        serialTicks += serialTimer.GetElapsed();
    }        
    printf( "Serial version  : %I64d ticks\n", serialTicks );

    __int64 parallelTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        srand( i );
        Timer parallelTimer;
        ParallelCollision( &obj[0], gNumObjects );
        parallelTicks += parallelTimer.GetElapsed();
    }        
    printf( "Parallel version: %I64d ticks\n", parallelTicks );
    printf( " (%fX faster)\n\n", (DOUBLE)serialTicks / 
                                 (DOUBLE)parallelTicks );
}

///////////////////////////////////////////////////////////////////////////////
//
// Qsort performance measurements

template< typename T, typename Generator >
void Qsort( Generator g )
{
    std::vector< T > arr( gQsortNum );
    
    __int64 serialTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        srand( i );
        generate( arr.begin(), arr.end(), g );
        Timer serialTimer;
        SerialQsort( &arr[0], 0, gQsortNum-1 );
        serialTicks += serialTimer.GetElapsed();
    }        
    printf( "Serial version  : %I64d ticks\n", serialTicks );

    // Save a copy for verification    
    std::vector< T > verify( arr );

    __int64 parallelTicks = 0;
    for( int i = 0; i < gTestRuns; ++i )
    {
        srand( i );
        generate( arr.begin(), arr.end(), g );
        Timer parallelTimer;
        ParallelQsort( &arr[0], 0, gQsortNum-1 );
        parallelTicks += parallelTimer.GetElapsed();
    }        
    printf( "Parallel version: %I64d ticks\n", parallelTicks );
    printf( " (%fX faster)\n\n", (DOUBLE)serialTicks / 
                                 (DOUBLE)parallelTicks );

    // Verify
    for( int i = 0; i < gQsortNum; ++i )
        assert( arr[i] == verify[i] );                                 
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a random string of size 1 through gMaxStrChars

std::string StrRand()
{
    int chars = 1 + ( rand() % gMaxStrChars );
    std::string str;
    for( int i = 0; i < chars; ++i )
        str += char( rand() + 1 ); // +1 guarantees non-null
    return str;        
}

} // end namespace Gems6

///////////////////////////////////////////////////////////////////////////////
//
// Main

int main()
{
    using namespace Gems6;
    
    Initialize();
    
    printf( "Particle system\n" );
    ParticleSystem();

    printf( "Collision detection\n" );
    CollisionDetection();
    
    printf( "Qsort (int)\n" );
    Qsort< int >( rand );

    printf( "Qsort (double)\n" );
    Qsort< double >( rand );

    printf( "Qsort (string)\n" );
    Qsort< std::string >( StrRand );

    return 0;
}
