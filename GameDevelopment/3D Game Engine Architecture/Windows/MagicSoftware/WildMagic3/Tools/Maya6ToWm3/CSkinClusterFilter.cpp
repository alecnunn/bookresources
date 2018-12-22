// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#include "Exporter.h"
#include "Wm3SkinController.h"
#include "SkinControllerData.h"
#include "MeshData.h"

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MFnTransform.h>
#include <maya/MItGeometry.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MMatrix.h>

using namespace Wm3;

//----------------------------------------------------------------------------
void Exporter::CSkinClusterFilter(const MFnDagNode& rkDagNode)
{
    // empty
    // This is the function that would get called during the
    // normal tree traversal.
}
//----------------------------------------------------------------------------
void Exporter::ConvertSkinController(const MFnSkinCluster& rkCluster,
    const MFnMesh& rkOutputMesh, int iOutputGeomKey,
    MDagPath& rkMeshDagPath,MDagPathArray& rkJoints)
{
    // Helper function for converting a skin controller, based on
    // this SkinCluster (rkCluster), the mesh that the skin controller
    // controls (rkOutputMesh), that same mesh in the scene graph (rkMesh),
    // the DAG Path to the skin controller (rkMeshDagPath), and an array of
    // joints (rkJoints).

    // It is assumed that skin clusters are converted only after
    // all the joints have been added into the scene graph.

    InstanceData* pkInstData = m_kMap.Find(iOutputGeomKey);

    if (!pkInstData)
    {
        DisplayError("Couldn't locate output geometry! Ignoring controller.");
        return;
    }

    Spatial* pkSpat = pkInstData->GetFirst();
    if (!pkSpat->IsDerived(TriMesh::TYPE))
    {
        DisplayError("Skin controller attached to non-trimesh! Ignoring.");
        return;
    }
    TriMesh* pkMesh = StaticCast<TriMesh>(pkSpat);

    // Look up all the bones, and add them into a bones array.
    int iBoneQuantity = rkJoints.length();
    Node** apkBones = new Node*[iBoneQuantity];
    MMatrix* akBindPose = new MMatrix[iBoneQuantity];

    // Look up joint information
    for(int i = 0; i < (int)rkJoints.length(); i++)
    {
        // Look up joints in map by full path name
        MString kMName = rkJoints[i].fullPathName(&m_kStatus);
        assert(m_kStatus);
        String kWMName(kMName.asChar());

        Node** ppkJoint = m_kJointHash.Find(kWMName);

        // If influence objects have not been added into the joint array, 
        // ignore this skin controller.
        if (!ppkJoint)
        {
            DisplayError("Joint " + kMName + " not found in joint"\
                " array.  Ignoring entire controller.");
            delete[] apkBones;
            delete[] akBindPose;
            return;
        }
        else
        {
            apkBones[i] = *ppkJoint;

            // From: http://greggman.com/pages/mayastuff.htm#r2
            // Need to get the bind pose from Maya, even if the artist has
            // accidently deleted the explicit bind pose.
            MPlug kBindPreMatrixArrayPlug = rkCluster.findPlug("bindPreMatrix",
                &m_kStatus);
            assert(m_kStatus);

            MPlug kBindPreMatrixPlug = 
                kBindPreMatrixArrayPlug.elementByLogicalIndex(i,
                &m_kStatus);
            assert(m_kStatus);

            MObject kDataObj;
            kBindPreMatrixPlug.getValue(kDataObj);
            MFnMatrixData kMatDataFn(kDataObj, &m_kStatus);
            MMatrix kWorldMatInv = kMatDataFn.matrix();

            // Store the bone's world inv transform in an array
            akBindPose[i] = kWorldMatInv;
        }
    }

    // Look up mesh data (redundant vertex mapping)
    MeshData* pkMeshData = m_kMeshDataMap.Find(pkMesh);
    if (!pkMeshData)
    {
        delete[] apkBones;
        delete[] akBindPose;
        return;
    }

	MItGeometry kGeometryIt( rkOutputMesh.object() );

    int iFoundVertices = 0;
    int iVertexQuantity = pkMesh->Vertices->GetQuantity();

    // Make sure original num verts == output num verts
    if (pkMeshData->GetNumOrigVerts() != rkOutputMesh.numVertices())
    {
        DisplayError("Bind pose skin num vertices does not"\
            "equal output num vertices.");
        delete[] apkBones;
        delete[] akBindPose;
        return;
    }

    float** aafWeight = new float*[iVertexQuantity];
    Vector3f** aakOffset = new Vector3f*[iVertexQuantity];

    // Set these pointers to NULL so that if there is an
    // error midway through, we know which ones to delete
    // and which ones not to.
    memset(aafWeight,0,sizeof(float*)*iVertexQuantity);
    memset(aakOffset,0,sizeof(Vector3f*)*iVertexQuantity);

    bool bError = false;

    MPlug kNormalizePlug = rkCluster.findPlug("normalizeWeights",&m_kStatus);
    bool bNormalize = false;
    kNormalizePlug.getValue(bNormalize);

    // Iterate through vertices
	for(/**/; !kGeometryIt.isDone();kGeometryIt.next())
	{
        // Create arrays for each vertex
        aafWeight[iFoundVertices] = new float[iBoneQuantity];
        aakOffset[iFoundVertices] = new Vector3f[iBoneQuantity];

        // Get vertex
		MObject kComponent = kGeometryIt.component(&m_kStatus);
        assert(m_kStatus);

        // Get weights for this vertex
        MFloatArray kWeights;
        unsigned int uiNumInfluences;
        m_kStatus = rkCluster.getWeights(rkMeshDagPath,kComponent,kWeights,
            uiNumInfluences);
        assert(m_kStatus);

        // Error checking
        if (iFoundVertices > iVertexQuantity)
        {
            // error message later
            // This break will avoid overrunning the weight and offset arrays
            break;
        }
        if (kWeights.length() != iBoneQuantity)
        {
            DisplayError("Vertex has a different number of"\
                " influences than number of joints.");
            bError = true;
            break;
        }

        // Now add these values into the weight/offset arrays
        int b;
        for(b = 0; b < iBoneQuantity; b++)
        {
            aafWeight[iFoundVertices][b] = kWeights[b];
        }

        if (bNormalize)
        {
            // Normalize weights
            float fWeightSum = aafWeight[iFoundVertices][0];
            for(b = 1; b < iBoneQuantity; b++)
            {
                fWeightSum += aafWeight[iFoundVertices][b];
            }
            for(b = 0; b < iBoneQuantity; b++)
            {
                aafWeight[iFoundVertices][b] /= fWeightSum;
            }
        }

        // Now calculate the offset by applying the inverse joint
        // transform to the mesh vertices in world space.  We use
        // the input mesh vertices with the output mesh world transform
        // to get the correct worldspace coordinates for the bind
        // pose skin.
        Vector3f kMeshVertex = (*pkMesh->Vertices)[iFoundVertices];
        kMeshVertex = pkMesh->World.ApplyForward(kMeshVertex);

        for(b = 0; b < iBoneQuantity; b++)
        {
            // Use Maya classes to multiply and then get back the
            // information.
            MPoint kMPoint((double)kMeshVertex.X(),
                (double)kMeshVertex.Y(),(double)kMeshVertex.Z());
            // Maya matrices are post-multiplied.
            kMPoint *= akBindPose[b];
            
            aakOffset[iFoundVertices][b] = Vector3f((float)kMPoint.x,
                (float)kMPoint.y,(float)kMPoint.z);
        }
        iFoundVertices++;
    }

    // bind pose no longer needed
    delete[] akBindPose;

    // Error checking
    if (iFoundVertices != pkMeshData->GetNumOrigVerts())
    {
        MString kError = "Vertices in skin controller (";
        kError += iFoundVertices;
        kError += ") do not match the number in the mesh (";
        kError += iVertexQuantity;
        kError += ").";
        DisplayError(kError);
        bError = true;
    }

    // Deal with the extra vertices.  Spatially, they are the
    // same as the vertex they are mapped to and thus will have
    // the same weight and offset as that vertex.
    int iExtraVerts = iVertexQuantity - iFoundVertices;
    for(int iV = 0; iV < iExtraVerts; iV++)
    {
        int iMap = pkMeshData->GetMapping(iV);
        aafWeight[iFoundVertices] = aafWeight[iMap];
        aakOffset[iFoundVertices] = aakOffset[iMap];
        iFoundVertices++;
    }

    if (iFoundVertices != iVertexQuantity)
    {
        MString kError = "Vertices in skin controller (";
        kError += iFoundVertices;
        kError += ") do not match the number in the mesh (";
        kError += iVertexQuantity;
        kError += ").";
        DisplayError(kError);
        bError = true;
    }

    if (bError)
    {
        DisplayWarning("Skin controller being ignored for mesh " +
            rkOutputMesh.name());

        // clean up memory
        for(int i = 0; i < iVertexQuantity; i++)
        {
            if (aafWeight[i])
                delete[] aafWeight[i];
            if (aakOffset[i])
                delete[] aakOffset[i];
        }
        delete[] aafWeight;
        delete[] aakOffset;
        delete[] apkBones;
    }
    else
    {
        // Now attach this controller to the first mesh
        // Only the first mesh needs the controller, as the other
        // instances will share the vertex data.
        SkinController* pkCon = new SkinController(iVertexQuantity,
            iBoneQuantity, apkBones, aafWeight, aakOffset);
        pkMesh->SetController(pkCon);
        DisplayInfo("Attaching controller to mesh " + rkOutputMesh.name() + 
            ".");
    }
}
//----------------------------------------------------------------------------
void Exporter::CSkinClusterFilter(const MObject& rkObject)
{
    if (!m_abFlags[FLAG_SKIN])
        return;

    // This function converts a non-DAG object, which is an MFnSkinCluster
    // object.  This function takes this kind of object and not a DagNode
    // because it is not part of the scene DAG.

    if (rkObject.apiType() != MFn::kSkinClusterFilter)
    {
        DisplayError("Not a skin cluster object!");
        return;
    }

    MFnSkinCluster kFnCluster(rkObject,&m_kStatus);
    assert(m_kStatus);

    DisplayInfo("--- Converting SkinClusterFilter " + 
        kFnCluster.name() + " ---");

    // Get the joints
    MDagPathArray kPathArray;
    kFnCluster.influenceObjects(kPathArray,&m_kStatus);
    assert(m_kStatus);

    // If no influence objects (i.e. joints), quit.
    if (kPathArray.length() <= 0)
    {
        DisplayWarning("Skin controller, but no influence objects. "\
            "Ignoring controller.");
        return;
    }

    // Find the mesh(es) that this controller is attached to
    MObjectArray kOutputGeom;
    m_kStatus = kFnCluster.getOutputGeometry(kOutputGeom);
    assert(m_kStatus);

    // Add the skin controller to each mesh...
    for(int g = 0; g < (int)kOutputGeom.length(); g++)
    {
        // Get the output mesh
        MFnMesh kOutputMesh(kOutputGeom[g],&m_kStatus);
        if (!m_kStatus)
        {
            DisplayError("Couldn't get output mesh.");
            continue;
        }

        int iOutputGeomKey = GetKey(kOutputGeom[g]);

        unsigned int uiPlugIndex = kFnCluster.indexForOutputConnection(
            (unsigned int)g,&m_kStatus);
        assert(m_kStatus);

        MDagPath kMeshDagPath;
        m_kStatus = kFnCluster.getPathAtIndex(uiPlugIndex, kMeshDagPath);

        // Now that we have all the info we need, we can go ahead
        // and convert this skin cluster.
        ConvertSkinController(kFnCluster,kOutputMesh,
            iOutputGeomKey,kMeshDagPath,kPathArray);
    }
 }
//----------------------------------------------------------------------------

