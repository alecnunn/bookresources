/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _STAR_H_
#define _STAR_H_
#include "assert.h"

template < class T > class Star
{
	private :
	union
	{
		T* mpT;
		const T* mpcT;
	};

	public :

	Star< T >( const T* pT = 0 ) : mpcT( pT ){;}
	Star< T >( const Star< T >& aStar ) : mpcT( aStar.mpcT ){;}

	Star< T >& operator=( const Star< T >& rhs )
	{
		if ( mpT != rhs.mpT )
		{
			delete mpT, mpT = rhs.mpT;
		}
		return *this;
	}

	void Clear()
	{
		delete mpT, mpT = 0;
	}
	~Star< T >()
	{
		Clear();
	}

	bool operator<( const Star< T >& aStar ) const
	{
		return ( *this < aStar );
	}

	bool operator!=( const Star< T >& aStar ) const
	{
		return !( *this == aStar );
	}

	bool operator==( const Star< T >& aStar ) const
	{
		return ( *this == aStar );
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


#endif // _STAR_H_

