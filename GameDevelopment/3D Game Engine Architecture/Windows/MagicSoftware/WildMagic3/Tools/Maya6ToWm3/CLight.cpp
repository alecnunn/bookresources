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
#include "Wm3Light.h"

#include <maya/MPlug.h>
#include <maya/MFnLight.h>
#include <maya/MFnAmbientLight.h>
#include <maya/MFnNonAmbientLight.h>
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnPointLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFloatVector.h>
using namespace Wm3;

//----------------------------------------------------------------------------
Light* Exporter::CreateLight (const MFnDagNode& rkDagNode)
{
    // A generic light creation routine that applies to all kinds of lights

    if (rkDagNode.isInstanced())
    {
        DisplayWarning("Instanced light will be duplicated but not "\
            "instanced in the scene graph.");
    }

    // Lights should not have children
    assert(rkDagNode.childCount() == 0);

    Light* pkLight = new Light();
    MStatus kStatus;
    MFnLight kMLight(rkDagNode.object(), &kStatus);
    assert(kStatus);

    pkLight->SetName(rkDagNode.name().asChar());
    MColor kMColor = kMLight.color();

    // Intensity is a multiplier of the light color
    pkLight->Intensity = kMLight.intensity(&kStatus);
    assert(kStatus);
    ColorRGBA kColor(kMColor.r,kMColor.g,kMColor.b,kMColor.a);

    bool bAmbient = kMLight.lightAmbient(&kStatus);
    assert(kStatus);
    bool bDiffuse = kMLight.lightDiffuse(&kStatus);
    assert(kStatus);
    bool bSpecular = kMLight.lightSpecular(&kStatus);
    assert(kStatus);

    // There isn't a method to specify different amounts of lighting
    // and so kColor will just get applied to any category of light that
    // Maya says this light has.

    if (bAmbient)
        pkLight->Ambient = kColor;
    if (bDiffuse)
        pkLight->Diffuse = kColor;
    if (bSpecular)
        pkLight->Specular = kColor;

    MFloatVector kDir = kMLight.lightDirection(0,MSpace::kWorld,&kStatus);
    assert(kStatus);
    pkLight->SetDirection(Vector3f((float)kDir.x,(float)kDir.y,
        (float)kDir.z));

    // No need for position here, as the transformation node (Maya
    // shape node) that is a parent to this light node will put this
    // light in the correct place.

    return pkLight;
}
//----------------------------------------------------------------------------
static void SetAttenuation(Light* pkLight, const MFnDagNode& rkDagNode)
{
    // This is just a helper function for light creation.  It sets
    // the attenuation of pkLight based on the attributes found in
    // rkDagNode (which should be a non-ambient light).

    MStatus kStatus;
    MFnNonAmbientLight kMLight(rkDagNode.object(),&kStatus);
    assert(kStatus);

    // Note: Maya only gives the type of decay, but not any factors.
    // So, I'll just use some arbitrary factors to give the user some idea
    // of what Maya had and they can adjust it themselves.
    int iDecay = kMLight.decayRate(&kStatus);
	switch(iDecay)
	{
	default:
        // This should never happen
        MGlobal::displayError("Unknown decay rate!");
        // fall through
	case 0:	// none
        pkLight->Attenuate = false;
		break;
	case 1:	// linear
        pkLight->Attenuate = true;
        pkLight->Linear = 1.0f;
		break;
	case 2:	// quadratic
        pkLight->Attenuate = true;
        pkLight->Quadratic = 1.0f;
		break;
	case 3:	// cubic
        pkLight->Attenuate = true;
        // Wildmagic has no cubic...
        pkLight->Quadratic = 2.0f;
		break;
	}
}
//----------------------------------------------------------------------------
void Exporter::CAmbientLight (const MFnDagNode& rkDagNode)
{
    // Create an ambient light from rkDagNode.

    if (!m_abFlags[FLAG_LIGHTS])
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
        DisplayInfo("Ignoring invisible light.");
        return;
    }

    MFnAmbientLight kMLight(rkDagNode.object(), &m_kStatus);
    assert(m_kStatus);

    Light* pkLight = CreateLight(rkDagNode);
    pkLight->Type = Light::LT_AMBIENT;

    AttachToParent(rkDagNode,pkLight);
    m_kWMLight.Append(pkLight);
}
//----------------------------------------------------------------------------
void Exporter::CDirectionalLight (const MFnDagNode& rkDagNode)
{
    // Create a directional light from rkDagNode.

    if (!m_abFlags[FLAG_LIGHTS])
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
        DisplayInfo("Ignoring invisible light.");
        return;
    }

    MFnDirectionalLight kMLight(rkDagNode.object(), &m_kStatus);
    assert(m_kStatus);

    Light* pkLight = CreateLight(rkDagNode);
    pkLight->Type = Light::LT_DIRECTIONAL;
    pkLight->SetDirection(Vector3f::UNIT_Z);

    AttachToParent(rkDagNode,pkLight);
    m_kWMLight.Append(pkLight);
}
//----------------------------------------------------------------------------
void Exporter::CPointLight (const MFnDagNode& rkDagNode)
{
    // Create a point light from rkDagNode.

    if (!m_abFlags[FLAG_LIGHTS])
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
        DisplayInfo("Ignoring invisible light.");
        return;
    }

    MFnPointLight kMLight(rkDagNode.object(), &m_kStatus);
    assert(m_kStatus);

    Light* pkLight = CreateLight(rkDagNode);
    pkLight->Type = Light::LT_POINT;

    SetAttenuation(pkLight,rkDagNode);

    AttachToParent(rkDagNode,pkLight);
    m_kWMLight.Append(pkLight);
}
//----------------------------------------------------------------------------
void Exporter::CSpotLight (const MFnDagNode& rkDagNode)
{
    // Create a spotlight from rkDagNode.

    if (!m_abFlags[FLAG_LIGHTS])
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
        DisplayInfo("Ignoring invisible light.");
        return;
    }

    MFnSpotLight kMLight(rkDagNode.object(), &m_kStatus);
    assert(m_kStatus);

    Light* pkLight = CreateLight(rkDagNode);
    pkLight->Type = Light::LT_SPOT;
    SetAttenuation(pkLight,rkDagNode); 

    // The cone angle in Maya is in degrees and is the full angle of the cone,
    // rather than the half-angle that WildMagic expects
    pkLight->Angle = Mathf::DEG_TO_RAD*
        (float)kMLight.coneAngle(&m_kStatus)/2.0f;
    assert(m_kStatus);

    // I'm not sure of the exact correspondence between dropOff and
    // WM3's Light::Exponent, but they seem to be more or less the same.
    pkLight->Exponent = (float)kMLight.dropOff(&m_kStatus);
    assert(m_kStatus);

    AttachToParent(rkDagNode,pkLight);
    m_kWMLight.Append(pkLight);
}
//----------------------------------------------------------------------------