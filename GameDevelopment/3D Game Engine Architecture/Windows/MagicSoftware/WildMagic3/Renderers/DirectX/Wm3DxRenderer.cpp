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
#include "Wm3Vector4.h"
#include "Wm3Polyline.h"
using namespace Wm3;

D3DPRIMITIVETYPE DxRenderer::ms_aeObjectType[Geometry::GT_MAX_QUANTITY] =
{
    D3DPT_POINTLIST,      // GT_POLYPOINT
    D3DPT_LINELIST,       // GT_POLYLINE_SEGMENTS
    D3DPT_LINESTRIP,      // GT_POLYLINE_OPEN
    D3DPT_LINESTRIP,      // GT_POLYLINE_CLOSED
    D3DPT_TRIANGLELIST    // GT_TRIMESH
};

const float DxRenderer::ms_fSqrtMaxReal = Mathf::Sqrt(Mathf::MAX_REAL);
HRESULT DxRenderer::ms_hResult = 0;

//----------------------------------------------------------------------------
int Renderer::GetType ()
{
    return RT_DIRECT3D;
}
//----------------------------------------------------------------------------
DxRenderer::DxRenderer (HWND hWnd, int iWidth, int iHeight)
    :
    Renderer(iWidth,iHeight),
    m_bCursorVisible(true),
    m_bDeviceLost(false)
{
    m_pqMain = Direct3DCreate9(D3D_SDK_VERSION);
    assert( m_pqMain );
    if ( !m_pqMain )
        return;

    D3DDISPLAYMODE kMode;
    ms_hResult = m_pqMain->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&kMode);
    assert( SUCCEEDED(ms_hResult) );

    m_kPresent.BackBufferWidth = iWidth;
    m_kPresent.BackBufferHeight = iHeight;
    m_kPresent.BackBufferFormat = kMode.Format;
    m_kPresent.BackBufferCount = 1;
    m_kPresent.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_kPresent.MultiSampleQuality = 0;
    m_kPresent.SwapEffect = D3DSWAPEFFECT_FLIP; 
    m_kPresent.hDeviceWindow = hWnd;
    m_kPresent.Windowed = true;
    m_kPresent.EnableAutoDepthStencil = TRUE;
    m_kPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_kPresent.Flags = 0;
    m_kPresent.FullScreen_RefreshRateInHz = 0;
    m_kPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    ms_hResult = m_pqMain->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
        &m_kPresent,
        &m_pqDevice);

    if ( FAILED(ms_hResult) )
    {
        assert( false );
        return;
    }

    // create default font
    m_iFontID = LoadFont("Arial",18,false,false);
    assert( m_iFontID == 0 );

    EstablishCapabilities();
    InitializeState();
}
//----------------------------------------------------------------------------
DxRenderer::~DxRenderer ()
{
    int i;
    for (i = 0; i < m_iMaxTextures; i++)
        m_pqDevice->SetTexture(i,0);

    // release all texture surfaces
    for (i = 0; i < m_kTextures.GetQuantity(); i++) 
    {
        ITexture* pqTexture = m_kTextures[i];
        pqTexture->Release();
    }

    // release all fonts
    for (i = 0; i < m_kFontArray.GetQuantity(); i++)
        UnloadFont(i);

    // clean up cursor
    if ( !m_bCursorVisible )
        ShowCursor(true);

    delete[] m_apkCurUVs;
    delete[] m_abTexUnitEnabled;
}
//----------------------------------------------------------------------------
void DxRenderer::InitializeState ()
{
    // enable z-buffering
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
    assert( SUCCEEDED(ms_hResult) );

    // turn off lighting (DX defaults to lighting on)
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
    assert( SUCCEEDED(ms_hResult) );

    m_apkCurUVs = new Vector2fArray*[m_iMaxTextures];
    m_abTexUnitEnabled = new bool[m_iMaxTextures];

    // initial world matrix is zero (will always be set properly later)
    memset(m_afWorldMatrix,0,16*sizeof(float));

    // disable all array state
    m_pkCurVertices = 0;
    m_pkCurNormals = 0;
    m_pkCurColorRGBs = 0;
    m_pkCurColorRGBAs = 0;
    for (int i = 0; i < m_iMaxTextures; i++)
    {
        m_apkCurUVs[i] = 0;
        m_abTexUnitEnabled[i] = false;
    }
    m_iCurTextureUnit = 0;
}
//----------------------------------------------------------------------------
bool DxRenderer::BeginScene ()
{
    ms_hResult = m_pqDevice->TestCooperativeLevel();
    
    switch ( ms_hResult )
    {
    case D3DERR_DEVICELOST:
        if ( !m_bDeviceLost )
        {
            m_bDeviceLost = true;
            OnDeviceLost();
        }
        return false;
    case D3DERR_DEVICENOTRESET:
        ResetDevice();
        m_bDeviceLost = false;
        break;
    }

    ms_hResult = m_pqDevice->BeginScene();
    assert( SUCCEEDED(ms_hResult) );

    return true;
}
//----------------------------------------------------------------------------
void DxRenderer::EndScene ()
{
    ms_hResult = m_pqDevice->EndScene();
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::OnDeviceLost ()
{
    m_kFontArray[0]->OnLostDevice();
}
//----------------------------------------------------------------------------
void DxRenderer::ResetDevice ()
{
    ms_hResult = m_pqDevice->Reset(&m_kPresent);
    assert( SUCCEEDED(ms_hResult) );
    m_kFontArray[0]->OnResetDevice();
    InitializeState();
}
//----------------------------------------------------------------------------
void DxRenderer::ClearBackBuffer ()
{
    ms_hResult = m_pqDevice->Clear(0,0,D3DCLEAR_TARGET,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearZBuffer ()
{
    ms_hResult = m_pqDevice->Clear(0,0,D3DCLEAR_ZBUFFER,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearStencilBuffer ()
{
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->Clear(0,0,D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearBuffers ()
{
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->Clear(0,0,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearBackBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    ms_hResult = m_pqDevice->Clear(1,&kRect,D3DCLEAR_TARGET,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearZBuffer (int iXPos, int iYPos, int iWidth, int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    ms_hResult = m_pqDevice->Clear(1,&kRect,D3DCLEAR_ZBUFFER,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->Clear(1,&kRect,D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ClearBuffers (int iXPos, int iYPos, int iWidth, int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->Clear(1,&kRect,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(m_kBackgroundColor.R(),m_kBackgroundColor.G(),
        m_kBackgroundColor.B(),1.0f),1.0f,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::DisplayBackBuffer ()
{
    ms_hResult = m_pqDevice->Present(0,0,0,0);
    if ( ms_hResult != D3DERR_DEVICELOST )
        assert( SUCCEEDED(ms_hResult) );
} 
//----------------------------------------------------------------------------
void DxRenderer::SetWorldMatrix ()
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
void DxRenderer::OnFrustumChange ()
{
    if ( m_pkCamera )
    {
        // WM uses right handed coordinates.  DX uses left handed coordinates.    
        // Define as if for right handed coordinates and then negate the
        // the z row.
        float fRMin, fRMax, fUMin, fUMax, fDMin, fDMax;
        m_pkCamera->GetFrustum(fRMin,fRMax,fUMin,fUMax,fDMin,fDMax);
        D3DXMatrixPerspectiveOffCenterRH(&m_kProjMatrix,fRMin,fRMax,fUMin,
            fUMax,fDMin,fDMax);

        m_kProjMatrix(2,0) = -m_kProjMatrix(2,0);
        m_kProjMatrix(2,1) = -m_kProjMatrix(2,1);
        m_kProjMatrix(2,2) = -m_kProjMatrix(2,2);
        m_kProjMatrix(2,3) = -m_kProjMatrix(2,3);
    }
    else
    {
        D3DXMatrixOrthoOffCenterRH(&m_kProjMatrix,0.0f,1.0f,0.0f,1.0f,0.0f,
            1.0f);
    }
                            
    ms_hResult = m_pqDevice->SetTransform(D3DTS_PROJECTION,&m_kProjMatrix);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::OnViewPortChange ()
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

    D3DVIEWPORT9 kViewport;
    kViewport.X = (DWORD)(fL*m_iWidth);
    kViewport.Y = (DWORD)(fB*m_iHeight);
    kViewport.Width = (DWORD)((fR - fL)*m_iWidth);
    kViewport.Height = (DWORD)((fT - fB)*m_iHeight);
    kViewport.MinZ = 0.0f;
    kViewport.MaxZ = 1.0f;

    ms_hResult = m_pqDevice->SetViewport(&kViewport);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::OnFrameChange ()
{
    if ( m_pkCamera )
    {
        // WM uses right handed coordinates.  DX uses left handed coordinates.
        // Define as if for right handed coordinates and then negate the z
        // column.
        Vector3f kL = m_pkCamera->GetLocation();
        Vector3f kU = m_pkCamera->GetUVector();
        Vector3f kD = m_pkCamera->GetDVector();

        D3DXVECTOR3 kEye(kL.X(),kL.Y(),kL.Z());
        D3DXVECTOR3 kAt(kL.X()+kD.X(),kL.Y()+kD.Y(),kL.Z()+kD.Z());
        D3DXVECTOR3 kUp(kU.X(),kU.Y(),kU.Z());
        D3DXMatrixLookAtRH(&m_kViewMatrix,&kEye,&kAt,&kUp);
        m_kViewMatrix(0,2) = -m_kViewMatrix(0,2);
        m_kViewMatrix(1,2) = -m_kViewMatrix(1,2);
        m_kViewMatrix(2,2) = -m_kViewMatrix(2,2);
        m_kViewMatrix(3,2) = -m_kViewMatrix(3,2);
    }
    else
    {
        D3DXVECTOR3 kEye(0.0f,0.0f,1.0f);
        D3DXVECTOR3 kAt(0.0f,0.0f,0.0f);
        D3DXVECTOR3 kUp(0.0f,1.0f,0.0f);
        D3DXMatrixLookAtRH(&m_kViewMatrix,&kEye,&kAt,&kUp);
    }

    ms_hResult = m_pqDevice->SetTransform(D3DTS_VIEW,&m_kViewMatrix);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::EnableLighting (int eEnable)
{
    m_bSpecularEnable = false;

    int iQuantity = m_pkGeometry->Lights.GetQuantity();
    if ( iQuantity >= m_iMaxLights )
        iQuantity = m_iMaxLights;

    if ( iQuantity > 0 )
    {
        for (int i = 0; i < iQuantity; i++)
        {
            const Light* pkLight = m_pkGeometry->Lights[i];
            if ( pkLight->On )
                EnableLight(eEnable,i,pkLight);
        }

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
        assert( SUCCEEDED(ms_hResult) );

        if ( m_bSpecularEnable )
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_SPECULARENABLE,
                TRUE);
            assert( SUCCEEDED(ms_hResult) );
        }
    }
}
//----------------------------------------------------------------------------
void DxRenderer::DisableLighting ()
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

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
        assert( SUCCEEDED(ms_hResult) );

        if ( m_bSpecularEnable )
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_SPECULARENABLE,
                FALSE);
            assert( SUCCEEDED(ms_hResult) );
        }

        m_bSpecularEnable = false;
    }
}
//----------------------------------------------------------------------------
void DxRenderer::EnableLight (int eEnable, int i, const Light* pkLight)
{
    // Enable the light source.
    ms_hResult = m_pqDevice->LightEnable(i,TRUE);
    assert( SUCCEEDED(ms_hResult) );

    D3DLIGHT9 kD3DLight;
    kD3DLight.Falloff = 1.0f;
    kD3DLight.Theta = 0.0f;
    kD3DLight.Phi = 0.0f;
    kD3DLight.Range = ms_fSqrtMaxReal;
    ColorRGBA kColor;

    if ( eEnable & EL_AMBIENT )
    {
        kColor = pkLight->Intensity*pkLight->Ambient;
        kColor.Clamp();
        kD3DLight.Ambient.r = kColor.R();
        kD3DLight.Ambient.g = kColor.G();
        kD3DLight.Ambient.b = kColor.B();
        kD3DLight.Ambient.a = kColor.A();
    }
    else
    {
        kD3DLight.Ambient.r = 0.0f;
        kD3DLight.Ambient.g = 0.0f;
        kD3DLight.Ambient.b = 0.0f;
        kD3DLight.Ambient.a = 1.0f;
    }

    // diffuse
    if ( (eEnable & EL_DIFFUSE) && pkLight->Type != Light::LT_AMBIENT )
    {
        kColor = pkLight->Intensity*pkLight->Diffuse;
        kColor.Clamp();
        kD3DLight.Diffuse.r = kColor.R();
        kD3DLight.Diffuse.g = kColor.G();
        kD3DLight.Diffuse.b = kColor.B();
        kD3DLight.Diffuse.a = kColor.A();
    }
    else
    {
        kD3DLight.Diffuse.r = 0.0f;
        kD3DLight.Diffuse.g = 0.0f;
        kD3DLight.Diffuse.b = 0.0f;
        kD3DLight.Diffuse.a = 1.0f;
    }

    // specular
    if ( (eEnable & EL_SPECULAR) && pkLight->Type != Light::LT_AMBIENT )
    {
        kColor = pkLight->Intensity*pkLight->Specular;
        kColor.Clamp();
        kD3DLight.Specular.r = kColor.R();
        kD3DLight.Specular.g = kColor.G();
        kD3DLight.Specular.b = kColor.B();
        kD3DLight.Specular.a = kColor.A();

        // DirectX only has an option to enable specular highlighting
        // or not.  If enabled, specular automatically occurs after
        // texture processing but before alpha blending.
        if ( ( kD3DLight.Specular.r > 0.0f ) ||
             ( kD3DLight.Specular.g > 0.0f ) ||
             ( kD3DLight.Specular.b > 0.0f ) )
        {
            m_bSpecularEnable = true;
        }
    }
    else
    {
        kD3DLight.Specular.r = 0.0f;
        kD3DLight.Specular.g = 0.0f;
        kD3DLight.Specular.b = 0.0f;
        kD3DLight.Specular.a = 1.0f;
    }

    if ( pkLight->Attenuate )
    {
        kD3DLight.Attenuation0 = pkLight->Constant;
        kD3DLight.Attenuation1 = pkLight->Linear;
        kD3DLight.Attenuation2 = pkLight->Quadratic;
    }
    else
    {
        kD3DLight.Attenuation0 = 1.0f;
        kD3DLight.Attenuation1 = 0.0f;
        kD3DLight.Attenuation2 = 0.0f;
    }

    switch ( pkLight->Type )
    {
    case Light::LT_AMBIENT:
        // These parameters are probably not necessary since the diffuse and
        // specular components of the light are black.  The values are the
        // same defaults that are used in the OpenGL renderer.
        kD3DLight.Type = D3DLIGHT_DIRECTIONAL;
        kD3DLight.Direction.x = 0.0f;
        kD3DLight.Direction.y = 0.0f;
        kD3DLight.Direction.z = 1.0f;
        break;
    case Light::LT_DIRECTIONAL:
        kD3DLight.Type = D3DLIGHT_DIRECTIONAL;
        kD3DLight.Direction.x = pkLight->GetDVector().X();
        kD3DLight.Direction.y = pkLight->GetDVector().Y();
        kD3DLight.Direction.z = pkLight->GetDVector().Z();
        break;
    case Light::LT_POINT:
        kD3DLight.Type = D3DLIGHT_POINT;
        kD3DLight.Position.x = pkLight->GetLocation().X();
        kD3DLight.Position.y = pkLight->GetLocation().Y();
        kD3DLight.Position.z = pkLight->GetLocation().Z();
        break;
    case Light::LT_SPOT:
        kD3DLight.Type = D3DLIGHT_SPOT;
        kD3DLight.Position.x = pkLight->GetLocation().X();
        kD3DLight.Position.y = pkLight->GetLocation().Y();
        kD3DLight.Position.z = pkLight->GetLocation().Z();
        kD3DLight.Direction.x = pkLight->GetDVector().X();
        kD3DLight.Direction.y = pkLight->GetDVector().Y();
        kD3DLight.Direction.z = pkLight->GetDVector().Z();
        kD3DLight.Phi = 2.1f*pkLight->Angle;
        kD3DLight.Theta = 2.0f*pkLight->Angle;
        kD3DLight.Falloff = pkLight->Exponent;  
        break;
    }

    ms_hResult = m_pqDevice->SetLight(i,&kD3DLight);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::DisableLight (int i, const Light* pkLight)
{
    ms_hResult = m_pqDevice->LightEnable(i,FALSE);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::SetWorldTransformation ()
{
    // primitive is 3D
    SetWorldMatrix();
    StoreWorldMatrix();

    D3DXMATRIX kWorld(m_afWorldMatrix);

    ms_hResult = m_pqDevice->SetTransform(D3DTS_WORLDMATRIX(0),
        &kWorld); 
    assert( SUCCEEDED(ms_hResult) );

    if ( m_pkGeometry->World.IsUniformScale() )
    {
        if ( m_pkGeometry->World.GetUniformScale() != 1.0f )
            m_pqDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
    }
    else
    {
        m_pqDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
    }
}
//----------------------------------------------------------------------------
void DxRenderer::RestoreWorldTransformation ()
{
    if ( m_pkGeometry->World.IsUniformScale() )
    {
        if ( m_pkGeometry->World.GetUniformScale() != 1.0f )
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_NORMALIZENORMALS,
                TRUE);
            assert( SUCCEEDED(ms_hResult) );
        }
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
        assert( SUCCEEDED(ms_hResult) );
    }
    RestoreWorldMatrix();
}
//----------------------------------------------------------------------------
void DxRenderer::SetScreenTransformation ()
{
    StoreWorldMatrix();

    // The primitive is a screen polygon, which is already in the correct
    // coordinates.  Set the world matrix to the identity.
    memcpy(m_afWorldMatrix,(const float*)Matrix4f::IDENTITY,16*sizeof(float));

    // Draw the polygon using an orthogonal frustum.  If the polygon has
    // a z-value of 0.0, it is drawn in front of everything.  If the
    // polygon has a z-value of 1.0, it is drawn in back of everything.

    // set up an orthogonal view frustum in normalized coordinates
    D3DXMatrixOrthoOffCenterRH(&m_kProjMatrix,0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    ms_hResult = m_pqDevice->SetTransform(D3DTS_PROJECTION,&m_kProjMatrix);
    assert( SUCCEEDED(ms_hResult) );

    D3DXVECTOR3 kEye(0.0f,0.0f,1.0f);
    D3DXVECTOR3 kAt(0.0f,0.0f,0.0f);
    D3DXVECTOR3 kUp(0.0f,1.0f,0.0f);
    D3DXMatrixLookAtRH(&m_kViewMatrix,&kEye,&kAt,&kUp);
    ms_hResult = m_pqDevice->SetTransform(D3DTS_VIEW,&m_kViewMatrix);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::RestoreScreenTransformation ()
{
    // restore world matrix
    RestoreWorldMatrix();

    // restore projection matrix
    OnFrustumChange();

    // restore view matrix
    OnFrameChange();
}
//----------------------------------------------------------------------------
void DxRenderer::StoreWorldMatrix ()
{
    // Store current world
    memcpy(m_afStoredMatrix,m_afWorldMatrix,16*sizeof(float));
}
//----------------------------------------------------------------------------
void DxRenderer::RestoreWorldMatrix ()
{
    // Restore previously stored world
    memcpy(m_afWorldMatrix,m_afStoredMatrix,16*sizeof(float));
}
//----------------------------------------------------------------------------
void DxRenderer::EnableVertices ()
{
    assert(m_pkGeometry->Vertices);
    assert(m_pkGeometry->Vertices->GetData());
    m_pkCurVertices = m_pkGeometry->Vertices;
}
//----------------------------------------------------------------------------
void DxRenderer::DisableVertices ()
{
    m_pkCurVertices = 0;
}
//----------------------------------------------------------------------------
void DxRenderer::EnableNormals ()
{
    assert(m_pkGeometry->Normals);
    assert(m_pkGeometry->Normals->GetData());
    m_pkCurNormals = m_pkGeometry->Normals;
}
//----------------------------------------------------------------------------
void DxRenderer::DisableNormals ()
{
    m_pkCurNormals = 0;
}
//----------------------------------------------------------------------------
void DxRenderer::EnableColorRGBAs ()
{
    assert(m_pkLocalEffect->ColorRGBAs);
    assert(m_pkLocalEffect->ColorRGBAs->GetData());
    m_pkCurColorRGBAs = m_pkLocalEffect->ColorRGBAs;
    m_pkCurColorRGBs = 0;

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORVERTEX,TRUE);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::DisableColorRGBAs ()
{
    m_pkCurColorRGBAs = 0;

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORVERTEX,FALSE);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::EnableColorRGBs ()
{
    assert(m_pkLocalEffect->ColorRGBs);
    assert(m_pkLocalEffect->ColorRGBs->GetData());
    m_pkCurColorRGBs = m_pkLocalEffect->ColorRGBs;
    m_pkCurColorRGBAs = 0;

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORVERTEX,TRUE);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::DisableColorRGBs ()
{
    m_pkCurColorRGBs = 0;

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORVERTEX,FALSE);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::EnableUVs (Vector2fArray* pkUVs)
{
    // Enabling UVs for a given texture unit if they are already enabled for
    // that unit will silently overwrite the previously texture coordinates.
    assert( pkUVs );
    assert( pkUVs->GetData() );
    assert( m_iCurTextureUnit < m_iMaxTextures );
    m_apkCurUVs[m_iCurTextureUnit] = pkUVs;
}
//----------------------------------------------------------------------------
void DxRenderer::DisableUVs (Vector2fArray* pkUVs)
{
    assert( m_iCurTextureUnit < m_iMaxTextures );
    m_apkCurUVs[m_iCurTextureUnit] = 0;
}
//----------------------------------------------------------------------------
void DxRenderer::DrawElements ()
{
    // Setup vertex format.  
    // The m_pkCur* member variables determine which components have been
    // enabled or disabled.

    // There should always be vertices
    assert( m_pkCurVertices );
    if ( !m_pkCurVertices )
        return;

    DWORD dwFormat = D3DFVF_XYZ;
    int iVertexSize = 3*sizeof(float);

    // Normals?
    if ( m_pkCurNormals )
    {
        dwFormat |= D3DFVF_NORMAL;
        iVertexSize += 3*sizeof(float);
    }

    // Colors?
    if ( m_pkCurColorRGBs || m_pkCurColorRGBAs )
    {
        dwFormat |= D3DFVF_DIFFUSE;
        iVertexSize += sizeof(DWORD);
    }

    // find max texture unit enabled
    for (int iTexUnits = m_iMaxTextures; iTexUnits > 0; iTexUnits--)
    {
        // If this texture unit is enabled, or if there are texture
        // coordinates, which may be the case for a shader with coordinates
        // but no texture, then this unit is considered active and is the
        // maximum texture unit being used.
        if ( m_abTexUnitEnabled[iTexUnits-1] || m_apkCurUVs[iTexUnits-1] )
            break;
    }

    // Set any texture unit in the range 0-iTexUnits which is not enabled to
    // pass through information.
    int iUnit;
    for (iUnit = 0; iUnit < iTexUnits; iUnit++)
    {
        if ( !m_abTexUnitEnabled[iUnit] )
        {
            // set texture unit to passthrough
            ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
                D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU+iUnit);
            assert( SUCCEEDED(ms_hResult) );
            // no automatic texture coordinate generation
            ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
                D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
            assert( SUCCEEDED(ms_hResult) );

            // unassociate the texture for this stage
            ms_hResult = m_pqDevice->SetTexture(iUnit,0);
            assert( SUCCEEDED(ms_hResult) );
        }

        dwFormat |= D3DFVF_TEXCOORDSIZE2(iUnit);
        iVertexSize += 2*sizeof(float);
    }

    // If fewer units than the maximum are enabled, disable texture unit
    // iTexUnits so that DirectX knows there are no more textures.
    if ( iTexUnits < m_iMaxTextures )
    {
        // disable this texture stage
        ms_hResult = m_pqDevice->SetTextureStageState(iTexUnits,
            D3DTSS_COLOROP,D3DTOP_DISABLE);
        assert( SUCCEEDED(ms_hResult) );

        // unassociate any texture for this stage
        ms_hResult = m_pqDevice->SetTexture(iTexUnits,0);
        assert( SUCCEEDED(ms_hResult) );
    }

    // The index of the maximum texture unit needed determines the maximum
    // number of texture coordinate sets defined.
    dwFormat |= (iTexUnits << D3DFVF_TEXCOUNT_SHIFT);

    // create a vertex buffer
    int iNumVertices = m_pkCurVertices->GetQuantity();

    IVertexBufferPtr pqVertexBuffer;
    ms_hResult = m_pqDevice->CreateVertexBuffer(iVertexSize*iNumVertices,
        D3DUSAGE_WRITEONLY,dwFormat,D3DPOOL_MANAGED,&pqVertexBuffer,0);
    assert( SUCCEEDED(ms_hResult) );

    // Pack the vertex buffer with Wild Magic data.  TO DO:  Some day allow
    // conditional compiling of the Wild Magic data to allow the format to
    // match what the renderer expects.  This will avoid repacking data in
    // a strided manner as occurs below.
    BYTE* pbVertices;
    ms_hResult = pqVertexBuffer->Lock(0,0,(void**)(&pbVertices),0);
    assert( SUCCEEDED(ms_hResult) );

    // In the Flexible Vertex Format, if the following vertex attributes are
    // defined, they must appear in the vertex "structure" in the following
    // order:
    //  . vertex coordinates
    //  . normal vector
    //  . color RGBA
    //  . texture coordinates
    
    int iE = 0;
    BYTE* pbCurrentVertex = (BYTE*)pbVertices;
    int i;
    Vector3f* akVectors = m_pkCurVertices->GetData();
    for (i = 0; i < iNumVertices; i++)
    {
        memcpy(pbCurrentVertex,akVectors,3*sizeof(float));
        pbCurrentVertex += iVertexSize;
        akVectors++;
    }

    pbVertices += 3*sizeof(float);

    if ( m_pkCurNormals )
    {
        akVectors = m_pkCurNormals->GetData();
        for (i = 0, pbCurrentVertex = pbVertices; i < iNumVertices; i++)
        {
            memcpy(pbCurrentVertex,akVectors,3*sizeof(float));
            pbCurrentVertex += iVertexSize;
            akVectors++;
        }
        pbVertices += 3*sizeof(float);
    }

    if ( m_pkCurColorRGBs )
    {
        ColorRGB* akColor = m_pkCurColorRGBs->GetData();
        for (i = 0, pbCurrentVertex = pbVertices; i < iNumVertices; i++)
        {
            DWORD dwColor = D3DCOLOR_COLORVALUE(akColor->R(),akColor->G(),
                akColor->B(),1.0f);
            memcpy(pbCurrentVertex,&dwColor,sizeof(DWORD));
            pbCurrentVertex += iVertexSize;
            akColor++;
        }
        pbVertices += sizeof(DWORD);
    }
    else if ( m_pkCurColorRGBAs )
    {
        ColorRGBA* akColor = m_pkCurColorRGBAs->GetData();
        for (i = 0, pbCurrentVertex = pbVertices; i < iNumVertices; i++)
        {
            DWORD dwColor = D3DCOLOR_COLORVALUE(akColor->R(),akColor->G(),
                akColor->B(),akColor->A());
            memcpy(pbCurrentVertex,&dwColor,sizeof(DWORD));
            pbCurrentVertex += iVertexSize;
            akColor++;
        }
        pbVertices += sizeof(DWORD);
    }

    for (iUnit = 0; iUnit < iTexUnits; iUnit++)
    {
        // UVs may be null here.  If none, then try to use the
        // first set of texture coordinates.
        Vector2f* akTexture = 0;
        if ( m_apkCurUVs[iUnit] )
            akTexture = m_apkCurUVs[iUnit]->GetData();
        else if ( m_apkCurUVs[0] )
            akTexture = m_apkCurUVs[0]->GetData();

        if ( akTexture )
        {
            for (i = 0, pbCurrentVertex = pbVertices; i < iNumVertices; i++)
            {
                memcpy(pbCurrentVertex,akTexture,2*sizeof(float));
                pbCurrentVertex += iVertexSize;
                akTexture++;
            }
        }
        pbVertices += 2*sizeof(float);
    }

    ms_hResult = pqVertexBuffer->Unlock();
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->SetStreamSource(0,pqVertexBuffer,0,iVertexSize);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->SetFVF(dwFormat);
    assert( SUCCEEDED(ms_hResult) );

    // create an index buffer
    int iIQuantity = m_pkGeometry->Indices->GetQuantity();

    // No direct support for closed polyline loops.  Just add another
    // index at the end to go back to the beginning.
    bool bIsPolyLoop =
        ( m_pkGeometry->GeometryType == Geometry::GT_POLYLINE_CLOSED );
    if ( bIsPolyLoop )
        iIQuantity++;

    unsigned int uiBufferLength = (unsigned int)(iIQuantity*sizeof(short));
    IIndexBufferPtr pqIndexBuffer;
    ms_hResult = m_pqDevice->CreateIndexBuffer(uiBufferLength,
        D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pqIndexBuffer,0);
    assert( SUCCEEDED(ms_hResult) );

    // Construct an index list.  D3D expects 'unsigned short' index values.
    // Wild Magic stores these as 'int' to allow for more than 65535 triangles
    // in a mesh.  Thus, the 'int' indices must be packaged appropriately.
    void* pbIndices;
    ms_hResult = pqIndexBuffer->Lock(0,uiBufferLength,&pbIndices,0);
    assert( SUCCEEDED(ms_hResult) );

    unsigned short* pusIndex = (unsigned short*)pbIndices;
    const int* piIndex = m_pkGeometry->Indices->GetData();
    for (int i = 0; i < m_pkGeometry->Indices->GetQuantity(); i++)
        *pusIndex++ = (unsigned short)(*piIndex++);

    // Copy first index into last index to complete the poly loop
    if ( bIsPolyLoop )
        *pusIndex = (unsigned short)((m_pkGeometry->Indices->GetData())[0]);

    ms_hResult = pqIndexBuffer->Unlock();
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->SetIndices(pqIndexBuffer);
    assert( SUCCEEDED(ms_hResult) );

    int iPQuantity;
    int iAQuantity;

    // depending on the geometry, get the correct active and primitive
    // quantities.
    switch ( m_pkGeometry->GeometryType )
    {
    case Geometry::GT_POLYLINE_SEGMENTS:
        iAQuantity = StaticCast<Polyline>(m_pkGeometry)->GetActiveQuantity();
        iPQuantity = iAQuantity / 2;
        break;
    case Geometry::GT_POLYLINE_OPEN:
        iAQuantity = StaticCast<Polyline>(m_pkGeometry)->GetActiveQuantity();
        iPQuantity = iAQuantity - 1;
        break;
    case Geometry::GT_POLYPOINT:
    case Geometry::GT_POLYLINE_CLOSED:
        iAQuantity = StaticCast<Polyline>(m_pkGeometry)->GetActiveQuantity();
        iPQuantity = iAQuantity;
        break;
    case Geometry::GT_TRIMESH:
        iAQuantity = iNumVertices;
        iPQuantity = iIQuantity/3;
        break;
    default:
        assert( false );
        iAQuantity = 0;
        iPQuantity = 0;
        break;
    }
    ms_hResult = m_pqDevice->DrawIndexedPrimitive(
        ms_aeObjectType[m_pkGeometry->GeometryType],0,0,iAQuantity,0,
        iPQuantity);

    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseArray (CachedColorRGBAArray* pkArray)
{
    // no caching of arrays for DirectX
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseArray (CachedColorRGBArray* pkArray)
{
    // no caching of arrays for DirectX
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseArray (CachedIntArray* pkArray)
{
    // no caching of arrays for DirectX
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseArray (CachedVector2fArray* pkArray)
{
    // no caching of arrays for DirectX
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseArray (CachedVector3fArray* pkArray)
{
    // no caching of arrays for DirectX
}
//----------------------------------------------------------------------------
void DxRenderer::EnableShader (VertexShader* pkVShader)
{
    IDirect3DVertexShader9* pkDxShader;
    pkVShader->BIArray.GetID(this,sizeof(IDirect3DVertexShader9*),
        &pkDxShader);

    if ( !pkDxShader )
    {
        // compiled program
        LPD3DXBUFFER pkCompiledShader;
        LPD3DXBUFFER pkErrors;

        ms_hResult = D3DXAssembleShader(pkVShader->GetProgram(),
            (UINT)strlen(pkVShader->GetProgram()),0,0,0,&pkCompiledShader,
            &pkErrors);
        assert( SUCCEEDED(ms_hResult) );
        assert( pkCompiledShader );

        ms_hResult = m_pqDevice->CreateVertexShader( 
            (DWORD*)(pkCompiledShader->GetBufferPointer()),&pkDxShader);
        pkVShader->BIArray.Bind(this,sizeof(IDirect3DVertexShader9*),
            &pkDxShader);
        assert( SUCCEEDED(ms_hResult) );

        // release buffers (if needed)
        if ( pkCompiledShader )
            pkCompiledShader->Release();
        if ( pkErrors )
            pkErrors->Release();
    }

    // enable this shader
    assert( pkDxShader );
    ms_hResult = m_pqDevice->SetVertexShader(pkDxShader);
    assert( SUCCEEDED(ms_hResult) );

    // set constants
    int iCQuantity = pkVShader->GetConstantQuantity();
    for (int i = 0; i < iCQuantity; i++)
    {
        // get a constant
        ShaderConstantPtr spkConstant = pkVShader->GetConstant(i);
        int iSCType = spkConstant->GetSCType();

        // constant is based on render state
        int iOption = spkConstant->GetSCOption();
        float* afData = spkConstant->Data();
        if ( iSCType != ShaderConstant::SC_USER_DEFINED
        &&   iSCType != ShaderConstant::SC_NUMERICAL_CONSTANT )
        {
            (this->*ms_aoSCFunction[iSCType])(iOption,afData);
        }

        // constant is user-defined
        int iRQuantity = spkConstant->GetRegisterQuantity();
        int iRegister = spkConstant->GetRegister();
        for (int j = 0; j < iRQuantity; j++)
        {
            ms_hResult = m_pqDevice->SetVertexShaderConstantF(iRegister,
                afData,iRQuantity);
            assert( SUCCEEDED( ms_hResult ) );
        }
    }
}
//----------------------------------------------------------------------------
void DxRenderer::EnableShader (PixelShader* pkPShader)
{
    assert(pkPShader);
    IDirect3DPixelShader9* pkDxShader;
    pkPShader->BIArray.GetID(this,sizeof(IDirect3DPixelShader9*),&pkDxShader);

    if ( !pkDxShader )
    {
        // compiled program
        LPD3DXBUFFER pkCompiledShader;
        LPD3DXBUFFER pkErrors;

        ms_hResult = D3DXAssembleShader(pkPShader->GetProgram(),
            (UINT)strlen(pkPShader->GetProgram()),0,0,0,&pkCompiledShader,
            &pkErrors);
        assert( SUCCEEDED(ms_hResult) );
        assert( pkCompiledShader );

        ms_hResult = m_pqDevice->CreatePixelShader( 
            (DWORD*)(pkCompiledShader->GetBufferPointer()),&pkDxShader);
        pkPShader->BIArray.Bind(this,sizeof(IDirect3DPixelShader9*),
            &pkDxShader);
        assert( SUCCEEDED(ms_hResult) );

        // release buffers (if needed)
        if ( pkCompiledShader )
            pkCompiledShader->Release();
        if ( pkErrors )
            pkErrors->Release();
    }

    // enable this shader
    assert( pkDxShader );
    ms_hResult = m_pqDevice->SetPixelShader(pkDxShader);
    assert( SUCCEEDED(ms_hResult) );

    // set constants
    int iCQuantity = pkPShader->GetConstantQuantity();
    for (int i = 0; i < iCQuantity; i++)
    {
        // get a constant
        ShaderConstantPtr spkConstant = pkPShader->GetConstant(i);
        int iSCType = spkConstant->GetSCType();

        // constant is based on render state
        int iOption = spkConstant->GetSCOption();
        float* afData = spkConstant->Data();
        if ( iSCType != ShaderConstant::SC_USER_DEFINED
        &&   iSCType != ShaderConstant::SC_NUMERICAL_CONSTANT )
        {
            (this->*ms_aoSCFunction[iSCType])(iOption,afData);
        }

        // constant is user-defined
        int iRQuantity = spkConstant->GetRegisterQuantity();
        int iRegister = spkConstant->GetRegister();
        for (int j = 0; j < iRQuantity; j++)
        {
            ms_hResult = m_pqDevice->SetPixelShaderConstantF(iRegister,
                afData,iRQuantity);
            assert( SUCCEEDED( ms_hResult ) );
        }
    }
}
//----------------------------------------------------------------------------
void DxRenderer::DisableShader (VertexShader*)
{
    ms_hResult = m_pqDevice->SetVertexShader(0);
    assert( SUCCEEDED( ms_hResult ) );
}
//----------------------------------------------------------------------------
void DxRenderer::DisableShader (PixelShader*)
{
    ms_hResult = m_pqDevice->SetPixelShader(0);
    assert( SUCCEEDED( ms_hResult ) );
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseShader (Shader* pkShader)
{
    assert( pkShader );
    if (!pkShader)
        return;

    if ( pkShader->GetShaderType() == Shader::ST_VERTEX_SHADER )
    {
        IDirect3DVertexShader9* pkDxShader;
        pkShader->BIArray.GetID(this,sizeof(IDirect3DVertexShader9*),
            &pkDxShader);
        assert(pkDxShader);
        pkShader->BIArray.Unbind(this);
        ms_hResult = pkDxShader->Release();
        assert( SUCCEEDED(ms_hResult) );
    }
    else if ( pkShader->GetShaderType() == Shader::ST_PIXEL_SHADER )
    {
        IDirect3DPixelShader9* pkDxShader;
        pkShader->BIArray.GetID(this,sizeof(IDirect3DPixelShader9*),
            &pkDxShader);
        assert(pkDxShader);
        pkShader->BIArray.Unbind(this);
        ms_hResult = pkDxShader->Release();
        assert( SUCCEEDED(ms_hResult) );
    }
}
//----------------------------------------------------------------------------
void DxRenderer::SetConstantTransformM (int iOption, float* afData)
{
    Matrix4f kM(m_afWorldMatrix,false);
    GetTransform(kM,iOption,afData);
}
//----------------------------------------------------------------------------
void DxRenderer::SetConstantTransformP (int iOption, float* afData)
{
    Matrix4f kP((float*)(&m_kProjMatrix),false);
    GetTransform(kP,iOption,afData);
}
//----------------------------------------------------------------------------
void DxRenderer::SetConstantTransformMV (int iOption, float* afData)
{
    Matrix4f kM(m_afWorldMatrix,false);
    Matrix4f kV((float*)(&m_kViewMatrix),false);
    Matrix4f kMV = kV*kM;
    GetTransform(kMV,iOption,afData);
}
//----------------------------------------------------------------------------
void DxRenderer::SetConstantTransformMVP (int iOption, float* afData)
{
    Matrix4f kM(m_afWorldMatrix,false);
    Matrix4f kV((float*)(&m_kViewMatrix),false);
    Matrix4f kP((float*)(&m_kProjMatrix),false);
    Matrix4f kPMV = kP*kV*kM;
    GetTransform(kPMV,iOption,afData);
}
//----------------------------------------------------------------------------
void DxRenderer::GetTransform (Matrix4f& rkMat, int iOption, float* afData)
{
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
void DxRenderer::Draw (const unsigned char* aucBuffer)
{
    if ( !aucBuffer )
        return;

    IDirect3DSurface9* pkBackBuffer = 0;
    ms_hResult = m_pqDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,
        &pkBackBuffer);
    assert( pkBackBuffer );
    assert( SUCCEEDED(ms_hResult) );
    if ( FAILED(ms_hResult) || !pkBackBuffer )
        return;

    RECT kSrcRect = { 0, 0, m_iWidth-1, m_iHeight-1 };
    ms_hResult = D3DXLoadSurfaceFromMemory(pkBackBuffer,0,0,aucBuffer,
        D3DFMT_R8G8B8,3*m_iWidth,0,&kSrcRect,D3DX_FILTER_NONE,0);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::EstablishCapabilities ()
{
    // Query the device for its capabilities.
    D3DCAPS9 kDeviceCaps;
    ms_hResult = m_pqMain->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
        &kDeviceCaps);
    assert( SUCCEEDED( ms_hResult ) );
    if ( FAILED( ms_hResult ) )
        return;

    // Initialize the maximum number of texture units.  Use the maximum number
    // of textures that can be placed into the texture units simultaneously.
    // This number is more relevant to how WM uses it than is the maximum
    // number of texture units.
    m_iMaxTextures = kDeviceCaps.MaxSimultaneousTextures;

    // The fact that at least one stencil bit is available is given by the
    // fact that we got this far which means we have at least 8 bit available
    // as requested in the CreateDevice call.  Make note that all stencil
    // operations used are supported.
    if ( kDeviceCaps.StencilCaps & (D3DSTENCILOP_KEEP|D3DSTENCILOP_REPLACE) )
        m_iMaxStencilIndices = 8;

    m_iMaxLights = (int)kDeviceCaps.MaxActiveLights;
}
//----------------------------------------------------------------------------
