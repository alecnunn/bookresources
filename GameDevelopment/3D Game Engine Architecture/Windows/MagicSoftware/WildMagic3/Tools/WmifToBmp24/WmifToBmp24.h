// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WMIFTOBMP24_H
#define WMIFTOBMP24_H

#include "Wm3ConsoleApplication.h"
using namespace Wm3;

#include <fstream>

class WmifToBmp24 : public ConsoleApplication
{
    WM3_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    void Usage ();
    void SaveRGBImage (std::ofstream& rkRGBStr, const Image* pkImage);
    void SaveRGBAImage (std::ofstream& rkRGBStr, std::ofstream& rkAStr,
        const Image* pkImage);
};

WM3_REGISTER_INITIALIZE(WmifToBmp24);

#endif
