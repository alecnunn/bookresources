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
template <class Real>
Tetrahedron3<Real>::Tetrahedron3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Tetrahedron3<Real>::Tetrahedron3 (const Vector3<Real>& rkV0,
    const Vector3<Real>& rkV1, const Vector3<Real>& rkV2,
    const Vector3<Real>& rkV3)
{
    V[0] = rkV0;
    V[1] = rkV1;
    V[2] = rkV2;
    V[3] = rkV3;
}
//----------------------------------------------------------------------------
template <class Real>
Tetrahedron3<Real>::Tetrahedron3 (const Vector3<Real> akV[4])
{
    memcpy(V,akV,4*sizeof(Vector3<Real>));
}
//----------------------------------------------------------------------------
template <class Real>
void Tetrahedron3<Real>::GetPlanes (Plane3<Real> akPlane[4]) const
{
    Vector3<Real> kEdge10 = V[1] - V[0];
    Vector3<Real> kEdge20 = V[2] - V[0];
    Vector3<Real> kEdge30 = V[3] - V[0];
    Vector3<Real> kEdge21 = V[2] - V[1];
    Vector3<Real> kEdge31 = V[3] - V[1];

    akPlane[0].Normal = kEdge20.UnitCross(kEdge10);  // <v0,v2,v1>
    akPlane[1].Normal = kEdge10.UnitCross(kEdge30);  // <v0,v1,v3>
    akPlane[2].Normal = kEdge30.UnitCross(kEdge20);  // <v0,v3,v2>
    akPlane[3].Normal = kEdge21.UnitCross(kEdge31);  // <v1,v2,v3>

    Real fDet = kEdge10.Dot(akPlane[3].Normal);
    int i;
    if ( fDet < (Real)0.0 )
    {
        // normals are inner pointing, reverse their directions
        for (i = 0; i < 4; i++)
            akPlane[i].Normal = -akPlane[i].Normal;
    }

    for (i = 0; i < 4; i++)
        akPlane[i].Constant = V[i].Dot(akPlane[i].Normal);
}
//----------------------------------------------------------------------------
