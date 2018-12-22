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
#include "iparamb2.h"
#include "iskin.h"

//----------------------------------------------------------------------------
void WSceneBuilder::ProcessSkin (INode* pkMaxNode, Modifier* pkSkinMod)
{
    // Construct skin controller.  If the Max mesh required splitting during
    // the export, each Magic mesh created during the split will need its own
    // skin controller.  The skin vertex offsets are calculated using the
    // world transformed mesh vertices at the animation's starting time, and
    // the bones' world transforms.
    //
    // pkMaxNode:
    //     Pointer to Max node to which skin modifier is applied.
    // pkSkinMod:
    //     Pointer to the skin modifier object.

    bool bNeedDel;
    TriObject* pkTriObj = GetTriObject(pkMaxNode,&bNeedDel);
    Mesh* pkMesh = &pkTriObj->GetMesh();

    // get interface to MAX's skin controller classes
    ISkin* pkSkin = (ISkin*)pkSkinMod->GetInterface(I_SKIN);
    ISkinContextData* pkSkinData = pkSkin->GetContextInterface(pkMaxNode);

    // Loop through bones, storing the initial bone rotations and
    // translations, and retrieving the corresponding Magic nodes from the
    // scene.
    int iB, iBoneQuantity = pkSkin->GetNumBones();
    Wm3::Node** apkWm3Bones = new Wm3::Node*[iBoneQuantity]; 
    for (iB = 0; iB < iBoneQuantity; iB++)
    {
        INode* pkBoneNode = pkSkin->GetBone(iB);
        apkWm3Bones[iB] = Wm3::StaticCast<Wm3::Node>(
            m_spkScene->GetObjectByName(pkBoneNode->GetName()));
    }

    // Check whether the Magic node corresponding to the Max skin mesh is
    // itself a mesh node (in which case, the MAX mesh didn't require
    // splitting earlier in the export) or whether the node is a "link" node
    // (i.e. the parent node of a number of split meshes).
    vector<Wm3::TriMesh*> akWm3Meshes;
    Wm3::Object* pkObject = m_spkScene->GetObjectByName(pkMaxNode->GetName());
    if ( pkObject->IsExactly(Wm3::TriMesh::TYPE) )
    {
        akWm3Meshes.push_back(Wm3::StaticCast<Wm3::TriMesh>(pkObject));
    }
    else
    {
        Wm3::Node* pkNode = Wm3::StaticCast<Wm3::Node>(pkObject);
        const char* acName = pkNode->GetName();
        for (int iC = 0; iC < pkNode->GetQuantity(); iC++)
        {
            Wm3::Spatial* pkChild = pkNode->GetChild(iC);
            const char* acCName = pkChild->GetName();
            if ( strncmp(acCName,acName,strlen(acName)) == 0 )
                akWm3Meshes.push_back(Wm3::StaticCast<Wm3::TriMesh>(pkChild));
        }
    }

    // Build a skin controller for each of the Magic meshes corresponding to
    // the Max mesh.
    int* aiVerticesPerBone = new int[iBoneQuantity];
    for (int iM = 0; iM < (int)akWm3Meshes.size(); iM++)
    {
        Wm3::TriMesh* pkWm3Mesh = akWm3Meshes[iM];

        // Determine which vertices from the Max mesh are contained in this
        // Magic mesh, and store the Max vertex indices.  TO DO:  The
        // comparison is vertex to vertex.  What if the original vertex
        // array had duplicate vertices?  The hash data structures used in
        // PackVertices should be persistent so that we may use them here
        // rather than doing a potentially expensive double loop.
        int iVQuantity = pkWm3Mesh->Vertices->GetQuantity();
        Wm3::Vector3f* pkV = pkWm3Mesh->Vertices->GetData();
        vector<int> kVIArray;
        int iV, i, j, k;
        for (iV = 0; iV < iVQuantity; iV++, pkV++)
        {
            for (i = 0; i < pkMesh->getNumVerts(); i++)
            {
                if ( pkMesh->verts[i].x == pkV->X()
                &&   pkMesh->verts[i].y == pkV->Y()
                &&   pkMesh->verts[i].z == pkV->Z() )
                {
                    kVIArray.push_back(i);
                    break;
                }
            }
        }

        // determine how many vertices are influenced by each bone
        iVQuantity = (int)kVIArray.size();
        memset(aiVerticesPerBone,0,sizeof(int)*iBoneQuantity);
        for (i = 0; i < iVQuantity; i++)
        {
            iV = kVIArray[i];
            for (j = 0; j < pkSkinData->GetNumAssignedBones(iV); j++)
            {
                iB = pkSkinData->GetAssignedBone(iV,j);
                aiVerticesPerBone[iB]++;
            }
        }

        // If the Max mesh has been split, then it is possible that some
        // bones will have no influence on the current Magic mesh, hence we
        // need to determine the number of "active" bones for this mesh.
        int iBQuantity = 0;
        for (iB = 0; iB < iBoneQuantity; iB++)
        {
            if ( aiVerticesPerBone[iB] > 0 )
                iBQuantity++;
        }
        assert( iBQuantity > 0 );
        if ( iBQuantity == 0 )
        {
            // mesh is not affected by any bones, go to the next mesh
            continue;
        }

        // allocate the arrays for the skin controller for this mesh
        Wm3::Node** apkBones = new Wm3::Node*[iBQuantity];
        float** aafWeight;
        Wm3::System::Allocate(iBQuantity,iVQuantity,aafWeight);
        memset(aafWeight[0],0,iBQuantity*iVQuantity*sizeof(float));
        Wm3::Vector3f** aakOffset;
        Wm3::System::Allocate(iBQuantity,iVQuantity,aakOffset);
        memset(aakOffset[0],0,iBQuantity*iVQuantity*sizeof(Wm3::Vector3f));

        // Store the Magic node pointers corresponding to the active bones
        // for this Magic mesh.
        vector<int> kBIArray(iBoneQuantity);
        for (iB = 0, k = 0; iB < iBoneQuantity; iB++)
        {
            if ( aiVerticesPerBone[iB] > 0 )
            {
                apkBones[k] = apkWm3Bones[iB];
                kBIArray[iB] = k;
                k++;
            }
        }

        // Loop through the list of Max vertices for this Magic mesh,
        // obtaining the vertex weights and calculating the vertex offsets.
        Wm3::Vector3f* akVertex = pkWm3Mesh->Vertices->GetData();
        for (i = 0; i < iVQuantity; i++)
        {
            iV = kVIArray[i];
            for (j = 0; j < pkSkinData->GetNumAssignedBones(iV); j++)
            {
                iB = pkSkinData->GetAssignedBone(iV,j);
                k = kBIArray[iB];
                aafWeight[i][k] = pkSkinData->GetBoneWeight(iV,j);
                aakOffset[i][k] = apkBones[k]->World.ApplyInverse(
                    pkWm3Mesh->World.ApplyForward(akVertex[i]));
            }
        }
        
        Wm3::SkinController* pkSkinController = new Wm3::SkinController(
            iVQuantity,iBQuantity,apkBones,aafWeight,aakOffset);
        pkSkinController->MinTime = 0.0f;
        pkSkinController->MaxTime = TicksToSec(m_iTimeEnd - m_iTimeStart);
        pkWm3Mesh->SetController(pkSkinController);
    }
    
    if ( bNeedDel ) 
        delete pkTriObj;

    delete[] apkWm3Bones;
    delete[] aiVerticesPerBone;
}
//----------------------------------------------------------------------------
