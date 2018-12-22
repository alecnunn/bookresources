// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "BinaryOperations.h"
using namespace Wm3;

WM3_CONSOLE_APPLICATION(BinaryOperations);

//----------------------------------------------------------------------------
void BinaryOperations::GetBoundaries (const Binary2D& rkImage)
{
    int* aiBoundaries = rkImage.GetBoundaries();
    if ( aiBoundaries )
    {
        ImageInt2D kOutput(rkImage.GetBound(0),rkImage.GetBound(1));
        int iIndex = 1;
        for (int iB = 0; iB < aiBoundaries[0]; iB++)
        {
            int iQuantity = aiBoundaries[iIndex++];
            for (int i = 0; i < iQuantity; i++)
                kOutput[aiBoundaries[iIndex++]] = 1;
        }

        kOutput.Save("Boundaries.im");
        delete[] aiBoundaries;
    }
}
//----------------------------------------------------------------------------
void BinaryOperations::GetComponents (const Binary2D& rkImage)
{
    int iQuantity;
    ImageInt2D kComponents(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetComponents(iQuantity,kComponents);
    kComponents.Save("Components.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::GetL1Distance (const Binary2D& rkImage)
{
    int iMaxDistance;
    ImageInt2D kDistance(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetL1Distance(iMaxDistance,kDistance);
    kDistance.Save("L1distance.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::GetL2Distance (const Binary2D& rkImage)
{
    double dMaxDistance;
    ImageDouble2D kDistance(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetL2Distance(dMaxDistance,kDistance);
    kDistance.Save("L2distance.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::GetSkeleton (const Binary2D& rkImage)
{
    ImageInt2D kSkeleton(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetSkeleton(kSkeleton);
    kSkeleton.Save("Skeleton.im");
}
//----------------------------------------------------------------------------
int BinaryOperations::Main (int, char**)
{
    Binary2D kImage(System::GetPath("../../Data/Im/Binary.im"));
    if ( kImage.GetDimensions() == 0 )
    {
        // image not found
        return -1;
    }

    GetBoundaries(kImage);
    GetComponents(kImage);
    GetL1Distance(kImage);
    GetL2Distance(kImage);
    GetSkeleton(kImage);
    return 0;
}
//----------------------------------------------------------------------------
