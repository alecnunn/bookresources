// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DxRenderer.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void DxRenderer::SetMaterialState (MaterialState* pkState)
{
    D3DMATERIAL9 kD3DMaterial;

    kD3DMaterial.Emissive.r = pkState->Emissive.R();
    kD3DMaterial.Emissive.g = pkState->Emissive.G();
    kD3DMaterial.Emissive.b = pkState->Emissive.B();
    kD3DMaterial.Emissive.a = pkState->Emissive.A();

    kD3DMaterial.Ambient.r = pkState->Ambient.R();
    kD3DMaterial.Ambient.g = pkState->Ambient.G();
    kD3DMaterial.Ambient.b = pkState->Ambient.B();
	kD3DMaterial.Ambient.a = pkState->Ambient.A();

    kD3DMaterial.Diffuse.r = pkState->Diffuse.R();
    kD3DMaterial.Diffuse.g = pkState->Diffuse.G();
    kD3DMaterial.Diffuse.b = pkState->Diffuse.B();
	kD3DMaterial.Diffuse.a = pkState->Diffuse.A();

    kD3DMaterial.Specular.r = pkState->Specular.R();
    kD3DMaterial.Specular.g = pkState->Specular.G();
    kD3DMaterial.Specular.b = pkState->Specular.B();
 	kD3DMaterial.Specular.a = pkState->Specular.A();
 
    kD3DMaterial.Power = pkState->Shininess;

    ms_hResult = m_pqDevice->SetMaterial(&kD3DMaterial);
    assert( SUCCEEDED(ms_hResult) );

    // Enable per-vertex color which allows the system to include the
    // color defined for individual vertices in its lighting calculations. 
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORVERTEX,TRUE);
    assert( SUCCEEDED(ms_hResult) );

    // Allow the diffuse and specular colors to come from the color
    // information specified along with the vertex.  Note that if
    // those particular colors are not defined along with the vertex
    // then the colors diffuse or specular color from the specified
    // material is used.
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,
        D3DMCS_COLOR1);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,
        D3DMCS_COLOR2);
    assert( SUCCEEDED(ms_hResult) );

    // Use the ambient and emissive colors defined in the material.
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,
        D3DMCS_MATERIAL);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,
        D3DMCS_MATERIAL);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
