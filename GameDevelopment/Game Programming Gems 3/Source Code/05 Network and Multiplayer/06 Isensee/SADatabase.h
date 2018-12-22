///////////////////////////////////////////////////////////////////////////////
//
//  SADatabase.h
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

#if !defined( SECURE_SOCK_SA_DATABASE_H )
#define SECURE_SOCK_SA_DATABASE_H

#include "Common.h"
#include "SecurityAssociation.h"
#include <map>

namespace SecureSock
{

///////////////////////////////////////////////////////////////////////////////
//
// SADatabase

class SADatabase
{
public:

    typedef std::map < SpiType, SecurityAssociation > SaList;
    typedef std::pair< SpiType, SecurityAssociation > SaPair;

public:

    SADatabase();
    ~SADatabase();

    void Insert( SpiType nSPIndex, const SecurityAssociation& sa );
    void Delete( SpiType nSPIndex );

    bool Find( SpiType nSPIndex ) const;
    const SecurityAssociation& GetSA( SpiType nSPIndex ) const;
    SecurityAssociation& GetSA( SpiType nSPIndex );

    SpiType GenNewSPI() const;

private:

    // Disabled
    SADatabase( const SADatabase& );
    SADatabase& operator=( const SADatabase& );

private:

    SaList mSaList;

    // Last accessed SA is cached for speed
    mutable SpiType mLastIndex;
    mutable SecurityAssociation mLastSa;

};

} // end namespace SecureSock

#endif // SECURE_SOCK_SA_DATABASE_H

///////////////////////////////////////////////////////////////////////////////
