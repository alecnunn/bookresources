// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3OpenGLRenderer.h"
#include "Wm3BumpMapEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void OpenGLRenderer::DrawBumpMap ()
{
    assert( m_pkGeometry->GeometryType == Geometry::GT_TRIMESH );
    assert( DynamicCast<BumpMapEffect>(m_pkGeometry->GetEffect()) );
    BumpMapEffect* pkBMEffect = (BumpMapEffect*)m_pkGeometry->GetEffect();

    // set up the model-to-world transformation
    SetWorldTransformation();

    // set the vertex array
    Vector3f* akVertex = m_pkGeometry->Vertices->GetData();
    assert( akVertex );
    glVertexPointer(3,GL_FLOAT,0,akVertex);

    //*** FIRST PASS
    SetGlobalState(m_pkGeometry->States);

    // no lighting, the color array stores the light vectors
    pkBMEffect->ComputeLightVectors(m_pkGeometry);
    assert( m_pkLocalEffect->ColorRGBs );
    ColorRGB* akColorRGB = m_pkLocalEffect->ColorRGBs->GetData();
    assert( akColorRGB );
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_FLOAT,0,akColorRGB);

    // texture unit 0 handles the normal map
    EnableTexture(0,1,pkBMEffect);

    // draw the object
    GLenum eType = ms_aeObjectType[m_pkGeometry->GeometryType];
    assert( m_pkGeometry->Indices );
    int iIndexQuantity = m_pkGeometry->Indices->GetQuantity();
    int* aiIndex = m_pkGeometry->Indices->GetData();
    assert( aiIndex );
    glDrawElements(eType,iIndexQuantity,GL_UNSIGNED_INT,aiIndex);

    //*** SECOND PASS
    EnableLighting();
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR,GL_ZERO);

    // set the normal array
    Vector3f* akNormal =
        (m_pkGeometry->Normals ? m_pkGeometry->Normals->GetData() : 0);
    assert( akNormal );
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT,0,akNormal);

    // texture unit 0 handles the primary texture
    EnableTexture(0,0,pkBMEffect);

    // draw the object
    glDrawElements(eType,iIndexQuantity,GL_UNSIGNED_INT,aiIndex);

    // disable texture state
    DisableTexture(0,0,pkBMEffect);

    // disable color state
    glDisableClientState(GL_COLOR_ARRAY);

    DisableLighting();

    // restore the default alpha state
    GlobalState* pkAState = GlobalState::Default[GlobalState::ALPHA];
    SetAlphaState((AlphaState*)pkAState);

    // restore the model-to-world transformation
    RestoreWorldTransformation();
}
//----------------------------------------------------------------------------
