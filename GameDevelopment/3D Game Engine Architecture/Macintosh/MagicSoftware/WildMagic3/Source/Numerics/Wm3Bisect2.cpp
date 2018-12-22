// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Bisect2.h"
#include "Wm3Math.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Bisect2<Real>::Bisect2 (Function oF, Function oG, int iMaxLevel,
    Real fTolerance)
{
    m_oF = oF;
    m_oG = oG;
    m_iMaxLevel = iMaxLevel;
    m_iLevel = 0;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect2<Real>::ZeroTest (Real fX, Real fY, Real& rfF, Real& rfG,
    Real& rfXRoot, Real& rfYRoot)
{
    rfF = m_oF(fX,fY);
    rfG = m_oG(fX,fY);

    if ( Math<Real>::FAbs(rfF) <= m_fTolerance
    &&   Math<Real>::FAbs(rfG) <= m_fTolerance )
    {
        rfXRoot = fX;
        rfYRoot = fY;
        m_iLevel--;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
typename Bisect2<Real>::BisectNode* Bisect2<Real>::AddNode (Real fX, Real fY,
    Real fF, Real fG)
{
    BisectNode* pkTemp = new BisectNode;
    pkTemp->m_fX = fX;
    pkTemp->m_fY = fY;
    pkTemp->m_fF = fF;
    pkTemp->m_fG = fG;
    return pkTemp;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect2<Real>::Bisect (Real fX0, Real fY0, Real fX1, Real fY1,
    Real& rfXRoot, Real& rfYRoot)
{
    // test four corner values
    if ( ZeroTest(fX0,fY0,m_fF00,m_fG00,rfXRoot,rfYRoot) )
        return true;

    if ( ZeroTest(fX1,fY0,m_fF10,m_fG10,rfXRoot,rfYRoot) )
        return true;

    if ( ZeroTest(fX0,fY1,m_fF01,m_fG01,rfXRoot,rfYRoot) )
        return true;

    if ( ZeroTest(fX1,fY1,m_fF11,m_fG11,rfXRoot,rfYRoot) )
        return true;

    // build initial quad

    // add pkN00
    m_pkGraph = new BisectNode;
    m_pkGraph->m_fX = fX0;
    m_pkGraph->m_fY = fY0;
    m_pkGraph->m_fF = m_fF00;
    m_pkGraph->m_fG = m_fG00;

    // add pkN10
    BisectNode* pkTemp = AddNode(fX1,fY0,m_fF10,m_fG10);
    pkTemp->m_pkXNext = 0;
    m_pkGraph->m_pkXNext = pkTemp;

    // add pkN01
    pkTemp = AddNode(fX0,fY1,m_fF01,m_fG01);
    pkTemp->m_pkYNext = 0;
    m_pkGraph->m_pkYNext = pkTemp;

    // add pkN11
    pkTemp = AddNode(fX1,fY1,m_fF11,m_fG11);
    pkTemp->m_pkXNext = 0;
    pkTemp->m_pkYNext = 0;
    m_pkGraph->m_pkXNext->m_pkYNext = pkTemp;
    m_pkGraph->m_pkYNext->m_pkXNext = pkTemp;

    m_iLevel = 0;
    bool bResult = BisectRecurse(m_pkGraph);
    if ( bResult )
    {
        rfXRoot = m_fXRoot;
        rfYRoot = m_fYRoot;
    }

    // remove remaining quad from m_pkGraph
    delete m_pkGraph->m_pkXNext->m_pkYNext;
    delete m_pkGraph->m_pkXNext;
    delete m_pkGraph->m_pkYNext;
    delete m_pkGraph;

    return bResult;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect2<Real>::BisectRecurse (BisectNode* pkN00)
{
    if ( ++m_iLevel == m_iMaxLevel )
    {
        m_iLevel--;
        return false;
    }

    BisectNode* pkN10 = pkN00->m_pkXNext;
    BisectNode* pkN11 = pkN10->m_pkYNext;
    BisectNode* pkN01 = pkN00->m_pkYNext;

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN00->m_fF) +
        Math<Real>::Sign(pkN01->m_fF) +
        Math<Real>::Sign(pkN10->m_fF) +
        Math<Real>::Sign(pkN11->m_fF));

    if ( abs(m_iNetSign) == 4 )
    {
        // F has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN00->m_fG) +
        Math<Real>::Sign(pkN01->m_fG) +
        Math<Real>::Sign(pkN10->m_fG) +
        Math<Real>::Sign(pkN11->m_fG));

    if ( abs(m_iNetSign) == 4 )
    {
        // G has same sign at corners
        m_iLevel--;
        return false;
    }

    // bisect the quad
    m_fX0 = pkN00->m_fX;
    m_fY0 = pkN00->m_fY;
    m_fX1 = pkN11->m_fX;
    m_fY1 = pkN11->m_fY;
    m_fXm = ((Real)0.5)*(m_fX0+m_fX1);
    m_fYm = ((Real)0.5)*(m_fY0+m_fY1);

    // r edge 10,11
    if ( ZeroTest(m_fX1,m_fYm,m_fF1m,m_fG1m,m_fXRoot,m_fYRoot) )
        return true;
    
    // b edge 01,11
    if ( ZeroTest(m_fXm,m_fY1,m_fFm1,m_fGm1,m_fXRoot,m_fYRoot) )
        return true;

    // t edge 00,10
    if ( ZeroTest(m_fXm,m_fY0,m_fFm0,m_fGm0,m_fXRoot,m_fYRoot) )
        return true;

    // l edge 00,01
    if ( ZeroTest(m_fX0,m_fYm,m_fF0m,m_fG0m,m_fXRoot,m_fYRoot) )
        return true;

    // center
    if ( ZeroTest(m_fXm,m_fYm,m_fFmm,m_fGmm,m_fXRoot,m_fYRoot) )
        return true;

    // right bisector
    BisectNode* pkTemp = AddNode(m_fX1,m_fYm,m_fF1m,m_fG1m);
    pkTemp->m_pkXNext = 0;
    pkTemp->m_pkYNext = pkN11;
    pkN10->m_pkYNext = pkTemp;

    // bottom bisector
    pkTemp = AddNode(m_fXm,m_fY1,m_fFm1,m_fGm1);
    pkTemp->m_pkXNext = pkN11;
    pkTemp->m_pkYNext = 0;
    pkN01->m_pkXNext = pkTemp;

    // top bisector
    pkTemp = AddNode(m_fXm,m_fY0,m_fFm0,m_fGm0);
    pkTemp->m_pkXNext = pkN10;
    pkN00->m_pkXNext = pkTemp;

    // left bisector
    pkTemp = AddNode(m_fX0,m_fYm,m_fF0m,m_fG0m);
    pkTemp->m_pkYNext = pkN01;
    pkN00->m_pkYNext = pkTemp;

    // middle bisector
    pkTemp = AddNode(m_fXm,m_fYm,m_fFmm,m_fGmm);
    pkTemp->m_pkXNext = pkN10->m_pkYNext;
    pkTemp->m_pkYNext = pkN01->m_pkXNext;
    pkN00->m_pkXNext->m_pkYNext = pkTemp;
    pkN00->m_pkYNext->m_pkXNext = pkTemp;

    // Search the subquads for roots
    bool bResult =
        BisectRecurse(pkN00) ||
        BisectRecurse(pkN00->m_pkXNext) ||
        BisectRecurse(pkN00->m_pkYNext) ||
        BisectRecurse(pkN00->m_pkXNext->m_pkYNext);

    // entire subquad check failed, remove the nodes that were added

    // center
    delete pkN00->m_pkXNext->m_pkYNext;

    // edges
    delete pkN00->m_pkXNext;
    pkN00->m_pkXNext = pkN10;
    delete pkN00->m_pkYNext;
    pkN00->m_pkYNext = pkN01;
    delete pkN01->m_pkXNext;
    pkN01->m_pkXNext = pkN11;
    delete pkN10->m_pkYNext;
    pkN10->m_pkYNext = pkN11;

    m_iLevel--;
    return bResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class Bisect2<float>;

// double
template WM3_ITEM
class Bisect2<double>;
//----------------------------------------------------------------------------
}
