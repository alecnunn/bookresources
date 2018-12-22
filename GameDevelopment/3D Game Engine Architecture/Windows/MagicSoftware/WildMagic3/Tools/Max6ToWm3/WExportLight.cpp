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

//----------------------------------------------------------------------------
void WSceneBuilder::BuildAmbientLight ()
{
    // Get Max's global ambient light settings, and build the corresponding
    // Wild Magic light.
    Color kColor = m_pkMax->GetAmbient(0,FOREVER);
    Wm3::Light* pkLight = new Wm3::Light(Wm3::Light::LT_AMBIENT);
    pkLight->Ambient = Wm3::ColorRGBA(kColor.r,kColor.g,kColor.b,1.0f);
    m_spkScene->SetLight(pkLight);
}
//----------------------------------------------------------------------------
void WSceneBuilder::BuildLight (INode* pkMaxNode, Wm3::Spatial* pkWm3Node)
{
    // Build a Wild Magic light corresponding to a light in the Max hierarchy.
    // Max's omnidirectional, directional and free spot lights are supported.
    // Note that Max assigns a node in the scene graph to each light, whereas
    // Wild Magic allows a list of lights to be attached to a node.  The
    // approach used here is to attach the Wild Magic lights to the node in
    // the Wild Magic scene graph which corresponds to the parent of the
    // light node in the Max scene graph.
    //
    // pkMaxNode:
    //     The Max node representing the light.
    // pkWm3Node:
    //     The Wild Magic node to which the corresponding light state will
    //     be applied.

    // get the light's parameters from Max
    ObjectState kObjectState = pkMaxNode->EvalWorldState(m_iTimeStart);
    GenLight* pkGenLight = (GenLight*)kObjectState.obj;
    LightState kMaxLightState;
    Interval kValid = FOREVER;
    pkGenLight->EvalLightState(m_iTimeStart,kValid,&kMaxLightState);

    Wm3::Light* pkWm3Light = NULL;

    switch( pkGenLight->Type() ) 
    {
    case OMNI_LIGHT:
        pkWm3Light = BuildPointLight(pkMaxNode);
        break;
    case TSPOT_LIGHT:
    case FSPOT_LIGHT:
        pkWm3Light = BuildSpotLight(pkMaxNode,kMaxLightState);
        break;
    case TDIR_LIGHT:
    case DIR_LIGHT:
        pkWm3Light = BuildDirectionalLight(pkMaxNode);
        break;
    default: // Light not supported
        assert( false );
        return;
    }
    
    pkWm3Light->On = ( kMaxLightState.on != 0 );
    pkWm3Light->Intensity = kMaxLightState.intens;
    
    // Max lights seem to specify only one color. Use this color for Wild
    // Magic light's specular and diffuse colors.
    if ( kMaxLightState.affectDiffuse )
    {
        pkWm3Light->Diffuse = Wm3::ColorRGBA(kMaxLightState.color.r,
            kMaxLightState.color.g,kMaxLightState.color.b,1.0f);
    }
    if ( kMaxLightState.affectSpecular )
    {
        pkWm3Light->Specular = Wm3::ColorRGBA(kMaxLightState.color.r,
            kMaxLightState.color.g,kMaxLightState.color.b,1.0f);
    }
    
    // Set the attenuation parameters. Max's decay model is simpler than Wild
    // Magic's in that the decay can be either constant, linear or quadratic,
    // but not a/ combination of all three
    if ( pkGenLight->GetDecayType() != 0 )
    {
        pkWm3Light->Attenuate = true;
        Interval kValid;
        float fDecayRadius = pkGenLight->GetDecayRadius(m_iTimeStart,kValid);
        
        switch ( pkGenLight->GetDecayType() )
        {
        case 0:
            pkWm3Light->Constant = ATTENUATION;
            pkWm3Light->Linear = 0.0f;
            pkWm3Light->Quadratic = 0.0f;
            break;
        case 1:
            pkWm3Light->Constant = 0.0f;
            pkWm3Light->Linear = ATTENUATION/fDecayRadius;
            pkWm3Light->Quadratic = 0.0f;
            break;
        case 2:
            pkWm3Light->Constant = 0.0f;
            pkWm3Light->Linear = 0.0f;
            pkWm3Light->Quadratic = ATTENUATION/(fDecayRadius*fDecayRadius);
            break;
        } 
    }
    else if ( pkGenLight->GetUseAtten() )
    {
        float fFarAtten = kMaxLightState.attenEnd;
        pkWm3Light->Attenuate = true;             
        pkWm3Light->Constant = 0.0f;
        pkWm3Light->Linear = ATTENUATION/fFarAtten;
        pkWm3Light->Quadratic = 0.0f;
    }

    pkWm3Node->SetLight(pkWm3Light);
}
//----------------------------------------------------------------------------
Wm3::Vector3f WSceneBuilder::GetLightLocation (INode* pkNode)
{
    // evaluate the node's object transform in local coordinates 
    Matrix3 kLocal = pkNode->GetNodeTM(m_iTimeStart) *
        Inverse(pkNode->GetParentTM(m_iTimeStart));

    // decompose the transform into its affine parts
    AffineParts kAffParts;
    decomp_affine(kLocal,&kAffParts);

    // extract the translation component
    return Wm3::Vector3f(kAffParts.t.x,kAffParts.t.y,kAffParts.t.z);
}
//----------------------------------------------------------------------------
Wm3::Light* WSceneBuilder::BuildPointLight (INode* pkNode)
{
    Wm3::Light* pkLight = new Wm3::Light(Wm3::Light::LT_POINT);

    // set the light's location
    pkLight->SetLocation(GetLightLocation(pkNode));
    
    return pkLight;
}
//----------------------------------------------------------------------------
Wm3::Light* WSceneBuilder::BuildSpotLight (INode* pkNode,
    LightState& rkLightState)
{
    Wm3::Light* pkLight = new Wm3::Light(Wm3::Light::LT_SPOT);

    // set the light's location and direction
    pkLight->SetLocation(GetLightLocation(pkNode));
    pkLight->SetDirection(-Wm3::Vector3f::UNIT_Z);

    // set the light's angle
    pkLight->Angle = HALFPI*rkLightState.fallsize/180.0f;

    // Set the light's exponent.  This equation seems to give reasonable
    // correspondance between Max and Wild Magic.
    pkLight->Exponent = 128.0f*((rkLightState.fallsize -
        rkLightState.hotsize)/rkLightState.fallsize);
    
    return pkLight;
}
//----------------------------------------------------------------------------
Wm3::Light* WSceneBuilder::BuildDirectionalLight (INode* pkNode)
{
    Wm3::Light* pkLight = new Wm3::Light(Wm3::Light::LT_DIRECTIONAL);

    // set the light's direction
    pkLight->SetDirection(-Wm3::Vector3f::UNIT_Z);

    return pkLight;
}
//----------------------------------------------------------------------------
