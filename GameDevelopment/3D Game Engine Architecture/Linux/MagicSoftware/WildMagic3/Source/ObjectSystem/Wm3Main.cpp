// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Main.h"
#include "Wm3Object.h"
using namespace Wm3;

Main::InitializerArray* Main::ms_pkInitializers = 0;
Main::TerminatorArray* Main::ms_pkTerminators = 0;
int Main::ms_iStartObjects = 0;
int Main::ms_iFinalObjects = 0;

//----------------------------------------------------------------------------
void Main::AddInitializer (Initializer oInitialize)
{
    if ( !ms_pkInitializers )
        ms_pkInitializers = new InitializerArray(IT_MAXQUANTITY,IT_GROWBY);

    ms_pkInitializers->Append(oInitialize);
}
//----------------------------------------------------------------------------
void Main::Initialize ()
{
    bool bCountIsCorrect = true;

    // objects should not be created pre-initialize
    ms_iStartObjects = Object::InUse.GetQuantity();
    if ( ms_iStartObjects != 0 )
    {
        bCountIsCorrect = false;
        Object::PrintInUse("AppLog.txt",
            "Objects were created before pre-main initialization");
    }

    assert( bCountIsCorrect );

    if ( ms_pkInitializers )
    {
        for (int i = 0; i < ms_pkInitializers->GetQuantity(); i++)
            (*ms_pkInitializers)[i]();
    }

    delete ms_pkInitializers;
    ms_pkInitializers = 0;

    // number of objects created during initialization
    ms_iStartObjects = Object::InUse.GetQuantity();
}
//----------------------------------------------------------------------------
void Main::AddTerminator (Terminator oTerminate)
{
    if ( !ms_pkTerminators )
        ms_pkTerminators = new TerminatorArray(IT_MAXQUANTITY,IT_GROWBY);

    ms_pkTerminators->Append(oTerminate);
}
//----------------------------------------------------------------------------
void Main::Terminate ()
{
    bool bCountIsCorrect = true;

    // all objects created during the application should be deleted by now
    ms_iFinalObjects = Object::InUse.GetQuantity();
    if ( ms_iStartObjects != ms_iFinalObjects )
    {
        bCountIsCorrect = false;
        Object::PrintInUse("AppLog.txt",
            "Not all objects were deleted before post-main termination");
    }

    if ( ms_pkTerminators )
    {
        for (int i = 0; i < ms_pkTerminators->GetQuantity(); i++)
            (*ms_pkTerminators)[i]();
    }

    delete ms_pkTerminators;
    ms_pkTerminators = 0;

    if ( bCountIsCorrect )
    {
        // objects should not be deleted post-terminate
        ms_iFinalObjects = Object::InUse.GetQuantity();
        if ( ms_iFinalObjects != 0 )
        {
            bCountIsCorrect = false;
            Object::PrintInUse("AppLog.txt",
                "Objects were deleted after post-main termination");
        }
    }

    assert( bCountIsCorrect );
}
//----------------------------------------------------------------------------
