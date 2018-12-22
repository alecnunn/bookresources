// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include "Wm3Spatial.h"

using namespace Wm3;

// Instance Data is mostly a "struct" class to hold information relating
// to a single Maya object and any instances it might have.

class InstanceData
{
public:
    InstanceData (Spatial* pkFirst);
    InstanceData ();
    ~InstanceData ();

    // An array of spatial objects created for a Maya object.  If a Maya
    // object is not instanced, then this array will be length one.
    TArray<Spatial*> Spats;

    // Convenience functions
    InstanceData& operator= (const InstanceData& rkData);

    // Return the first object in the array.
    Spatial* GetFirst ();
};

#endif