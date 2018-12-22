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
#define WM3_DECLARE_RTTI \
public: \
    static const Rtti TYPE; \
    virtual const Rtti& GetType () const { return TYPE; }
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_RTTI(nsname,classname,baseclassname) \
    const Rtti classname::TYPE(#nsname"."#classname,&baseclassname::TYPE)
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_TEMPLATE_RTTI(nsname,classname,baseclassname) \
    template <> \
    const Rtti classname::TYPE(#nsname"."#classname,&baseclassname::TYPE)
//----------------------------------------------------------------------------
