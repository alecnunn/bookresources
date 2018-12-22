// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3CollisionGroup.h"
#include "Wm3CollisionRecord.h"
using namespace Wm3;

//----------------------------------------------------------------------------
CollisionGroup::CollisionGroup ()
    :
    m_kRecord(8,8)
{
}
//----------------------------------------------------------------------------
CollisionGroup::~CollisionGroup ()
{
    for (int i = 0; i < m_kRecord.GetQuantity(); i++)
        delete m_kRecord[i];
}
//----------------------------------------------------------------------------
bool CollisionGroup::Add (CollisionRecord* pkRecord)
{
    for (int i = 0; i < m_kRecord.GetQuantity(); i++)
    {
        if ( pkRecord == m_kRecord[i] )
            return false;
    }

    m_kRecord.Append(pkRecord);
    return true;
}
//----------------------------------------------------------------------------
bool CollisionGroup::Remove (CollisionRecord* pkRecord)
{
    for (int i = 0; i < m_kRecord.GetQuantity(); i++)
    {
        if ( pkRecord == m_kRecord[i] )
        {
            delete pkRecord;
            m_kRecord.Remove(i);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void CollisionGroup::TestIntersection ()
{
    // objects are assumed to be stationary, compare all pairs
    for (int i0 = 0; i0 < m_kRecord.GetQuantity(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < m_kRecord.GetQuantity(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            pkRecord0->TestIntersection(*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
void CollisionGroup::FindIntersection ()
{
    // objects are assumed to be stationary, compare all pairs
    for (int i0 = 0; i0 < m_kRecord.GetQuantity(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < m_kRecord.GetQuantity(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            pkRecord0->FindIntersection(*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
void CollisionGroup::TestIntersection (float fTMax)
{
    // objects are assumed to be moving, compare all pairs
    for (int i0 = 0; i0 < m_kRecord.GetQuantity(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < m_kRecord.GetQuantity(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            if ( pkRecord0->GetVelocity() || pkRecord1->GetVelocity() )
                pkRecord0->TestIntersection(fTMax,*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
void CollisionGroup::FindIntersection (float fTMax)
{
    // objects are assumed to be moving, compare all pairs
    for (int i0 = 0; i0 < m_kRecord.GetQuantity(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < m_kRecord.GetQuantity(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            if ( pkRecord0->GetVelocity() || pkRecord1->GetVelocity() )
                pkRecord0->FindIntersection(fTMax,*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
