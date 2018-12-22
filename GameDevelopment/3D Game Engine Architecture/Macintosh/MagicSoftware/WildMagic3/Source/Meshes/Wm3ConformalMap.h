// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONFORMALMAP_H
#define WM3CONFORMALMAP_H

#include "Wm3Vector2.h"
#include "Wm3Vector3.h"
#include "Wm3BasicMesh.h"
#include "Wm3LinearSystem.h"
#include "Wm3PolynomialRoots.h"
#include <utility>

namespace Wm3
{

template <class Real>
class ConformalMap
{
public:
    // The input mesh should be a closed, manifold surface.  That is, it must
    // have the topology of a sphere.
    //
    // The number of vertices in the input is iVQuantity.  The vertex array
    // is usually passed as akPoint, but this input can be any data type you
    // prefer (points+attributes).  The number of triangles is iTQuantity.
    // The triangles are represented as triples of indices into the vertex
    // array.  These triples are stored in aiIndex.  The caller is responsible
    // for deleting the input arrays.
    ConformalMap (int iVQuantity, const Vector3<Real>* akPoint, int iTQuantity,
        const int* aiIndex);

    ~ConformalMap ();

    // Conformal mapping of mesh to plane.  The array of coordinates has a
    // one-to-one correspondence with the input vertex array.
    const Vector2<Real>* GetPlaneCoordinates () const;
    const Vector2<Real>& GetPlaneMin () const;
    const Vector2<Real>& GetPlaneMax () const;

    // Conformal mapping of mesh to sphere (centered at origin).  The array
    // of coordinates has a one-to-one correspondence with the input vertex
    // array.
    const Vector3<Real>* GetSphereCoordinates () const;
    Real GetSphereRadius () const;

private:
    Real ComputeRadius (const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
        const Vector2<Real>& rkV2, Real fAreaFraction) const;

    // Conformal mapping to a plane.  The plane's (px,py) points correspond to
    // the mesh's (mx,my,mz) points.
    Vector2<Real>* m_akPlane;
    Vector2<Real> m_kPlaneMin, m_kPlaneMax;

    // Conformal mapping to a sphere.  The sphere's (sx,sy,sz) points
    // correspond to the mesh's (mx,my,mz) points.
    Vector3<Real>* m_akSphere;
    Real m_fRadius;
};

#include "Wm3ConformalMap.inl"

typedef ConformalMap<float> ConformalMapf;
typedef ConformalMap<double> ConformalMapd;

}

#endif
