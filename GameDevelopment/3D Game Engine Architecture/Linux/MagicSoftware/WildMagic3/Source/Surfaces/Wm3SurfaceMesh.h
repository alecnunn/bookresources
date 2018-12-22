// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SURFACEMESH_H
#define WM3SURFACEMESH_H

#include "Wm3TriMesh.h"
#include "Wm3SurfacePatch.h"

namespace Wm3
{

class WM3_ITEM SurfaceMesh : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  If there are T triangles, spkIndices
    // has 3*T elements, aspkPatch has T elements, and spkParams has 3*T
    // elements.  Each triangle has a patch for constructing vertices in the
    // subdivision.  The subdivision requires knowing the parameter values
    // at the vertices of the original triangles.  SurfaceMesh assumes
    // responsibility for deleting the aspkPatch array (so it must be
    // dynamically allocated).  The parameter bAllowAttributes should be set
    // to 'true' when you want to construct vertex attributes at all surface
    // mesh vertices.  The parameter bAllowDynamicChange should be set to
    // 'true' when you plan on dynamically changing the surface patches.  For
    // example, you might modify the control points in a Bezier surface patch.
    SurfaceMesh (Vector3fArrayPtr spkVertices, IntArrayPtr spkIndices,
        bool bGenerateNormals, Vector2fArrayPtr spkParams,
        SurfacePatchPtr* aspkPatch, bool bAllowAttributes,
        bool bAllowDynamicChange);

    virtual ~SurfaceMesh ();

    // level of subdivision
    void SetLevel (int iLevel);
    int GetLevel () const;

    // Construct vertex attributes.  The input array must have quantity of
    // elements equal to that of the vertex array passed to the SurfaceMesh
    // constructor.  The iSize parameter is how many 'float' items per
    // vertex attribute.  For example, iSize is 2 for Vector2f and is 4 for
    // ColorRGBA.  The afAttr pointer is to an array of vertex attribute
    // objects such as Vertex2f or ColorRGBA.  The returned array is null if
    // bAllowAttributes was set to 'false' in the constructor.  Otherwise, the
    // returned array has the quantity of elements equal to the quantity of
    // surface mesh vertices, the value returned by Vertices.GetQuantity().
    // The caller is responsible for deleting the input array if it was
    // dynamically allocated.
    float* GetAttributes (int iSize, const float* afAttr) const;

    // Call this function when one or more of your surface patches has
    // dynamically changed.
    void OnDynamicChange ();

    // Free up all memory used by the object, other than the TriMesh data.
    // The intent is that the surface mesh is in its final form.  Once this
    // function is called, SetLevel is ignored.
    void Lock ();
    bool IsLocked () const;

protected:
    SurfaceMesh ();

    Vector3fArrayPtr m_spkOrigVertices;
    IntArrayPtr m_spkOrigIndices;
    Vector2fArrayPtr m_spkOrigParams;
    SurfacePatchPtr* m_aspkPatch;
    int m_iLevel, m_iFullVQuantity, m_iPatchQuantity;

private:
    // support for subdivision of the mesh
    class Edge
    {
    public:
        Edge (int iV0 = -1, int iV1 = -1);

        // support for hash sets
        static int HashFunction (const Edge& rkEdge);
        bool operator== (const Edge& rkEdge) const;
        bool operator!= (const Edge& rkEdge) const;

        SurfacePatchPtr Patch;  // surface for subdivision evaluations
        int V[2];               // vertex indices for end points
        Vector2f Param[2];      // surface parameter values for end points
        int VMid;               // index for new vertex
        Vector2f ParamMid;      // parameter values for new vertex
        int References;         // number of triangles sharing the edge
    };

    class Triangle
    {
    public:
        Triangle ();

        SurfacePatchPtr Patch;  // surface for subdivision evaluations
        int V[3];               // indices for vertices
    };

    void Allocate (int& riVQTotal, Vector3f*& rakVertex, int& riEQTotal,
        THashSet<Edge>*& rpkESet, int& riTQTotal, Triangle*& rakTriangle);

    void Subdivide (int& riVQ, Vector3f* akVertex, int& riEQ,
        THashSet<Edge>* pkESet, int& riTQ, Triangle* akTriangle);

    void InsertInto (THashSet<Edge>* pkESet, SurfacePatchPtr spkPatch,
        int iV0, int iV1, const Vector2f& rkParam0,
        const Vector2f& rkParam1, int iNewReferences);


    // support for constructing vertex attributes
    class VertexTree
    {
    public:
        VertexTree ();

        int V[2];
    };
    VertexTree* m_akVTree;
    bool m_bAllowAttributes;

    // support for dynamic changes in the surface patches
    class SurfaceInfo
    {
    public:
        SurfaceInfo ();

        SurfacePatchPtr Patch;
        Vector2f Param;
    };
    void InitializeSurfaceInfo ();
    bool m_bAllowDynamicChange;
    SurfaceInfo* m_akSInfo;
};

WM3_REGISTER_STREAM(SurfaceMesh);
typedef Pointer<SurfaceMesh> SurfaceMeshPtr;
#include "Wm3SurfaceMesh.inl"

}

#endif
