// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SKINCONTROLLER_H
#define WM3SKINCONTROLLER_H

#include "Wm3Controller.h"
#include "Wm3Vector3.h"

namespace Wm3
{

class Node;

class WM3_ITEM SkinController : public Controller
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  SkinController accepts responsibility
    // for deleting the input arrays.  The weight and offset arrays must have
    // been allocated using the template function Allocate found in
    // Wm3System.h.
    SkinController (int iVertexQuantity, int iBoneQuantity, Node** apkBones,
        float** aafWeight, Vector3f** aakOffset);
    virtual ~SkinController ();

    // member access
    int GetVertexQuantity () const;
    int GetBoneQuantity () const;
    Node* GetBone (int iBone) const;
    float& Weight (int iVertex, int iBone);
    Vector3f& Offset (int iVertex, int iBone);

    // animation update
    virtual bool Update (double dAppTime);

protected:
    SkinController ();

    int m_iVertexQuantity;    // vq
    int m_iBoneQuantity;      // bq
    Node** m_apkBones;        // bones[bq]
    float** m_aafWeight;      // weight[vq][bq]
    Vector3f** m_aakOffset;   // offset[vq][bq]
};

WM3_REGISTER_STREAM(SkinController);
typedef Pointer<SkinController> SkinControllerPtr;
#include "Wm3SkinController.inl"

}

#endif
