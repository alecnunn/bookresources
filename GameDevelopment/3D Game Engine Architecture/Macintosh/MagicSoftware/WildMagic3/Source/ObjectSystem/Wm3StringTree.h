// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3STRINGTREE_H
#define WM3STRINGTREE_H

#include "Wm3System.h"
#include "Wm3Line3.h"
#include "Wm3Matrix2.h"
#include "Wm3Matrix3.h"
#include "Wm3Matrix4.h"
#include "Wm3Plane3.h"
#include "Wm3Quaternion.h"

namespace Wm3
{

class BoundingVolume;
class ColorRGBA;
class ColorRGB;
class Rtti;

class WM3_ITEM StringTree
{
public:
    // construction and destruction
    StringTree (int iStringQuantity, int iChildQuantity);
    ~StringTree ();

    // strings
    int GetStringQuantity () const;
    void SetString (int i, char* acString);
    char* GetString (int i);

    // children
    int GetChildQuantity () const;
    void SetChild (int i, StringTree* pkChild);
    StringTree* GetChild (int i);

    // string creation helpers (native types)
    static char* Format (const Rtti* pkRTTI, const char* acName);
    static char* Format (const char* acString);

    static char* Format (const char* acPrefix, bool bValue);
    static char* Format (const char* acPrefix, char cValue);
    static char* Format (const char* acPrefix, unsigned char ucValue);
    static char* Format (const char* acPrefix, short sValue);
    static char* Format (const char* acPrefix, unsigned short usValue);
    static char* Format (const char* acPrefix, int iValue);
    static char* Format (const char* acPrefix, unsigned int uiValue);
    static char* Format (const char* acPrefix, long lValue);
    static char* Format (const char* acPrefix, unsigned long ulValue);
    static char* Format (const char* acPrefix, float fValue);
    static char* Format (const char* acPrefix, double dValue);
    static char* Format (const char* acPrefix, void* pvValue);
    static char* Format (const char* acPrefix, const char* acValue);

    static char* Format (const char* acPrefix, const BoundingVolume* pkValue);
    static char* Format (const char* acPrefix, const ColorRGBA& rkValue);
    static char* Format (const char* acPrefix, const ColorRGB& rkValue);
    static char* Format (const char* acPrefix, const Line3f& rkValue);
    static char* Format (const char* acPrefix, const Matrix2f& rkValue);
    static char* Format (const char* acPrefix, const Matrix3f& rkValue);
    static char* Format (const char* acPrefix, const Matrix4f& rkValue);
    static char* Format (const char* acPrefix, const Plane3f& rkValue);
    static char* Format (const char* acPrefix, const Quaternionf& rkValue);
    static char* Format (const char* acPrefix, const Vector2f& rkValue);
    static char* Format (const char* acPrefix, const Vector3f& rkValue);
    static char* Format (const char* acPrefix, const Vector4f& rkValue);
    static char* Format (const char* acPrefix, const Line3d& rkValue);
    static char* Format (const char* acPrefix, const Matrix2d& rkValue);
    static char* Format (const char* acPrefix, const Matrix3d& rkValue);
    static char* Format (const char* acPrefix, const Matrix4d& rkValue);
    static char* Format (const char* acPrefix, const Plane3d& rkValue);
    static char* Format (const char* acPrefix, const Quaterniond& rkValue);
    static char* Format (const char* acPrefix, const Vector2d& rkValue);
    static char* Format (const char* acPrefix, const Vector3d& rkValue);
    static char* Format (const char* acPrefix, const Vector4d& rkValue);

    template <class T>
    static StringTree* Format (const char* acTitle, int iQuantity,
        const T* atValue);

    // streaming
    bool Save (const char* acFilename, int iTabSize = 4);

private:
    // streaming (recursive)
    void Save (FILE* pkOFile, int iLevel, int iTabSize);

    TArray<char*> m_kStrings;
    TArray<StringTree*> m_kChildren;

    static void FormatFloat (float fValue, char* acString);
    static void FormatDouble (double dValue, char* acString);
};

#include "Wm3StringTree.inl"

}

#endif
