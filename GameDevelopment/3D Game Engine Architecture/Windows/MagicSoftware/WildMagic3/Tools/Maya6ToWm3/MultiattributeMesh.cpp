#include "MultiattributeMesh.h"
using namespace Wm3;
using namespace std;

//----------------------------------------------------------------------------
MultiattributeMesh::MultiattributeMesh ()
{
    Reset();
}
//----------------------------------------------------------------------------
void MultiattributeMesh::Reset ()
{
    m_kVerts.RemoveAll();
    m_kConnect.RemoveAll();
    iOriginalVerts = 0;
}
//----------------------------------------------------------------------------
void MultiattributeMesh::AddVertexLoc(int iV, const Vertex& rkVertex)
{
    // Create spatial data for a vertex at index iV using the
    // spatial data found in rkVertex

    m_kVerts.SetElement(iV,rkVertex);

    // Set that it hasn't gotten any normal or uv info yet
    for(int iE = 3; iE < Vertex::MAXVALUES; iE++)
    {
        m_kVerts[iV][iE] = Mathf::MAX_REAL;
    }
    iOriginalVerts++;
}
//----------------------------------------------------------------------------
int MultiattributeMesh::UpdateVertexInfo(int iV, const Vertex& rkVertex)
{
    // Add UV and normal info from rkVertex to the vertex at index iV

    // This vertex should already exist from AddVertexLoc
    assert(m_kVerts.GetQuantity() > iV);

    // The spatial data should be the same (sanity check)
    assert(m_kVerts[iV][0] == rkVertex[0]);
    assert(m_kVerts[iV][1] == rkVertex[1]);
    assert(m_kVerts[iV][2] == rkVertex[2]);

    // There are several cases here:
    // 1) m_kVerts[iV] does not have uv/normal info yet
    // 2) rkVertex doesn't have any uv/normal info
    // 2) m_kVerts[iV] does have normal uv/info and it's the same
    // 3) m_kVerts[iV] does have normal uv/info and it's not the same
    bool bCollision = false;
    for(int iE = 3; iE < Vertex::MAXVALUES; iE++)
    {
        if ((m_kVerts[iV][iE] != Mathf::MAX_REAL) && 
            (rkVertex[iE] != Mathf::MAX_REAL) &&
            (rkVertex[iE] != m_kVerts[iV][iE]))
        {
            // m_kVerts has data, rkVertex has data, and they differ from
            // each other.
            bCollision = true;
            break;
        }
    }

    if (bCollision)
    {
        // collision, so we need to create a new vertex!
        // However, maybe this vertex already exists, so search the
        // extra vertices first.

        for(int iV2 = iOriginalVerts; iV2 < m_kVerts.GetQuantity(); iV2++)
        {
            if (m_kVerts[iV2] == rkVertex)
            {
                // no need to assign anything, information already there
                return iV2;
            }
        }

        // Not found, create another vertex
        int iNewIndex = m_kVerts.GetQuantity();

        assert(m_kVertMapping.GetQuantity() == (iNewIndex - iOriginalVerts));

        // Map this new vertex back to the old one
        m_kVertMapping.Append(iV);

        // Insert the data in the array
        m_kVerts.Append(rkVertex);

        // Return the index to the new vertex, for use in face creation
        return iNewIndex;
    }
    else
    {
        // no collision, just assign
        for(int iE = 3; iE < Vertex::MAXVALUES; iE++)
        {
            if (rkVertex[iE] != Mathf::MAX_REAL)
            {
                m_kVerts[iV][iE] = rkVertex[iE];
            }
        }

        // index is the same
        return iV;
    }
}
//----------------------------------------------------------------------------
void MultiattributeMesh::AddTriangle (int iV0, int iV1, int iV2)
{
    m_kConnect.Append(iV0);
    m_kConnect.Append(iV1);
    m_kConnect.Append(iV2);
}
//----------------------------------------------------------------------------
TriMesh* MultiattributeMesh::GetMesh (Vector2fArray*& rpkUV0, 
    Vector2fArray*& rpkUV1,Vector2fArray*& rpkUV2, Vector2fArray*& rpkUV3)
{
    int iVQuantity = m_kVerts.GetQuantity();
    if ( iVQuantity == 0 )
        return NULL;

    bool bHasNormals = true;
    bool bHasColors = true;
    bool bHasUV0s = true;
    bool bHasUV1s = true;
    bool bHasUV2s = true;
    bool bHasUV3s = true;

    for (int i = 0; i < m_kVerts.GetQuantity(); i++)
    {
        const Vertex& rkVertex = m_kVerts[i];
        if ( rkVertex[3] == Mathf::MAX_REAL )
            bHasNormals = false;
        if ( rkVertex[6] == Mathf::MAX_REAL )
            bHasColors = false;
        if ( rkVertex[10] == Mathf::MAX_REAL )
            bHasUV0s = false;
        if ( rkVertex[12] == Mathf::MAX_REAL )
            bHasUV1s = false;
        if ( rkVertex[14] == Mathf::MAX_REAL )
            bHasUV2s = false;
        if ( rkVertex[16] == Mathf::MAX_REAL )
            bHasUV3s = false;
    }

    Vector3f* akVertex = new Vector3f[iVQuantity];
    Vector3f* akNormal = (bHasNormals ? new Vector3f[iVQuantity] : NULL);
    ColorRGBA* akColor = (bHasColors ? new ColorRGBA[iVQuantity] : NULL);
    Vector2f* akUV0 = (bHasUV0s ? new Vector2f[iVQuantity] : NULL);
    Vector2f* akUV1 = (bHasUV1s ? new Vector2f[iVQuantity] : NULL);
    Vector2f* akUV2 = (bHasUV2s ? new Vector2f[iVQuantity] : NULL);
    Vector2f* akUV3 = (bHasUV3s ? new Vector2f[iVQuantity] : NULL);

    for (int i = 0; i < m_kVerts.GetQuantity(); i++)
    {
        const Vertex& rkVertex = m_kVerts[i];
        akVertex[i].X() = rkVertex[0];
        akVertex[i].Y() = rkVertex[1];
        akVertex[i].Z() = rkVertex[2];

        if ( bHasNormals )
        {
            akNormal[i].X() = rkVertex[3];
            akNormal[i].Y() = rkVertex[4];
            akNormal[i].Z() = rkVertex[5];
        }

        if ( bHasColors )
        {
            akColor[i].R() = rkVertex[6];
            akColor[i].G() = rkVertex[7];
            akColor[i].B() = rkVertex[8];
            akColor[i].A() = rkVertex[9];
        }

        if ( bHasUV0s )
        {
            akUV0[i].X() = rkVertex[10];
            akUV0[i].Y() = rkVertex[11];
        }

        if ( bHasUV1s )
        {
            akUV1[i].X() = rkVertex[12];
            akUV1[i].Y() = rkVertex[13];
        }

        if ( bHasUV2s )
        {
            akUV2[i].X() = rkVertex[14];
            akUV2[i].Y() = rkVertex[15];
        }

        if ( bHasUV3s )
        {
            akUV3[i].X() = rkVertex[16];
            akUV3[i].Y() = rkVertex[17];
        }
    }

    rpkUV0 = (bHasUV0s ? new Vector2fArray(iVQuantity,akUV0) : NULL);
    rpkUV1 = (bHasUV1s ? new Vector2fArray(iVQuantity,akUV1) : NULL);
    rpkUV2 = (bHasUV2s ? new Vector2fArray(iVQuantity,akUV2) : NULL);
    rpkUV3 = (bHasUV3s ? new Vector2fArray(iVQuantity,akUV3) : NULL);

    int iTQuantity = (int)(m_kConnect.GetQuantity()/3);
    int* aiConnect = new int[m_kConnect.GetQuantity()];
    memcpy(aiConnect,m_kConnect.GetArray(),m_kConnect.GetQuantity()*sizeof(int));

    Vector3fArrayPtr spkVertices = new Vector3fArray(iVQuantity,akVertex);
    IntArrayPtr spkIndices = new IntArray(iTQuantity*3,aiConnect);
    TriMesh* pkMesh = new TriMesh(spkVertices, spkIndices, !bHasNormals);
    if (bHasNormals)
    {
        Vector3fArrayPtr spkNormals = new Vector3fArray(iVQuantity, akNormal);
        pkMesh->Normals = spkNormals;
    }

    return pkMesh;
}
//----------------------------------------------------------------------------
MeshData MultiattributeMesh::GetData()
{
    int iNewVerts = m_kVerts.GetQuantity() - iOriginalVerts;
    if (iNewVerts <= 0)
    {
        return MeshData(0,NULL);
    }
    else
    {
        // make a copy of this data
        int* aiData = new int[iNewVerts];
        for (int i = 0; i < iNewVerts; i++)
        {
            aiData[i] = m_kVertMapping[i];
        }
        IntArrayPtr spkMapping = new IntArray(iNewVerts,aiData);
        return MeshData(iOriginalVerts,spkMapping);
    }
}
//----------------------------------------------------------------------------