// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef EXTRACTLEVELCURVES_H
#define EXTRACTLEVELCURVES_H

#include "Wm3ConsoleApplication.h"
using namespace Wm3;

class ExtractLevelCurves : public ConsoleApplication
{
    WM3_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    enum { MAGNIFY = 8, SIZE = 32, XPOS = 100, YPOS = 100 };

    bool Extract ();

    static void DrawPixel (int iX, int iY);
    static ImageRGB82D ms_kColor;
    static unsigned int ms_uiColor;
    static float ms_fMultiply;
};

WM3_REGISTER_INITIALIZE(ExtractLevelCurves);

#endif
