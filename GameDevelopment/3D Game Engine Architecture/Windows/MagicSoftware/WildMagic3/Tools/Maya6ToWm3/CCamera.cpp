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
#include "Wm3Camera.h"

#include <maya/MFnCamera.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MPlug.h>
using namespace Wm3;

//----------------------------------------------------------------------------
void Exporter::CCamera (const MFnDagNode& rkDagNode)
{
    // Convert a Camera object given by the Maya scenegraph node rkDagNode,
    // and attach it into the WM scenetree.

    // cameras are always leaf nodes
    assert( rkDagNode.childCount() == 0 );

    if (!m_abFlags[FLAG_CAMERAS])
        return;

    bool bVisible = true;
    MPlug kVisibility = rkDagNode.findPlug("visibility",&m_kStatus);
    if (m_kStatus)
    {
        kVisibility.getValue(bVisible);
        m_bObjectInvisible |= !bVisible;
    }
    else
    {
        DisplayWarning("Couldn't find visibility plug.  Assuming visible.");
    }

    if (m_bObjectInvisible)
    {
        DisplayInfo("Ignoring invisible camera.");
        return;
    }

    if (rkDagNode.isInstanced())
    {
        DisplayWarning("Instanced camera will be duplicated but not "\
            "instanced in the scene graph.");
    }

    // get Maya camera attributes
    MFnCamera kFnCamera(rkDagNode.object(),&m_kStatus);
    assert( m_kStatus );

    MPoint kMEye = kFnCamera.eyePoint(MSpace::kObject,&m_kStatus);
    assert( m_kStatus );

    MVector kMDir = kFnCamera.viewDirection(MSpace::kObject,&m_kStatus);
    assert( m_kStatus );

    MVector kMUp = kFnCamera.upDirection(MSpace::kObject,&m_kStatus);
    assert( m_kStatus );

    MVector kMRight = kFnCamera.rightDirection(MSpace::kObject,&m_kStatus);
    assert( m_kStatus );

    double dMAspect = kFnCamera.aspectRatio(&m_kStatus);
    assert( m_kStatus );

    double dMLeft, dMRight, dMTop, dMBottom;
    m_kStatus = kFnCamera.getViewingFrustum(dMAspect,dMLeft,dMRight,
        dMBottom,dMTop);
    assert( m_kStatus );

    double dMNear = kFnCamera.nearClippingPlane(&m_kStatus);
    assert( m_kStatus );

    double dMFar = kFnCamera.farClippingPlane(&m_kStatus);
    assert( m_kStatus );

    // create Wild Magic camera and add to list of available cameras
    Camera* pkCamera = new Camera;
    
    pkCamera->SetFrustum((float)dMLeft,(float)dMRight,(float)dMBottom,
        (float)dMTop,(float)dMNear,(float)dMFar);
    
    Vector3f kLoc((float)kMEye.x,(float)kMEye.y,(float)kMEye.z);
    Vector3f kDir((float)kMDir.x,(float)kMDir.y,(float)kMDir.z);
    Vector3f kUp((float)kMUp.x,(float)kMUp.y,(float)kMUp.z);
    Vector3f kRight((float)kMRight.x,(float)kMRight.y,(float)kMRight.z);
    pkCamera->SetFrame(kLoc,kDir,kUp,kRight);

    pkCamera->SetName(rkDagNode.name().asChar());
    m_kWMCamera.Append(pkCamera);
    AttachToParent(rkDagNode,pkCamera);
}
//----------------------------------------------------------------------------

