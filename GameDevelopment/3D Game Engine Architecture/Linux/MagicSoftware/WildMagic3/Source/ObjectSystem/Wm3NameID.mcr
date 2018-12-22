// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
#define WM3_DECLARE_NAME_ID \
public: \
    virtual Object* GetObjectByName (const String& rkName); \
    virtual void GetAllObjectsByName (const String& rkName, \
        TArray<Object*>& rkObjects); \
    virtual Object* GetObjectByID (unsigned int uiID)
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_DEFAULT_NAME_ID(classname,baseclassname) \
Object* classname::GetObjectByName (const String& rkName) \
{ \
    return baseclassname::GetObjectByName(rkName); \
} \
\
void classname::GetAllObjectsByName (const String& rkName, \
    TArray<Object*>& rkObjects) \
{ \
    baseclassname::GetAllObjectsByName(rkName,rkObjects); \
} \
\
Object* classname::GetObjectByID (unsigned int uiID) \
{ \
    return baseclassname::GetObjectByID(uiID); \
}
//----------------------------------------------------------------------------
