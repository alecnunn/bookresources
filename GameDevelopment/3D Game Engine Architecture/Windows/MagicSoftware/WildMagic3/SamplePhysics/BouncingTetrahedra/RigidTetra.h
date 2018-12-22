// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef RIGIDTETRA_H
#define RIGIDTETRA_H

#include "Wm3RigidBody.h"
#include "Wm3TriMesh.h"
using namespace Wm3;

class RigidTetra : public RigidBodyf
{
public:
    RigidTetra (float fSize, float fMass, const Vector3f& rkPos,
        const Vector3f& rkLinMom, const Vector3f& rkAngMom);

    TriMeshPtr Mesh () const;
    float GetRadius () const;
    void GetVertices (Vector3f* akVertex) const;

    bool Moved;

private:
    TriMeshPtr m_spkMesh;
    float m_fRadius;
};

#endif
