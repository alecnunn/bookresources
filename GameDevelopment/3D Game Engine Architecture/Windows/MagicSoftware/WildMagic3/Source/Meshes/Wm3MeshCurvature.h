// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MESHCURVATURE_H
#define WM3MESHCURVATURE_H

#include "Wm3Matrix3.h"
#include "Wm3Matrix2.h"

namespace Wm3
{

template <class Real>
class MeshCurvature
{
public:
    // The caller is responsible for deleting the input arrays.
    MeshCurvature (int iVQuantity, const Vector3<Real>* akVertex,
        int iTQuantity, const int* aiIndex);

    virtual ~MeshCurvature ();

    // input values from the constructor
    int GetVQuantity () const;
    const Vector3<Real>* GetVertices () const;
    int GetTQuantity () const;
    const int* GetIndices () const;

    // derived quantites from the input mesh
    const Vector3<Real>* GetNormals () const;
    const Real* GetMinCurvatures () const;
    const Real* GetMaxCurvatures () const;
    const Vector3<Real>* GetMinDirections () const;
    const Vector3<Real>* GetMaxDirections () const;

protected:
    int m_iVQuantity;
    const Vector3<Real>* m_akVertex;
    int m_iTQuantity;
    const int* m_aiIndex;

    Vector3<Real>* m_akNormal;
    Real* m_afMinCurvature;
    Real* m_afMaxCurvature;
    Vector3<Real>* m_akMinDirection;
    Vector3<Real>* m_akMaxDirection;
};

#include "Wm3MeshCurvature.inl"

typedef MeshCurvature<float> MeshCurvaturef;
typedef MeshCurvature<double> MeshCurvatured;

}

#endif
