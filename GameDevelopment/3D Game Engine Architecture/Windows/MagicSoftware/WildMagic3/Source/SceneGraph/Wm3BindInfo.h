// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BINDINFO_H
#define WM3BINDINFO_H

#include "Wm3System.h"
#include "Wm3TArray.h"

namespace Wm3
{

class Renderer;

class WM3_ITEM BindInfo
{
public:
    BindInfo ();

    // The renderer to which the resource is bound.
    Renderer* User;

    // The identifier of the resource for the renderer's use.  The number
    // of bytes used is up to 8, but only the renderer knows the true
    // size.
    char ID[8];
};

class WM3_ITEM BindInfoArray
{
public:
    BindInfoArray (int iQuantity, int iGrowBy);

    const TArray<BindInfo>& GetArray () const;

    void Bind (Renderer* pkUser, int iSize, const void* pvID);
    void Unbind (Renderer* pkUser);
    void GetID (Renderer* pkUser, int iSize, void* pvID);

private:
    TArray<BindInfo> m_kBind;
};

};

#endif
