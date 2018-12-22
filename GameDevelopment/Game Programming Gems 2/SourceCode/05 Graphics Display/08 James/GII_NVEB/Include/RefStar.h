/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _REFSTAR_H_
#define _REFSTAR_H_

class RefCounted
{
	private :

		int mRefCount;
	public :

		int AddRef()
		{
			return ( ++mRefCount );
		}

		int Release()
		{
			if ( mRefCount == 1 )
			{
				delete this; 
				return 0;
			}	
			--mRefCount;
			return mRefCount;
		}

		virtual ~RefCounted(){ mRefCount = 0;}

		RefCounted() : mRefCount( 0 ) {;}
};

template < class T >
class RefStar
{
	private :
		union
		{
				  T* mpT;
			const T* mpCT;
		};

	public :

		RefStar( const T* pT = 0 ) : mpCT( pT )
		{
			if ( mpT ) mpT->AddRef();
		}

		~RefStar()
		{
			if ( mpT ) mpT->Release();
		}

		RefStar( const RefStar& rhs )
		{
			mpCT = rhs.mpCT;
			if ( mpT ) mpT->AddRef();
		}
			
		RefStar& operator=( const RefStar& rhs )
		{
			if ( this != &rhs )
			{
				if ( mpT ) mpT->Release();
				mpCT = rhs.mpCT;
				if ( mpT ) mpT->AddRef();
			}
			return *this;
		}

		const T& operator*() const
		{
			return (*mpCT);
		}

		T& operator*()
		{
			return (*mpT);
		}

		const T* operator->() const
		{
			return mpCT;
		}

		T* operator->()
		{
			return mpT;
		}

		bool operator==( const RefStar& rhs ) const
		{
			if ( ( mpT ) && ( rhs.mpT ) )
			{
				return ( (*mpT ) == ( *rhs.mpT ) );
			}
			return true;
		}

		bool operator<( const RefStar& rhs ) const
		{
			if ( ( mpT ) && ( rhs.mpT ) )
			{
				return ( (*mpT ) < ( *rhs.mpT ) );
			}
			return false;
		}

		operator const T*() const
		{
			return mpCT;
		}

		operator T*()
		{
			return mpT;
		}

		operator bool() const
		{
			return ( mpCT != 0 );
		}
};



#endif  _REFSTAR_H_
