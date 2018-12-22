// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Texture.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Texture,Object);
WM3_IMPLEMENT_STREAM(Texture);

//----------------------------------------------------------------------------
Texture::Texture (Image* pkImage)
    :
    BlendColor(ColorRGBA(0.0f,0.0f,0.0f,0.0f)),
    BorderColor(ColorRGBA::INVALID),
    m_spkImage(pkImage),
    BIArray(1,1)
{
    Correction = CM_PERSPECTIVE;
    Apply = AM_REPLACE;
    CoordU = WM_CLAMP_EDGE;
    CoordV = WM_CLAMP_EDGE;
    Filter = FM_LINEAR;
    Mipmap = MM_LINEAR_LINEAR;
    Texgen = TG_NONE;
    CombineFuncRGB = ACF_REPLACE;
    CombineFuncAlpha = ACF_REPLACE;
    CombineSrc0RGB = ACS_TEXTURE;
    CombineSrc1RGB = ACS_TEXTURE;
    CombineSrc2RGB = ACS_TEXTURE;
    CombineSrc0Alpha = ACS_TEXTURE;
    CombineSrc1Alpha = ACS_TEXTURE;
    CombineSrc2Alpha = ACS_TEXTURE;
    CombineOp0RGB = ACO_SRC_COLOR;
    CombineOp1RGB = ACO_SRC_COLOR;
    CombineOp2RGB = ACO_SRC_COLOR;
    CombineOp0Alpha = ACO_SRC_COLOR;
    CombineOp1Alpha = ACO_SRC_COLOR;
    CombineOp2Alpha = ACO_SRC_COLOR;
    CombineScaleRGB = ACSC_ONE;
    CombineScaleAlpha = ACSC_ONE;
}
//----------------------------------------------------------------------------
Texture::~Texture ()
{
    // Inform all renderers using this texture that it is being destroyed.
    // This allows the renderer to free up any associated resources.
    const TArray<BindInfo>& rkArray = BIArray.GetArray();
    for (int i = 0; i < rkArray.GetQuantity(); i++)
        rkArray[i].User->ReleaseTexture(this);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Texture::GetObjectByName (const String& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_spkImage )
    {
        pkFound = m_spkImage->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Texture::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkImage )
        m_spkImage->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* Texture::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_spkImage )
    {
        pkFound = m_spkImage->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Texture::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Correction);
    rkStream.Read(Apply);
    rkStream.Read(CoordU);
    rkStream.Read(CoordV);
    rkStream.Read(Filter);
    rkStream.Read(Mipmap);
    rkStream.Read(Texgen);
    rkStream.Read(BlendColor);
    rkStream.Read(BorderColor);
    rkStream.Read(CombineFuncRGB);
    rkStream.Read(CombineFuncAlpha);
    rkStream.Read(CombineSrc0RGB);
    rkStream.Read(CombineSrc1RGB);
    rkStream.Read(CombineSrc2RGB);
    rkStream.Read(CombineSrc0Alpha);
    rkStream.Read(CombineSrc1Alpha);
    rkStream.Read(CombineSrc2Alpha);
    rkStream.Read(CombineOp0RGB);
    rkStream.Read(CombineOp1RGB);
    rkStream.Read(CombineOp2RGB);
    rkStream.Read(CombineOp0Alpha);
    rkStream.Read(CombineOp1Alpha);
    rkStream.Read(CombineOp2Alpha);
    rkStream.Read(CombineScaleRGB);
    rkStream.Read(CombineScaleAlpha);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkImage
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(Texture);
}
//----------------------------------------------------------------------------
void Texture::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkImage = (Image*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Texture::Register (Stream& rkStream) const
{
    if ( !Object::Register(rkStream) )
        return false;

    if ( m_spkImage )
        m_spkImage->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void Texture::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(Correction);
    rkStream.Write(Apply);
    rkStream.Write(CoordU);
    rkStream.Write(CoordV);
    rkStream.Write(Filter);
    rkStream.Write(Mipmap);
    rkStream.Write(Texgen);
    rkStream.Write(BlendColor);
    rkStream.Write(BorderColor);
    rkStream.Write(CombineFuncRGB);
    rkStream.Write(CombineFuncAlpha);
    rkStream.Write(CombineSrc0RGB);
    rkStream.Write(CombineSrc1RGB);
    rkStream.Write(CombineSrc2RGB);
    rkStream.Write(CombineSrc0Alpha);
    rkStream.Write(CombineSrc1Alpha);
    rkStream.Write(CombineSrc2Alpha);
    rkStream.Write(CombineOp0RGB);
    rkStream.Write(CombineOp1RGB);
    rkStream.Write(CombineOp2RGB);
    rkStream.Write(CombineOp0Alpha);
    rkStream.Write(CombineOp1Alpha);
    rkStream.Write(CombineOp2Alpha);
    rkStream.Write(CombineScaleRGB);
    rkStream.Write(CombineScaleAlpha);

    // link data
    rkStream.Write(m_spkImage);

    WM3_END_DEBUG_STREAM_SAVE(Texture);
}
//----------------------------------------------------------------------------
StringTree* Texture::SaveStrings (const char*)
{
    int iSQuantity = ( Apply == AM_COMBINE ? 26 : 10 );
    int iCQuantity = ( m_spkImage ? 2 : 1 );
    StringTree* pkTree = new StringTree(iSQuantity,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    char* acString = 0;
    switch ( Correction )
    {
    case CM_AFFINE:
        acString = StringTree::Format("correction = AFFINE");
        break;
    case CM_PERSPECTIVE:
        acString = StringTree::Format("correction = PERSPECTIVE");
        break;
    default:  // CM_QUANTITY
        break;
    };
    pkTree->SetString(1,acString);

    acString = 0;
    switch ( Apply )
    {
    case AM_REPLACE:
        acString = StringTree::Format("apply = REPLACE");
        break;
    case AM_DECAL:
        acString = StringTree::Format("apply = DECAL");
        break;
    case AM_MODULATE:
        acString = StringTree::Format("apply = MODULATE");
        break;
    case AM_BLEND:
        acString = StringTree::Format("apply = BLEND");
        break;
    case AM_ADD:
        acString = StringTree::Format("apply = ADD");
        break;
    case AM_COMBINE:
        acString = StringTree::Format("apply = COMBINE");
        break;
    default:  // AM_QUANTITY
        break;
    };
    pkTree->SetString(2,acString);

    acString = 0;
    switch ( CoordU )
    {
    case WM_CLAMP:
        acString = StringTree::Format("u = clamp");
        break;
    case WM_REPEAT:
        acString = StringTree::Format("u = repeat");
        break;
    case WM_CLAMP_BORDER:
        acString = StringTree::Format("u = border");
        break;
    case WM_CLAMP_EDGE:
        acString = StringTree::Format("u = edge");
        break;
    default:  // WM_QUANTITY
        break;
    };
    pkTree->SetString(3,acString);

    acString = 0;
    switch ( CoordV )
    {
    case WM_CLAMP:
        acString = StringTree::Format("v = clamp");
        break;
    case WM_REPEAT:
        acString = StringTree::Format("v = repeat");
        break;
    case WM_CLAMP_BORDER:
        acString = StringTree::Format("v = border");
        break;
    case WM_CLAMP_EDGE:
        acString = StringTree::Format("v = edge");
        break;
    default:  // WM_QUANTITY
        break;
    };
    pkTree->SetString(4,acString);

    acString = 0;
    switch ( Filter )
    {
    case FM_NEAREST:
        acString = StringTree::Format("filter = NEAREST");
        break;
    case FM_LINEAR:
        acString = StringTree::Format("filter = LINEAR");
        break;
    default:  // FM_QUANTITY
        break;
    };
    pkTree->SetString(5,acString);

    acString = 0;
    switch ( Mipmap )
    {
    case MM_NEAREST:
        acString = StringTree::Format("mipmap = NEAREST");
        break;
    case MM_LINEAR:
        acString = StringTree::Format("mipmap = LINEAR");
        break;
    case MM_NEAREST_NEAREST:
        acString = StringTree::Format("mipmap = NEAREST_NEAREST");
        break;
    case MM_NEAREST_LINEAR:
        acString = StringTree::Format("mipmap = NEAREST_LINEAR");
        break;
    case MM_LINEAR_NEAREST:
        acString = StringTree::Format("mipmap = LINEAR_NEAREST");
        break;
    case MM_LINEAR_LINEAR:
        acString = StringTree::Format("mipmap = LINEAR_LINEAR");
        break;
    default:  // MM_QUANTITY
        break;
    }
    pkTree->SetString(6,acString);

    acString = 0;
    switch ( Texgen )
    {
    case TG_NONE:
        acString = StringTree::Format("texgen = NONE");
        break;
    case TG_ENVIRONMENT_MAP:
        acString = StringTree::Format("texgen = ENVIRONMENT_MAP");
        break;
    case TG_PROJECTED_TEXTURE:
        acString = StringTree::Format("texgen = PROJECTED_TEXTURE");
        break;
    default:  // TG_QUANTITY
        break;
    }
    pkTree->SetString(7,acString);

    pkTree->SetString(8,StringTree::Format("blend color =",BlendColor));
    pkTree->SetString(9,StringTree::Format("border color =",BorderColor));

    if ( Apply == AM_COMBINE )
    {
        acString = 0;
        switch ( CombineFuncRGB )
        {
        case ACF_REPLACE:
            acString = StringTree::Format("CombFuncRGB = REPLACE");
            break;
        case ACF_MODULATE:
            acString = StringTree::Format("CombFuncRGB = MODULATE");
            break;
        case ACF_ADD:
            acString = StringTree::Format("CombFuncRGB = ADD");
            break;
        case ACF_ADD_SIGNED:
            acString = StringTree::Format("CombFuncRGB = ADD_SIGNED");
            break;
        case ACF_SUBTRACT:
            acString = StringTree::Format("CombFuncRGB = SUBTRACT");
            break;
        case ACF_INTERPOLATE:
            acString = StringTree::Format("CombFuncRGB = INTERPOLATE");
            break;
        case ACF_DOT3_RGB:
            acString = StringTree::Format("CombFuncRGB = DOT3_RGB");
            break;
        case ACF_DOT3_RGBA:
            acString = StringTree::Format("CombFuncRGB = DOT3_RGBA");
            break;
        default:  // ACF_QUANTITY
            break;
        }
        pkTree->SetString(10,acString);

        acString = 0;
        switch ( CombineFuncAlpha )
        {
        case ACF_REPLACE:
            acString = StringTree::Format("CombineFuncAlpha = REPLACE");
            break;
        case ACF_MODULATE:
            acString = StringTree::Format("CombineFuncAlpha = MODULATE");
            break;
        case ACF_ADD:
            acString = StringTree::Format("CombineFuncAlpha = ADD");
            break;
        case ACF_ADD_SIGNED:
            acString = StringTree::Format("CombineFuncAlpha = ADD_SIGNED");
            break;
        case ACF_SUBTRACT:
            acString = StringTree::Format("CombineFuncAlpha = SUBTRACT");
            break;
        case ACF_INTERPOLATE:
            acString = StringTree::Format("CombineFuncAlpha = INTERPOLATE");
            break;
        case ACF_DOT3_RGB:
            acString = StringTree::Format("CombineFuncAlpha = DOT3_RGB");
            break;
        case ACF_DOT3_RGBA:
            acString = StringTree::Format("CombineFuncAlpha = DOT3_RGBA");
            break;
        default:  // ACF_QUANTITY
            break;
        }
        pkTree->SetString(11,acString);

        acString = 0;
        switch ( CombineSrc0RGB )
        {
        case ACS_TEXTURE:
            acString = StringTree::Format("CombineSrc0RGB = TEXTURE");
            break;
        case ACS_PRIMARY_COLOR:
            acString = StringTree::Format("CombineSrc0RGB = PRIMARY_COLOR");
            break;
        case ACS_CONSTANT:
            acString = StringTree::Format("CombineSrc0RGB = CONSTANT");
            break;
        case ACS_PREVIOUS:
            acString = StringTree::Format("CombineSrc0RGB = PREVIOUS");
            break;
        default:  // ACS_QUANTITY
            break;
        }
        pkTree->SetString(12,acString);

        acString = 0;
        switch ( CombineSrc1RGB )
        {
        case ACS_TEXTURE:
            acString = StringTree::Format("CombineSrc1RGB = TEXTURE");
            break;
        case ACS_PRIMARY_COLOR:
            acString = StringTree::Format("CombineSrc1RGB = PRIMARY_COLOR");
            break;
        case ACS_CONSTANT:
            acString = StringTree::Format("CombineSrc1RGB = CONSTANT");
            break;
        case ACS_PREVIOUS:
            acString = StringTree::Format("CombineSrc1RGB = PREVIOUS");
            break;
        default:  // ACS_QUANTITY
            break;
        }
        pkTree->SetString(13,acString);

        acString = 0;
        switch ( CombineSrc2RGB )
        {
        case ACS_TEXTURE:
            acString = StringTree::Format("CombineSrc1RGB = TEXTURE");
            break;
        case ACS_PRIMARY_COLOR:
            acString = StringTree::Format("CombineSrc1RGB = PRIMARY_COLOR");
            break;
        case ACS_CONSTANT:
            acString = StringTree::Format("CombineSrc1RGB = CONSTANT");
            break;
        case ACS_PREVIOUS:
            acString = StringTree::Format("CombineSrc1RGB = PREVIOUS");
            break;
        default:  // ACS_QUANTITY
            break;
        }
        pkTree->SetString(14,acString);

        acString = 0;
        switch ( CombineSrc0Alpha )
        {
        case ACS_TEXTURE:
            acString = StringTree::Format("CombineSrc0Alpha = TEXTURE");
            break;
        case ACS_PRIMARY_COLOR:
            acString = StringTree::Format("CombineSrc0Alpha = PRIMARY_COLOR");
            break;
        case ACS_CONSTANT:
            acString = StringTree::Format("CombineSrc0Alpha = CONSTANT");
            break;
        case ACS_PREVIOUS:
            acString = StringTree::Format("CombineSrc0Alpha = PREVIOUS");
            break;
        default:  // ACS_QUANTITY
            break;
        }
        pkTree->SetString(15,acString);

        acString = 0;
        switch ( CombineSrc1Alpha )
        {
        case ACS_TEXTURE:
            acString = StringTree::Format("CombineSrc1Alpha = TEXTURE");
            break;
        case ACS_PRIMARY_COLOR:
            acString = StringTree::Format("CombineSrc1Alpha = PRIMARY_COLOR");
            break;
        case ACS_CONSTANT:
            acString = StringTree::Format("CombineSrc1Alpha = CONSTANT");
            break;
        case ACS_PREVIOUS:
            acString = StringTree::Format("CombineSrc1Alpha = PREVIOUS");
            break;
        default:  // ACS_QUANTITY
            break;
        }
        pkTree->SetString(16,acString);

        acString = 0;
        switch ( CombineSrc2Alpha )
        {
        case ACS_TEXTURE:
            acString = StringTree::Format("CombineSrc2Alpha = TEXTURE");
            break;
        case ACS_PRIMARY_COLOR:
            acString = StringTree::Format("CombineSrc2Alpha = PRIMARY_COLOR");
            break;
        case ACS_CONSTANT:
            acString = StringTree::Format("CombineSrc2Alpha = CONSTANT");
            break;
        case ACS_PREVIOUS:
            acString = StringTree::Format("CombineSrc2Alpha = PREVIOUS");
            break;
        default:  // ACS_QUANTITY
            break;
        }
        pkTree->SetString(17,acString);

        acString = 0;
        switch ( CombineOp0RGB )
        {
        case ACO_SRC_COLOR:
            acString = StringTree::Format("CombineOp0RGB = SRC_COLOR");
            break;
        case ACO_ONE_MINUS_SRC_COLOR:
            acString = StringTree::Format(
                "CombineOp0RGB = ONE_MINUS_SRC_COLOR");
            break;
        case ACO_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp0RGB = SRC_ALPHA");
            break;
        case ACO_ONE_MINUS_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp0RGB = ONE_MINUS_SRC_ALPHA");
            break;
        default:  // ACO_QUANTITY
            break;
        }
        pkTree->SetString(18,acString);

        acString = 0;
        switch ( CombineOp1RGB )
        {
        case ACO_SRC_COLOR:
            acString = StringTree::Format("CombineOp1RGB = SRC_COLOR");
            break;
        case ACO_ONE_MINUS_SRC_COLOR:
            acString = StringTree::Format(
                "CombineOp1RGB = ONE_MINUS_SRC_COLOR");
            break;
        case ACO_SRC_ALPHA:
            acString = StringTree::Format("CombineOp1RGB = SRC_ALPHA");
            break;
        case ACO_ONE_MINUS_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp1RGB = ONE_MINUS_SRC_ALPHA");
            break;
        default:  // ACO_QUANTITY
            break;
        }
        pkTree->SetString(19,acString);

        acString = 0;
        switch ( CombineOp2RGB )
        {
        case ACO_SRC_COLOR:
            acString = StringTree::Format("CombineOp2RGB = SRC_COLOR");
            break;
        case ACO_ONE_MINUS_SRC_COLOR:
            acString = StringTree::Format(
                "CombineOp2RGB = ONE_MINUS_SRC_COLOR");
            break;
        case ACO_SRC_ALPHA:
            acString = StringTree::Format("CombineOp2RGB = SRC_ALPHA");
            break;
        case ACO_ONE_MINUS_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp2RGB = ONE_MINUS_SRC_ALPHA");
            break;
        default:  // ACO_QUANTITY
            break;
        }
        pkTree->SetString(20,acString);

        acString = 0;
        switch ( CombineOp0Alpha )
        {
        case ACO_SRC_COLOR:
            acString = StringTree::Format("CombineOp0Alpha = SRC_COLOR");
            break;
        case ACO_ONE_MINUS_SRC_COLOR:
            acString = StringTree::Format(
                "CombineOp0Alpha = ONE_MINUS_SRC_COLOR");
            break;
        case ACO_SRC_ALPHA:
            acString = StringTree::Format("CombineOp0Alpha = SRC_ALPHA");
            break;
        case ACO_ONE_MINUS_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp0Alpha = ONE_MINUS_SRC_ALPHA");
            break;
        default:  // ACO_QUANTITY
            break;
        }
        pkTree->SetString(21,acString);

        acString = 0;
        switch ( CombineOp1Alpha )
        {
        case ACO_SRC_COLOR:
            acString = StringTree::Format("CombineOp1Alpha = SRC_COLOR");
            break;
        case ACO_ONE_MINUS_SRC_COLOR:
            acString = StringTree::Format(
                "CombineOp1Alpha = ONE_MINUS_SRC_COLOR");
            break;
        case ACO_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp1Alpha = SRC_ALPHA");
            break;
        case ACO_ONE_MINUS_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp1Alpha = ONE_MINUS_SRC_ALPHA");
            break;
        default:  // ACO_QUANTITY
            break;
        }
        pkTree->SetString(22,acString);

        acString = 0;
        switch ( CombineOp2Alpha )
        {
        case ACO_SRC_COLOR:
            acString = StringTree::Format("CombineOp2Alpha = SRC_COLOR");
            break;
        case ACO_ONE_MINUS_SRC_COLOR:
            acString = StringTree::Format(
                "CombineOp2Alpha = ONE_MINUS_SRC_COLOR");
            break;
        case ACO_SRC_ALPHA:
            acString = StringTree::Format("CombineOp2Alpha = SRC_ALPHA");
            break;
        case ACO_ONE_MINUS_SRC_ALPHA:
            acString = StringTree::Format(
                "CombineOp2Alpha = ONE_MINUS_SRC_ALPHA");
            break;
        default:  // ACO_QUANTITY
            break;
        }
        pkTree->SetString(23,acString);

        acString = 0;
        switch ( CombineScaleRGB )
        {
        case ACSC_ONE:
            acString = StringTree::Format("CombineScaleRGB = ONE");
            break;
        case ACSC_TWO:
            acString = StringTree::Format("CombineScaleRGB = TWO");
            break;
        case ACSC_FOUR:
            acString = StringTree::Format("CombineScaleRGB = FOUR");
            break;
        default:  // ACSC_QUANTITY
            break;
        }
        pkTree->SetString(24,acString);

        acString = 0;
        switch ( CombineScaleAlpha )
        {
        case ACSC_ONE:
            acString = StringTree::Format("CombineScaleAlpha = ONE");
            break;
        case ACSC_TWO:
            acString = StringTree::Format("CombineScaleAlpha = TWO");
            break;
        case ACSC_FOUR:
            acString = StringTree::Format("CombineScaleAlpha = FOUR");
            break;
        default:  // ACSC_QUANTITY
            break;
        }
        pkTree->SetString(25,acString);
    }

    // children
    pkTree->SetChild(0,Object::SaveStrings());
    if ( m_spkImage )
        pkTree->SetChild(1,m_spkImage->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Texture::GetMemoryUsed () const
{
    return sizeof(Texture) - sizeof(Object) + Object::GetMemoryUsed() +
        BIArray.GetArray().GetMaxQuantity()*sizeof(BindInfo);
}
//----------------------------------------------------------------------------
int Texture::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(m_spkImage) +
        sizeof(Correction) +
        sizeof(Apply) +
        sizeof(CoordU) +
        sizeof(CoordV) +
        sizeof(Filter) +
        sizeof(Mipmap) +
        sizeof(Texgen) +
        sizeof(BlendColor) +
        sizeof(BorderColor) +
        sizeof(CombineFuncRGB) +
        sizeof(CombineFuncAlpha) +
        sizeof(CombineSrc0RGB) +
        sizeof(CombineSrc1RGB) +
        sizeof(CombineSrc2RGB) +
        sizeof(CombineSrc0Alpha) +
        sizeof(CombineSrc1Alpha) +
        sizeof(CombineSrc2Alpha) +
        sizeof(CombineOp0RGB) +
        sizeof(CombineOp1RGB) +
        sizeof(CombineOp2RGB) +
        sizeof(CombineOp0Alpha) +
        sizeof(CombineOp1Alpha) +
        sizeof(CombineOp2Alpha) +
        sizeof(CombineScaleRGB) +
        sizeof(CombineScaleAlpha);
}
//----------------------------------------------------------------------------
