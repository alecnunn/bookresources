// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WSceneBuilder.h"
#include "WUnimaterialMesh.h"
#include <set>
using namespace std;

//----------------------------------------------------------------------------
TriObject* WSceneBuilder::GetTriObject (INode* pkNode, bool* pbNeedDel)
{
    // Determine whether or not a Max node has an associated mesh object.
    //
    // pkNode:
    //     Node to test.
    // pbNeedDel:
    //     True, if the caller should delete the returned mesh pointer after
    //     use, false otherwise.
    // Returns pointer to mesh object, or NULL, if the node has no associated
    // mesh.

    Object* pkObj = pkNode->EvalWorldState(m_iTimeStart).obj;

    // See if the object associated with this node can be converted to a
    // trimesh.
    if ( !pkObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) ) 
        return NULL;

    // do the conversion
    TriObject* pkTriObj = (TriObject*)pkObj->ConvertToType(m_iTimeStart,
        Class_ID(TRIOBJ_CLASS_ID,0));
    if ( pkTriObj == NULL ) 
        return NULL;

    // See whether or not the caller is responsible for deleting the mesh
    // pointer.
    *pbNeedDel = (pkTriObj != pkObj ? true : false);
    return pkTriObj;
}
//----------------------------------------------------------------------------
Wm3::Spatial* WSceneBuilder::BuildMesh (INode* pkMaxNode,
    Wm3::Spatial* pkWm3Node)
{
    // Convert a Max trimesh to one or more equivalent Magic trimeshes.
    //
    // pkMaxNode:
    //     Mesh node in the Max hierarchy.
    // pkWm3Node:
    //     Parent node in Magic scene to which newly created.
    // Returns pointer to new child node in the Magic scene.  This will point
    // directly to a trimesh object, if there is only one Magic mesh, or to
    // a "link" node, whose children are the mulitple trimeshes needed to
    // represent the Max mesh.

    bool bNeedDel = false;
    TriObject* pkTriObj = GetTriObject(pkMaxNode,&bNeedDel);
    if ( !pkTriObj ) 
        return NULL;
    Mesh* pkMaxMesh = &pkTriObj->GetMesh();

    Mtl* pkMtl = pkMaxNode->GetMtl();
    assert( pkMtl );
    int iMtlID = m_kMtlList.GetID(pkMtl);
    assert( iMtlID >= 0 );

    Wm3::Spatial* pkLink;
    bool bHasLink = (pkWm3Node->GetName().GetLength() > 0 &&
        strcmp(pkMaxNode->GetName(),pkWm3Node->GetName()) == 0);
    char* acMaxName = pkMaxNode->GetName();
    int i;

    // If only a single Magic mesh is required to represent the Max mesh,
    // then link it directly to the parent node in the Magic scene, otherwise
    // create a "link" node, whose children will be the individual Magic
    // meshes needed to represent the Max mesh.
    vector<WUnimaterialMesh*> kUMeshes;
    SplitGeometry(pkMaxMesh,iMtlID,kUMeshes);
    if ( kUMeshes.size() > 1  )
    {
        if ( !bHasLink )
            pkLink = BuildSpatial(pkMaxNode,pkWm3Node);
        else 
            pkLink = pkWm3Node; 

        assert( pkLink->IsDerived(Wm3::Node::TYPE) );

        for (i = 0; i < (int)kUMeshes.size(); i++)
        {
            Wm3::TriMesh* pkTriMesh = kUMeshes[i]->ToTriMesh();
            char acMeshNumber[6];
            sprintf(acMeshNumber,"_%d",i + 1);
            char* acWm3Name =
                new char[strlen(acMaxName) + strlen(acMeshNumber) + 1];
            strcpy(acWm3Name,acMaxName);
            strcat(acWm3Name,acMeshNumber);         
            pkTriMesh->SetName(acWm3Name);
            delete[] acWm3Name;
            ((Wm3::Node*)pkLink)->AttachChild(pkTriMesh);
        }
    }
    else
    {
        Wm3::TriMesh* pkTriMesh = kUMeshes[0]->ToTriMesh();
        if ( !bHasLink )
        {
            pkTriMesh->SetName(acMaxName);
            pkTriMesh->Local = GetLocalTransform(pkMaxNode,m_iTimeStart);
        }
        else
        {
            char* acWm3Name = new char[strlen(acMaxName) + 3];
            strcpy(acWm3Name,acMaxName);
            strcat(acWm3Name,"_1");
            pkTriMesh->SetName(acWm3Name);
            delete[] acWm3Name;
        }
        assert( pkWm3Node->IsDerived(Wm3::Node::TYPE) );
        ((Wm3::Node*)pkWm3Node)->AttachChild(pkTriMesh);
        pkLink = pkTriMesh;
    }
    for (i = 0; i < (int)kUMeshes.size(); i++)
        delete kUMeshes[i];

    if ( bNeedDel ) 
        delete pkTriObj;

    return pkLink;
}
//----------------------------------------------------------------------------
void WSceneBuilder::SplitGeometry (Mesh* pkMaxMesh, int iMtlID, 
    vector<WUnimaterialMesh*>& rkUMeshes)
{
    // If a mesh is encountered that has multi-submaterials *and* uses two
    // or more submaterials, that mesh needs to be split since Wild Magic
    // only supports one material per object.
    //
    // pkMaxMesh:
    //     Pointer to Max mesh to be split.
    // rkUMeshes:
    //     Vector, which will receive the individual WUnimaterialMesh's
    //     required to represent the Max mesh.
    // bHasReflection:
    //     True, if the node's transform contains a relection, false
    //     otherwise.

    int i, j;

    // compute vertex normals
    Wm3::Vector3f* akNormal = NULL;
    if ( m_pkSettings->IncludeNormals )
    {
        pkMaxMesh->buildNormals();
        akNormal = new Wm3::Vector3f[pkMaxMesh->numVerts];
        for (i = 0; i < pkMaxMesh->numFaces; i++)
        {
            Face& rkFace = pkMaxMesh->faces[i];
            for (j = 0; j < 3; j++)
            {
                int iVertex = rkFace.getVert(j);
                akNormal[iVertex] = GetVertexNormal(pkMaxMesh,i,iVertex);
            }
        }       
    }

    // number of submaterials
    WMaterialTree& rkTree = m_kMtlTree[iMtlID];
    int iSubquantity = 0;
    if ( iMtlID >= 0 )
        iSubquantity = rkTree.GetMChildQuantity();

    // compute the maximum submaterial index used by the geometry object
    int iFace, iSubID, iMaxSubID = -1;
    for (iFace = 0; iFace < pkMaxMesh->numFaces; iFace++)
    {
        iSubID = pkMaxMesh->faces[iFace].getMatID();
        if ( iSubID >= iSubquantity )
        {
            if ( iSubquantity > 0 )
                iSubID = iSubID % iSubquantity;
            else
                iSubID = 0;
        }
        if ( iSubID > iMaxSubID )
            iMaxSubID = iSubID;
    }

    // partition the faces by material ID
    vector<int>* akPartition = new vector<int>[iMaxSubID+1];
    for (iFace = 0; iFace < pkMaxMesh->numFaces; iFace++)
    {
        iSubID = pkMaxMesh->faces[iFace].getMatID();
        if ( iSubID >= iSubquantity )
        {
            if ( iSubquantity > 0 )
                iSubID = iSubID % iSubquantity;
            else
                iSubID = 0;
        }
        akPartition[iSubID].push_back(iFace);
    }

    // allocate the unimaterial meshes
    for (iSubID = 0; iSubID <= iMaxSubID; iSubID++)
    {
        if ( akPartition[iSubID].size() == 0 )
        {
            // submaterial ID not used
            continue;
        }

        WUnimaterialMesh* pkMesh = new WUnimaterialMesh;
        if ( iMtlID >= 0 )
        {
            // TO DO.  For now, just grab the first nonnull texture.  Later
            // we should see multiple textures for special effects.
            if ( iSubquantity > 0 )
            {
                WMaterialTree& rkSubtree = rkTree.GetMChild(iSubID);
                pkMesh->MState() = rkSubtree.GetMaterial();
                for (i = 0; i < (int)rkSubtree.GetTChildQuantity(); i++)
                {
                    if ( rkSubtree.GetTexture(i) )
                    {
                        pkMesh->TState() = rkSubtree.GetTexture(i);
                        break;
                    }
                }
            }
            else
            {
                pkMesh->MState() = rkTree.GetMaterial();
                for (i = 0; i < (int)rkTree.GetTChildQuantity(); i++)
                {
                    if ( rkTree.GetTexture(i) )
                    {
                        pkMesh->TState() = rkTree.GetTexture(i);
                        break;
                    }
                }
            }
        }

        PackVertices(pkMesh,pkMaxMesh,akPartition[iSubID],akNormal);

        if ( m_pkSettings->IncludeVertexColors && pkMaxMesh->numCVerts > 0 )
            PackColors(pkMesh,pkMaxMesh,akPartition[iSubID]);

        if ( m_pkSettings->IncludeTexCoords
        &&   pkMaxMesh->numTVerts > 0
        &&   pkMesh->TState() )
        {
            PackTextures(pkMesh,pkMaxMesh,akPartition[iSubID]);
        }

        rkUMeshes.push_back(pkMesh);
    }

    delete[] akPartition;
    delete[] akNormal;

    // Duplicate vertex data to meet Wild Magic constraints on one
    // color/normal/uv per vertex position.
    for (i = 0; i < (int)rkUMeshes.size(); i++)
        rkUMeshes[i]->DuplicateGeometry();  
}
//----------------------------------------------------------------------------
void WSceneBuilder::PackVertices (WUnimaterialMesh* pkUniMesh,
    Mesh* pkMaxMesh, vector<int>& rkPartition, Wm3::Vector3f* akNormal)
{
    // Take a partition of a Max mesh (represented by a vector of face
    // indices), and pack the corresponding vertices and normals into a
    // WUnimaterialMesh.
    //
    // pkUniMesh:
    //     WUnimaterialMesh to pack.
    // pkMaxMesh:
    //     Max mesh from which to extract vertices.
    // rkPartition:
    //     Vector of face indices representing the mesh partition.
    // akNormal:
    //     Array of mesh normal vectors, from which to extract the partition's
    //     normals.

    // get vertex indices used by the partition
    set<int> kVIndex;
    int i, j;
    for (i = 0; i < (int)rkPartition.size(); i++)
    {
        Face& rkFace = pkMaxMesh->faces[rkPartition[i]];
        for (j = 0; j < 3; j++)
            kVIndex.insert(rkFace.v[j]);
    }

    // Pack into contiguous arrays.  The packing relies on the STL set having
    // elements in increasing order.  If kVIndex[i] = j, then aiVMap[j] = i.
    // The aiVMap array is made large enough to hold all possible j, but
    // aiVMap[j] = -1 if there is no corresponding i.
    int iVMax = *kVIndex.rbegin();
    int* aiVMap = new int[iVMax+1];
    memset(aiVMap,0xFF,(iVMax+1)*sizeof(int));

    pkUniMesh->VQuantity() = kVIndex.size();
    pkUniMesh->Vertex() = new Wm3::Vector3f[pkUniMesh->VQuantity()];
    pkUniMesh->Normal() = new Wm3::Vector3f[pkUniMesh->VQuantity()];

    set<int>::iterator kIter = kVIndex.begin();
    for (i = 0; i < (int)kVIndex.size(); i++, kIter++)
    {
        j = *kIter;
        aiVMap[j] = i;

        pkUniMesh->Vertex()[i].X() = pkMaxMesh->verts[j].x;
        pkUniMesh->Vertex()[i].Y() = pkMaxMesh->verts[j].y;
        pkUniMesh->Vertex()[i].Z() = pkMaxMesh->verts[j].z;
        pkUniMesh->Normal()[i] = akNormal[j];
    }

    // remap vertex face connectivity
    pkUniMesh->FQuantity() = rkPartition.size();
    pkUniMesh->Face() = new int[3*pkUniMesh->FQuantity()];
    for (i = 0; i < (int)rkPartition.size(); i++)
    {
        Face& rkFace = pkMaxMesh->faces[rkPartition[i]];
        for (j = 0; j < 3; j++)
            pkUniMesh->Face()[3*i+j] = aiVMap[rkFace.v[j]];
    }
    delete[] aiVMap;
}
//----------------------------------------------------------------------------
void WSceneBuilder::PackColors (WUnimaterialMesh* pkUniMesh, Mesh* pkMaxMesh, 
    vector<int>& rkPartition)
{
    // Take a partition of a Max mesh (represented by a vector of face
    // indices), and pack the corresponding vertex colors into a
    // WUnimaterialMesh.
    //
    // pkUniMesh:
    //     WUnimaterialMesh to pack.
    // pkMaxMesh:
    //     Max mesh from which to extract colors.
    // rkPartition:
    //     Vector of face indices representing the mesh partition.

    // get color indices used by the partition
    set<int> kCIndex;
    int i, j;
    for (i = 0; i < (int)rkPartition.size(); i++)
    {
        TVFace& rkCFace = pkMaxMesh->vcFace[rkPartition[i]];
        for (j = 0; j < 3; j++)
            kCIndex.insert(rkCFace.t[j]);
    }

    // Pack into contiguous arrays.  The packing relies on the STL set having
    // elements in increasing order.  If kCIndex[i] = j, then aiCMap[j] = i.
    // The aiCMap array is made large enough to hold all possible j, but
    // aiCMap[j] = -1 if there is no corresponding i.
    int iCMax = *kCIndex.rbegin();
    int* aiCMap = new int[iCMax+1];
    memset(aiCMap,0xFF,(iCMax+1)*sizeof(int));

    pkUniMesh->CQuantity() = kCIndex.size();
    pkUniMesh->Color() = new Wm3::ColorRGB[pkUniMesh->CQuantity()];

    set<int>::iterator kIter = kCIndex.begin();
    for (i = 0; i < (int)kCIndex.size(); i++, kIter++)
    {
        j = *kIter;
        aiCMap[j] = i;

        pkUniMesh->Color()[i].R() = pkMaxMesh->vertCol[j].x;
        pkUniMesh->Color()[i].G() = pkMaxMesh->vertCol[j].y;
        pkUniMesh->Color()[i].B() = pkMaxMesh->vertCol[j].z;
    }

    // remap color face connectivity
    pkUniMesh->CFace() = new int[3*pkUniMesh->FQuantity()];
    for (i = 0; i < (int)rkPartition.size(); i++)
    {
        TVFace& rkCFace = pkMaxMesh->vcFace[rkPartition[i]];
        for (j = 0; j < 3; j++)
            pkUniMesh->CFace()[3*i+j] = aiCMap[rkCFace.t[j]];
    }
    delete[] aiCMap;
}
//----------------------------------------------------------------------------
void WSceneBuilder::PackTextures (WUnimaterialMesh* pkUniMesh,
    Mesh* pkMaxMesh, vector<int>& rkPartition)
{
    // Take a partition of a Max mesh (represented by a vector of face
    // indices), and pack the corresponding texture coordinates into a
    // WUnimaterialMesh.
    //
    // pkUniMesh:
    //     WUnimaterialMesh to pack.
    // pkMaxMesh:
    //     Max mesh from which to extract texture coords.
    // rkPartition:
    //     Vector of face indices representing the mesh partition.

    // get texture indices used by the partition
    set<int> kTIndex;
    int i, j;
    for (i = 0; i < (int)rkPartition.size(); i++)
    {
        TVFace& rkTFace = pkMaxMesh->tvFace[rkPartition[i]];
        for (j = 0; j < 3; j++)
            kTIndex.insert(rkTFace.t[j]);
    }

    // Pack into contiguous arrays.  The packing relies on the STL set having
    // elements in increasing order.  If kTIndex[i] = j, then aiTMap[j] = i.
    // The aiTMap array is made large enough to hold all possible j, but
    // aiTMap[j] = -1 if there is no corresponding i.
    int iTMax = *kTIndex.rbegin();
    int* aiTMap = new int[iTMax+1];
    memset(aiTMap,0xFF,(iTMax + 1)*sizeof(int));

    pkUniMesh->TQuantity() = kTIndex.size();
    pkUniMesh->Texture() = new Wm3::Vector2f[pkUniMesh->TQuantity()];

    set<int>::iterator kIter = kTIndex.begin();
    for (i = 0; i < (int)kTIndex.size(); i++, kIter++)
    {
        j = *kIter;
        aiTMap[j] = i;

        pkUniMesh->Texture()[i].X() = pkMaxMesh->tVerts[j].x;
        pkUniMesh->Texture()[i].Y() = pkMaxMesh->tVerts[j].y;
    }

    // remap texture face connectivity
    pkUniMesh->TFace() = new int[3*pkUniMesh->FQuantity()];
    for (i = 0; i < (int)rkPartition.size(); i++)
    {
        TVFace& rkTFace = pkMaxMesh->tvFace[rkPartition[i]];
        for (j = 0; j < 3; j++)
            pkUniMesh->TFace()[3*i+j] = aiTMap[rkTFace.t[j]];
    }
    delete[] aiTMap;
}
//----------------------------------------------------------------------------
Wm3::Vector3f WSceneBuilder::GetVertexNormal (Mesh* pkMaxMesh, int iFace,
    int iVertex)
{
    // Get a vertex normal, for a specified Max mesh vertex index.
    //
    // pkMaxMesh:
    //     Max mesh containing the vertex.
    // iFace:
    //     Index of face to which vertex belongs.
    // iVertex:
    //     Index of vertex for which to retrieve normal.
    // returns the normal vector

    Point3 kNormal;

    RVertex* pkRV = pkMaxMesh->getRVertPtr(iVertex);
    
    if ( pkRV->rFlags & SPECIFIED_NORMAL )
    {
        kNormal = pkRV->rn.getNormal();
    }
    else
    {
        // If RVertex does not contain a specified normal vector, the normal
        // is calculated from a smoothing group, if it exists.  If no such
        // group exists, the face normal is used.
        Face& rkFace = pkMaxMesh->faces[iFace];
        DWORD dwSmoothingGroup = rkFace.smGroup;
        int iNumNormals = pkRV->rFlags & NORCT_MASK;
        if ( iNumNormals && dwSmoothingGroup )
        {
            if ( iNumNormals == 1 )
            {
                // only one normal exists in group, rkRV already stored it
                kNormal = pkRV->rn.getNormal();
            }
            else
            {
                // Lookup the normal from the smoothing group that contains
                // the face.
                for (int i = 0; i < iNumNormals; i++)
                {
                    if ( pkRV->ern[i].getSmGroup() & dwSmoothingGroup ){
                        kNormal = pkRV->ern[i].getNormal();
                        break;
                    }
                }
            }
        }
        else
        {
            // use the face normal when no smoothing group exists
            kNormal = pkMaxMesh->FaceNormal(iFace, true);
        }
    }

    return Wm3::Vector3f(kNormal.x,kNormal.y,kNormal.z);
}
//----------------------------------------------------------------------------
