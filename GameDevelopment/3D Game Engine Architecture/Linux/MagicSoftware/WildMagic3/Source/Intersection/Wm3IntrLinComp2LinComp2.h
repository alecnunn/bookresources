// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINCOMP2LINCOMP2_H
#define WM3INTRLINCOMP2LINCOMP2_H

#include "Wm3Intersector.h"
#include "Wm3Intersector1.h"
#include "Wm3LinComp2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLinComp2LinComp2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLinComp2LinComp2 (const LinComp2<Real>& rkComponent0,
        const LinComp2<Real>& rkComponent1);

    // object access
    const LinComp2<Real>& GetComponent0 () const;
    const LinComp2<Real>& GetComponent1 () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // Information about the intersection set.  GetIntersectionType() returns
    // IT_EMPTY, IT_POINT, IT_LINE, IT_RAY, or IT_SEGMENT.
    const LinComp2<Real>& GetIntersectionComponent() const;

protected:
    // The linear components are P0 + s0*D0 and P1 + s1*D1.  Determine if
    // the lines intersect in a single point, are the same line, or are
    // parallel and disjoint.  If you want to know the parameters s0 and s1,
    // pass a nonnull array to afS; it must have two elements.  If you want
    // to know P1-P0, pass a nonnull pointer to pkDiff.  If you want to know
    // (P1-P0)/|P1-P0|, pass a nonnull pointer to pkDiffN.  The return value
    // is one of the IT_* enumerations.  The afS values are valid only when
    // the return is IT_POINT.  The pkDiff value is valid always.  The pkDiffN
    // value is valid only when the return value is IT_EMPTY or IT_LINE.
    static int Classify (const LinComp2<Real>& rkComponent0,
        const LinComp2<Real>& rkComponent1, Real* afS = 0,
        Vector2<Real>* pkDiff = 0, Vector2<Real>* pkDiffN = 0);

    // the objects to intersect
    const LinComp2<Real>& m_rkComponent0;
    const LinComp2<Real>& m_rkComponent1;

    // information about the intersection set
    LinComp2<Real> m_kIntrComponent;
};

typedef IntrLinComp2LinComp2<float> IntrLinComp2LinComp2f;
typedef IntrLinComp2LinComp2<double> IntrLinComp2LinComp2d;

}

#endif
