// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "ObjToWm3.h"
#include "LightWaveReader.h"

WM3_CONSOLE_APPLICATION(ObjToWm3);

//----------------------------------------------------------------------------
// To convert from LightWave's obj format to Wild Magic's wmof:
//   ObjToWm3 model.lwo model.wmof [-smooth] [-verbose]
//
// Any texture images needed by the obj file must be located in the same
// directory as the obj file, and must be .bmp or .tga files.
//----------------------------------------------------------------------------
int ObjToWm3::Main (int iQuantity, char**)
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

    LightWaveReader kReader(acInputFile);
    delete[] acInputFile;

    bool bVerbose = ( TheCommand->Boolean("verbose") ? true : false );
    bool bSmooth = ( TheCommand->Boolean("smooth") ? true : false) ;

    if ( !kReader.InitReader(bVerbose) )
    {
        delete[] acOutputFile;
        return -4;
    }
    
    if ( !kReader.Parse(bSmooth) )
    {
        delete[] acOutputFile;
        return -5;
    }
    
    NodePtr spkScene = kReader.GetObject();

    if ( !spkScene )
    {
        delete[] acOutputFile;
        return -6;
    }

    Stream kOStream;
    kOStream.Insert(spkScene);
    kOStream.Save(acOutputFile);


    delete[] acOutputFile;

    return 0;
}
//----------------------------------------------------------------------------