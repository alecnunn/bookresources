// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef RANDOMCONTROLLER_H
#define RANDOMCONTROLLER_H

#include "Wm3PointController.h"

namespace Wm3
{

class RandomController : public PointController
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    RandomController ();
protected:
    virtual void UpdatePointMotion (float fCtrlTime);
};

WM3_REGISTER_STREAM(RandomController);

}

#endif
