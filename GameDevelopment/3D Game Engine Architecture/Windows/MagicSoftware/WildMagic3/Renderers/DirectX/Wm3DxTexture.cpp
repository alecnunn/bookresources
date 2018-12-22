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
#include "Wm3Camera.h"
#include "Wm3ProjectedTextureEffect.h"
using namespace Wm3;

int DxRenderer::ms_aiTexSize[Image::IT_QUANTITY] =
{
    4,    // Image::IT_RGBA4444
    3,    // Image::IT_RGB888
    4,    // Image::IT_RGBA5551
    4,    // Image::IT_RGBA8888
};

DWORD DxRenderer::ms_adwTexFormat[Image::IT_QUANTITY] =
{
    D3DFMT_A4R4G4B4,    // Image::IT_RGBA4444
    D3DFMT_R8G8B8,      // Image::IT_RGB888
    D3DFMT_A1R5G5B5,    // Image::IT_RGBA5551
    D3DFMT_A8R8G8B8,    // Image::IT_RGBA8888
};

DWORD DxRenderer::ms_adwTexMagFilter[Texture::FM_QUANTITY] =
{
    D3DTEXF_POINT,      // Texture::FM_NEAREST
    D3DTEXF_LINEAR,     // Texture::FM_LINEAR
};

DWORD DxRenderer::ms_adwTexMinFilter[Texture::MM_QUANTITY] =
{
    D3DTEXF_POINT,  // Texture::MM_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_LINEAR
    D3DTEXF_POINT,  // Texture::MM_NEAREST_NEAREST
    D3DTEXF_POINT,  // Texture::MM_NEAREST_LINEAR
    D3DTEXF_LINEAR, // Texture::MM_LINEAR_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_LINEAR_LINEAR
};

DWORD DxRenderer::ms_adwTexMipFilter[Texture::MM_QUANTITY] =
{
    D3DTEXF_NONE,   // Texture::MM_NEAREST
    D3DTEXF_NONE,   // Texture::MM_LINEAR
    D3DTEXF_POINT,  // Texture::MM_NEAREST_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_NEAREST_LINEAR
    D3DTEXF_POINT,  // Texture::MM_LINEAR_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_LINEAR_LINEAR
};

DWORD DxRenderer::ms_adwTexWrapMode[Texture::WM_QUANTITY] =
{
    D3DTADDRESS_CLAMP,      // Texture::WM_CLAMP
    D3DTADDRESS_WRAP,       // Texture::WM_WRAP
    D3DTADDRESS_BORDER,     // Texture::WM_CLAMP_BORDER
    D3DTADDRESS_CLAMP,      // Texture::WM_CLAMP_EDGE
};

DWORD DxRenderer::ms_adwTexCombineSource[Texture::ACS_QUANTITY] =
{
    D3DTA_TEXTURE,  // Texture::ACS_TEXTURE
    D3DTA_DIFFUSE,  // Texture::ACS_PRIMARY_COLOR
    D3DTA_TFACTOR,  // Texture::ACS_CONSTANT
    D3DTA_CURRENT,  // Texture::ACS_PREVIOUS
};

DWORD DxRenderer::ms_adwTexCombineOperand[Texture::ACO_QUANTITY] =
{
    0,                                    // Texture::ACO_SRC_COLOR
    D3DTA_COMPLEMENT,                     // Texture::ACO_ONE_MINUS_SRC_COLOR
    D3DTA_ALPHAREPLICATE,                 // Texture::ACO_SRC_ALPHA
    D3DTA_COMPLEMENT |
        D3DTA_ALPHAREPLICATE              // Texture::ACO_ONE_MINUS_SRC_ALPHA
};

const ApplyParameters DxRenderer::ms_kApPass(D3DTA_CURRENT);

const ApplyParameters DxRenderer::ms_kApColorReplace(D3DTA_TEXTURE);
const ApplyParameters DxRenderer::ms_kApColorDecal(D3DTOP_BLENDTEXTUREALPHA,
    D3DTA_TEXTURE,D3DTA_CURRENT);
const ApplyParameters DxRenderer::ms_kApColorModulate(D3DTOP_MODULATE,
    D3DTA_TEXTURE,D3DTA_CURRENT);
const ApplyParameters DxRenderer::ms_kApColorBlend(D3DTOP_LERP,D3DTA_TEXTURE,
    D3DTA_TFACTOR,D3DTA_CURRENT);
const ApplyParameters DxRenderer::ms_kApColorAdd(D3DTOP_ADD,D3DTA_TEXTURE,
    D3DTA_CURRENT);

const ApplyParameters DxRenderer::ms_kApAlphaReplace(D3DTA_TEXTURE);
const ApplyParameters DxRenderer::ms_kApAlphaDecal(D3DTA_CURRENT);
const ApplyParameters DxRenderer::ms_kApAlphaModulate(D3DTOP_MODULATE,
    D3DTA_TEXTURE,D3DTA_CURRENT);
const ApplyParameters DxRenderer::ms_kApAlphaBlend(D3DTOP_MODULATE,
    D3DTA_TEXTURE,D3DTA_CURRENT);
const ApplyParameters DxRenderer::ms_kApAlphaAdd(D3DTOP_ADD,D3DTA_TEXTURE,
    D3DTA_CURRENT);

//----------------------------------------------------------------------------
void DxRenderer::SetTextureStageApplyPass (const int iUnit)
{
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLOROP,
        D3DTOP_SELECTARG1);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_ALPHAOP,
        D3DTOP_SELECTARG1);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLORARG1,
        D3DTA_CURRENT);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_ALPHAARG1,
        D3DTA_CURRENT);
    assert( SUCCEEDED(ms_hResult) );
}
//----------------------------------------------------------------------------
void DxRenderer::EnableTexture (int iUnit, int i, Effect* pkEffect)
{
    // Set the texture for this stage.  Create the texture in the device if
    // not already done so.
    ITexture* pqTexture;
    Texture* pkTexture = pkEffect->Textures[i];
    Image* pkTextureImage = pkTexture->GetImage();
    if ( pkTextureImage == 0 )
    {
        assert( false );
        return;
    }

    pkTexture->BIArray.GetID(this,sizeof(ITexture*),&pqTexture);
    if ( pqTexture == 0 )
    {
        pqTexture = CreateTexture(pkTextureImage);
        if ( pqTexture == 0 )
        {
            assert( false );
            return;
        }
        pkTexture->BIArray.Bind(this,sizeof(ITexture*),&pqTexture);
    }
    ms_hResult = m_pqDevice->SetTexture(iUnit,pqTexture);
    assert( SUCCEEDED(ms_hResult) );

    // get the right texture coordinates to use
    Vector2fArray* pkUVs = 0;
    if ( i < pkEffect->UVs.GetQuantity() )
    {
        pkUVs = pkEffect->UVs[i];
    }

    SetActiveTextureUnit(iUnit);

    if ( pkUVs )
        EnableUVs(pkUVs);
    m_abTexUnitEnabled[iUnit] = true;

    // set the texture priority
    DWORD dwPriority = 0xFFFFFFFF;
    pqTexture->SetPriority(dwPriority);

    // Setup the texture apply (combine) mode.  Legend for the texture combine
    // mode definitions.  Legend symbols mainly taken from the OpenGL
    // "red book".
    //  C  - result color (RGB)
    //  A  - result alpha
    //  Ct - texture color
    //  At - texture alpha
    //  Cf - fragment (current) color
    //  Af - fragment (current) alpha
    //  Cc - constant color
    //  Ac - constant alpha
    //  C1 - color argument 1
    //  A1 - alpha argument 1
    //  C2 - alpha argument 2
    //  A2 - alpha argument 2

    // default is to pass the texture input to the output
    ApplyParameters kApColor, kApAlpha;

    switch ( pkTexture->Apply )
    {
    case Texture::AM_REPLACE:
    {
        // C=Ct, Ct=C1, A=At, At=A1
        kApColor = ms_kApColorReplace;
        kApAlpha = ms_kApAlphaReplace;
        break;
    }
    case Texture::AM_DECAL:
    {
        // C=At*Ct+(1-At)*Cf, Ct=C1, Cf=C2, A=Af, Af=A1
        kApColor = ms_kApColorDecal;
        kApAlpha = ms_kApAlphaDecal;
        break;
    }
    case Texture::AM_MODULATE:
    {
        // C=Ct*Cf, Ct=C1, Cf=C2, A=At*Af, At=A1, Af=A2
        kApColor = ms_kApColorModulate;
        kApAlpha = ms_kApAlphaModulate;
        break;
    }
    case Texture::AM_BLEND:
    {
        // C=Ct*Cc+(1-Ct)*Cf, Ct=C1, Cc=C2, Cf=C3, A=At*Af, At=A1, Af=A2
        kApColor = ms_kApColorBlend;
        kApAlpha = ms_kApAlphaBlend;
        break;
    }
    case Texture::AM_ADD:
    {
        // C=Ct+Cf, Ct=C1, Cf=C2, A=At+Af, At=C1, Af=A2
        kApColor = ms_kApColorAdd;
        kApAlpha = ms_kApAlphaAdd;
        break;
    }
    case Texture::AM_COMBINE:
    {
        // retrieve the color arguments and the color combine parameters
        DWORD dwColorArg0 =
            ms_adwTexCombineSource[pkTexture->CombineSrc0RGB] |
            ms_adwTexCombineOperand[pkTexture->CombineOp0RGB];
        DWORD dwColorArg1 =
            ms_adwTexCombineSource[pkTexture->CombineSrc1RGB] |
            ms_adwTexCombineOperand[pkTexture->CombineOp1RGB];
        DWORD dwColorArg2 =
            ms_adwTexCombineSource[pkTexture->CombineSrc2RGB] |
            ms_adwTexCombineOperand[pkTexture->CombineOp2RGB];
        bool bValidCombineColor = kApColor.ConvertWmToDx(
            pkTexture->CombineFuncRGB,pkTexture->CombineScaleRGB,
            dwColorArg0,dwColorArg1,dwColorArg2);

        // retrieve the alpha arguments
        DWORD dwAlphaArg0 =
            ms_adwTexCombineSource[pkTexture->CombineSrc0Alpha] |
            ms_adwTexCombineOperand[pkTexture->CombineOp0Alpha];
        DWORD dwAlphaArg1 =
            ms_adwTexCombineSource[pkTexture->CombineSrc1Alpha] |
            ms_adwTexCombineOperand[pkTexture->CombineOp1Alpha];
        DWORD dwAlphaArg2 =
            ms_adwTexCombineSource[pkTexture->CombineSrc2Alpha] |
            ms_adwTexCombineOperand[pkTexture->CombineOp2Alpha];
        bool bValidCombineAlpha = kApAlpha.ConvertWmToDx(
            pkTexture->CombineFuncAlpha,pkTexture->CombineScaleAlpha,
            dwAlphaArg0,dwAlphaArg1,dwAlphaArg2);

        // If either the color or alpha texture combine function is not
        // supported, then just setup the texture stage to pass the input to
        // the output.
        if ( bValidCombineColor && !bValidCombineAlpha )
        {
            kApColor = ms_kApPass;
        }
        else if ( !bValidCombineColor && bValidCombineAlpha )
        {
            kApAlpha = ms_kApPass;
        }
        break;
    }
    default:
        assert( false );
        kApColor = ms_kApPass;
        kApAlpha = ms_kApPass;
        break;
    }


    // WM supports 3- and 4-component textures.  The texture color combine
    // operation is the same regardless of whether the texture used in the
    // operation has 3- or 4-components.  However, the texture alpha combine
    // operation does change in the case of a 3-component texture.  In that
    // case, the alpha from the previous stage is passed as the output of this
    // stage.
    if ( ms_aiTexSize[pkTextureImage->GetFormat()] == 3 )
        kApAlpha = ms_kApPass;


    // Set the texture color combine parameters.
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLOROP,
        kApColor.Op);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLORARG1,
        kApColor.Arg1);
    assert( SUCCEEDED(ms_hResult) );
    if ( kApColor.Arguments > 1 )
    {
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_COLORARG2,kApColor.Arg2);
        assert( SUCCEEDED(ms_hResult) );
    }
    if ( kApColor.Arguments > 2 )
    {
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_COLORARG0,kApColor.Arg0);
        assert( SUCCEEDED(ms_hResult) );
    }

    // set the texture alpha combine parameters
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_ALPHAOP,
        kApAlpha.Op);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_ALPHAARG1,
        kApAlpha.Arg1);
    assert( SUCCEEDED(ms_hResult) );
    if ( kApAlpha.Arguments > 1 )
    {
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_ALPHAARG2,kApAlpha.Arg2);
        assert( SUCCEEDED(ms_hResult) );
    }
    if ( kApAlpha.Arguments > 2 )
    {
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_ALPHAARG0,kApAlpha.Arg0);
        assert( SUCCEEDED(ms_hResult) );
    }


    // The AF_BLEND texture apply mode uses a constant color for the texture
    // blend operation.  This same constant color may be used by the
    // AF_COMBINE texture apply mode depending on the combine operation.
    //
    // NOTE: If any other texture stage also has the AM_BLEND mode or the
    // AM_COMBINE mode with ACS_CONSTANT as one of its operands, then there
    // is a resource conflict as there is only a single constant texture color
    // for use with all texture units.
    const ColorRGBA& rkBlendColor = pkTexture->BlendColor;
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_TEXTUREFACTOR,
        D3DCOLOR_COLORVALUE(rkBlendColor.R(),rkBlendColor.G(),
        rkBlendColor.B(),rkBlendColor.A()));
    assert( SUCCEEDED(ms_hResult) );


    // set up texture magnification, minification, and mipmap filters
    ms_hResult = m_pqDevice->SetSamplerState(iUnit,D3DSAMP_MAGFILTER,
        ms_adwTexMagFilter[pkTexture->Filter]);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->SetSamplerState(iUnit,D3DSAMP_MINFILTER,
        ms_adwTexMinFilter[pkTexture->Mipmap]);
    assert( SUCCEEDED(ms_hResult) );

    ms_hResult = m_pqDevice->SetSamplerState(iUnit,D3DSAMP_MIPFILTER,
        ms_adwTexMipFilter[pkTexture->Mipmap]);
    assert( SUCCEEDED(ms_hResult) );

    // check for special setup if border texture addressing mode is used
    int eWrapModeU = pkTexture->CoordU;
    int eWrapModeV = pkTexture->CoordV;

    if ( Texture::WM_CLAMP_BORDER == eWrapModeV || 
        Texture::WM_CLAMP_BORDER == eWrapModeU )
    {
        // set up border color
        const ColorRGBA& rkBorderColor = pkTexture->BorderColor;
        ms_hResult = m_pqDevice->SetSamplerState(iUnit,
            D3DSAMP_BORDERCOLOR,
            D3DCOLOR_COLORVALUE(rkBorderColor.R(),rkBorderColor.G(),
            rkBorderColor.B(),rkBorderColor.A()));
        assert( SUCCEEDED(ms_hResult) );
    }

    // set up texture addressing mode
    ms_hResult = m_pqDevice->SetSamplerState(iUnit,D3DSAMP_ADDRESSU,
        ms_adwTexWrapMode[eWrapModeU]);
    assert( SUCCEEDED(ms_hResult) );
    ms_hResult = m_pqDevice->SetSamplerState(iUnit,D3DSAMP_ADDRESSV,
        ms_adwTexWrapMode[eWrapModeV]);
    assert( SUCCEEDED(ms_hResult) );

    // set up texture coordinate generation
    switch ( pkTexture->Texgen )
    {
    case Texture::TG_NONE:
        // set pass through for texture coordinates
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU+iUnit);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
        assert( SUCCEEDED(ms_hResult) );
        break;
    case Texture::TG_ENVIRONMENT_MAP:
    {
        // Use the vertex normal to compute the following texture coordinate
        // transformation:
        //  u = 0.5 + Nx / 2
        //  v = 0.5 + Ny / 2

        // Bias and scale the texture so that it covers the near plane.  Note
        // that integral screen coordinates represent pixel centers whereas
        // integral texture coordinates represent texel boundaries.  We also
        // need to know the dimensions of the texture being projected.
        float fOffsetX = 0.5f + (0.5f / pkTextureImage->GetWidth());
        float fOffsetY = 0.5f + (0.5f / pkTextureImage->GetHeight());
        D3DXMATRIX kMatrixTexture(
            0.5f,0.0f,0.0f,0.0f,
            0.0f,0.5f,0.0f,0.0f,
            0.0f,0.0f,1.0f,0.0f,
            fOffsetX,fOffsetY,0.0f,1.0f);
        ms_hResult = m_pqDevice->SetTransform(
            D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+iUnit),
            &kMatrixTexture);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACENORMAL+iUnit);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
        assert( SUCCEEDED(ms_hResult) );
        break;
    }
    case Texture::TG_PROJECTED_TEXTURE:
    {
        float fNear, fFar, fLeft, fRight, fTop, fBottom;
        D3DXMATRIX kMatrixViewCamera;
        D3DXMATRIX kMatrixInverseViewCamera;
        D3DXMATRIX kMatrixViewProjector;
        D3DXMATRIX kMatrixProjectionProjector;
        D3DXMATRIX kMatTmp1, kMatTmp2;
        D3DXMATRIX kMatrixTextureProjector;

        Camera* pkCamera = ((ProjectedTextureEffect*)pkEffect)->GetCamera();

        // Set up the inverse for the current camera viewing transform.  The
        // texture coordinates will be obtained from camera space position,
        // but that camera viewing transform needs to be undone to get back to
        // world coordinates.
        ms_hResult = m_pqDevice->GetTransform(D3DTS_VIEW,&kMatrixViewCamera); 
        assert( SUCCEEDED(ms_hResult) );
        D3DXMatrixInverse(&kMatrixInverseViewCamera,0,&kMatrixViewCamera);
        

        // Set up viewing transform for the projector.  Set up a lookat matrix
        // using the projected texture camera viewpoint and view direction.
        const Vector3f& rkCameraLoc = pkCamera->GetLocation();
        const Vector3f& rkCameraDir = pkCamera->GetDVector();
        const Vector3f& rkCameraUp = pkCamera->GetUVector();
        const Vector3f& rkCameraLookAt = rkCameraLoc + rkCameraDir;
        D3DXVECTOR3 kProjectorEye(rkCameraLoc.X(),rkCameraLoc.Y(),
            rkCameraLoc.Z());
        D3DXVECTOR3 kProjectorUp(rkCameraUp.X(),rkCameraUp.Y(),
            rkCameraUp.Z());
        D3DXVECTOR3 kProjectorLookAt(rkCameraLookAt.X(),rkCameraLookAt.Y(),
            rkCameraLookAt.Z());
        D3DXMatrixLookAtRH(&kMatrixViewProjector,&kProjectorEye,
            &kProjectorLookAt,&kProjectorUp);

        // Set up the projection transform for the projector,
        // ProjectionProjector.  Set up a right-handed off-center perspective
        // projection matrix using the projected texture camera frustrum.
        pkCamera->GetFrustum(fNear,fFar,fLeft,fRight,fTop,fBottom);
        D3DXMatrixPerspectiveOffCenterRH(&kMatrixProjectionProjector,fLeft,
            fRight,fBottom,fTop,fNear,fFar);

        // Bias and scale the texture so that it covers the near plane.  Note
        // that integral screen coordinates represent pixel centers, whereas
        // integral texture coordinates represent texel boundaries.  We also
        // need to know the dimensions of the texture being projected.
        Image* pkTextureImage = pkTexture->GetImage();
        float fOffsetX = 0.5f + (0.5f / pkTextureImage->GetWidth());
        float fOffsetY = 0.5f + (0.5f / pkTextureImage->GetHeight());
        D3DXMATRIX kMatrixScaleBiasTexture(
            0.5f,0.0f,0.0f,0.0f,
            0.0f,0.5f,0.0f,0.0f,
            0.0f,0.0f,1.0f,0.0f,
            fOffsetX,fOffsetY,0.0f,1.0f);

        // Create the projected texture transform by combining the following 
        // transformations in the given order (left to right):
        //   kMatrixInverseViewCamera
        //   kMatrixViewProjector
        //   kMatrixProjectionProjector
        //   kMatrixScaleBiasTexture
        D3DXMatrixMultiply(&kMatrixTextureProjector,
            D3DXMatrixMultiply(&kMatTmp1,&kMatrixInverseViewCamera,
                &kMatrixViewProjector),
            D3DXMatrixMultiply(&kMatTmp2,&kMatrixProjectionProjector,
                &kMatrixScaleBiasTexture));
        ms_hResult = m_pqDevice->SetTransform(
            D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+iUnit),
            &kMatrixTextureProjector);
        assert( SUCCEEDED(ms_hResult) );


        // Use the texture coordinate set defined for this stage, but the
        // texture coordinates for this stage are based on the transformed
        // vertex position.
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION|iUnit);
        assert( SUCCEEDED(ms_hResult) );


        // All four dimenions of the texture transform are processed.  The
        // coordinates are then dealt with as a projected texture.
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,
            D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);
        assert( SUCCEEDED(ms_hResult) );
        break;
    }
    default:
        assert( false );
        break;
    }
}
//----------------------------------------------------------------------------
ITexture* DxRenderer::CreateTexture (Image* pkImage)
{
    // 16-bit textures are not supported.
    switch ( pkImage->GetFormat() )
    {
    default:
    case Image::IT_RGBA4444:
    case Image::IT_RGBA5551:
        assert( false );
        return 0;
        break;
    case Image::IT_RGB888:
    case Image::IT_RGBA8888:
        break;
    }

    // Get parameters which define image header.
    int iSize = ms_aiTexSize[pkImage->GetFormat()];
    int iWidth = pkImage->GetWidth();
    int iHeight = pkImage->GetHeight();
    int iQuantity = iWidth*iHeight;
    int iDataBytes = iSize*iQuantity;
    int iFileBytes = iDataBytes + sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPV4HEADER);

    // reverse byte order (Windows stores BGR (lowest byte to highest byte,
    // MIF stores RGB)
    unsigned char* aucData = pkImage->GetData();
    unsigned char* aucFileInMemory = new unsigned char[iFileBytes];
    unsigned char* aucRData = aucFileInMemory + sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPV4HEADER);
    unsigned char* pucTmp = aucData;

    // window bmps are stored as top-down
    int i, iPixel;
    if ( iSize == 3 )
    {
        for (i = 0; i < iQuantity; i++)
        {
            iPixel = 3*i;
            aucRData[iPixel + 0] = aucData[iPixel + 2];
            aucRData[iPixel + 1] = aucData[iPixel + 1];
            aucRData[iPixel + 2] = aucData[iPixel + 0];
        }
    }
    else
    {
        for (i = 0; i < iQuantity; i++)
        {
            iPixel = 4*i;
            aucRData[iPixel + 0] = aucData[iPixel + 2];
            aucRData[iPixel + 1] = aucData[iPixel + 1];
            aucRData[iPixel + 2] = aucData[iPixel + 0];
            aucRData[iPixel + 3] = aucData[iPixel + 3];
        }
    }

    BITMAPFILEHEADER* pkFileHeader = (BITMAPFILEHEADER*)aucFileInMemory;
    pkFileHeader->bfType = 0x4d42;  // "BM"
    pkFileHeader->bfSize = iDataBytes;
    pkFileHeader->bfReserved1 = 0;
    pkFileHeader->bfReserved2 = 0;
    pkFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPV4HEADER);

    BITMAPV4HEADER* pkV4Header = (BITMAPV4HEADER*)(aucFileInMemory +
        sizeof(BITMAPFILEHEADER));
    pkV4Header->bV4Size = sizeof(BITMAPV4HEADER);
    pkV4Header->bV4Width = iWidth;
    pkV4Header->bV4Height = -iHeight;
    pkV4Header->bV4Planes = 1;
    pkV4Header->bV4BitCount = ( iSize == 3 ? 24 : 32 );
    pkV4Header->bV4V4Compression = ( iSize == 3 ? BI_RGB : BI_BITFIELDS );
    pkV4Header->bV4SizeImage = ( iSize == 3 ? 0 : iDataBytes );
    pkV4Header->bV4XPelsPerMeter = 0;
    pkV4Header->bV4YPelsPerMeter = 0;
    pkV4Header->bV4ClrUsed = 0;
    pkV4Header->bV4ClrImportant = 0;
    pkV4Header->bV4RedMask   = 0x00FF0000;
    pkV4Header->bV4GreenMask = 0x0000FF00;
    pkV4Header->bV4BlueMask  = 0x000000FF;
    pkV4Header->bV4AlphaMask = 0xFF000000;
    pkV4Header->bV4CSType = LCS_sRGB;
    pkV4Header->bV4GammaRed = 0;
    pkV4Header->bV4GammaGreen = 0;
    pkV4Header->bV4GammaBlue = 0;

    ITexture* pqTexture; 

    ms_hResult = D3DXCreateTextureFromFileInMemoryEx(
        m_pqDevice, 
        aucFileInMemory, 
        iFileBytes,
        0,                  // take width from file
        0,                  // take height from file
        0,                  // create full mip chain
        0,
        (D3DFORMAT)ms_adwTexFormat[pkImage->GetFormat()],
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,       // D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER 
        D3DX_DEFAULT,       // D3DX_FILTER_BOX
        0x00000000,         // disable opaque colorkey
        0,                  // D3DXIMAGE_INFO
        0,                  // no PALETTEENTRY
        &pqTexture
    );
    assert( SUCCEEDED(ms_hResult) );
    
    delete[] aucFileInMemory;
    return pqTexture; 
}
//----------------------------------------------------------------------------
void DxRenderer::DisableTexture (int iUnit, int i, Effect* pkEffect)
{
    // disable this texture unit
    ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLOROP,
        D3DTOP_DISABLE);
    assert( SUCCEEDED(ms_hResult) );

    // unassociate the texture for this stage
    ms_hResult = m_pqDevice->SetTexture(iUnit,0);
    assert( SUCCEEDED(ms_hResult) );

    m_abTexUnitEnabled[iUnit] = false;

    m_iCurTextureUnit = iUnit;
    DisableUVs(0);
}
//----------------------------------------------------------------------------
void DxRenderer::SetActiveTextureUnit (int iUnit)
{
    m_iCurTextureUnit = iUnit;
}
//----------------------------------------------------------------------------
void DxRenderer::ReleaseTexture (Texture* pkTexture)
{
    assert( pkTexture );
    if ( pkTexture )
    {
        ITexture* pqTexture;
        pkTexture->BIArray.GetID(this,sizeof(ITexture*),&pqTexture);
        if ( pqTexture )
        {
            pqTexture->Release();
            pkTexture->BIArray.Unbind(this);
        }
    }
}
//----------------------------------------------------------------------------
