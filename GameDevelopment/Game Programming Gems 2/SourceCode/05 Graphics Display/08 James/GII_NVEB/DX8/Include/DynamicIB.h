/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _DYNAMICIB_H_
#define _DYNAMICIB_H_

#include <d3d8.h>
#include <assert.h>

/////////////////////////////
// D. Sim Dietrich Jr.
// sim.dietrich@nvidia.com
//////////////////////

template < class IndexType >
class DynamicIB 
{
	private :

		LPDIRECT3DINDEXBUFFER8 mpIB;

		unsigned int mIndexCount;
		unsigned int mPosition;

		bool		 mbLocked;
		bool		 mbFlush;

		D3DINDEXBUFFER_DESC mDesc;

		enum LOCK_FLAGS
		{
			LOCKFLAGS_FLUSH  = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
			LOCKFLAGS_APPEND = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
		};

	public :

		DynamicIB( const LPDIRECT3DDEVICE8 pD3D, const unsigned int& theIndexCount )
		{
			mpIB = 0;
			mPosition = 0;

			mbFlush = true;

			mbLocked = false;

			mIndexCount = theIndexCount;
		
			memset( &mDesc, 0x00, sizeof( mDesc ) );
			if ( sizeof( IndexType ) == 2 )
			{
				mDesc.Format = D3DFMT_INDEX16;
			}
			else
			{
				assert( sizeof( IndexType ) == 4 );
				mDesc.Format = D3DFMT_INDEX32;
			}

			mDesc.Size = sizeof( IndexType ) * theIndexCount;
			mDesc.Type = D3DRTYPE_INDEXBUFFER;
			mDesc.Pool = D3DPOOL_DEFAULT;

			mDesc.Usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;

			HRESULT hr = pD3D->CreateIndexBuffer( mIndexCount * sizeof( IndexType ),
				                                   mDesc.Usage,
												   mDesc.Format,
												   mDesc.Pool,
												   &mpIB);

			if ( mpIB )
			{
				D3DINDEXBUFFER_DESC aDesc;
				mpIB->GetDesc( &aDesc );
				assert( memcmp( &aDesc, &mDesc, sizeof( mDesc ) ) == 0 );
			}

			assert( ( hr == D3D_OK ) && ( mpIB ) );
		}

		LPDIRECT3DINDEXBUFFER8 GetInterface() const { return mpIB; }

		// Use at beginning of frame to force a flush of IB contents on first draw
		void FlushAtFrameStart() { mbFlush = true; }


		IndexType* Lock( const unsigned int& theLockCount, unsigned int& theStartIndex )
		{
			theStartIndex = 0;
			IndexType* pLockedData = 0;

			// Ensure there is enough space in the IB for this data
			if ( theLockCount > mIndexCount ) { assert( false ); return 0; }

			if ( mpIB )
			{
				DWORD dwFlags = LOCKFLAGS_APPEND;

				// If either user forced us to flush,
				//  or there is not enough space for the Index data,
				//  then flush the buffer contents
				//
				if ( mbFlush || ( ( theLockCount + mPosition ) > mIndexCount ) )
				{
					mbFlush = false;
					mPosition = 0;
					dwFlags = LOCKFLAGS_FLUSH;
				}

				DWORD dwSize = 0;
				HRESULT hr = mpIB->Lock( mPosition * sizeof( IndexType ), 
										 theLockCount * sizeof( IndexType ), 
										 reinterpret_cast< BYTE** >( &pLockedData ), 
										 dwFlags );

				assert( hr == D3D_OK );
				if ( hr == D3D_OK )
				{
					assert( pLockedData != 0 );
					mbLocked = true;
					theStartIndex = mPosition;
					mPosition += theLockCount;
				}
			}

			return pLockedData;
		}

		void Unlock()
		{
			if ( ( mbLocked ) && ( mpIB ) )
			{
				HRESULT hr = mpIB->Unlock();				
				assert( hr == D3D_OK );
				mbLocked = false;
			}
		}

		~DynamicIB()
		{
			Unlock();
			if ( mpIB )
			{
				mpIB->Release();
			}
		}
	
};

typedef DynamicIB< unsigned short > DynamicIB16;
typedef DynamicIB< unsigned int   > DynamicIB32;

#endif  _DYNAMICIB_H_
