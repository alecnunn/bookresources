// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BoxBVTree.h"
#include "Wm3ContBox3.h"
using namespace Wm3;

WM3_IMPLEMENT_INITIALIZE(BoxBVTree);

//----------------------------------------------------------------------------
void BoxBVTree::Initialize ()
{
    ms_aoCreateModelBound[BoundingVolume::BV_BOX] =
        &BoxBVTree::CreateModelBound;

    ms_aoCreateWorldBound[BoundingVolume::BV_BOX] =
        &BoxBVTree::CreateWorldBound;
}
//----------------------------------------------------------------------------
BoxBVTree::BoxBVTree (const TriMesh* pkMesh, int iMaxTrisPerLeaf,
    bool bStoreInteriorTris)
    :
    BoundingVolumeTree(BoundingVolume::BV_BOX,pkMesh,iMaxTrisPerLeaf,
        bStoreInteriorTris)
{
}
//----------------------------------------------------------------------------
BoundingVolume* BoxBVTree::CreateModelBound (const TriMesh* pkMesh, int i0,
    int i1, int* aiISplit, Line3f& rkLine)
{
    // tag vertices that are used in the submesh
    int iVQuantity = pkMesh->Vertices->GetQuantity();
    Vector3f* akVertex = pkMesh->Vertices->GetData();
    const int* aiIndex = pkMesh->Indices->GetData();
    bool* abValid = new bool[iVQuantity];
    memset(abValid,0,iVQuantity*sizeof(bool));
    for (int i = i0; i <= i1; i++)
    {
        int j = 3*aiISplit[i];
        abValid[aiIndex[j++]] = true;
        abValid[aiIndex[j++]] = true;
        abValid[aiIndex[j++]] = true;
    }

    BoxBV* pkModelBound = new BoxBV;
    pkModelBound->Box() = ContOrientedBox<float>(iVQuantity,akVertex,abValid);
    delete[] abValid;

    rkLine.Origin = pkModelBound->Box().Center;
    rkLine.Direction = pkModelBound->Box().Axis[2];
    return pkModelBound;
}
//----------------------------------------------------------------------------
BoundingVolume* BoxBVTree::CreateWorldBound ()
{
    return new BoxBV;
}
//----------------------------------------------------------------------------
