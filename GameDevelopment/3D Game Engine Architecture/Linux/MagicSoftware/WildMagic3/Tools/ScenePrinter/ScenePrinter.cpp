// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "ScenePrinter.h"

WM3_CONSOLE_APPLICATION(ScenePrinter);

//----------------------------------------------------------------------------
void ScenePrinter::Usage ()
{
    std::cout << std::endl;
    std::cout << "To print a scene graph in ASCII format, use" << std::endl;
    std::cout << "    ScenePrinter myfile.wmof myfile.txt" << std::endl;
}
//----------------------------------------------------------------------------
int ScenePrinter::Main (int iArgQuantity, char** apcArgument)
{
    if ( iArgQuantity <= 2 )
    {
        Usage();
        return -1;
    }

    // get files
    const char* acInFile = apcArgument[1];
    const char* acOutFile = apcArgument[2];

    Stream* pkStream = new Stream;
    bool bLoaded = pkStream->Load(acInFile);
    if ( !bLoaded )
       return -2;

    for (int i = 0; i < pkStream->GetObjectCount(); i++)
    {
        Spatial* pkSpatial = DynamicCast<Spatial>(pkStream->GetObjectAt(i));
        if ( pkSpatial )
        {
            pkSpatial->UpdateGS();
            pkSpatial->UpdateRS();
        }
    }

    bool bSaved = pkStream->SaveText(acOutFile);
    if ( !bSaved )
    {
        delete pkStream;
        return -3;
    }

    delete pkStream;
    return 0;
}
//----------------------------------------------------------------------------
