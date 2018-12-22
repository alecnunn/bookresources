//
// Installation program for OpenPTC 1.0 Win32
// Copyright (c) Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include <windows.h>
#include "resource.h"
#include "Config.h"
#include "Routines.h"




int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    // quiet operation
    BOOL quiet = FALSE;

    // force installation
    BOOL force = FALSE;

    // check quiet
    if (!quiet)
    {
        // ask if user wants to install
        int result = MessageBox(0,"This program will install OpenPTC for Windows on your system\n\nDo you wish to continue?","Installer",MB_YESNO|MB_ICONQUESTION);

        // check result
        if (result==IDNO)
        {
            // user exit
            MessageBox(0,"OpenPTC for Windows has not been installed","Installer",MB_OK);
            return 1;
        }
    }

    // setup data
    BOOL ptc_dll = TRUE;
    BOOL ptcdebug_dll = TRUE;
    BOOL hermes_dll = TRUE;

#ifdef __INSTALL_PTC_DLL__

    // install "ptc.dll"
    ptc_dll = InstallDLL(IDR_PTC_DLL,"ptc.dll",force);

#endif

#ifdef __INSTALL_PTCDEBUG_DLL__

    // install "ptcdebug.dll"
    ptcdebug_dll = InstallDLL(IDR_PTCDEBUG_DLL,"ptcdebug.dll",force);

#endif

#ifdef __INSTALL_HERMES_DLL__

    // install "hermes.dll"
    hermes_dll = InstallDLL(IDR_HERMES_DLL,"hermes.dll",force);

#endif

    // check installation results
    if (ptc_dll && ptcdebug_dll && hermes_dll)
    {
        // check quiet
        if (!quiet)
        {
            // success notice
            MessageBox(0,"OpenPTC for Windows has been successfully installed on your system","Installer",MB_OK);
        }

        // success
        return 0;
    }
    else
    {
        // check quiet
        if (!quiet)
        {
            // failure notice
            MessageBox(0,"OpenPTC for Windows installation has failed","Installer",MB_OK|MB_ICONEXCLAMATION);
        }

        // failure
        return 1;
    }
}