/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _INDEXBUFFER_H_
#define _INDEXBUFFER_H_

#include <assert.h>
#include <d3d8.h>
#include <d3d8types.h>

interface IDirect3DIndexBuffer8;
interface IDirect3DDevice8;

class IndexBuffer16 
{
	private :

		IDirect3DIndexBuffer8* mpIB;
		unsigned int mCount;
		unsigned int mPosition;
		bool		 mbLocked;
		unsigned int mUsage;
		bool		 mbFlush;

		enum LockFlags
		{
			LOCKFLAGS_STATIC =  D3DLOCK_NOSYSLOCK,
			LOCKFLAGS_FLUSH =  D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
			LOCKFLAGS_APPEND = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
		};

	public :


		IndexBuffer16::IndexBuffer16( IDirect3DDevice8* pD3D, 
					  const unsigned int& theCount, 
					  const DWORD& theUsage = D3DUSAGE_DONOTCLIP | D3DUSAGE_WRITEONLY | D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DYNAMIC,
					  const D3DPOOL& thePool = D3DPOOL_DEFAULT )
		{
			mpIB = 0;
			mPosition = 0;

			mbLocked = false;
			mbFlush = true;

			mCount = theCount;

			mUsage = theUsage;

			HRESULT hr = pD3D->CreateIndexBuffer( mCount * sizeof( unsigned short ), mUsage, D3DFMT_INDEX16, thePool, &mpIB  );
			assert( ( hr == D3D_OK ) && ( mpIB ) );
		}

		IDirect3DIndexBuffer8* GetInterface() const { return mpIB; }

		unsigned int GetPosition() const { return mPosition; }

		void FlushAtFrameStart() { mbFlush = true; }

		unsigned short* Lock( const unsigned int& theSize, unsigned short& theFirstIndex )
		{
			theFirstIndex = 0;
			unsigned char* pData = 0;

			if ( mpIB )
			{
				DWORD dwFlags = LOCKFLAGS_STATIC;

				if ( mbFlush || ( ( theSize + mPosition ) >= mCount ) )
				{
					mbFlush = false;
					mPosition = 0;

					if ( mUsage & D3DUSAGE_DYNAMIC )
					{
						dwFlags = LOCKFLAGS_FLUSH;
					}
				}
				else
				{
					if (theSize > mCount)
					{
						assert( theSize <= mCount );
						return NULL;
					}
					if ( mUsage & D3DUSAGE_DYNAMIC )
					{
						dwFlags = LOCKFLAGS_APPEND;
					}
				}

				HRESULT hr = mpIB->Lock( mPosition * sizeof( unsigned short ), 
										 theSize * sizeof( unsigned short ), 
										 &( pData ), 
										 dwFlags );

				assert( hr == D3D_OK );

				if ( hr == D3D_OK )
				{
					assert(pData);
					theFirstIndex = mPosition;
					mPosition += theSize;
					mbLocked = true;
				}
			}
			return reinterpret_cast< unsigned short* >( pData );
		}

		inline void Unlock()
		{
			if ( mbLocked )
			{
				HRESULT hr = mpIB->Unlock();				
				mbLocked = false;
				assert( hr == D3D_OK );
			}
		}


		int Release()
		{
			int theRef = 0;

			if ( mpIB ) 
			{
				theRef = mpIB->Release(), mpIB = 0;
			}
			return theRef;
		}
		~IndexBuffer16()
		{
			Unlock();
			Release();
		}
	
};


class IndexBuffer32 
{
	private :

		IDirect3DIndexBuffer8* mpIB;
		unsigned int mCount;
		unsigned int mPosition;
		bool		 mbLocked;
		unsigned int mUsage;

		bool		 mbFlush;

		enum LockFlags
		{
			LOCKFLAGS_STATIC =  D3DLOCK_NOSYSLOCK,
			LOCKFLAGS_FLUSH =  D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
			LOCKFLAGS_APPEND = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
		};

	public :

		IndexBuffer32::IndexBuffer32( IDirect3DDevice8* pD3D, 
					  const unsigned int& theCount, 
					  const DWORD& theUsage = D3DUSAGE_DONOTCLIP | D3DUSAGE_WRITEONLY | D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DYNAMIC,
					  const D3DPOOL& thePool = D3DPOOL_DEFAULT )
		{
			mpIB = 0;
			mPosition = 0;

			mbLocked = false;
			mbFlush = true;

			mCount = theCount;

			mUsage = theUsage;

			HRESULT hr = pD3D->CreateIndexBuffer( mCount * sizeof( unsigned int ), mUsage, D3DFMT_INDEX32, thePool, &mpIB  );
			assert( ( hr == D3D_OK ) && ( mpIB ) );
		}

		IDirect3DIndexBuffer8* GetInterface() const { return mpIB; }

		unsigned int GetPosition() const { return mPosition; }

		void FlushAtFrameStart() { mbFlush = true; }

		unsigned int* Lock( const unsigned int& theSize, unsigned int theFirstIndex )
		{
			theFirstIndex = 0;
			unsigned char* pData = 0;

			if ( mpIB )
			{
				DWORD dwFlags = LOCKFLAGS_STATIC;

				if ( mUsage & D3DUSAGE_DYNAMIC )
				{
					dwFlags = LOCKFLAGS_APPEND;
				}

				if ( mbFlush || ( ( theSize + mPosition ) >= mCount ) )
				{
					mbFlush = false;
					mPosition = 0;

					if ( mUsage & D3DUSAGE_DYNAMIC )
					{
						dwFlags = LOCKFLAGS_FLUSH;
					}
				}
				else
				{
					if (theSize > mCount)
					{
						assert( theSize <= mCount );
						return NULL;
					}
				}

				HRESULT hr = mpIB->Lock( mPosition * sizeof( unsigned int ), 
										 theSize * sizeof( unsigned int ), 
										 &( pData ), 
										 dwFlags );

				assert( hr == D3D_OK );

				if ( hr == D3D_OK )
				{
					assert(pData);
					theFirstIndex = mPosition;
					mPosition += theSize;
					mbLocked = true;
				}
			}
			return reinterpret_cast< unsigned int* >( pData );
		}

		inline void Unlock()
		{
			if ( mbLocked )
			{
				HRESULT hr = mpIB->Unlock();				
				mbLocked = false;
				assert( hr == D3D_OK );
			}
		}


		int Release()
		{
			int theRef = 0;

			if ( mpIB ) 
			{
				theRef = mpIB->Release(), mpIB = 0;
			}
			return theRef;

		}
		~IndexBuffer32()
		{
			Unlock();
			Release();
		}
	
};


#endif  _INDEXBUFFER_H_
