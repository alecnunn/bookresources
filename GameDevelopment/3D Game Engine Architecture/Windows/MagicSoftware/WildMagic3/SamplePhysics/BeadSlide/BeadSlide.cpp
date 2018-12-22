// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "BeadSlide.h"
#include "PhysicsModule.h"
using namespace Wm3;

WM3_CONSOLE_APPLICATION(BeadSlide);

//----------------------------------------------------------------------------
BeadSlide::BeadSlide ()
{
}
//----------------------------------------------------------------------------
void BeadSlide::Simulation ()
{
    // set up the physics module
    PhysicsModule kModule;
    kModule.Gravity = 1.0;
    kModule.Mass = 0.1;

    double dTime = 0.0;
    double dDeltaTime = 0.001;
    double dQ = 1.0;
    double dQDot = 0.0;
    kModule.Initialize(dTime,dDeltaTime,dQ,dQDot);

    // run the simulation
    std::ofstream kOStr("simulation.txt");
    kOStr << "time  q  qder  position" << std::endl;
    const int iMax = 2500;
    for (int i = 0; i < iMax; i++)
    {
        double dX = dQ, dY = dQ*dQ, dZ = dQ*dY;

        char acMsg[512];
        sprintf(acMsg,"%5.3lf %+16.8lf %+16.8lf %+8.4lf %+8.4lf %+8.4lf",
            dTime,dQ,dQDot,dX,dY,dZ);
        kOStr << acMsg << std::endl;

        kModule.Update();

        dTime = kModule.GetTime();
        dQ = kModule.GetQ();
        dQDot = kModule.GetQDot();
    }
    kOStr.close();
}
//----------------------------------------------------------------------------
int BeadSlide::Main (int, char**)
{
    Simulation();
    return 0;
}
//----------------------------------------------------------------------------
