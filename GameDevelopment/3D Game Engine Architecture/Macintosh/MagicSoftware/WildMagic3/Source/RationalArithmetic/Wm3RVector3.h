// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RVECTOR3_H
#define WM3RVECTOR3_H

#include "Wm3TRVector.h"

namespace Wm3
{

template <int ISIZE>
class WM3_ITEM RVector3 : public TRVector<3,ISIZE>
{
public:
    // construction
    RVector3 ();
    RVector3 (const RVector3& rkV);
    RVector3 (const TRVector<3,ISIZE>& rkV);
    RVector3 (const TRational<ISIZE>& rkX, const TRational<ISIZE>& rkY,
        const TRational<ISIZE>& rkZ);

    // member access
    TRational<ISIZE> X () const;
    TRational<ISIZE>& X ();
    TRational<ISIZE> Y () const;
    TRational<ISIZE>& Y ();
    TRational<ISIZE> Z () const;
    TRational<ISIZE>& Z ();

    // assignment
    RVector3& operator= (const RVector3& rkV);
    RVector3& operator= (const TRVector<3,ISIZE>& rkV);

    // returns Dot(this,V)
    TRational<ISIZE> Dot (const RVector3& rkV) const;

    // returns Cross(this,V)
    RVector3 Cross (const RVector3& rkV) const;

    // returns Dot(this,Cross(U,V))
    TRational<ISIZE> TripleScalar (const RVector3& rkU, const RVector3& rkV)
        const;
};

#include "Wm3RVector3.inl"

}

#endif
