// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef SIMPLEPENDULUM_H
#define SIMPLEPENDULUM_H

#include "Wm3ConsoleApplication.h"
using namespace Wm3;

class SimplePendulum : public ConsoleApplication
{
    WM3_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    enum { SIZE = 512 };

    static float* ExplicitEuler (float fX0, float fY0, float fH);
    static float* ImplicitEuler (float fX0, float fY0, float fH);
    static float* RungeKutta (float fX0, float fY0, float fH);
    static float* LeapFrog (float fX0, float fY0, float fH);
    void SolveMethod (float* (*oMethod)(float,float,float),
        const char* acOutIm, const char* acOutTxt);

    void Stiff1 ();
    float F0 (float fT, float fX, float fY);
    float F1 (float fT, float fX, float fY);
    void Stiff2True ();
    void Stiff2Approximate ();

    static void DrawPixel (int iX, int iY);
    static ImageRGB82D ms_kImage;
    static unsigned int ms_uiColor;
    static float ms_fK;
};

WM3_REGISTER_INITIALIZE(SimplePendulum);

#endif
