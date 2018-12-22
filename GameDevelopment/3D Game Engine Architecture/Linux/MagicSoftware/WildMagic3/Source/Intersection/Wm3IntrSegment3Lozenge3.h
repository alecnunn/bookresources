// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSEGMENT3LOZENGE3_H
#define WM3INTRSEGMENT3LOZENGE3_H

#include "Wm3Intersector.h"
#include "Wm3Segment3.h"
#include "Wm3Lozenge3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment3Lozenge3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Lozenge3 (const Segment3<Real>& rkSegment,
        const Lozenge3<Real>& rkLozenge);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Lozenge3<Real>& GetLozenge () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Segment3<Real>& m_rkSegment;
    const Lozenge3<Real>& m_rkLozenge;
};

typedef IntrSegment3Lozenge3<float> IntrSegment3Lozenge3f;
typedef IntrSegment3Lozenge3<double> IntrSegment3Lozenge3d;

}

#endif
