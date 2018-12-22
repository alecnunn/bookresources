#ifndef   INCLUDED_MathTypes
#define   INCLUDED_MathTypes
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// this type is used to distinguish constructors which initialize an object to identity or
// zero, in order to distuingish from constructors which leave data uninitialized. Such 
// constructors should not actually use the value passed, but should merely by overloaded
// by type.
enum ZeroType
{
	Zero = 0
};

enum IdentityType
{
	Identity = 1
};

enum MinimumType
{
	Minimum = 2
};

enum MaximumType
{
	Maximum = 3
};


#endif // INCLUDED_MathTypes
