// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "3dSToWmof.h"
#include "Loader3dS.h"
using namespace Wm3;

WM3_CONSOLE_APPLICATION(M3dsToWmof);

//----------------------------------------------------------------------------
// To convert from 3D Studio Max's 3dS format to Wild Magic's wmof format:
//   3dSToWmof model.3ds model.wmof
//
// Any texture images needed by the 3ds file must be located in the same
// directory as the 3dS file, must have been converted to a .wmif format
// image, and must replace the old image filename extension with .wmif.
//----------------------------------------------------------------------------
int M3dsToWmof::Main (int iQuantity, char**)
{
    if ( iQuantity <= 1 )
        return -1;

    char* acInputFile = 0;
    TheCommand->Filename(acInputFile);
    if ( !acInputFile )
    {
        return -2;
    }

    char* acOutputFile = 0;
    TheCommand->Filename(acOutputFile);
    if ( !acOutputFile )
    {
        delete[] acInputFile;
        return -3;
    }

    Loader3ds kLoader;
    
    if ( !kLoader.LoadFile(acInputFile) )
    {
        delete[] acInputFile;
        delete[] acOutputFile;
        return -4;
    }

    NodePtr spkScene = kLoader.LoadSceneGraph(true);

    Stream kOStream;
    kOStream.Insert(spkScene);
    kOStream.Save(acOutputFile);

    delete[] acInputFile;
    delete[] acOutputFile;

    return 0;
}
//----------------------------------------------------------------------------