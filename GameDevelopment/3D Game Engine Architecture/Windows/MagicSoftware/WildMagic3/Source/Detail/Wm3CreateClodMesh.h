// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CREATECLODMESH_H
#define WM3CREATECLODMESH_H

#include "Wm3CollapseRecord.h"
#include "Wm3ColorRGB.h"
#include "Wm3Vector2.h"
#include "Wm3Vector3.h"
#include <map>


namespace Wm3
{

class WM3_ITEM CreateClodMesh
{
public:
    CreateClodMesh (int iVQuantity, Vector3f* akVertex, int iTQuantity,
        int* aiTConnect, int& riCQuantity, CollapseRecord*& rakCRecord);

    ~CreateClodMesh ();

    template <class T> void Reorder (T*& ratVertexAttribute);

private:
    // vertex is <v>
    // edge is <v0,v1> where v0 = min(v0,v1)
    // triangle is <v0,v1,v2> where v0 = min(v0,v1,v2)

    class Edge
    {
    public:
        Edge ();
        Edge (int iV0, int iV1);
        bool operator< (const Edge& rkE) const;
        bool operator== (const Edge& rkE) const;
        bool operator!= (const Edge& rkE) const;

        int m_aiV[2];
    };

    class Triangle
    {
    public:
        Triangle ();
        Triangle (int iV0, int iV1, int iV2);
        bool operator< (const Triangle& rkT) const;
        bool operator== (const Triangle& rkT) const;
        bool operator!= (const Triangle& rkT) const;

        int m_aiV[3];
    };

    class HeapRecord
    {
    public:
        HeapRecord () : m_kEdge(-1,-1)
        {
            m_iHIndex = -1;
            m_fMetric = -1.0f;
        }

        Edge m_kEdge;
        int m_iHIndex;
        float m_fMetric;
    };

    class VertexAttribute
    {
    public:
        VertexAttribute ()
            :
            m_kESet(8,8),
            m_kTSet(8,8)
        {
        }

        TSet<Edge> m_kESet;
        TSet<Triangle> m_kTSet;
    };

    class EdgeAttribute
    {
    public:
        EdgeAttribute ()
            :
            m_kTSet(2,2)
        {
            Record = 0;
        }

        HeapRecord* Record;
        TSet<Triangle> m_kTSet;
    };

    class TriangleAttribute
    {
    public:
        TriangleAttribute ()
        {
            Index = -1;
        }

        int Index;
    };

    // for readability of the code
    typedef std::map<int,VertexAttribute> VMap;
    typedef VMap::iterator MVIter;
    typedef VMap::const_iterator MVCIter;
    typedef std::map<Edge,EdgeAttribute> EMap;
    typedef EMap::iterator MEIter;
    typedef EMap::const_iterator MECIter;
    typedef std::map<Triangle,TriangleAttribute> TMap;
    typedef TMap::iterator MTIter;
    typedef TMap::const_iterator MTCIter;

    // insert and remove triangles
    void InsertTriangle (const Triangle& rkT);
    void RemoveTriangle (const Triangle& rkT);

    // Callbacks that are executed when vertices, edges, or triangles are
    // inserted or removed during triangle insertion, triangle removal, or
    // edge collapsing.
    void OnEdgeInsert (const Edge& rkE, bool bCreate, HeapRecord*& rpkRecord);
    void OnEdgeRemove (const Edge& rkE, bool bDestroy, HeapRecord* pkRecord);

    // triangle attributes
    void SetData (const Triangle& rkT, int iIndex);
    int GetData (const Triangle& rkT);

    void RemoveTriangleDuringCollapse (const Triangle& rkT);
    void ModifyTriangle (Triangle& rkT, int iVKeep, int iVThrow);
    void CollapseEdge (int iVKeep, int iVThrow);
    void FlushVertices ();
    void FlushTriangles ();

    void DoCollapse ();
    bool CollapseCausesFolding (int iVKeep, int iVThrow);
    float GetMetric (MEIter pkE);
    void Reorder ();
    void ComputeRecords (int& riCQuantity, CollapseRecord*& rakCRecord);

    void InitializeHeap ();
    void Sort ();
    void Add (float fMetric);
    void Remove ();
    void Update (int iHIndex, float fMetric);
    bool IsValidHeap (int iStart, int iFinal);
    bool IsValidHeap ();

    // write the heap to an ASCII file (for debugging)
    void PrintHeap (const char* acFilename);

    // write the mesh to an ASCII file (for debugging)
    void Print (const char* acFilename) const;

    // triangle mesh to be decimated
    int m_iVQuantity;
    Vector3f* m_akVertex;
    int m_iTQuantity;
    int* m_aiConnect;

    int m_iHQuantity;
    HeapRecord** m_apkHeap;
    bool m_bCollapsing;

    // for reordering vertices and triangles
    TSet<int> m_kVDelete;
    int m_iVCurrent, m_iTCurrent;
    int* m_aiVOrdered;
    int* m_aiVPermute;
    int* m_aiNewConnect;
    TArray<CollapseRecord> m_kEDelete;

    // vertex-edge-triangle data structure
    std::map<int,VertexAttribute> m_kVMap;
    std::map<Edge,EdgeAttribute> m_kEMap;
    std::map<Triangle,TriangleAttribute> m_kTMap;
};

#include "Wm3CreateClodMesh.inl"

}

#endif
