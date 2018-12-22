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

#include "SkinControllerData.h"

using namespace Wm3;

//----------------------------------------------------------------------------
SkinControllerData::SkinControllerData ()
{
    Mesh = NULL;
    Weight = NULL;
    Offset = NULL;
    Bones = NULL;
}
//----------------------------------------------------------------------------
SkinControllerData::~SkinControllerData()
{
    if (Bones)
        delete[] Bones;
}
//----------------------------------------------------------------------------