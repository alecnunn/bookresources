// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CURVEMESH_H
#define WM3CURVEMESH_H

#include "Wm3Polyline.h"
#include "Wm3CurveSegment.h"
#include "Wm3FloatArray.h"

namespace Wm3
{

class WM3_ITEM CurveMesh : public Polyline
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  The N segments are assumed to be ordered
    // to form a continuous curve.  The curve segments are stored in
    // aspkSegment.  CurveMesh assumes responsibility for deleting this array,
    // so it must be dynamically allocated.  The array spkParams stores the
    // parameter values for the curve segment end points.  This array must
    // have 2*(N-1) values.  Curve segment P[i] has domain
    // [spkParams[2*i],spkParams[2*i+1]].  Continuity requires that
    // P[i](spkParams[2*i+1]) = P[i+1](spkParams[2*(i+1)]).  The polyline
    // produced by the subdivision is flagged as "open".  If you want a
    // closed polyline, you should make certain that the first end point of
    // P[0] matches the last end point of P[N-1].
    //
    // The parameter bAllowAttributes should be set to 'true' when you want to
    // construct vertex attributes at all curve mesh vertices.  The parameter
    // bAllowDynamicChange should be set to 'true' when you plan on
    // dynamically changing the curve segments.  For example, you might modify
    // the control points in a Bezier curve segment.
    CurveMesh (int iSegmentQuantity, CurveSegmentPtr* aspkSegment,
        FloatArrayPtr spkParams, bool bAllowAttributes,
        bool bAllowDynamicChange);

    virtual ~CurveMesh ();

    // level of subdivision
    void SetLevel (int iLevel);
    int GetLevel () const;

    // Construct vertex attributes.  The input array must have quantity of
    // elements equal to that of the vertex array passed to the CurveMesh
    // constructor.  The iSize parameter is how many 'float' items per
    // vertex attribute.  For example, iSize is 2 for Vector2f and is 4 for
    // ColorRGBA.  The afAttr pointer is to an array of vertex attribute
    // objects such as Vertex2f or ColorRGBA.  The returned array is null if
    // bAllowAttributes was set to 'false' in the constructor.  Otherwise, the
    // returned array has the quantity of elements equal to the quantity of
    // curve mesh vertices, the value returned by Vertices.GetQuantity().
    // The caller is responsible for deleting the input array if it was
    // dynamically allocated.
    float* GetAttributes (int iSize, const float* afAttr) const;

    // Call this function when one or more of your curve segments has
    // dynamically changed.
    void OnDynamicChange ();

    // Free up all memory used by the object, other than the Polyline data.
    // The intent is that the curve mesh is in its final form.  Once this
    // function is called, SetLevel is ignored.
    void Lock ();
    bool IsLocked () const;

protected:
    CurveMesh ();

    int m_iSegmentQuantity;
    CurveSegmentPtr* m_aspkSegment;
    FloatArrayPtr m_spkOrigParams;
    int m_iLevel, m_iFullVQuantity;

private:
    // support for subdivision of the mesh
    class Edge
    {
    public:
        Edge ();

        CurveSegmentPtr Segment;  // curve for subdivision evaluations
        int V[2];                 // indices for vertices
        float Param[2];           // curve parameter values for end points
    };

    void Allocate (int& riVQTotal, Vector3f*& rakVertex, int& riEQTotal,
        Edge*& rakEdge);

    void Subdivide (int& riVQ, Vector3f* akVertex, int& riEQ, Edge* akEdge);

    // support for constructing vertex attributes
    class VertexTree
    {
    public:
        VertexTree ();

        int V[2];
    };
    VertexTree* m_akVTree;
    bool m_bAllowAttributes;

    // support for dynamic changes in the curve segments
    class CurveInfo
    {
    public:
        CurveInfo ();

        CurveSegmentPtr Segment;
        float Param;
    };
    void InitializeCurveInfo ();
    bool m_bAllowDynamicChange;
    CurveInfo* m_akCInfo;
};

WM3_REGISTER_STREAM(CurveMesh);
typedef Pointer<CurveMesh> CurveMeshPtr;
#include "Wm3CurveMesh.inl"

}

#endif
