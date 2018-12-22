// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BoundingVolume.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BoundingVolume,Object);
WM3_IMPLEMENT_ABSTRACT_STREAM(BoundingVolume);
WM3_IMPLEMENT_DEFAULT_STREAM(BoundingVolume,Object);
WM3_IMPLEMENT_DEFAULT_NAME_ID(BoundingVolume,Object);

//----------------------------------------------------------------------------
BoundingVolume::BoundingVolume ()
{
}
//----------------------------------------------------------------------------
BoundingVolume::~BoundingVolume ()
{
}
//----------------------------------------------------------------------------
