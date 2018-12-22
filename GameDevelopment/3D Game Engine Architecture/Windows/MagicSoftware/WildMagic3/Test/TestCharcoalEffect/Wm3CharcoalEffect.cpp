// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3CharcoalEffect.h"
#include "Wm3CharcoalVShader.h"
#include "Wm3CharcoalPShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,CharcoalEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(CharcoalEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(CharcoalEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(CharcoalEffect,ShaderEffect);

//----------------------------------------------------------------------------
CharcoalEffect::CharcoalEffect (Image* pkContrastTexture,
    Image* pkRandomTexture, Image* pkPaperTexture)
{
    VShader = new CharcoalVShader;
    PShader = new CharcoalPShader;

    Texture* pkContrastTextureTex = new Texture;
    pkContrastTextureTex->SetImage(pkContrastTexture);
    pkContrastTextureTex->Mipmap = Texture::MM_NEAREST;
    pkContrastTextureTex->Apply = Texture::AM_DECAL;
    Textures.Append(pkContrastTextureTex);

    Texture* pkRandomTextureTex = new Texture;
    pkRandomTextureTex->SetImage(pkRandomTexture);
    pkRandomTextureTex->Mipmap = Texture::MM_NEAREST;
    pkRandomTextureTex->Apply = Texture::AM_DECAL;
    Textures.Append(pkRandomTextureTex);

    Texture* pkPaperTextureTex = new Texture;
    pkPaperTextureTex->SetImage(pkPaperTexture);
    pkPaperTextureTex->Mipmap = Texture::MM_NEAREST;
    pkPaperTextureTex->Apply = Texture::AM_DECAL;
    Textures.Append(pkPaperTextureTex);
}
//----------------------------------------------------------------------------
CharcoalEffect::CharcoalEffect ()
{
}
//----------------------------------------------------------------------------
CharcoalEffect::~CharcoalEffect ()
{
}
//----------------------------------------------------------------------------
Effect* CharcoalEffect::Clone ()
{
    CharcoalEffect* pkClone = new CharcoalEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void CharcoalEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
void CharcoalEffect::SetContrastExponent (float fContrastExponent)
{
    StaticCast<CharcoalVShader>(VShader)->SetContrastExponent(
        fContrastExponent);

    GenerateContrastImage();
}
//----------------------------------------------------------------------------
void CharcoalEffect::GenerateContrastImage ()
{
    Image* pkImage = Textures[0]->GetImage();
    assert( pkImage && pkImage->GetFormat() == Image::IT_RGBA8888 );
    if ( !pkImage || pkImage->GetFormat() != Image::IT_RGBA8888 )
        return;

    // create a white image
    int iWidth = pkImage->GetWidth();
    int iHeight = pkImage->GetHeight();
    int iQuantity = iWidth*iHeight;
    int iNumBytes = 4*iQuantity;
    unsigned char* aucData = pkImage->GetData();
    memset(aucData,-1,iNumBytes*sizeof(unsigned char));

    // add black speckles to the image randomly
    float fNoiseDensity = 2.0f;
    float fContrastExponent = GetContrastExponent();
    int iNumNoiseTexels = (int)(fNoiseDensity*iQuantity);
    for (int i = 0 ; i < iNumNoiseTexels ; i++)
    {
        // generate x and y texture coordinates randomly between 0 and 1
        float fX = Mathf::UnitRandom();
        float fY = Mathf::UnitRandom();
        fY = Mathf::Pow(fY,fContrastExponent);
        int iX = (int)(fX*(iWidth - 1.0));
        int iY = (int)(fY*(iHeight - 1.0));
        unsigned char* aucColor = &aucData[4*(iX+iWidth*iY)];
        aucColor[0] = 0;
        aucColor[1] = 0;
        aucColor[2] = 0;
    }
}
//----------------------------------------------------------------------------
void CharcoalEffect::SetContrastTextureCoordinates (int iVQuantity)
{
    Vector2f* afUV = new Vector2f[iVQuantity];
    for (int i = 0; i < iVQuantity; i++ )
    {
        afUV[i].X() = Mathf::UnitRandom();
        afUV[i].Y() = Mathf::UnitRandom();
    }
    Vector2fArray* pkUVs = new Vector2fArray(iVQuantity,afUV);

    // the paper texture is assigned to unit 2
    UVs.SetElement(0,pkUVs);
}
//----------------------------------------------------------------------------
