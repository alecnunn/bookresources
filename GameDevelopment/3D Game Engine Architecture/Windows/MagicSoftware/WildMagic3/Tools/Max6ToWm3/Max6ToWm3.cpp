// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Max6ToWm3.h"
#include "WClassDesc.h"
#include "max.h"
#include "resource.h"
#include <cassert>

Max6ToWm3 Max6ToWm3::ThePlugin;

//----------------------------------------------------------------------------
Max6ToWm3::Max6ToWm3 ()
{
    m_hInstance = NULL;
    m_bControlsInitialized = false;
    memset(m_acResourceString,0,MAX_RESOURCE_STRING*sizeof(TCHAR));
}
//----------------------------------------------------------------------------
Max6ToWm3::~Max6ToWm3 ()
{
}
//----------------------------------------------------------------------------
void Max6ToWm3::SetInstance (HINSTANCE hInstance)
{
    m_hInstance = hInstance;
}
//----------------------------------------------------------------------------
HINSTANCE Max6ToWm3::GetInstance () const
{
    return m_hInstance;
}
//----------------------------------------------------------------------------
void Max6ToWm3::SetControlsInitialized ()
{
    m_bControlsInitialized = true;
}
//----------------------------------------------------------------------------
bool Max6ToWm3::GetControlsInitialized () const
{
    return m_bControlsInitialized;
}
//----------------------------------------------------------------------------
const TCHAR* Max6ToWm3::GetResourceString (unsigned int uiID) const
{
    assert( m_hInstance != NULL );
    if ( !m_hInstance )
        return NULL;

    int iQuantity = LoadString(m_hInstance,uiID,(LPSTR)m_acResourceString,
        MAX_RESOURCE_STRING);
    return ( iQuantity > 0 ? m_acResourceString : NULL );
}
//----------------------------------------------------------------------------
BOOL WINAPI DllMain (HINSTANCE hInstance, ULONG, LPVOID)
{
    // This function is called by Windows when the DLL is loaded.
    Max6ToWm3::ThePlugin.SetInstance(hInstance);
    if ( !Max6ToWm3::ThePlugin.GetControlsInitialized() ) 
    {
        Max6ToWm3::ThePlugin.SetControlsInitialized();

        // initialize MAX's custom controls
        InitCustomControls(hInstance);

        // initialize Windows' controls
        InitCommonControls();
    }
    return TRUE;
}
//----------------------------------------------------------------------------
__declspec(dllexport) const TCHAR* LibDescription ()
{
    // This function returns a string that describes the DLL and where the
    // users can purchase the DLL if they do not have it.
    return Max6ToWm3::ThePlugin.GetResourceString(IDS_LIBDESCRIPTION);
}
//----------------------------------------------------------------------------
__declspec(dllexport) int LibNumberClasses ()
{
    // This function returns the number of plugin classes this DLL.
    return 1;
}
//----------------------------------------------------------------------------
__declspec(dllexport) ClassDesc* LibClassDesc (int i)
{
    // This function returns descriptions of the plugins.  Only one plugin
    // exists in this DLL.
    return &WClassDesc::TheDescription;
}
//----------------------------------------------------------------------------
__declspec(dllexport) ULONG LibVersion ()
{
    // This function returns a pre-defined constant indicating the version of
    // the system under which it was compiled.  It is used to allow the system
    // to catch obsolete DLLs.
    return VERSION_3DSMAX;
}
//----------------------------------------------------------------------------
