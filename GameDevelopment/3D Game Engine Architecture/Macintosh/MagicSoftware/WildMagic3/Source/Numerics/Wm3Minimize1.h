// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MINIMIZE1_H
#define WM3MINIMIZE1_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM Minimize1
{
public:
    typedef Real (*Function)(Real,void*);

    Minimize1 (Function oFunction, int iMaxLevel, int iMaxBracket,
        void* pvData = 0);

    int& MaxLevel ();
    int& MaxBracket ();
    void*& UserData ();

    void GetMinimum (Real fT0, Real fT1, Real fTInitial, Real& rfTMin,
        Real& rfFMin);

private:
    Function m_oFunction;
    int m_iMaxLevel, m_iMaxBracket;
    Real m_fTMin, m_fFMin;
    void* m_pvData;

    void GetMinimum (Real fT0, Real fF0, Real fT1, Real fF1, int iLevel);

    void GetMinimum (Real fT0, Real fF0, Real fTm, Real fFm, Real fT1,
        Real fF1, int iLevel);

    void GetBracketedMinimum (Real fT0, Real fF0, Real fTm,
        Real fFm, Real fT1, Real fF1, int iLevel);
};

typedef Minimize1<float> Minimize1f;
typedef Minimize1<double> Minimize1d;

}

#endif
