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
#include "decomp.h"

const float WSceneBuilder::ATTENUATION = 1e+03f;
const float WSceneBuilder::MIN_DIFFERENCE = 1e-03f;

//----------------------------------------------------------------------------
WSceneBuilder::WSceneBuilder (const TCHAR* acFilename, BOOL bExportSelected, 
    WExportSettings* pkSettings, ExpInterface* pkExport, Interface* pkMax, 
    INode* pkRootNode)
{
    // acFilename:
    //     String containing the filename to which the scene graph should be
    //     saved.
    // bExportSelected:
    //     True if the user chose to export a selected portion of the scene
    //     hierarchy, false if the entire scene is to be exported.
    // pkSettings:
    //     ExportSettings pointer with details of the exporter options
    //     selected by the user.
    // pkExport:
    //     Interface pointer to MAX's exporter specific methods.
    // pkMax:
    //     Interface pointer to MAX's general functionality.
    // pkRootNode:
    //     Top level node in the hierarchy to be exported (not necessarily
    //     the root node of the MAX scene, if the user chose "Export
    //     Selected").

    m_acFilename = new TCHAR[_tcslen(acFilename) + 1];
    _tcscpy(m_acFilename,acFilename);
    m_bExportSelected = bExportSelected;
    m_pkSettings = pkSettings;
    m_pkExport = pkExport;
    m_pkMax = pkMax;
    m_pkRootNode = pkRootNode;  
    m_iTicksPerFrame = GetTicksPerFrame();

    if ( m_pkSettings->IncludeCurrentFrame )
    {
        // User has chosen to export the current frame only.  Get the time at
        // which it occurs.
        m_iTimeStart = m_pkMax->GetAnimRange().Start();
        m_iTimeEnd = m_iTimeStart;
    }
    else
    {
        // Calculate the start time in ticks, of the first and last frames.
        m_iTimeStart = m_pkMax->GetAnimRange().Start();
        m_iTimeEnd = m_pkMax->GetAnimRange().End();
        if ( m_pkSettings->UseLocalTime )
            m_iTimeOffset = m_iTimeStart;
        else
            m_iTimeOffset = 0;
    }

    // Traverse the Max scene hierarchy three times.  The first pass processes
    // the materials, the second pass builds the node hierarchy, and the third
    // pass applies the modifiers.

    // Construct the world root node of the Magic scene.  Always use a
    // depth buffer.  If the application does not want it, then it should
    // remove the depth buffer itself.
    m_spkScene = new Wm3::Node;
    m_spkScene->SetGlobalState(new Wm3::ZBufferState);

    // traverse the Max hierarchy, and process materials
    ConvertMaterials(m_pkRootNode);

    if ( !m_bExportSelected )
    {
        // export the entire scene

        // process global ambient light settings
        if ( m_pkSettings->IncludeLights )
            BuildAmbientLight();

        // build the Wild Magic node hierarchy
        for (int i = 0; i < m_pkRootNode->NumberOfChildren(); i++ )
            Traverse(m_pkRootNode->GetChildNode(i),m_spkScene);
    }
    else
    {
        // export selection only
        Traverse(m_pkRootNode,m_spkScene);
    }

    if ( m_pkSettings->IncludeModifiers )
    {
        m_spkScene->UpdateGS((double)(TicksToSec(0.0f)));
        ApplyModifiers();
    }

    // save the Wild Magic scene to disk
    Wm3::Stream kStream;

    if ( m_bExportSelected )
        kStream.Insert(m_spkScene->GetChild(0));
    else
        kStream.Insert(m_spkScene);

    kStream.Save(m_acFilename); 
}
//----------------------------------------------------------------------------
WSceneBuilder::~WSceneBuilder ()
{
    delete[] m_acFilename;
    m_spkScene = NULL;
    vector<ModifierInfo*>::iterator pkIter;
    for (pkIter = m_kModList.begin(); pkIter < m_kModList.end(); pkIter++)
        delete *pkIter;
}
//----------------------------------------------------------------------------
bool WSceneBuilder::Traverse (INode* pkMaxNode, Wm3::Spatial* pkWm3Node)
{
    // Recursively traverse the Max node hierarchy, constructing and 
    // attaching the corresponding objects to the Wild Magic scene graph.
    // pkMaxNode:
    //     The current node in the Max hierarchy.
    // pkWm3Node:
    //     The parent node of the next node to be attached to the Wild Magic
    //     hierarchy.
    // Returns true if recursion is to continue, false otherwise.

    Wm3::Spatial* pkWm3Child = NULL;

    // Get the Max node to evaluate its world state at the time of the start
    // frame.
    ObjectState kObjectState = pkMaxNode->EvalWorldState(m_iTimeStart);
    bool bSupported = true;

    // Determine what kind of object we are dealing with, and process
    // accordingly.  If a node has an associated object (such as a mesh), and
    // the user has decided not to export objects of that type, we preserve
    // the spatial relationship in the hierarchy by linking in a node with
    // the same transform, but no associated object.  Note that this allows
    // us to export animation information only, independently of any
    // geometry, if desired.
    if ( kObjectState.obj )
    {
        switch ( kObjectState.obj->SuperClassID() )
        {
        case GEOMOBJECT_CLASS_ID:
            pkWm3Child = BuildGeometry(pkMaxNode,pkWm3Node);
            break;
        case CAMERA_CLASS_ID:
            // TO DO.  Not yet supported.
            break;
        case LIGHT_CLASS_ID:
            if ( m_pkSettings->IncludeLights && !pkMaxNode->IsHidden() )
                BuildLight(pkMaxNode,pkWm3Node);
            break;
        case HELPER_CLASS_ID:
            // The "helper" nodes have no associated geometry (e.g. dummy, or
            // bone nodes). Create and link a node with the same transform in
            // the Wild Magic scene graph.
            pkWm3Child = BuildSpatial(pkMaxNode,pkWm3Node);
            break;
        default:
            assert( false );
            bSupported = false;
            return false;
        }
    }

    // attach animation information to the node
    if ( pkWm3Child )
    {   
        if ( m_pkSettings->IncludeKeyFrames )
            BuildKeyFrameController(pkMaxNode,pkWm3Child);
        else if ( m_pkSettings->IncludeAllFrames )
            BuildFrameController(pkMaxNode,pkWm3Child);
    }

    // Accumulate the modifiers.  These are attached to the Wild Magic scene
    // in the WSceneBuilder constructor after the traversals are finished.
    if ( m_pkSettings->IncludeModifiers )
    {
        ModifierInfo* pkModInfo = new ModifierInfo;
        FindModifiers(pkMaxNode,pkModInfo->Modifiers);
        if ( pkModInfo->Modifiers.empty() )
        {
            delete pkModInfo;
        }
        else
        {
            pkModInfo->Node = pkMaxNode;
            m_kModList.push_back(pkModInfo);
        }
    }

    int iNumChildren = pkMaxNode->NumberOfChildren();
    if ( iNumChildren == 0 ) 
        return true;

    for (int i = 0; i < iNumChildren; i++)
    {
        if ( !Traverse(pkMaxNode->GetChildNode(i),pkWm3Child) 
        ||   m_pkMax->GetCancel() )
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
Wm3::Spatial* WSceneBuilder::BuildGeometry (INode* pkMaxNode,
    Wm3::Spatial* pkWm3Node)
{
    // Build and link a geometric object into the scene.
    // pkMaxNode:
    //     Pointer node with geometry in Max scene.
    // pkWm3Node:
    //     Parent to which to attach new node(s).

    Wm3::Spatial* pkWm3Child = NULL;
    Wm3::Spatial* pkLink = NULL;

    // In a Max scene, geometry nodes can have geometry children, whereas
    // with Wild Magic, all geometry nodes must be leaf nodes.  If this Max
    // node has geometry children, then a "link" node has to be created in
    // order to allow the geometry to be attached to the Wild Magic scene as
    // a leaf node.
    for (int i = 0; i < pkMaxNode->NumberOfChildren(); i++)
    {
        ObjectState kObjectState = pkMaxNode->EvalWorldState(m_iTimeStart); 
        if ( kObjectState.obj->SuperClassID() == GEOMOBJECT_CLASS_ID )
        {
            pkLink = BuildSpatial(pkMaxNode,pkWm3Node);
            break;
        }
    }

    if ( pkLink == NULL )
        pkLink = pkWm3Node;

    if ( m_pkSettings->IncludeMeshes && !pkMaxNode->IsHidden() )
        pkWm3Child = BuildMesh(pkMaxNode,pkLink);
    else
        pkWm3Child = BuildSpatial(pkMaxNode,pkLink);

    if ( pkLink != pkWm3Node )
        return pkLink;

    return pkWm3Child;
}
//----------------------------------------------------------------------------
Wm3::Spatial* WSceneBuilder::BuildSpatial (INode* pkMaxNode,
    Wm3::Spatial* pkWm3Node)
{
    Wm3::Node* pkWm3Child = new Wm3::Node;
    pkWm3Child->SetName(pkMaxNode->GetName());

    pkWm3Child->Local = GetLocalTransform(pkMaxNode,m_iTimeStart);

    assert( pkWm3Node->IsDerived(Wm3::Node::TYPE) );
    ((Wm3::Node*)pkWm3Node)->AttachChild(pkWm3Child);
    return pkWm3Child;
}
//----------------------------------------------------------------------------
Wm3::Transformation WSceneBuilder::GetLocalTransform (INode* pkNode,
    TimeValue iTime)
{
    // Compute a node's local transform.  Max's node transform access methods
    // all supply the node's world space transform, so we have to perform
    // some manipulation to get the node's local transform.  Also, Max can
    // supply the node's pivot transform via GetNodeTM, or object transform
    // via GetObjectTM(), GetObjTMBeforeWSM(), and GetObjTMAfterWSM(). The
    // "object offset" refers to the transformation of the object associated
    // with the node, relative to the node's pivot, so the node's pivot
    // transform is not in itself sufficient to locate the object in local
    // coordinates.  Note that the object transform access methods return the
    // aggregate of the node's pivot and object offset transformations.

    // evaluate the node's object transform in local coordinates 
    Matrix3 kMaxLocal = pkNode->GetObjTMAfterWSM(iTime) * 
        Inverse(pkNode->GetParentNode()->GetObjTMAfterWSM(iTime));

    // decompose the transform into its affine parts
    AffineParts kAffParts;
    decomp_affine(kMaxLocal,&kAffParts);

    // Determine if the translation is the zero vector.
    bool bTranslationIsZero =
        fabsf(kAffParts.t.x) < MIN_DIFFERENCE &&
        fabsf(kAffParts.t.y) < MIN_DIFFERENCE &&
        fabsf(kAffParts.t.z) < MIN_DIFFERENCE;

    // Determine if the rotation is the identity.
    float fQSign = ( kAffParts.q.w >= 0.0f ? 1.0f : -1.0f );
    bool bRotationIsIdentity =
        fabsf(fQSign*kAffParts.q.w - 1.0f) < MIN_DIFFERENCE &&
        fabsf(kAffParts.q.x) < MIN_DIFFERENCE &&
        fabsf(kAffParts.q.y) < MIN_DIFFERENCE &&
        fabsf(kAffParts.q.z) < MIN_DIFFERENCE;

    // Determine if the transformation has a reflection.
    bool bHasReflection = ( kAffParts.f < 0.0f );

    // Determine if the scales are uniform.
    bool bScaleIsUniform =
        fabsf(kAffParts.k.x - kAffParts.k.y) < MIN_DIFFERENCE &&
        fabsf(kAffParts.k.y - kAffParts.k.z) < MIN_DIFFERENCE;

    // Determine if the uniform scales are all 1.
    bool bScaleIsUnity = bScaleIsUniform &&
        fabsf(kAffParts.k.x - 1.0f) < MIN_DIFFERENCE;

    // Determine if the scale-orientation is the identity.
    float fUSign = ( kAffParts.u.w >= 0.0f ? 1.0f : -1.0f );
    bool bOrientIsIdentity = bScaleIsUniform || (
        fabsf(fUSign*kAffParts.u.w - 1.0f) < MIN_DIFFERENCE &&
        fabsf(kAffParts.u.x) < MIN_DIFFERENCE &&
        fabsf(kAffParts.u.y) < MIN_DIFFERENCE &&
        fabsf(kAffParts.u.z) < MIN_DIFFERENCE);

    // Compute the equivalent Wild Magic transformation.  The careful testing
    // of Max transformation state is designed so that the Wild Magic hints
    // in the Transformation class are properly set.
    Wm3::Transformation kWm3Local;

    if ( !bTranslationIsZero )
    {
        kWm3Local.SetTranslate(Wm3::Vector3f(kAffParts.t.x,kAffParts.t.y,
            kAffParts.t.z));
    }

    if ( bHasReflection )
        kAffParts.k *= -1.0f;

    if ( bScaleIsUniform )
    {
        // matrix of form R*(s*I), where s is uniform scale
        if ( !bRotationIsIdentity )
        {
            Wm3::Matrix3f kRot;
            Wm3::Quaternionf(kAffParts.q.w,-kAffParts.q.x,-kAffParts.q.y,
                -kAffParts.q.z).ToRotationMatrix(kRot);
            kWm3Local.SetRotate(kRot);
        }
        if ( !bScaleIsUnity )
            kWm3Local.SetUniformScale(kAffParts.k.x);
    }
    else if ( bOrientIsIdentity )
    {
        // matrix of form R*S, where S is diagonal with nonuniform scales
        if ( !bRotationIsIdentity )
        {
            Wm3::Matrix3f kRot;
            Wm3::Quaternionf(kAffParts.q.w,-kAffParts.q.x,-kAffParts.q.y,
                -kAffParts.q.z).ToRotationMatrix(kRot);
            kWm3Local.SetRotate(kRot);
        }

        kWm3Local.SetScale(Wm3::Vector3f(kAffParts.k.x,kAffParts.k.y,
            kAffParts.k.z));
    }
    else
    {
        // Matrix of form R*U^t*S*U, where U is the orientation matrix and
        // S is diagonal with nonuniform scales.
        Wm3::Matrix3f kMatrix(
            kMaxLocal.GetAddr()[0][0],
            kMaxLocal.GetAddr()[1][0],
            kMaxLocal.GetAddr()[2][0],
            kMaxLocal.GetAddr()[0][1],
            kMaxLocal.GetAddr()[1][1],
            kMaxLocal.GetAddr()[2][1],
            kMaxLocal.GetAddr()[0][2],
            kMaxLocal.GetAddr()[1][2],
            kMaxLocal.GetAddr()[2][2]);

        kWm3Local.SetMatrix(kMatrix);
    }

    return kWm3Local;
}
//----------------------------------------------------------------------------
