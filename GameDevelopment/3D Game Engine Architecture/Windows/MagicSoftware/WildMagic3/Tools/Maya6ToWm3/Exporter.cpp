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

#include "Wm3BumpMapEffect.h"
#include "Wm3KeyframeController.h"

#include <maya/MDagPath.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MGlobal.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>

#include <fstream>
using namespace std;
using namespace Wm3;

//----------------------------------------------------------------------------
Exporter::Exporter ()
    :m_kJointHash(10),m_kMap(10),m_kMeshDataMap(1)
{
    // Default flags
    m_abFlags[FLAG_OBJFILE] = true;
    m_abFlags[FLAG_TEXTFILE] = false;
    m_abFlags[FLAG_QUIET] = false;
    m_abFlags[FLAG_COLLAPSE] = false;
    m_abFlags[FLAG_CAMERAS] = true;
    m_abFlags[FLAG_INVISIBLE] = false;
    m_abFlags[FLAG_LIGHTS] = true;
    m_abFlags[FLAG_TEXTURES] = true;
    m_abFlags[FLAG_BUMP] = true;
    m_abFlags[FLAG_KEYS] = true;
    m_abFlags[FLAG_SKIN] = true;
    m_abFlags[FLAG_BIND] = true;
    m_abFlags[FLAG_CALC_NORM] = true;
}
//----------------------------------------------------------------------------
void Exporter::DoExport(const char* acFilename)
{
    m_bSuccess = true;

    // Clean up invalid flag options
    if (!m_abFlags[FLAG_TEXTURES] || !m_abFlags[FLAG_LIGHTS])
        m_abFlags[FLAG_BUMP] = false;

    // Reset data structures
    m_kWMCamera.RemoveAll();
    m_kWMBumpMap.RemoveAll();
    m_kWMLight.RemoveAll();
    m_kMap.RemoveAll();
    m_kJointHash.RemoveAll();
    m_spkWMScene = NULL;
    m_bObjectInvisible = false;

    // Make text file first since it is less likely to fail.
    if (m_abFlags[FLAG_TEXTFILE])
        ExportTextFile(acFilename);

    if (m_abFlags[FLAG_OBJFILE])
        ExportObjectFile(acFilename);
}
//----------------------------------------------------------------------------
void Exporter::ExportObjectFile(const char* acFilename)
{
    MItDag* pkMayaScene = new MItDag(MItDag::kDepthFirst,MFn::kInvalid,
        &m_kStatus);
    assert( m_kStatus );

    // Dagpath height under which nodes are invisible (0 if all visible)
    int iInvisibleHeight = 0;

    // Main pass: Depth first walkthrough of scene graph
    for (/**/;!pkMayaScene->isDone(); pkMayaScene->next())
    {
        MDagPath kDagPath;
        m_kStatus = pkMayaScene->getPath(kDagPath);
        assert( m_kStatus );
        MFnDagNode kDagNode(kDagPath,&m_kStatus);
        assert( m_kStatus );
        MFn::Type eType = kDagNode.object().apiType();
        DisplayInfo(MString("--- Converting ") + 
            MString(TYPE_STRING[eType]) + " " + 
            kDagNode.name().asChar() + " ---");

        int iCurrentHeight = (int)kDagPath.length();
        // Make the path stack the correct height so that the correct
        // parent node is on top of the stack.
        while (iCurrentHeight < m_kParentStack.GetQuantity())
        {
            m_kParentStack.Remove(m_kParentStack.GetQuantity()-1);
        }
        
        if (m_abFlags[FLAG_INVISIBLE])
        {
            // If convert invisible flag, then convert all objects
            m_bObjectInvisible = false;
            (this->*CONVERT[eType])(kDagNode);
        }
        else if (!iInvisibleHeight || iCurrentHeight <= iInvisibleHeight)
        {
            // If this object is not part of an invisible subtree...

            // reset height flag and invisible flag
            iInvisibleHeight = 0;
            m_bObjectInvisible = false;

            // convert object
            (this->*CONVERT[eType])(kDagNode);

            // Was this object invisible? If so set the height boundary.
            if (m_bObjectInvisible)
            {
                iInvisibleHeight = iCurrentHeight;
            }
        }
        else
        {
            // If in an invisible subtree...
            assert(m_bObjectInvisible);
            (this->*CONVERT[eType])(kDagNode);
        }  
    }

    // Need to update world transforms before the skin controller pass
    m_spkWMScene->UpdateGS();

    // Second pass for skin (which needs the joints from the DAG
    // and also whose controllers are not in the DAG itself)
    if (m_abFlags[FLAG_SKIN])
    {
        MItDependencyNodes* pkSkinScene = new MItDependencyNodes(
            MFn::kSkinClusterFilter,&m_kStatus);
        assert( m_kStatus );
        for (/**/; !pkSkinScene->isDone(); pkSkinScene->next())
        {        
            // Sanity check
            assert( pkSkinScene->item().apiType() == MFn::kSkinClusterFilter );
            CSkinClusterFilter(pkSkinScene->item());
        }

        delete pkSkinScene;
    }

    // Deferred bumpmaps (because lights need to be known
    // at bumpmap creation time).
    if (m_kWMBumpMap.GetQuantity() > 0)
    {
        Light* pkFirstLight = NULL;
        for(int i = 0; i < m_kWMLight.GetQuantity(); i++)
        {
            // find the first non-ambient light
            if (m_kWMLight[i]->Type != Light::LT_AMBIENT)
            {
                pkFirstLight = m_kWMLight[i];
                break;
            }
        }

        if (!pkFirstLight)
        {
            DisplayError("No non-ambient lights in the scene."\
                "  Ignoring all bump map effects.");
        }
        else if (m_abFlags[FLAG_BUMP])
        {
            // Process all bump map effects
            for(int b = 0; b < m_kWMBumpMap.GetQuantity(); b++)
            {
                // default light to use will be the first one in the
                // scene
                Light* pkBumpLight = pkFirstLight;
                InstanceData* pkFound = m_kMap.Find(m_kWMBumpMap[b].MeshKey);
                
                if (!pkFound || pkFound->Spats.GetQuantity() <= 0)
                {
                    DisplayInfo("--- Converting BumpMapEffect ---");
                    DisplayError("Couldn't find mesh for bump map effect.");
                    continue;
                }
                MString kObjName(pkFound->GetFirst()->GetName());

                DisplayInfo("--- Converting BumpMapEffect for " +
                    kObjName + "---");

                // find a light for this bump map
                for (int i = 0; i < m_kWMLight.GetQuantity() ;i++)
                {
                    // If a light has the name of the object as a suffix,
                    // then that light will be used.
                    MString kLightName(m_kWMLight[i]->GetName());
                    MString kSuffix = kLightName.substring(
                        kLightName.length()-kObjName.length(),
                        kLightName.length());

                    if (kObjName == kSuffix)
                    {
                        pkBumpLight = m_kWMLight[i];
                        break;
                    }
                }

                if (pkBumpLight->GetName())
                {
                    DisplayInfo("Using light " + 
                        MString(pkBumpLight->GetName()));
                }

                BumpMapEffect* pkEffect = new BumpMapEffect(
                    m_kWMBumpMap[b].Primary,m_kWMBumpMap[b].PrimaryUVs,
                    m_kWMBumpMap[b].NormalMap,m_kWMBumpMap[b].NormalUVs,
                    pkBumpLight);

                // Set bumpmap for object and its instances
                pkFound->Spats[0]->SetEffect(pkEffect);
                for (int i = 1; i < pkFound->Spats.GetQuantity(); i++)
                    pkFound->Spats[i]->SetEffect(pkEffect->Clone());
            }
        }
    }

    // Add the lights to the scene
    for (int iL = 0; iL < m_kWMLight.GetQuantity(); iL++)
    {
        m_spkWMScene->SetLight(m_kWMLight[iL]);
    }
    
    m_spkWMScene->UpdateGS();

    Stream kOut;
    kOut.Insert(m_spkWMScene);
    kOut.Save(acFilename);

    delete pkMayaScene;
}
//----------------------------------------------------------------------------
void Exporter::ExportTextFile(const char* acFilename)
{
    MItDag* pkMayaScene = new MItDag(MItDag::kDepthFirst,MFn::kInvalid,
        &m_kStatus);
    assert( m_kStatus );

    MString kStr(acFilename);
    kStr += "-data.txt";
    ofstream kOStr(kStr.asChar());

    for (/**/;!pkMayaScene->isDone(); pkMayaScene->next() )
    {
        MDagPath kDagPath;
        m_kStatus = pkMayaScene->getPath(kDagPath);
        assert( m_kStatus );

        m_kStatus.clear();
        MFnDagNode kDagNode(kDagPath,&m_kStatus);
        assert( m_kStatus );

        unsigned int uiLength = kDagPath.length(&m_kStatus);
        assert( m_kStatus );
        for (unsigned int i = 0; i < 4*uiLength; i++)
            kOStr << ' ';

        MFn::Type eType = kDagNode.object().apiType();
        kOStr << TYPE_STRING[eType] << " (" <<
            kDagNode.name().asChar() << ")" << endl;
    }
    delete pkMayaScene;
    kOStr.close();
}
//----------------------------------------------------------------------------
Exporter::~Exporter ()
{
    m_spkWMScene = NULL;

    m_kWMBumpMap.RemoveAll();
    m_kWMCamera.RemoveAll();
    m_kWMLight.RemoveAll();
}
//----------------------------------------------------------------------------
int Exporter::GetKey (const MObject& rkObject) const
{
    int iKey = *(int*)&rkObject;
    return iKey;
}
//----------------------------------------------------------------------------
void Exporter::AttachChainToParent (const MFnDagNode& rkDagNode, 
    Spatial* pkFirst, Spatial* pkLast, bool bAttachToWorld)
{
    // Attach a chain to its parent, insert the node into the object map,
    // and put the end of the chain onto the parent stack.

    // rkDagNode is the Maya node that pkChild was created from.
    // pkFirst is the head of the chain that will be attached to the chain's
    //   parent.
    // pkLast is the tail of the chain.  Other objects that attach to
    //   rkDagNode will be attached to pkLast.
    // bAttachToWorld is the boolean flag for if this
    //   object should be attached to the world or to its correct parent.

    // Scene must be created and the world node may not be a chain
    assert (m_spkWMScene);
    assert (m_spkWMScene != pkFirst);
    assert (m_spkWMScene != pkLast);

    if (bAttachToWorld)
    {
        // Attaching to the world instead of parent
        m_spkWMScene->AttachChild(pkFirst);
    }
    else
    {
        // Attaching to a parent
        assert (m_kParentStack.GetQuantity() > 0);
        Spatial* pkParent = m_kParentStack[m_kParentStack.GetQuantity()-1];
        assert (pkParent->IsDerived(Node::TYPE));
        Node* pkParentN = StaticCast<Node>(pkParent);
        pkParentN->AttachChild(pkFirst);
    }

    int iKey = GetKey(rkDagNode.object());
    InstanceData* pkFound = m_kMap.Find(iKey);
    if (pkFound)
    {
        DisplayError("Chain has already been created.  Ignoring chain.");
    }
    else
    {
        m_kMap.Insert(iKey,InstanceData(pkLast));
    }

    m_kParentStack.Append(pkLast);
}
//----------------------------------------------------------------------------
void Exporter::AttachToParent (const MFnDagNode& rkDagNode, Spatial* pkChild,
    bool bAttachToWorld)
{
    // Attach an object to its parent, insert the node into the object map,
    // and put the object onto the parent stack.

    // rkDagNode is the Maya object that pkChild was created from.
    //
    // This should be called for the world node, though m_spkWMScene should
    // already be set to the world node before the call.  It will insert
    // the world node into the map and parent stack, but won't attach it
    // anywhere.  This slightly special behavior is to avoid having two
    // functions (Attach and InsertIntoMap) which need to be called
    // in a particular order every time.

    if (!m_spkWMScene)
    {
        DisplayError("No scene to attach to!");
    }
    else if (m_spkWMScene == pkChild)
    {
        // Special case for the world, don't do anything
    }
    else if (bAttachToWorld)
    {
        // Attaching to the world instead of parent
        m_spkWMScene->AttachChild(pkChild);
    }
    else
    {
        // Attaching to a parent
        assert (m_kParentStack.GetQuantity() > 0);
        Spatial* pkParent = m_kParentStack[m_kParentStack.GetQuantity()-1];
        assert (pkParent->IsDerived(Node::TYPE));
        Node* pkParentN = StaticCast<Node>(pkParent);
        pkParentN->AttachChild(pkChild);
    }

    int iKey = GetKey(rkDagNode.object());
    InstanceData* pkFound = m_kMap.Find(iKey);
    if (pkFound)
    {
        pkFound->Spats.Append(pkChild);
    }
    else
    {
        m_kMap.Insert(iKey,InstanceData(pkChild));
    }

    m_kParentStack.Append(pkChild);
}
//----------------------------------------------------------------------------