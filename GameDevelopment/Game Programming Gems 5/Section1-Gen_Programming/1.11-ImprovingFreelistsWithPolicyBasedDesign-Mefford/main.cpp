
#include "FreeList.h"
#include <iostream>

// A simple test class 
class Foo
{
public:
	Foo() 
		: a(10), x(-1.0f) 
	{
	}
	Foo( int a_ ) 
		: a(a_), x(2.0f) 
	{
	}
	Foo( float x_ ) 
		: a(0), x(x_) 
	{
	}

	Foo( int a_, float x1, float x2 = 1.0f )
		: a(a_)
	{
		x = x1 * x2;
	}
	
	~Foo()
	{
		a = -100;
		x = -100.0f;
	}

private:
	int a;
	float x;
};

// Another test class.
// NOTE that this class is the same size as Foo (assuming sizeof(int) == 4)
class Bar
{
public:
	Bar() 
		: d(3.1415926535)
	{
	}

	~Bar()
	{
		d = 2.7182818;
	}

private:
	double d;
};

template< class FreeList > void PrintUsage( FreeList& freeList )
{
	std::cout << "Num In Use:  " << freeList.GetNumberOfBlocksInUse();
	std::cout << "  Peak:  " << freeList.GetPeakNumberOfBlocksInUse();
	std::cout << "  Allocated:  " << freeList.GetNumBlocksAllocated() << std::endl; 
}

template< class T, class FreeList > void RunSimpleTest( FreeList& freeList )
{
	PrintUsage( freeList );

	// Allocate a few objects
	T* pAt1 = freeList.Allocate();
	T* pAt2 = freeList.Allocate();
	T* pAt3 = freeList.Allocate();

	PrintUsage( freeList );

	// Free one of them
	freeList.Free( pAt1 );

	PrintUsage( freeList );

	// Allocate 10,000 objects quickly
	T* tempArray[10000];
	for( int ix = 0; ix < 10000; ++ix )
	{
		tempArray[ix] = freeList.Allocate();
	}
	PrintUsage( freeList );

	// Return those 10,000 objects back to the freelist
	for( int ix = 0; ix < 10000; ++ix )
	{
		freeList.Free( tempArray[ix] );
	}

	PrintUsage( freeList );
}

void main()
{
	gems::FreeList< Foo > aDefaultTList;

	std::cout << std::endl <<
		"Testing FreeList<Foo, PlacementNewLinkedList<Foo>, DoubleGrowthSize<16> >" << std::endl;
	RunSimpleTest< Foo >( aDefaultTList );

	// Show that it can handle various constructors
	Foo* pFoo1 = aDefaultTList.Allocate( 123 );
	Foo* pFoo2 = aDefaultTList.Allocate( 1.414f );
	Foo* pFoo3 = aDefaultTList.Allocate( -321, 0.1f );

	gems::FreeList< Foo, 
		gems::ConstructOnceStack<Foo>, 
		gems::ConstantGrowthPolicy<16,16,1024> > aGPG4FreeList;

	std::cout << std::endl <<
		"Testing FreeList<Foo, ConstructOnceStack<Foo>, ConstantGrowthPolicy<16,16,1024> >" << std::endl;
	RunSimpleTest< Foo >( aGPG4FreeList );

	// Test the additional capabilities of the ConstructOnceStack
	// Note that these functions are not provided by all policies, but if aTList2 were ever changed to use
	// a different policy, these lines would produce compile errors instead of runtime errors.
	aGPG4FreeList.FreeAll();
	PrintUsage( aGPG4FreeList );

	aGPG4FreeList.Reset();
	PrintUsage( aGPG4FreeList );

	gems::FreeList< Foo, 
		gems::CompactableChunkPolicy<Foo>,
		gems::ConstantChunkSizeGrowthPolicy<Foo, 4096> > aCompactableTList;
	
	std::cout << std::endl <<
		"Testing FreeList<Foo, CompactableChunkPolicy<Foo>, ConstantChunkSizeGrowthPolicy<Foo, 4096> >" << std::endl;
	RunSimpleTest< Foo >( aCompactableTList );

	gems::FreeList< Foo,
		gems::SharedChunkPolicy<Foo> > aSharedTList;

	std::cout << std::endl <<
		"Testing FreeList<Foo, SharedChunkPolicy<Foo>, DoubleGrowthSize<16> >" << std::endl;
	RunSimpleTest< Foo >( aSharedTList );

	// Run another test with Bar in a shared chunk policy--notice that it uses the same underlying list as the 
	// FreeList above
	gems::FreeList< Bar, 
		gems::SharedChunkPolicy<Bar> > aSecondSharedTList;

	std::cout << std::endl <<
		"Testing FreeList<Bar, SharedChunkPolicy<Bar>, DoubleGrowthSize<16> >" << std::endl;
	RunSimpleTest< Bar >( aSecondSharedTList );

	// Show that it even works on fundamental types
	gems::FreeList< int > aIntList;

	std::cout << std::endl <<
		"Testing FreeList<int, PlacementNewLinkedList<int>, DoubleGrowthSize<16> >" << std::endl;
	RunSimpleTest< int >( aIntList );

	int* pInt2 = aIntList.Allocate( 7 ); // Use the build-in constructor for int

}