// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef MAX4TOWM3_H
#define MAX4TOWM3_H

#include <windows.h>

class Max4ToWm3
{
public:
    Max4ToWm3 ();
    ~Max4ToWm3 ();

    void SetInstance (HINSTANCE hInstance);
    HINSTANCE GetInstance () const;
    void SetControlsInitialized ();
    bool GetControlsInitialized () const;
    const TCHAR* GetResourceString (unsigned int uiID) const;

    static Max4ToWm3 ThePlugin;

private:
    HINSTANCE m_hInstance;
    bool m_bControlsInitialized;

    enum { MAX_RESOURCE_STRING = 512 };
    TCHAR m_acResourceString[MAX_RESOURCE_STRING];
};

#endif
