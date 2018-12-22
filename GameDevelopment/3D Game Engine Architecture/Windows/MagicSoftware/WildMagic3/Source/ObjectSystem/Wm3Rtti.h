// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RTTI_H
#define WM3RTTI_H

#include "Wm3String.h"
#include "Wm3Rtti.mcr"

namespace Wm3
{

class WM3_ITEM Rtti
{
public:
    // The name must be unique among all objects in the system.  In the Wm3
    // namespace, a class Foo should use "Wm3.Foo".  If an application has
    // another namespace, SomeName, then the name should be "SomeName.Foo".
    Rtti (const String& rkName, const Rtti* pkBaseType);
    ~Rtti ();

    const String& GetName () const;

    bool IsExactly (const Rtti& rkType) const;
    bool IsDerived (const Rtti& rkType) const;

private:
    String m_kName;
    const Rtti* m_pkBaseType;
};

#include "Wm3Rtti.inl"

}

#endif

