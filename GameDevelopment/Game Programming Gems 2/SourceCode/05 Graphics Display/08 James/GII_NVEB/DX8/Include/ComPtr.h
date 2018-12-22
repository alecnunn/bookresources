/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _COMPTR_H_
#define _COMPTR_H_

enum ComPtrAddRefOrNot
{		
	COMPTR_DO_ADD_REF = 1,
	COMPTR_DONT_ADD_REF = 2
};

template < class pT > class ComPtr
{
	private :

		pT mpT;

	public :

	ComPtr( const ComPtr& aCPtr, const ComPtrAddRefOrNot& aRef = COMPTR_DO_ADD_REF  ) : mpT( aCPtr.mpT )
	{
		if ( mpT && ( aRef == COMPTR_DO_ADD_REF ) ){ mpT->AddRef(); }
	}

	ComPtr( pT apT = 0, const ComPtrAddRefOrNot& aRef = COMPTR_DO_ADD_REF ) : mpT( apT )
	{
		if ( mpT && ( aRef == COMPTR_DO_ADD_REF ) )
		{ 
			mpT->AddRef(); 
		}
	}

	~ComPtr()
	{
		Clear();
	}

	void Clear()
	{
		if ( mpT ) 
		{ 
			mpT->Release();
			mpT = 0; 
		}
	}

	bool operator!=( const ComPtr& rhs ) const
	{
		return !( *this == rhs );
	}

	bool operator>( const ComPtr& rhs ) const
	{
		if ( mpT && rhs.mpT )
		{
			return ( *mpT > *(rhs.mpT));
		}
		assert( mpT && rhs.mpT )
		return false;
	}	

	bool operator<=( const ComPtr& rhs ) const
	{
		return !( *this > rhs );
	}	

	bool operator>=( const ComPtr& rhs ) const
	{
		return !( *this < rhs );
	}	

	bool operator==( const ComPtr& rhs ) const
	{
		if ( mpT && rhs.mpT )
		{
			return ( *mpT == *(rhs.mpT));
		}
		assert( mpT && rhs.mpT )
		return false;
	}

	bool operator<( const ComPtr& rhs ) const
	{
		if ( mpT && rhs.mpT )
		{
			return ( *mpT < *(rhs.mpT));
		}
		assert( mpT && rhs.mpT )
		return false;
	}	

	ComPtr& operator=( const ComPtr& aPtr )
	{
		if ( aPtr.mpT ){ aPtr.mpT->AddRef(); }
		if ( mpT ) { mpT->Release(); }
		mpT = aPtr.mpT;
		return (*this);
	}

	pT Ptr() { return  mpT; }
	const pT Ptr() const { return  mpT; }

	operator pT()
	{
		return mpT;
	}

	pT& operator*()
	{
		assert( mpT );
		return mpT;
	}

	pT operator->( void )
	{
		assert( mpT );
		return mpT;
	}

	operator const pT() const
	{
		return mpT;
	}

	const pT& operator*() const
	{
		assert( mpT );
		return mpT;
	}

	const pT operator->( void ) const
	{
		assert( mpT );
		return &mpT;
	}

};

#endif //_COMPTR_H_
