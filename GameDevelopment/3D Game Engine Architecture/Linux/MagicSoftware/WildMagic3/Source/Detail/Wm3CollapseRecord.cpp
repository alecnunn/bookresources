// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3CollapseRecord.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void CollapseRecord::Read (Stream& rkStream)
{
    rkStream.Read(VKeep);
    rkStream.Read(VThrow);
    rkStream.Read(VQuantity);
    rkStream.Read(TQuantity);
    rkStream.Read(IQuantity);
    if ( IQuantity > 0 )
    {
        Indices = new int[IQuantity];
        rkStream.Read(IQuantity,Indices);
    }
    else
    {
        Indices = 0;
    }
}
//----------------------------------------------------------------------------
void CollapseRecord::Write (Stream& rkStream)
{
    rkStream.Write(VKeep);
    rkStream.Write(VThrow);
    rkStream.Write(VQuantity);
    rkStream.Write(TQuantity);
    rkStream.Write(IQuantity);
    if ( IQuantity > 0 )
        rkStream.Write(IQuantity,Indices);
}
//----------------------------------------------------------------------------
int CollapseRecord::GetMemoryUsed () const
{
    return sizeof(CollapseRecord) + IQuantity*sizeof(Indices[0]);
}
//----------------------------------------------------------------------------
int CollapseRecord::GetDiskUsed () const
{
    return sizeof(VKeep) + sizeof(VThrow) + sizeof(VQuantity) +
        sizeof(TQuantity) + sizeof(IQuantity) + IQuantity*sizeof(Indices[0]);
}
//----------------------------------------------------------------------------
