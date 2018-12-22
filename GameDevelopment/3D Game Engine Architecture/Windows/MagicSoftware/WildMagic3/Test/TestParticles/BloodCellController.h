// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BLOODCELLCONTROLLER_H
#define BLOODCELLCONTROLLER_H

#include "Wm3ParticleController.h"

namespace Wm3
{

class BloodCellController : public ParticleController
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    BloodCellController ();
protected:
    virtual void UpdatePointMotion (float fCtrlTime);
};

WM3_REGISTER_STREAM(BloodCellController);

}

#endif
