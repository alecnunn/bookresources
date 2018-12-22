// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BMP24TOWMIF_H
#define BMP24TOWMIF_H

#include "Wm3ConsoleApplication.h"
using namespace Wm3;

class Bmp24ToWmif : public ConsoleApplication
{
    WM3_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    void Usage ();
    int GetImage (const char* acBMPName, int& riWidth, int& riHeight,
        unsigned char*& raucData);
};

WM3_REGISTER_INITIALIZE(Bmp24ToWmif);

#endif
