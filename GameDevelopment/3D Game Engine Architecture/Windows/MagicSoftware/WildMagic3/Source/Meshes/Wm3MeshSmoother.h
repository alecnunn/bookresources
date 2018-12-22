// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MESHSMOOTHER_H
#define WM3MESHSMOOTHER_H

#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class MeshSmoother
{
public:
    // The caller is responsible for deleting the input arrays.
    MeshSmoother (int iVQuantity, Vector3<Real>* akVertex, int iTQuantity,
        const int* aiIndex);

    virtual ~MeshSmoother ();

    // For deferred construction and destruction.  The caller is responsible
    // for deleting the input arrays.
    MeshSmoother ();
    void Create (int iVQuantity, Vector3<Real>* akVertex, int iTQuantity,
        const int* aiIndex);
    void Destroy ();

    // input values from the constructor
    int GetVQuantity () const;
    const Vector3<Real>* GetVertices () const;
    int GetTQuantity () const;
    const int* GetIndices () const;

    // derived quantites from the input mesh
    const Vector3<Real>* GetNormals () const;
    const Vector3<Real>* GetMeans () const;

    // Apply one iteration of the smoother.  The input time is supported for
    // applications where the surface evolution is time-dependent.
    void Update (Real fTime = (Real)0.0);

protected:
    virtual bool VertexInfluenced (int i, Real fTime);
    virtual Real GetTangentWeight (int i, Real fTime);
    virtual Real GetNormalWeight (int i, Real fTime);

    int m_iVQuantity;
    Vector3<Real>* m_akVertex;
    int m_iTQuantity;
    const int* m_aiIndex;

    Vector3<Real>* m_akNormal;
    Vector3<Real>* m_akMean;
    int* m_aiNeighborCount;
};

#include "Wm3MeshSmoother.inl"

typedef MeshSmoother<float> MeshSmootherf;
typedef MeshSmoother<double> MeshSmootherd;

}

#endif
