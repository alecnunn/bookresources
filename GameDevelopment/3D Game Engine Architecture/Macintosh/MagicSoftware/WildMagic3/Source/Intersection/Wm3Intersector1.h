// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTERSECTOR1_H
#define WM3INTERSECTOR1_H

#include "Wm3Math.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM Intersector1
{
public:
    // A class for intersection of intervals [u0,u1] and [v0,v1].  The end
    // points must be ordered:  u0 <= u1 and v0 <= v1.  Values of MAX_REAL
    // and -MAX_REAL are allowed, and degenerate intervals are allowed:
    // u0 = u1 or v0 = v1.
    Intersector1 (Real fU0, Real fU1, Real fV0, Real fV1);
    Intersector1 (Real afU[2], Real afV[2]);
    virtual ~Intersector1 ();

    // object access
    Real GetU (int i) const;
    Real GetV (int i) const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // Dynamic intersection queries.  The Find query produces a set of first
    // contact.
    virtual bool Test (Real fTMax, Real fSpeedU, Real fSpeedV);
    virtual bool Find (Real fTMax, Real fSpeedU, Real fSpeedV);

    // The time at which two intervals are in first/last contact for the
    // dynamic intersection queries.
    Real GetFirstTime () const;
    Real GetLastTime () const;

    // information about the intersection set
    int GetQuantity () const;
    Real GetOverlap (int i) const;

protected:
    // the intervals to intersect
    Real m_afU[2], m_afV[2];

    // information about the intersection set
    Real m_fFirstTime, m_fLastTime;
    int m_iQuantity;
    Real m_afOverlap[2];
};

typedef Intersector1<float> Intersector1f;
typedef Intersector1<double> Intersector1d;

}

#endif
