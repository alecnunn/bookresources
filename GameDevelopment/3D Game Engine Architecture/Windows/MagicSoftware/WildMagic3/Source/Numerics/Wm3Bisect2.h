// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BISECT2_H
#define WM3BISECT2_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM Bisect2
{
public:
    typedef Real (*Function)(Real,Real);

    Bisect2 (Function oF, Function oG, int iMaxLevel, Real fTolerance);

    bool Bisect (Real fX0, Real fY0, Real fX1, Real fY1, Real& rfXRoot,
        Real& rfYRoot);

private:
    class WM3_ITEM BisectNode
    {
    public:
        Real m_fX, m_fY, m_fF, m_fG;
        BisectNode* m_pkXNext;
        BisectNode* m_pkYNext;
    };

    bool ZeroTest (Real fX, Real fY, Real& rfF, Real& rfG, Real& rfXRoot,
        Real& rfYRoot);

    static BisectNode* AddNode (Real fX, Real fY, Real fF, Real fG);

    // input data and functions
    Function m_oF, m_oG;
    int m_iLevel, m_iMaxLevel;
    Real m_fTolerance;
    Real m_fXRoot, m_fYRoot;

    // fixed storage to avoid stack depletion during recursion
    Real m_fX0, m_fXm, m_fX1, m_fY0, m_fYm, m_fY1;
    Real m_fF00, m_fF10, m_fF01, m_fF11, m_fF0m, m_fF1m, m_fFm0, m_fFm1;
    Real m_fFmm, m_fG00, m_fG10, m_fG01, m_fG11, m_fG0m, m_fG1m, m_fGm0;
    Real m_fGm1, m_fGmm;
    int m_iNetSign;

    // the graph and recursion routine for building it
    BisectNode* m_pkGraph;
    bool BisectRecurse (BisectNode* pkN00);
};

typedef Bisect2<float> Bisect2f;
typedef Bisect2<double> Bisect2d;

}

#endif
