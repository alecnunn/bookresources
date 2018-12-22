// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTEGRATE1_H
#define WM3INTEGRATE1_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM Integrate1
{
public:
    // last parameter is for user-defined data
    typedef Real (*Function)(Real,void*);

    // Romberg integration
    static Real RombergIntegral (int iOrder, Real fA, Real fB, Function oF,
        void* pvUserData = 0);

    // Gaussian quadrature
    static Real GaussianQuadrature (Real fA, Real fB, Function oF,
        void* pvUserData = 0);
};

typedef Integrate1<float> Integrate1f;
typedef Integrate1<double> Integrate1d;

}

#endif
