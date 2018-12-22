// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2003.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WildMagic3.h"
#include "TreeControl.h"
using namespace Wm3;

//----------------------------------------------------------------------------
static int Run (HINSTANCE hInstance, LPSTR acIFile)
{
    if ( !acIFile || strlen(acIFile) <= 0 )
        return -1;

    Stream kStream;
    bool bLoaded = kStream.Load(acIFile);
    if ( !bLoaded )
    {
        Main::Terminate();
        return -2;
    }

    SpatialPtr spkRoot = DynamicCast<Spatial>(kStream.GetObjectAt(0));
    if ( !spkRoot )
    {
        Main::Terminate();
        return -3;
    }

    spkRoot->UpdateGS();
    spkRoot->UpdateRS();

    TreeControl kTree(hInstance,NULL,spkRoot,acIFile,0,0,640,480);

    MSG kMsg;
    while ( true )
    {
        if ( PeekMessage(&kMsg,NULL,0,0,PM_REMOVE) )
        {
            if ( kMsg.message == WM_QUIT )
                break;

            HACCEL hAccel = 0;
            if ( !TranslateAccelerator(NULL,hAccel,&kMsg) )
            {
                TranslateMessage(&kMsg);
                DispatchMessage(&kMsg);
            }
        }
    }

    spkRoot = NULL;
    return 0;
}
//----------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR acIFile, int)
{
    Main::Initialize();
    int iResult = Run(hInstance,acIFile);
    Main::Terminate();
    return iResult;
}
//----------------------------------------------------------------------------
