// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BISECT1_H
#define WM3BISECT1_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM Bisect1
{
public:
    typedef Real (*Function)(Real);

    Bisect1 (Function oF, int iMaxLevel, Real fTolerance);

    bool Bisect (Real fX0, Real fX1, Real& rfRoot);

private:
    // input data and functions
    Function m_oF;
    int m_iMaxLevel;
    Real m_fTolerance;
};

typedef Bisect1<float> Bisect1f;
typedef Bisect1<double> Bisect1d;

}

#endif
