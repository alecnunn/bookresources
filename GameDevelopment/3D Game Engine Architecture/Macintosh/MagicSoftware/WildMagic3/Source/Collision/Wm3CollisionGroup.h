// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3COLLISIONGROUP_H
#define WM3COLLISIONGROUP_H

#include "Wm3System.h"

namespace Wm3
{

class CollisionRecord;

class WM3_ITEM CollisionGroup
{
public:
    CollisionGroup ();
    ~CollisionGroup ();

    // CollisionGroup assumes responsibility for deleting the collision
    // records, so the input records should be dynamically allocated.
    bool Add (CollisionRecord* pkRecord);
    bool Remove (CollisionRecord* pkRecord);

    // Intersection queries.  If two objects in the group collide, the
    // corresponding records process the information accordingly.

    // The objects are assumed to be stationary (velocities are ignored) and
    // all pairs of objects are compared.
    void TestIntersection ();
    void FindIntersection ();
    
    // The objects are assumed to be moving.  Objects are compared when at
    // least one of them has a velocity vector associated with it (that
    // vector is allowed to be the zero vector).
    void TestIntersection (float fTMax);
    void FindIntersection (float fTMax);

protected:
    TArray<CollisionRecord*> m_kRecord;
};

}

#endif
