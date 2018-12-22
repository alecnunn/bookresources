// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MAIN_H
#define WM3MAIN_H

#include "Wm3System.h"
#include "Wm3Main.mcr"

namespace Wm3
{

class WM3_ITEM Main
{
public:
    typedef void (*Initializer)(void);
    typedef TArray<Initializer> InitializerArray;
    static void AddInitializer (Initializer oInitialize);
    static void Initialize ();

    typedef void (*Terminator)(void);
    typedef TArray<Terminator> TerminatorArray;
    static void AddTerminator (Terminator oTerminate);
    static void Terminate ();

private:
    enum { IT_MAXQUANTITY = 256, IT_GROWBY = 256 };
    static InitializerArray* ms_pkInitializers;
    static TerminatorArray* ms_pkTerminators;

    static int ms_iStartObjects, ms_iFinalObjects;
};

}

#endif
