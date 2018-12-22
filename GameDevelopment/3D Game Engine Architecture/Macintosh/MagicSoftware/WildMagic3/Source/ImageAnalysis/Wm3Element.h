// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3ELEMENT_H
#define WM3ELEMENT_H

#include "Wm3System.h"

// wrappers for native types
const int WM3_ELEMENT_QUANTITY = 12;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short rgb5;
typedef unsigned int rgb8;

#define Wm3DeclareElement(T) \
class WM3_ITEM E##T \
{ \
public: \
    E##T (T tValue = 0) { m_tValue = tValue; } \
    \
    E##T& operator= (E##T kElement) \
    { \
        m_tValue = kElement.m_tValue; \
        return *this; \
    } \
    \
    operator T () { return m_tValue; } \
    \
    static int GetRTTI () { return ms_iRTTI; } \
    \
protected: \
    T m_tValue; \
    static const int ms_iRTTI; \
}

#define Wm3ImplementElement(T,iRTTI) \
const int E##T::ms_iRTTI = iRTTI

namespace Wm3
{

Wm3DeclareElement(char);
Wm3DeclareElement(uchar);
Wm3DeclareElement(short);
Wm3DeclareElement(ushort);
Wm3DeclareElement(int);
Wm3DeclareElement(uint);
Wm3DeclareElement(long);
Wm3DeclareElement(ulong);
Wm3DeclareElement(float);
Wm3DeclareElement(double);
Wm3DeclareElement(rgb5);
Wm3DeclareElement(rgb8);

#include "Wm3Element.inl"

}

#endif
