// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WCLASSDESC_H
#define WCLASSDESC_H

#include "max.h"
#include "iparamb2.h"

class WClassDesc : public ClassDesc2
{
public:
    WClassDesc ();

    virtual int IsPublic ();
    virtual void* Create (BOOL bLoading = FALSE);
    virtual const TCHAR* ClassName ();
    virtual SClass_ID SuperClassID ();
    virtual Class_ID ClassID ();
    virtual const TCHAR* Category ();
    virtual const TCHAR* InternalName ();
    virtual HINSTANCE HInstance();

    static Class_ID ID;
    static WClassDesc TheDescription;
};

#endif
