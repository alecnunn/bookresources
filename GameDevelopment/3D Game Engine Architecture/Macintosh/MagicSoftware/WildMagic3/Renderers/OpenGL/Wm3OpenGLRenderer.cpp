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
#include "Wm3Camera.h"
#include "Wm3ProjectedTextureEffect.h"
#include "Wm3Vector4.h"
using namespace Wm3;

GLenum OpenGLRenderer::ms_aeObjectType[Geometry::GT_MAX_QUANTITY] =
{
    GL_POINTS,      // GT_POLYPOINT
    GL_LINES,       // GT_POLYLINE_SEGMENTS
    GL_LINE_STRIP,  // GT_POLYLINE_OPEN
    GL_LINE_LOOP,   // GT_POLYLINE_CLOSED
    GL_TRIANGLES    // GT_TRIMESH
};

GLenum OpenGLRenderer::ms_aeTextureCorrection[Texture::CM_QUANTITY] =
{
    GL_FASTEST,     // CM_AFFINE
    GL_NICEST       // CM_PERSPECTIVE
};

GLenum OpenGLRenderer::ms_aeTextureApply[Texture::AM_QUANTITY] =
{
    GL_REPLACE,
    GL_DECAL,
    GL_MODULATE,
    GL_BLEND,
    GL_ADD,
    GL_COMBINE
};

GLenum OpenGLRenderer::ms_aeTextureCombineFunc[Texture::ACF_QUANTITY] =
{
    GL_REPLACE,
    GL_MODULATE,
    GL_ADD,
    GL_ADD_SIGNED,
    GL_SUBTRACT,
    GL_INTERPOLATE,
    GL_DOT3_RGB,
    GL_DOT3_RGBA
};

GLenum OpenGLRenderer::ms_aeTextureCombineSrc[Texture::ACS_QUANTITY] = 
{
    GL_TEXTURE,
    GL_PRIMARY_COLOR,
    GL_CONSTANT,
    GL_PREVIOUS
};

GLenum OpenGLRenderer::ms_aeTextureCombineOperand[Texture::ACO_QUANTITY] =
{
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA
};

GLint OpenGLRenderer::ms_afTextureCombineScale[Texture::ACSC_QUANTITY] =
{
    1,
    2,
    4
};

GLenum OpenGLRenderer::ms_aeTextureFilter[Texture::FM_QUANTITY] =
{
    GL_NEAREST,
    GL_LINEAR
};

GLenum OpenGLRenderer::ms_aeTextureMipmap[Texture::MM_QUANTITY] =
{
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

GLenum OpenGLRenderer::ms_aeImageComponents[Image::IT_QUANTITY] =
{
    GL_RGBA4,
    GL_RGB8,
    GL_RGB5_A1,
    GL_RGBA8
};

GLenum OpenGLRenderer::ms_aeImageFormats[Image::IT_QUANTITY] =
{
    GL_RGBA,
    GL_RGB,
    GL_RGBA,
    GL_RGBA
};

//----------------------------------------------------------------------------
int Renderer::GetType ()
{
    return RT_OPENGL;
}
//----------------------------------------------------------------------------
OpenGLRenderer::OpenGLRenderer (int iWidth, int iHeight)
    :
    Renderer(iWidth,iHeight),
    m_kDLInfo(4,4)
{
    // initial world matrix is zero (will always be set properly later)
    memset(m_afWorldMatrix,0,16*sizeof(float));
}
//----------------------------------------------------------------------------
OpenGLRenderer::~OpenGLRenderer ()
{
    for (int i = 0; i < m_kDLInfo.GetQuantity(); i++)
    {
        DisplayListInfo kInfo = m_kDLInfo[i];
        glDeleteLists(kInfo.Start,kInfo.Quantity);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::InitializeState ()
{
    // initialize global render state to default settings
    SetGlobalState(GlobalState::Default);

    // vertices always exist
    glEnableClientState(GL_VERTEX_ARRAY);

    // colors disabled, current color is WHITE
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4fv((const float*)ColorRGBA::WHITE);

    // normals disabled
    glDisableClientState(GL_NORMAL_ARRAY);

    // Get number of supported texture units.  Initially disable them.
    GLint iMaxTextures;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&iMaxTextures);
    m_iMaxTextures = (int)iMaxTextures;
    for (int i = 0; i < m_iMaxTextures; i++)
    {
        glClientActiveTextureARB(GL_TEXTURE0_ARB+i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
        glActiveTextureARB(GL_TEXTURE0_ARB+i);
        glDisable(GL_TEXTURE_2D);
    }

    // set up light model
    m_iMaxLights = 8;  // OpenGL limits the number of lights to eight
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,(const float*)ColorRGBA::BLACK);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    glDisable(GL_LIGHTING);

     // get stencil buffer size
     GLint iBits = 0;
     glGetIntegerv((GLenum)GL_STENCIL_BITS,&iBits);
     m_iMaxStencilIndices = ( iBits > 0 ? (1 << iBits) : 0 );

    // disable drawing of lines as sequences of dashes (want solid lines)
    glDisable(GL_LINE_STIPPLE);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetBackgroundColor (const ColorRGB& rkColor)
{
    Renderer::SetBackgroundColor(rkColor);
    glClearColor(rkColor[0],rkColor[1],rkColor[2],1.0f);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBackBuffer ()
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,m_iWidth,m_iHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearZBuffer ()
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,m_iWidth,m_iHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearStencilBuffer ()
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,m_iWidth,m_iHeight);
    glStencilMask(~0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBuffers ()
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,m_iWidth,m_iHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glStencilMask(~0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBackBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearZBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glStencilMask(~0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBuffers (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glStencilMask(~0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetWorldMatrix ()
{
    if ( m_pkGeometry->World.IsRSMatrix() )
    {
        const Matrix3f& rkRotate = m_pkGeometry->World.GetRotate();
        const Vector3f& rkScale = m_pkGeometry->World.GetScale();

        m_afWorldMatrix[ 0] = rkScale.X()*rkRotate[0][0];
        m_afWorldMatrix[ 1] = rkScale.X()*rkRotate[1][0];
        m_afWorldMatrix[ 2] = rkScale.X()*rkRotate[2][0];
        m_afWorldMatrix[ 3] = 0.0f;
        m_afWorldMatrix[ 4] = rkScale.Y()*rkRotate[0][1];
        m_afWorldMatrix[ 5] = rkScale.Y()*rkRotate[1][1];
        m_afWorldMatrix[ 6] = rkScale.Y()*rkRotate[2][1];
        m_afWorldMatrix[ 7] = 0.0f;
        m_afWorldMatrix[ 8] = rkScale.Z()*rkRotate[0][2];
        m_afWorldMatrix[ 9] = rkScale.Z()*rkRotate[1][2];
        m_afWorldMatrix[10] = rkScale.Z()*rkRotate[2][2];
        m_afWorldMatrix[11] = 0.0f;
    }
    else
    {
        const Matrix3f& rkMatrix = m_pkGeometry->World.GetMatrix();
        m_afWorldMatrix[ 0] = rkMatrix[0][0];
        m_afWorldMatrix[ 1] = rkMatrix[1][0];
        m_afWorldMatrix[ 2] = rkMatrix[2][0];
        m_afWorldMatrix[ 3] = 0.0f;
        m_afWorldMatrix[ 4] = rkMatrix[0][1];
        m_afWorldMatrix[ 5] = rkMatrix[1][1];
        m_afWorldMatrix[ 6] = rkMatrix[2][1];
        m_afWorldMatrix[ 7] = 0.0f;
        m_afWorldMatrix[ 8] = rkMatrix[0][2];
        m_afWorldMatrix[ 9] = rkMatrix[1][2];
        m_afWorldMatrix[10] = rkMatrix[2][2];
        m_afWorldMatrix[11] = 0.0f;
    }

    const Vector3f& rkTranslate = m_pkGeometry->World.GetTranslate();
    m_afWorldMatrix[12] = rkTranslate.X();
    m_afWorldMatrix[13] = rkTranslate.Y();
    m_afWorldMatrix[14] = rkTranslate.Z();
    m_afWorldMatrix[15] = 1.0f;
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnFrustumChange ()
{
    if ( m_pkCamera )
    {
        float fL, fR, fB, fT, fN, fF;
        m_pkCamera->GetFrustum(fL,fR,fB,fT,fN,fF);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(fL,fR,fB,fT,fN,fF);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnViewPortChange ()
{
    float fL, fR, fT, fB;
    if ( m_pkCamera )
    {
        // 3D applications use cameras
        m_pkCamera->GetViewPort(fL,fR,fT,fB);
    }
    else
    {
        // 2D applications do not use camera
        fL = 0.0f;
        fR = 1.0f;
        fT = 1.0f;
        fB = 0.0f;
    }

    GLint iX = (GLint)(fL*m_iWidth);
    GLint iY = (GLint)(fB*m_iHeight);
    GLsizei iW = (GLsizei)((fR - fL)*m_iWidth);
    GLsizei iH = (GLsizei)((fT - fB)*m_iHeight);
    glViewport(iX,iY,iW,iH);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnFrameChange ()
{
    Vector3f kLoc  = m_pkCamera->GetWorldLocation();
    Vector3f kUVec = m_pkCamera->GetWorldUVector();
    Vector3f kDVec = m_pkCamera->GetWorldDVector();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3f kLookAt = kLoc + kDVec;
    gluLookAt(kLoc.X(),kLoc.Y(),kLoc.Z(),kLookAt.X(),kLookAt.Y(),kLookAt.Z(),
        kUVec.X(),kUVec.Y(),kUVec.Z());
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableLighting (int eEnable)
{
    int iQuantity = m_pkGeometry->Lights.GetQuantity();
    if ( iQuantity >= m_iMaxLights )
        iQuantity = m_iMaxLights;

    if ( iQuantity > 0 )
    {
        glEnable(GL_LIGHTING);
        for (int i = 0; i < iQuantity; i++)
        {
            const Light* pkLight = m_pkGeometry->Lights[i];
            if ( pkLight->On )
                EnableLight(eEnable,i,pkLight);
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableLighting ()
{
    int iQuantity = m_pkGeometry->Lights.GetQuantity();
    if ( iQuantity >= m_iMaxLights )
        iQuantity = m_iMaxLights;

    if ( iQuantity > 0 )
    {
        for (int i = 0; i < iQuantity; i++)
        {
            const Light* pkLight = m_pkGeometry->Lights[i];
            if ( pkLight->On )
                DisableLight(i,pkLight);
        }
        glDisable(GL_LIGHTING);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableLight (int eEnable, int i, const Light* pkLight)
{
    GLenum eIndex = (GLenum)(GL_LIGHT0 + i);
    glEnable(eIndex);

    // ambient
    ColorRGBA kColor;
    if ( eEnable & EL_AMBIENT )
    {
        kColor = pkLight->Intensity*pkLight->Ambient;
        kColor.Clamp();
    }
    else
    {
        kColor = ColorRGBA::BLACK;
    }
    glLightfv(eIndex,GL_AMBIENT,(float*)kColor);

    // diffuse
    if ( (eEnable & EL_DIFFUSE) && pkLight->Type != Light::LT_AMBIENT )
    {
        kColor = pkLight->Intensity*pkLight->Diffuse;
        kColor.Clamp();
    }
    else
    {
        kColor = ColorRGBA::BLACK;
    }
    glLightfv(eIndex,GL_DIFFUSE,(float*)kColor);

    // specular
    if ( (eEnable & EL_SPECULAR) && pkLight->Type != Light::LT_AMBIENT )
    {
        kColor = pkLight->Intensity*pkLight->Specular;
        kColor.Clamp();
    }
    else
    {
        kColor = ColorRGBA::BLACK;
    }
    glLightfv(eIndex,GL_SPECULAR,(float*)kColor);

    if ( pkLight->Attenuate )
    {
        glLightf(eIndex,GL_CONSTANT_ATTENUATION,
            pkLight->Constant);
        glLightf(eIndex,GL_LINEAR_ATTENUATION,
            pkLight->Linear);
        glLightf(eIndex,GL_QUADRATIC_ATTENUATION,
            pkLight->Quadratic);
    }
    else
    {
        glLightf(eIndex,GL_CONSTANT_ATTENUATION,1.0f);
        glLightf(eIndex,GL_LINEAR_ATTENUATION,0.0f);
        glLightf(eIndex,GL_QUADRATIC_ATTENUATION,0.0f);
    }

    GLfloat afTemp[4];
    Vector3f kLoc, kDir;

    switch ( pkLight->Type )
    {
    case Light::LT_AMBIENT:
        afTemp[0] = 0.0f;
        afTemp[1] = 0.0f;
        afTemp[2] = 1.0f;
        afTemp[3] = 0.0f;
        glLightfv(eIndex,GL_POSITION,afTemp);
        break;
    case Light::LT_DIRECTIONAL:
        kDir = pkLight->GetWorldDVector();
        afTemp[0] = -kDir.X();
        afTemp[1] = -kDir.Y();
        afTemp[2] = -kDir.Z();
        afTemp[3] = 0.0f;
        glLightfv(eIndex,GL_POSITION,afTemp);
        break;
    case Light::LT_POINT:
    case Light::LT_SPOT:
        kLoc = pkLight->GetWorldLocation();
        afTemp[0] = kLoc.X();
        afTemp[1] = kLoc.Y();
        afTemp[2] = kLoc.Z();
        afTemp[3] = 1.0f;
        glLightfv(eIndex,GL_POSITION,afTemp);
        break;
    default:  // Light::LT_QUANTITY
        assert( false );
        break;
    }

    if ( pkLight->Type == Light::LT_SPOT )
    {
        float fDegrees = Mathf::RAD_TO_DEG*pkLight->Angle;
        glLightf(eIndex,GL_SPOT_CUTOFF,fDegrees);

        kDir = -pkLight->GetWorldDVector();
        glLightfv(eIndex,GL_SPOT_DIRECTION,(float*)kDir);

        glLightf(eIndex,GL_SPOT_EXPONENT,pkLight->Exponent);
    }
    else
    {
        GLfloat afDefaultDir[3] = { 0.0f, 0.0f, -1.0f };
        glLightf(eIndex,GL_SPOT_CUTOFF,180.0f);
        glLightfv(eIndex,GL_SPOT_DIRECTION,afDefaultDir);
        glLightf(eIndex,GL_SPOT_EXPONENT,0.0f);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableLight (int i, const Light* pkLight)
{
    GLenum eIndex = (GLenum)(GL_LIGHT0 + i);
    glDisable(eIndex);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableTexture (int iUnit, int i, Effect* pkEffect)
{
    SetActiveTextureUnit(iUnit);
    if ( i < pkEffect->UVs.GetQuantity() )
    {
        Vector2fArray* pkUVs = pkEffect->UVs[i];
        if ( pkUVs )
            EnableUVs(pkUVs);
    }

    Texture* pkTexture = pkEffect->Textures[i];
    glEnable(GL_TEXTURE_2D);

    GLuint uiID;
    pkTexture->BIArray.GetID(this,sizeof(GLuint),&uiID);

    if ( uiID != 0 )
    {
        // texture already exists in OpenGL, just bind it
        glBindTexture(GL_TEXTURE_2D,uiID);
    }
    else
    {
        // texture seen first time, generate name and create data
        glGenTextures((GLsizei)1,&uiID);
        pkTexture->BIArray.Bind(this,sizeof(GLuint),&uiID);

        // bind the texture
        glBindTexture(GL_TEXTURE_2D,uiID);

        // Get texture image data.  Not all textures have image data.
        // For example, AM_COMBINE modes can use primary colors,
        // texture output, and constants to modify fragments via the
        // texture units.
        ImagePtr spkImage = pkTexture->GetImage();
        if ( spkImage )
        {
            if ( pkTexture->Mipmap == Texture::MM_NEAREST
            ||   pkTexture->Mipmap == Texture::MM_LINEAR )
            {
                glTexImage2D(GL_TEXTURE_2D,0,
                    ms_aeImageComponents[spkImage->GetFormat()],
                    spkImage->GetWidth(),spkImage->GetHeight(),0,
                    ms_aeImageFormats[spkImage->GetFormat()],
                    GL_UNSIGNED_BYTE,spkImage->GetData());
            }
            else
            {
                gluBuild2DMipmaps(GL_TEXTURE_2D,
                    ms_aeImageComponents[spkImage->GetFormat()],
                    spkImage->GetWidth(),spkImage->GetHeight(),
                    ms_aeImageFormats[spkImage->GetFormat()],
                    GL_UNSIGNED_BYTE,spkImage->GetData());
            }
        }

        // set up coordinate mode
        switch ( pkTexture->CoordU )
        {
        case Texture::WM_CLAMP:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
            break;
        case Texture::WM_REPEAT:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            break;
        case Texture::WM_CLAMP_BORDER:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,
                GL_CLAMP_TO_BORDER);
            break;
        case Texture::WM_CLAMP_EDGE:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            break;
        default:  // Texture::WM_QUANTITY
            break;
        }

        switch ( pkTexture->CoordV )
        {
        case Texture::WM_CLAMP:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
            break;
        case Texture::WM_REPEAT:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            break;
        case Texture::WM_CLAMP_BORDER:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,
                GL_CLAMP_TO_BORDER);
            break;
        case Texture::WM_CLAMP_EDGE:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            break;
        default:  // Texture::WM_QUANTITY
            break;
        }

        // set up filter mode
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
            ms_aeTextureFilter[pkTexture->Filter]);

        // set up mipmap mode
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
            ms_aeTextureMipmap[pkTexture->Mipmap]);

        // set up the border color (for clamp to border)
        if ( pkTexture->BorderColor != ColorRGBA::INVALID )
        {
            glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,
                (const float*)pkTexture->BorderColor);
        }
    }

    // set up correction mode
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,
        ms_aeTextureCorrection[pkTexture->Correction]);

    // set up the blend color
    glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,
        (const float*)pkTexture->BlendColor);

    // set up apply mode
    if ( pkTexture->Apply != Texture::AM_COMBINE )
    {
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,
            ms_aeTextureApply[pkTexture->Apply]);
    }
    else  // AM_COMBINE
    {
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,
            ms_aeTextureApply[Texture::AM_COMBINE]);

        glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,
            ms_aeTextureCombineFunc[pkTexture->CombineFuncRGB]);

        glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,
            ms_aeTextureCombineFunc[pkTexture->CombineFuncAlpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,
            ms_aeTextureCombineSrc[pkTexture->CombineSrc0RGB]);
            
        glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,
            ms_aeTextureCombineSrc[pkTexture->CombineSrc1RGB]);  

        glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,
            ms_aeTextureCombineSrc[pkTexture->CombineSrc2RGB]);

        glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,
            ms_aeTextureCombineSrc[pkTexture->CombineSrc0Alpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,
            ms_aeTextureCombineSrc[pkTexture->CombineSrc1Alpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,
            ms_aeTextureCombineSrc[pkTexture->CombineSrc2Alpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,
            ms_aeTextureCombineOperand[pkTexture->CombineOp0RGB]);

        glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,
            ms_aeTextureCombineOperand[pkTexture->CombineOp1RGB]);

        glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB,
            ms_aeTextureCombineOperand[pkTexture->CombineOp2RGB]);

        glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,
            ms_aeTextureCombineOperand[pkTexture->CombineOp0Alpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,
            ms_aeTextureCombineOperand[pkTexture->CombineOp1Alpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_ALPHA,
            ms_aeTextureCombineOperand[pkTexture->CombineOp2Alpha]);

        glTexEnvi(GL_TEXTURE_ENV,GL_RGB_SCALE,
            ms_aeTextureCombineOperand[pkTexture->CombineScaleRGB]);

        glTexEnvi(GL_TEXTURE_ENV,GL_ALPHA_SCALE,
            ms_aeTextureCombineOperand[pkTexture->CombineScaleAlpha]);
    }

    // set up texture coordinate generation
    switch ( pkTexture->Texgen )
    {
    case Texture::TG_NONE:
        break;
    case Texture::TG_ENVIRONMENT_MAP:
    {
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
        glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
        break;
    }
    case Texture::TG_PROJECTED_TEXTURE:
    {
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);

        // Select the camera coordinates of the model for the projected
        // texture coordinates:  (s,t,r,q) = (x_cam,y_cam,z_cam,w_cam)
        glTexGenfv(GL_S,GL_EYE_PLANE,(const float*)Vector4f::UNIT_X);
        glTexGenfv(GL_T,GL_EYE_PLANE,(const float*)Vector4f::UNIT_Y);
        glTexGenfv(GL_R,GL_EYE_PLANE,(const float*)Vector4f::UNIT_Z);
        glTexGenfv(GL_Q,GL_EYE_PLANE,(const float*)Vector4f::UNIT_W);
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
        glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
        glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
        glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

        // Create the transformation to map (s,t,r,q) to the coordinate
        // system of the projector camera.
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();

        // bias and scale the texture so it covers the near plane
        glTranslatef(0.5f,0.5f,0.0f);
        glScalef(0.5f,0.5f,1.0f);

        // set the perspective projection for the projector camera
        Camera* pkCamera = ((ProjectedTextureEffect*)pkEffect)->GetCamera();
        float fL, fR, fB, fT, fN, fF;
        pkCamera->GetFrustum(fL,fR,fB,fT,fN,fF);
        glFrustum(fL,fR,fB,fT,fN,fF);

        // set the model-view matrix for the projector camera
        Vector3f kLoc = pkCamera->GetWorldLocation();
        Vector3f kUVec = pkCamera->GetWorldUVector();
        Vector3f kDVec = pkCamera->GetWorldDVector();
        Vector3f kLookAt = kLoc + kDVec;
        gluLookAt(kLoc.X(),kLoc.Y(),kLoc.Z(),kLookAt.X(),kLookAt.Y(),
            kLookAt.Z(),kUVec.X(),kUVec.Y(),kUVec.Z());
        break;
    }
    default:
        assert( false );
        break;
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableTexture (int iUnit, int i, Effect* pkEffect)
{
    Texture* pkTexture = pkEffect->Textures[i];
    glActiveTextureARB(GL_TEXTURE0_ARB+iUnit);

    switch ( pkTexture->Texgen )
    {
    case Texture::TG_NONE:
        break;
    case Texture::TG_ENVIRONMENT_MAP:
    {
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        break;
    }
    case Texture::TG_PROJECTED_TEXTURE:
    {
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
        break;
    }
    default:
        assert( false );
        break;
    }

    glDisable(GL_TEXTURE_2D);

    glClientActiveTextureARB(GL_TEXTURE0_ARB+iUnit);
    if ( i < pkEffect->UVs.GetQuantity() )
    {
        Vector2fArray* pkUVs = pkEffect->UVs[i];
        if ( pkUVs )
            DisableUVs(pkUVs);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetActiveTextureUnit (int iUnit)
{
    glClientActiveTextureARB(GL_TEXTURE0_ARB+iUnit);
    glActiveTextureARB(GL_TEXTURE0_ARB+iUnit);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseTexture (Texture* pkTexture)
{
    // The Macintosh OpenGL appears to have a problem if the UserData value
    // is zero 0 (a null pointer).  The OpenGL reference on glDeleteTextures
    // indicates the zero should be ignored...
    assert( pkTexture );
    GLuint uiID;
    pkTexture->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteTextures((GLsizei)1,(GLuint*)&uiID);
        pkTexture->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetWorldTransformation ()
{
    // primitive is 3D
    SetWorldMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m_afWorldMatrix);
    if ( m_pkGeometry->World.IsUniformScale() )
    {
        if ( m_pkGeometry->World.GetUniformScale() != 1.0f )
            glEnable(GL_RESCALE_NORMAL);
    }
    else
    {
        glEnable(GL_NORMALIZE);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::RestoreWorldTransformation ()
{
    if ( m_pkGeometry->World.IsUniformScale() )
    {
        if ( m_pkGeometry->World.GetUniformScale() != 1.0f )
            glDisable(GL_RESCALE_NORMAL);
    }
    else
    {
        glDisable(GL_NORMALIZE);
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetScreenTransformation ()
{
    // primitive is screen polygon, already in correct coordinates
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();

    // Draw the polygon using an orthogonal frustum.  If the polygon has
    // a z-value of 0.0, it is drawn in front of everything.  If the
    // polygon has a z-value of 1.0, it is drawn in back of everything.
    glMatrixMode(GL_PROJECTION);    
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0,1.0,0.0,1.0,0.0,-1.0);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::RestoreScreenTransformation ()
{
    // restore to a perspective frustum
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableVertices ()
{
    // get vertices
    Vector3fArray* pkVertices = m_pkGeometry->Vertices;
    assert( pkVertices );
    Vector3f* akVertex = pkVertices->GetData();
    assert( akVertex );

    if ( pkVertices->IsExactly(CachedVector3fArray::TYPE) )
    {
        // vertices are cached
        CachedVector3fArray* pkCVertices = (CachedVector3fArray*)pkVertices;
        GLuint uiID;
        pkCVertices->BIArray.GetID(this,sizeof(GLuint),&uiID);

        if ( uiID > 0 )
        {
            // vertices already cached, just bind them
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);
        }
        else
        {
            // vertices seen first time, generate name and create data
            glGenBuffersARB((GLsizei)1,&uiID);
            pkCVertices->BIArray.Bind(this,sizeof(GLuint),&uiID);

            // bind the vertices
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);

            // copy the data to the buffer
            glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                pkVertices->GetQuantity()*sizeof(Vector3f),akVertex,
                GL_STATIC_DRAW_ARB);
        }

        akVertex = 0;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,akVertex);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableVertices ()
{
    if ( m_pkGeometry->Vertices->IsExactly(CachedVector3fArray::TYPE) )
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

    glDisableClientState(GL_VERTEX_ARRAY);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableNormals ()
{
    // get normals
    Vector3fArray* pkNormals = m_pkGeometry->Normals;
    Vector3f* akNormal = pkNormals->GetData();

    if ( pkNormals->IsExactly(CachedVector3fArray::TYPE) )
    {
        // normals are cached
        CachedVector3fArray* pkCNormals = (CachedVector3fArray*)pkNormals;
        GLuint uiID;
        pkCNormals->BIArray.GetID(this,sizeof(GLuint),&uiID);

        if ( uiID > 0 )
        {
            // normals already cached, just bind them
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);
        }
        else
        {
            // normals seen first time, generate name and create data
            glGenBuffersARB((GLsizei)1,&uiID);
            pkCNormals->BIArray.Bind(this,sizeof(GLuint),&uiID);

            // bind the normals
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);

            // copy the data to the buffer
            glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                pkNormals->GetQuantity()*sizeof(Vector3f),akNormal,
                GL_STATIC_DRAW_ARB);
        }

        akNormal = 0;
    }

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT,0,akNormal);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableNormals ()
{
    if ( m_pkGeometry->Normals->IsExactly(CachedVector3fArray::TYPE) )
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

    glDisableClientState(GL_NORMAL_ARRAY);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableColorRGBAs ()
{
    // get colors
    ColorRGBAArray* pkColors = m_pkLocalEffect->ColorRGBAs;
    ColorRGBA* akColor = pkColors->GetData();

    if ( pkColors->IsExactly(CachedColorRGBAArray::TYPE) )
    {
        // colors are cached
        CachedColorRGBAArray* pkCColors = (CachedColorRGBAArray*)pkColors;
        GLuint uiID;
        pkCColors->BIArray.GetID(this,sizeof(GLuint),&uiID);

        if ( uiID > 0 )
        {
            // colors already cached, just bind them
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);
        }
        else
        {
            // colors seen first time, generate name and create data
            glGenBuffersARB((GLsizei)1,&uiID);
            pkCColors->BIArray.Bind(this,sizeof(GLuint),&uiID);

            // bind the colors
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);

            // copy the data to the buffer
            glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                pkColors->GetQuantity()*sizeof(ColorRGBA),akColor,
                GL_STATIC_DRAW_ARB);
        }

        akColor = 0;
    }

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4,GL_FLOAT,0,akColor);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableColorRGBAs ()
{
    if ( m_pkLocalEffect->ColorRGBAs->IsExactly(CachedColorRGBAArray::TYPE) )
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

    glDisableClientState(GL_COLOR_ARRAY);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableColorRGBs ()
{
    // get colors
    ColorRGBArray* pkColors = m_pkLocalEffect->ColorRGBs;
    ColorRGB* akColor = pkColors->GetData();

    if ( pkColors->IsExactly(CachedColorRGBArray::TYPE) )
    {
        // colors are cached
        CachedColorRGBArray* pkCColors = (CachedColorRGBArray*)pkColors;
        GLuint uiID;
        pkCColors->BIArray.GetID(this,sizeof(GLuint),&uiID);

        if ( uiID > 0 )
        {
            // colors already cached, just bind them
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);
        }
        else
        {
            // colors seen first time, generate name and create data
            glGenBuffersARB((GLsizei)1,&uiID);
            pkCColors->BIArray.Bind(this,sizeof(GLuint),&uiID);

            // bind the colors
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);

            // copy the data to the buffer
            glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                pkColors->GetQuantity()*sizeof(ColorRGB),akColor,
                GL_STATIC_DRAW_ARB);
        }

        akColor = 0;
    }

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_FLOAT,0,akColor);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableColorRGBs ()
{
    if ( m_pkLocalEffect->ColorRGBs->IsExactly(CachedColorRGBArray::TYPE) )
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

    glDisableClientState(GL_COLOR_ARRAY);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableUVs (Vector2fArray* pkUVs)
{
    // get uv's
    Vector2f* akUV = pkUVs->GetData();

    if ( pkUVs->IsExactly(CachedVector2fArray::TYPE) )
    {
        // uv's are cached
        CachedVector2fArray* pkCUVs = (CachedVector2fArray*)pkUVs;
        GLuint uiID;
        pkCUVs->BIArray.GetID(this,sizeof(GLuint),&uiID);

        if ( uiID > 0 )
        {
            // uv's already cached, just bind them
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);
        }
        else
        {
            // uv's seen first time, generate name and create data
            glGenBuffersARB((GLsizei)1,&uiID);
            pkCUVs->BIArray.Bind(this,sizeof(GLuint),&uiID);

            // bind the uv's
            glBindBufferARB(GL_ARRAY_BUFFER_ARB,uiID);

            // copy the data to the buffer
            glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                pkUVs->GetQuantity()*sizeof(Vector2f),akUV,
                GL_STATIC_DRAW_ARB);
        }

        akUV = 0;
    }

    glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
    glTexCoordPointer(2,GL_FLOAT,0,akUV);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableUVs (Vector2fArray* pkUVs)
{
    if ( pkUVs->IsExactly(CachedVector2fArray::TYPE) )
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DrawElements ()
{
    // get indices
    IntArray* pkIndices = m_pkGeometry->Indices;
    int iIQuantity = pkIndices->GetQuantity();
    int* aiIndex = pkIndices->GetData();

    bool bCached = pkIndices->IsExactly(CachedIntArray::TYPE);
    if ( bCached )
    {
        // indices are cached
        CachedIntArray* pkCIndices = (CachedIntArray*)pkIndices;
        GLuint uiID;
        pkCIndices->BIArray.GetID(this,sizeof(GLuint),&uiID);

        if ( uiID > 0 )
        {
            // indices already cached, just bind them
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,uiID);
        }
        else
        {
            // indices seen first time, generate name and create data
            glGenBuffersARB((GLsizei)1,&uiID);
            pkCIndices->BIArray.Bind(this,sizeof(GLuint),&uiID);

            // bind the indices
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,uiID);

            // copy the data to the buffer
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                iIQuantity*sizeof(int),aiIndex,GL_STATIC_DRAW_ARB);
        }

        aiIndex = 0;
    }

    GLenum eType = ms_aeObjectType[m_pkGeometry->GeometryType];
    glDrawElements(eType,iIQuantity,GL_UNSIGNED_INT,aiIndex);

    if ( bCached )
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseArray (CachedColorRGBAArray* pkArray)
{
    assert( pkArray );
    GLuint uiID;
    pkArray->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteBuffersARB((GLsizei)1,(GLuint*)&uiID);
        pkArray->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseArray (CachedColorRGBArray* pkArray)
{
    assert( pkArray );
    GLuint uiID;
    pkArray->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteBuffersARB((GLsizei)1,(GLuint*)&uiID);
        pkArray->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseArray (CachedIntArray* pkArray)
{
    assert( pkArray );
    GLuint uiID;
    pkArray->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteBuffersARB((GLsizei)1,(GLuint*)&uiID);
        pkArray->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseArray (CachedVector2fArray* pkArray)
{
    assert( pkArray );
    GLuint uiID;
    pkArray->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteBuffersARB((GLsizei)1,(GLuint*)&uiID);
        pkArray->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseArray (CachedVector3fArray* pkArray)
{
    assert( pkArray );
    GLuint uiID;
    pkArray->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteBuffersARB((GLsizei)1,(GLuint*)&uiID);
        pkArray->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableShader (VertexShader* pkVShader)
{
    glEnable(GL_VERTEX_PROGRAM_ARB);

    GLuint uiID;
    pkVShader->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID != 0 )
    {
        // shader already exists in OpenGL, just bind it
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB,uiID);
    }
    else
    {
        // shader seen first time, compile it
        glGenProgramsARB((GLsizei)1,&uiID);
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB,uiID);

        const char* acProgram = pkVShader->GetProgram();
        glProgramStringARB(GL_VERTEX_PROGRAM_ARB,
            GL_PROGRAM_FORMAT_ASCII_ARB,(GLsizei)strlen(acProgram),acProgram);

        pkVShader->BIArray.Bind(this,sizeof(GLuint),&uiID);
    }

    int iCQuantity = pkVShader->GetConstantQuantity();
    for (int i = 0; i < iCQuantity; i++)
    {
        // get a constant
        ShaderConstantPtr spkConstant = pkVShader->GetConstant(i);
        int iSCType = spkConstant->GetSCType();

        // numerical constants are handled automatically by openGL
        if ( iSCType == ShaderConstant::SC_NUMERICAL_CONSTANT )
            continue;

        // constant is based on render state
        int iOption = spkConstant->GetSCOption();
        float* afData = spkConstant->Data();
        if ( iSCType != ShaderConstant::SC_USER_DEFINED )
            (this->*ms_aoSCFunction[iSCType])(iOption,afData);

        // constant is user-defined
        int iRQuantity = spkConstant->GetRegisterQuantity();
        int iRegister = spkConstant->GetRegister();
        for (int j = 0; j < iRQuantity; j++)
        {
            glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB,
                iRegister+j,&afData[4*j]);
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableShader (PixelShader* pkPShader)
{
    glEnable(GL_FRAGMENT_PROGRAM_ARB);

    GLuint uiID;
    pkPShader->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID != 0 )
    {
        // shader already exists in OpenGL, just bind it
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,uiID);
    }
    else
    {
        // shader seen first time, compile it
        glGenProgramsARB((GLsizei)1,&uiID);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,uiID);

        const char* acProgram = pkPShader->GetProgram();
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
            GL_PROGRAM_FORMAT_ASCII_ARB,(GLsizei)strlen(acProgram),acProgram);

        pkPShader->BIArray.Bind(this,sizeof(GLuint),&uiID);
    }

    int iCQuantity = pkPShader->GetConstantQuantity();
    for (int i = 0; i < iCQuantity; i++)
    {
        // get a constant
        ShaderConstantPtr spkConstant = pkPShader->GetConstant(i);
        int iSCType = spkConstant->GetSCType();

        // numerical constants are handled automatically by openGL
        if ( iSCType == ShaderConstant::SC_NUMERICAL_CONSTANT )
            continue;

        // constant is based on render state
        int iOption = spkConstant->GetSCOption();
        float* afData = spkConstant->Data();
        if ( iSCType != ShaderConstant::SC_USER_DEFINED )
            (this->*ms_aoSCFunction[iSCType])(iOption,afData);

        // constant is user-defined
        int iRQuantity = spkConstant->GetRegisterQuantity();
        int iRegister = spkConstant->GetRegister();
        for (int j = 0; j < iRQuantity; j++)
        {
            glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,
                iRegister+j,&afData[4*j]);
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableShader (VertexShader*)
{
    glDisable(GL_VERTEX_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableShader (PixelShader*)
{
    glDisable(GL_FRAGMENT_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ReleaseShader (Shader* pkShader)
{
    assert( pkShader );
    GLuint uiID;
    pkShader->BIArray.GetID(this,sizeof(GLuint),&uiID);
    if ( uiID > 0 )
    {
        glDeleteProgramsARB((GLsizei)1,(GLuint*)&uiID);
        pkShader->BIArray.Unbind(this);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetConstantTransformM (int iOption, float* afData)
{
    Matrix4f kM(m_afWorldMatrix,false);
    GetTransform(kM,iOption,afData);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetConstantTransformP (int iOption, float* afData)
{
    float afProjection[16];
    glGetFloatv(GL_PROJECTION_MATRIX,afProjection);
    Matrix4f kP(afProjection,false);
    GetTransform(kP,iOption,afData);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetConstantTransformMV (int iOption, float* afData)
{
    float afModelView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,afModelView);
    Matrix4f kMV(afModelView,false);
    GetTransform(kMV,iOption,afData);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetConstantTransformMVP (int iOption, float* afData)
{
    float afProjection[16];
    glGetFloatv(GL_PROJECTION_MATRIX,afProjection);
    Matrix4f kP(afProjection,false);

    float afModelView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,afModelView);
    Matrix4f kMV(afModelView,false);

    Matrix4f kPMV = kP*kMV;
    GetTransform(kPMV,iOption,afData);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::GetTransform (Matrix4f& rkMat, int iOption,
    float* afData)
{
    // Wild Magic uses matrix M, OpenGL uses matrix Transpose(M).  The input
    // matrix is guaranteed to be in OpenGL format.  This ensures that the
    // final matrix is in row-major order and the memcpy to afData produces
    // a float[16] array in the format OpenGL expects.

    switch ( iOption )
    {
    case ShaderConstant::SCO_MATRIX:
        break;
    case ShaderConstant::SCO_TRANSPOSE:
        rkMat = rkMat.Transpose();
        break;
    case ShaderConstant::SCO_INVERSE:
        rkMat = rkMat.Inverse();
        break;
    case ShaderConstant::SCO_INVERSE_TRANSPOSE:
        rkMat = rkMat.Inverse().Transpose();
        break;
    default:
        assert( false );
        break;
    }

    memcpy(afData,(const float*)rkMat,16*sizeof(float));
}
//----------------------------------------------------------------------------
void OpenGLRenderer::Draw (const unsigned char* aucBuffer)
{
    if ( !aucBuffer )
        return;

    // disable other states that are not necessary for the buffer copy
    GLboolean bDepthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean bLighting = glIsEnabled(GL_LIGHTING);
    GLboolean bTexture2D = glIsEnabled(GL_TEXTURE_2D);
    if ( bDepthTest )
        glDisable(GL_DEPTH_TEST);
    if ( bLighting )
        glDisable(GL_LIGHTING);
    if ( bTexture2D )
        glDisable(GL_TEXTURE_2D);

    // Set raster position to window coord (0,H-1).  The hack here avoids
    // problems with invalid raster positions which would cause glDrawPixels
    // not to execute.  OpenGL uses right-handed screen coordinates, so using
    // (0,H-1) as the raster position followed by glPixelZoom(1,-1) tells
    // OpenGL to draw the screen in left-handed coordinates starting at the
    // top row of the screen and finishing at the bottom row.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0,(double)m_iWidth,0.0,(double)m_iHeight,0.0,1.0);
    glRasterPos3f(0.0,0.0,0.0);
    GLubyte aucBitmap[1] = {0};
    glBitmap(0,0,0.0f,0.0f,0.0f,(float)m_iHeight,aucBitmap);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPixelZoom(1.0f,-1.0f);
    glDrawPixels((GLint)m_iWidth,(GLint)m_iHeight,GL_BGR,GL_UNSIGNED_BYTE,
        (const GLvoid*)aucBuffer);
    glPixelZoom(1.0f,1.0f);

    // reenable states
    if ( bDepthTest )
        glEnable(GL_DEPTH_TEST);
    if ( bLighting )
        glEnable(GL_LIGHTING);
    if ( bTexture2D )
        glEnable(GL_TEXTURE_2D);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// driver and extension information
//----------------------------------------------------------------------------
const char* OpenGLRenderer::GetVendor ()
{
    return (const char*)glGetString(GL_VENDOR);
}
//----------------------------------------------------------------------------
const char* OpenGLRenderer::GetRenderer ()
{
    return (const char*)glGetString(GL_RENDERER);
}
//----------------------------------------------------------------------------
const char* OpenGLRenderer::GetVersion ()
{
    return (const char*)glGetString(GL_VERSION);
}
//----------------------------------------------------------------------------
const char* OpenGLRenderer::GetGluVersion ()
{
    return (const char*)glGetString((GLenum)GLU_VERSION);
}
//----------------------------------------------------------------------------
const char* OpenGLRenderer::GetExtensions ()
{
    return (const char*)glGetString(GL_EXTENSIONS);
}
//----------------------------------------------------------------------------
bool OpenGLRenderer::IsMinimumVersion (int iMajor, int iMinor, int iRelease)
{
    int iVMajor = 0, iVMinor = 0, iVRelease = 0;
    sscanf(GetVersion(),"%d.%d.%d",&iVMajor,&iVMinor,&iVRelease);

    if ( iVMajor < iMajor )
        return false;

    if ( iVMajor > iMajor )
        return true;

    if ( iVMinor < iMinor )
        return false;

    if ( iVMinor > iMinor )
        return true;

    if ( iVRelease < iRelease )
        return false;

    return true;
}
//----------------------------------------------------------------------------
bool OpenGLRenderer::ExtensionSupported (const char* acExt)
{
    // make sure a proper string has been passed
    if ( !acExt || acExt[0] == 0 )
        return false;

    // Without the typecast on strstr, CodeWarrior 9.x generates an error
    // because its C library appears to define the return value of strstr
    // as 'const char*'.  Other compilers appear to define the return value
    // as 'char*'.
    char* acBegin = (char*)strstr(GetExtensions(),acExt);
    if ( acBegin )
    {
        // The extension was found, but make sure it is not a proper substring
        // of another extension.  TO DO:  Should cEnd be compared to tabs or
        // other white space characters?
        char cEnd = *(acBegin + strlen(acExt));
        return cEnd == ' ' || cEnd == 0;
    }

    return false;
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EstablishCapabilities ()
{
    bool b1p2 = IsMinimumVersion(1,2,0);
    bool b1p3 = IsMinimumVersion(1,3,0);
    bool b1p4 = IsMinimumVersion(1,4,0);
    (void)b1p4;  // avoid compiler warning in release mode

    int iMaxTextureUnits;
    int iBits = 0;
    bool bCapSpecularAfterTexture;
    bool bCapMultitexture;
    bool bCapTextureClampToBorder;
    bool bCapTextureApplyAdd;
    bool bCapTextureApplyCombine;
    bool bCapTextureApplyCombineDot3;
    bool bCapDot3BumpMapping;
    bool bCapPlanarShadow;
    bool bCapPlanarReflection;
    bool bCapPixShaderVersion;
    bool bCapVertShaderVersion;

    //bool bVBO = ExtensionSupported("GL_ARB_vertex_buffer_object");

    // specular highlights drawn untinted after the texture color
    bCapSpecularAfterTexture = b1p2
        || ExtensionSupported("GL_EXT_separate_specular_color");

    // initialize the number of supported texture units
    if ( b1p3 || ExtensionSupported("GL_ARB_multitexture") )
    {
        // This code may look strange, but Apple OpenGL 1.2 defines GLint
        // as "long" whereas Windows and Linux define it as "int".
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&iMaxTextureUnits);
        bCapMultitexture = true;
    }
    else
    {
        iMaxTextureUnits = 1;
        bCapMultitexture = false;
    }

    // texture clamping to border
    bCapTextureClampToBorder = b1p3
        || ExtensionSupported("GL_ARB_texture_border_clamp");

    // texture apply add mode
    bCapTextureApplyAdd = b1p3
        || ExtensionSupported("GL_ARB_texture_env_add");

    // texture apply combine mode
    bCapTextureApplyCombine = b1p3
        || ExtensionSupported("GL_ARB_texture_env_combine");
    if ( bCapTextureApplyCombine )
    {
        bCapTextureApplyCombineDot3 = b1p3
            || ExtensionSupported("GL_ARB_texture_env_dot3");
    }

    // Dot3 bump mapping (combine, combine dot3, and multitexturing)
    bCapDot3BumpMapping =
        bCapTextureApplyCombine &&
        bCapTextureApplyCombineDot3 &&
        iMaxTextureUnits > 1;

    // query if stencil buffer was really created
    glGetIntegerv((GLenum)GL_STENCIL_BITS,&iBits);

    // projected planar shadow (uses stencil buffer)
    bCapPlanarShadow = ( iBits > 0 );

    // planar reflection (uses stencil buffer and blend color)
    bCapPlanarReflection = ( iBits > 0 &&
        ExtensionSupported("GL_EXT_blend_color") );

    // vertex/pixel shaders
    bCapPixShaderVersion = ExtensionSupported("GL_ARB_fragment_program");
    bCapVertShaderVersion = ExtensionSupported("GL_ARB_vertex_program");
}
//----------------------------------------------------------------------------
