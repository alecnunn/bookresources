// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Application.h"
using namespace Wm3;

Application::EntryPoint Application::Run = 0;
Command* Application::TheCommand = 0;
Application* Application::TheApplication = 0;

//----------------------------------------------------------------------------
Application::Application ()
{
}
//----------------------------------------------------------------------------
Application::~Application ()
{
}
//----------------------------------------------------------------------------
int main (int iQuantity, char** apcArgument)
{
    Main::Initialize();

    int iExitCode = 0;
    if ( Application::Run )
    {
        Application::TheCommand = new Command(iQuantity,apcArgument);
        iExitCode = Application::Run(iQuantity,apcArgument);
        delete Application::TheCommand;
        Application::TheCommand = 0;
    }
    else
    {
        iExitCode = INT_MAX;
    }

    Main::Terminate();

    delete Application::TheApplication;
    Application::TheApplication = 0;
    return iExitCode;
}
//----------------------------------------------------------------------------
