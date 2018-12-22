/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _ARRAYSTAR_H_
#define _ARRAYSTAR_H_
#include "assert.h"

template < class T > class ArrayStar
{
	private :
	union
	{
		T* mpT;
		const T* mpcT;
	};

	public :

	ArrayStar< T >( const T* pT = 0 ) : mpcT( pT ){;}
	ArrayStar< T >( const ArrayStar< T >& aArrayStar ) : mpcT( aArrayStar.mpcT ){;}

	ArrayStar< T >& operator=( const ArrayStar< T >& rhs )
	{
		// No Need to check for assignemnt to this 
		if ( mpT != rhs.mpT )
		{
			delete mpT, mpT = rhs.mpT;
		}
		return *this;
	}

	void Clear()
	{
		delete []mpT, mpT = 0;
	}
	~ArrayStar< T >()
	{
		Clear();
	}

	operator const T*() const
	{
		assert( mpT );
		return mpcT;
	}

	operator T*()
	{
		assert( mpT );
		return mpT;
	}

	T& operator*() const
	{
		assert( mpT );
		return *mpT;
	}

	T* operator->( void ) const
	{
		assert( mpT );
		return mpT;
	}
};


#endif // _ARRAYSTAR_H_

