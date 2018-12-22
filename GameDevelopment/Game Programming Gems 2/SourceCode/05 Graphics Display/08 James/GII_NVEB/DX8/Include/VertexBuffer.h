/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include <d3d8.h>
#include <d3d8types.h>
#include <assert.h>

interface IDirect3DVertexBuffer8;
interface IDirect3DDevice8;

template < class VertexType >
class VertexBuffer 
{
	private :

		IDirect3DVertexBuffer8* mpVB;

		unsigned int mCount;
		unsigned int mPosition;
		unsigned int mUsage;

		bool		 mbLocked;
		bool		 mbFlush;

		enum LockFlags
		{
			FLUSH_LOCK_FLAGS =  D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
			APPEND_LOCK_FLAGS = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
		};
	public :

		VertexBuffer( IDirect3DDevice8* pD3D, 
					  const unsigned int& theCount, 
 					  const DWORD& theFVF, 
					  const DWORD& theUsage = D3DUSAGE_DONOTCLIP | D3DUSAGE_WRITEONLY | D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DYNAMIC,
					  const D3DPOOL& thePool = D3DPOOL_DEFAULT )
		{
			mpVB = 0;
			mPosition = 0;

			mbLocked = false;
			mbFlush = true;

			mCount = theCount;
			mUsage = theUsage;

			HRESULT hr = pD3D->CreateVertexBuffer( mCount * sizeof( VertexType ), mUsage, theFVF, thePool, &mpVB  );
			assert( ( hr == D3D_OK ) && ( mpVB ) );
		}

		IDirect3DVertexBuffer8* GetInterface() const { return mpVB; }

		unsigned int GetPosition() const { return mPosition; }

		void FlushAtFrameStart() { mbFlush = true; }

		inline void Restore();

		VertexType* Lock( const unsigned int& theVertexCount, unsigned int& theFirstVertex )
		{
			theFirstVertex = 0;

			unsigned char* pData = 0;

			assert( mpVB != 0 );

			if ( mpVB )
			{
				DWORD dwFlags = D3DLOCK_NOSYSLOCK;

				if ( mbFlush || ( theVertexCount + mPosition ) >= mCount ) 
				{
					mbFlush = false;
					if ( mUsage & D3DUSAGE_DYNAMIC )
					{
						dwFlags = FLUSH_LOCK_FLAGS;
						mPosition = 0;
					}
				}
				else
				{
					assert( theVertexCount <= mCount );
					if ( mUsage & D3DUSAGE_DYNAMIC )
					{
						dwFlags = APPEND_LOCK_FLAGS;
					}
				}

				HRESULT hr = mpVB->Lock( mPosition * sizeof( VertexType ), 
										 theVertexCount * sizeof( VertexType ), 
										 &pData, 
										 dwFlags );

				assert( hr == D3D_OK );

				if ( hr == D3D_OK )
				{
					assert( pData != 0 );
					mbLocked = true;
				}
			}

			theFirstVertex = mPosition;

			mPosition += theVertexCount;

			return reinterpret_cast< VertexType* >( pData );
		}

		void Unlock()
		{
			if ( mbLocked )
			{
				HRESULT hr = mpVB->Unlock();				
				mbLocked = false;
				assert( hr == D3D_OK );
			}
		}

		void Release()
		{
			if ( mpVB )
			{
				mpVB->Release(), mpVB = 0;
			}
		}
		~VertexBuffer()
		{
			Unlock();
			Release();
		}
	
};


#endif  _VERTEXBUFFER_H_
