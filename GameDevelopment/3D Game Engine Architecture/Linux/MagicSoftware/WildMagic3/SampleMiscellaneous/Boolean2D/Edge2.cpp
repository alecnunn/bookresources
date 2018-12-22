// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Edge2.h"

//----------------------------------------------------------------------------
Edge2::Edge2 ()
{
    i0 = -1;
    i1 = -1;
}
//----------------------------------------------------------------------------
Edge2::Edge2 (int j0, int j1)
{
    i0 = j0;
    i1 = j1;
}
//----------------------------------------------------------------------------
bool Edge2::operator< (const Edge2& rkE) const
{
    if ( i1 < rkE.i1 )
        return true;

    if ( i1 == rkE.i1 )
        return i0 < rkE.i0;

    return false;
}
//----------------------------------------------------------------------------



