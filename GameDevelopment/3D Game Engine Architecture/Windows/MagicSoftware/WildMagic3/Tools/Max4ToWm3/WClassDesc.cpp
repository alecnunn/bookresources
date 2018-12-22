// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WClassDesc.h"
#include "Max4ToWm3.h"
#include "resource.h"
#include "WSceneExport.h"

Class_ID WClassDesc::ID(0x554dbc9c,0x8f17519);
WClassDesc WClassDesc::TheDescription;

//----------------------------------------------------------------------------
WClassDesc::WClassDesc ()
{
}
//----------------------------------------------------------------------------
int WClassDesc::IsPublic () 
{ 
    return TRUE; 
}
//----------------------------------------------------------------------------
void* WClassDesc::Create (BOOL) 
{ 
    return new WSceneExport; 
}
//----------------------------------------------------------------------------
const TCHAR* WClassDesc::ClassName () 
{ 
    return Max4ToWm3::ThePlugin.GetResourceString(IDS_CLASS_NAME); 
}
//----------------------------------------------------------------------------
SClass_ID WClassDesc::SuperClassID () 
{ 
    return SCENE_EXPORT_CLASS_ID; 
}
//----------------------------------------------------------------------------
Class_ID WClassDesc:: ClassID () 
{ 
    return ID;
}
//----------------------------------------------------------------------------
const TCHAR* WClassDesc::Category ()
{ 
    return Max4ToWm3::ThePlugin.GetResourceString(IDS_CATEGORY); 
}
//----------------------------------------------------------------------------
const TCHAR* WClassDesc::InternalName () 
{ 
    return Max4ToWm3::ThePlugin.GetResourceString(IDS_INTERNALNAME); 
}
//----------------------------------------------------------------------------
HINSTANCE WClassDesc::HInstance () 
{ 
    return Max4ToWm3::ThePlugin.GetInstance(); 
}
//----------------------------------------------------------------------------
