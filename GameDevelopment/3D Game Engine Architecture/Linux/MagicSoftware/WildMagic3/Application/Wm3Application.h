// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPLICATION_H
#define WM3APPLICATION_H

#include "WildMagic3.h"
#include "Wm3Command.h"

namespace Wm3
{

class WM3_ITEM Application
{
public:
    // abstract base class
    virtual ~Application ();

    // the unique application object
    static Application* TheApplication;

    // the unique command-line arguments object
    static Command* TheCommand;

    // Derived classes must set this function pointer.  The int parameter is
    // the number of command line arguments.  The char** parameter is the
    // array of argument strings.
    typedef int (*EntryPoint)(int, char**);
    static EntryPoint Run;

protected:
    Application ();
};

#include "Wm3Application.mcr"

}

#endif
