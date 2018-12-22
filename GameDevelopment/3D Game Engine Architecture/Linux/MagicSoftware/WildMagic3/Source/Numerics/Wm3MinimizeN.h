// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MINIMIZEN_H
#define WM3MINIMIZEN_H

#include "Wm3Minimize1.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM MinimizeN
{
public:
    typedef Real (*Function)(const Real*,void*);

    MinimizeN (int iDimensions, Function oFunction, int iMaxLevel,
        int iMaxBracket, int iMaxIterations, void* pvData = 0);

    ~MinimizeN ();

    int& MaxLevel ();
    int& MaxBracket ();
    void*& UserData ();

    // find minimum on Cartesian-product domain
    void GetMinimum (const Real* afT0, const Real* afT1,
        const Real* afTInitial, Real* afTMin, Real& rfFMin);

private:
    int m_iDimensions;
    Function m_oFunction;
    int m_iMaxIterations;
    void* m_pvData;
    Minimize1<Real> m_kMinimizer;
    Real* m_afDirectionStorage;
    Real** m_aafDirection;
    Real* m_afDConj;
    Real* m_afDCurr;
    Real* m_afTSave;
    Real* m_afTCurr;
    Real m_fFCurr;
    Real* m_afLineArg;

    void ComputeDomain (const Real* afT0, const Real* afT1, Real& rfL0,
        Real& rfL1);

    static Real LineFunction (Real fT, void* pvData);
};

typedef MinimizeN<float> MinimizeNf;
typedef MinimizeN<double> MinimizeNd;

}

#endif
