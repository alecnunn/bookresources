///////////////////////////////////////////////////////////////////////////////
//
//  SADatabase.cpp
//
//  Security Association database
//
//  Copyright © 2002 Pete Isensee (PKIsensee@msn.com).
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

#include "SADatabase.h"

namespace SecureSock
{

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

SADatabase::SADatabase()
:
    mSaList(),
    mLastIndex( 0 ),
    mLastSa()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Dtor

SADatabase::~SADatabase()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Add a Security Association to the database

void SADatabase::Insert( SpiType nSPIndex, const SecurityAssociation& sa )
{
    assert( nSPIndex != 0 );
    mSaList.insert( SaPair( nSPIndex, sa ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Delete a Security Association from the database 

void SADatabase::Delete( SpiType nSPIndex )
{
    mSaList.erase( nSPIndex );

    // If the SA was cached, flush the cache
    if( nSPIndex == mLastIndex )
        mLastIndex = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Find a Security Association in the database

bool SADatabase::Find( SpiType nSPIndex ) const
{
    // If we cached the last access, we're done
    if( nSPIndex == mLastIndex )
        return true;

    SaList::const_iterator i = mSaList.find( nSPIndex );
    if( i != mSaList.end() )
    {
        // Cache this value
        mLastIndex = nSPIndex;
        mLastSa = i->second;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
// Find a Security Association in the database

const SecurityAssociation& SADatabase::GetSA( SpiType nSPIndex ) const
{
    // If we cached the last access, return it
    if( nSPIndex == mLastIndex )
        return mLastSa;

    SaList::const_iterator i = mSaList.find( nSPIndex );
    assert( i != mSaList.end() );

    // Cache this value
    mLastIndex = nSPIndex;
    mLastSa = i->second;
    return mLastSa;
}

///////////////////////////////////////////////////////////////////////////////
//
// Find a Security Association in the database

SecurityAssociation& SADatabase::GetSA( SpiType nSPIndex )
{
    // If we cached the last access, return it
    if( nSPIndex == mLastIndex )
        return mLastSa;

    SaList::const_iterator i = mSaList.find( nSPIndex );
    assert( i != mSaList.end() );

    // Cache this value
    mLastIndex = nSPIndex;
    mLastSa = i->second;
    return mLastSa;
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a new unique Security Parameters Index

SpiType SADatabase::GenNewSPI() const
{
    for( ;; )
    {
        // Generate a random number
        Buffer bufRand;
        Key::GenRandom( sizeof(SpiType), bufRand );
        SpiType nRand = *reinterpret_cast< const SpiType* >( bufRand.data() );

        // Search for the number in the database. If it's not there, use it.
        if( nRand != 0 && !Find( nRand ) )
            return nRand;
    }
}

} // end namespace SecureSock

///////////////////////////////////////////////////////////////////////////////
