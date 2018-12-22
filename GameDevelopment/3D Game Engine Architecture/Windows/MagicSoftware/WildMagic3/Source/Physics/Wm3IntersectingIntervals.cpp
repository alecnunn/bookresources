// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntersectingIntervals.h"
#include <algorithm>

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntersectingIntervals<Real>::IntersectingIntervals (
    std::vector<Vector2<Real> >& rkInterval)
    :
    m_rkInterval(rkInterval)
{
    Initialize();
}
//----------------------------------------------------------------------------
template <class Real>
IntersectingIntervals<Real>::~IntersectingIntervals ()
{
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::Initialize ()
{
    // get the interval end points
    int iISize = (int)m_rkInterval.size(), iESize = 2*iISize;
    m_kEndPoint.resize(iESize);
    int i, j;
    for (i = 0, j = 0; i < iISize; i++)
    {
        EndPoint& rkEMin = m_kEndPoint[j++];
        rkEMin.Type = 0;
        rkEMin.Value = m_rkInterval[i][0];
        rkEMin.Index = i;

        EndPoint& rkEMax = m_kEndPoint[j++];
        rkEMax.Type = 1;
        rkEMax.Value = m_rkInterval[i][1];
        rkEMax.Index = i;
    }

    // sort the interval end points
    sort(m_kEndPoint.begin(),m_kEndPoint.end());

    // create the interval-to-endpoint lookup table
    m_kLookup.resize(iESize);
    for (j = 0; j < iESize; j++)
    {
        EndPoint& rkE = m_kEndPoint[j];
        m_kLookup[2*rkE.Index + rkE.Type] = j;
    }

    // active set of intervals (stored by index in array)
    std::set<int> kActive;

    // set of overlapping intervals (stored by pairs of indices in array)
    m_kOverlap.clear();

    // sweep through the end points to determine overlapping intervals
    for (i = 0; i < iESize; i++)
    {
        EndPoint& rkEnd = m_kEndPoint[i];
        int iIndex = rkEnd.Index;
        if ( rkEnd.Type == 0 )  // an interval 'begin' value
        {
            std::set<int>::iterator pkIter = kActive.begin();
            for (/**/; pkIter != kActive.end(); pkIter++)
            {
                int iAIndex = *pkIter;
                if ( iAIndex < iIndex )
                    m_kOverlap.insert(EdgeKey(iAIndex,iIndex));
                else
                    m_kOverlap.insert(EdgeKey(iIndex,iAIndex));
            }
            kActive.insert(iIndex);
        }
        else  // an interval 'end' value
        {
            kActive.erase(iIndex);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::SetInterval (int i, Real fMin, Real fMax)
{
    assert( 0 <= i && i < (int)m_rkInterval.size() );
    m_rkInterval[i][0] = fMin;
    m_rkInterval[i][1] = fMax;
    m_kEndPoint[m_kLookup[2*i]].Value = fMin;
    m_kEndPoint[m_kLookup[2*i+1]].Value = fMax;
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::GetInterval (int i, Real& rfMin,
    Real& rfMax) const
{
    assert( 0 <= i && i < (int)m_rkInterval.size() );
    rfMin = m_rkInterval[i][0];
    rfMax = m_rkInterval[i][1];
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::Update ()
{
    // Apply an insertion sort.  Under the assumption that the intervals
    // have not changed much since the last call, the end points are nearly
    // sorted.  The insertion sort should be very fast in this case.
    int iESize = (int)m_kEndPoint.size();
    for (int j = 1; j < iESize; j++)
    {
        EndPoint kKey = m_kEndPoint[j];
        int i = j - 1;
        while ( i >= 0 && kKey < m_kEndPoint[i] )
        {
            EndPoint kE0 = m_kEndPoint[i];
            EndPoint kE1 = m_kEndPoint[i+1];

            // update the overlap status
            if ( kE0.Type == 0 )
            {
                if ( kE1.Type == 1 )
                {
                    // The 'b' of interval E0.Index was smaller than the 'e'
                    // of interval E1.Index, and the intervals *might have
                    // been* overlapping.  Now 'b' and 'e' are swapped, and
                    // the intervals cannot overlap.  Remove the pair from
                    // the overlap set.  The removal operation needs to find
                    // the pair and erase it if it exists.  Finding the pair
                    // is the expensive part of the operation, so there is no
                    // real time savings in testing for existence first, then
                    // deleting if it does.
                    if ( kE0.Index < kE1.Index )
                        m_kOverlap.erase(EdgeKey(kE0.Index,kE1.Index));
                    else
                        m_kOverlap.erase(EdgeKey(kE1.Index,kE0.Index));
                }
            }
            else
            {
                if ( kE1.Type == 0 )
                {
                    // The 'b' of interval E0.index was larger than the 'e'
                    // of interval E1.index, and the intervals were not
                    // overlapping.  Now 'b' and 'e' are swapped, and the
                    // intervals *might be* overlapping.  We need to determine
                    // if this is so and insert only if they do overlap.
                    const Vector2<Real>& rkI0 = m_rkInterval[kE0.Index];
                    const Vector2<Real>& rkI1 = m_rkInterval[kE1.Index];
                    if ( rkI0[1] >= rkI1[0] )
                    {
                        if ( kE0.Index < kE1.Index )
                            m_kOverlap.insert(EdgeKey(kE0.Index,kE1.Index));
                        else
                            m_kOverlap.insert(EdgeKey(kE1.Index,kE0.Index));
                    }
                }
            }

            // reorder the items to maintain the sorted list
            m_kEndPoint[i] = kE1;
            m_kEndPoint[i+1] = kE0;
            m_kLookup[2*kE1.Index + kE1.Type] = i;
            m_kLookup[2*kE0.Index + kE0.Type] = i+1;
            i--;
        }
        m_kEndPoint[i+1] = kKey;
        m_kLookup[2*kKey.Index + kKey.Type] = i+1;
    }
}
//----------------------------------------------------------------------------
template <class Real>
const std::set<EdgeKey>& IntersectingIntervals<Real>::GetOverlap () const
{
    return m_kOverlap;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntersectingIntervals<float>;

// double
template WM3_ITEM
class IntersectingIntervals<double>;
//----------------------------------------------------------------------------
}
