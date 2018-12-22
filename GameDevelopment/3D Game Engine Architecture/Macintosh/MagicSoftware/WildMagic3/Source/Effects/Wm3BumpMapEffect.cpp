// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BumpMapEffect.h"
#include "Wm3TriMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BumpMapEffect,Effect);
WM3_IMPLEMENT_STREAM(BumpMapEffect);

//----------------------------------------------------------------------------
BumpMapEffect::BumpMapEffect (Image* pkPrimary, Vector2fArray* pkPrimaryUVs,
    Image* pkNormalMap, Vector2fArray* pkNormalUVs, Light* pkLight)
    :
    m_spkLight(pkLight)
{
    Draw = &Renderer::DrawBumpMap;
    int iVQuantity = pkPrimaryUVs->GetQuantity();
    ColorRGB* akColor = new ColorRGB[iVQuantity];
    ColorRGBs = new ColorRGBArray(iVQuantity,akColor);

    // primary texture
    Texture* pkTexture = new Texture;
    pkTexture->SetImage(pkPrimary);
    pkTexture->Apply = Texture::AM_REPLACE;
    pkTexture->Filter = Texture::FM_LINEAR;
    pkTexture->Mipmap = Texture::MM_LINEAR_LINEAR;
    Textures.Append(pkTexture);
    UVs.Append(pkPrimaryUVs);

    // normal map texture
    pkTexture = new Texture;
    pkTexture->SetImage(pkNormalMap);
    pkTexture->Apply = Texture::AM_COMBINE;
    pkTexture->Filter = Texture::FM_LINEAR;
    pkTexture->Mipmap = Texture::MM_LINEAR;
    pkTexture->CombineFuncRGB = Texture::ACF_DOT3_RGB;
    pkTexture->CombineSrc0RGB = Texture::ACS_TEXTURE;
    pkTexture->CombineOp0RGB = Texture::ACO_SRC_COLOR;
    pkTexture->CombineSrc1RGB = Texture::ACS_PRIMARY_COLOR;
    pkTexture->CombineOp1RGB = Texture::ACO_SRC_COLOR;
    Textures.Append(pkTexture);
    UVs.Append(pkNormalUVs);
}
//----------------------------------------------------------------------------
BumpMapEffect::BumpMapEffect ()
{
    Draw = &Renderer::DrawBumpMap;
}
//----------------------------------------------------------------------------
BumpMapEffect::~BumpMapEffect ()
{
}
//----------------------------------------------------------------------------
Effect* BumpMapEffect::Clone ()
{
    BumpMapEffect* pkClone = new BumpMapEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void BumpMapEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);

    BumpMapEffect* pkDClone = (BumpMapEffect*)pkClone;
    pkDClone->m_spkLight = m_spkLight;
}
//----------------------------------------------------------------------------
void BumpMapEffect::ComputeLightVectors (Geometry* pkMesh)
{
    // Generate light direction vectors in the surface local space and store
    // them in the color array for interpolation via the rasterizer.

    // Compute the world direction from the light to the world translation
    // of the triangle mesh.
    Vector3f kWDir;
    switch ( m_spkLight->Type )
    {
    case Light::LT_DIRECTIONAL:
        kWDir = -m_spkLight->GetWorldDVector();
        break;
    case Light::LT_POINT:
    case Light::LT_SPOT:
        kWDir = m_spkLight->GetWorldLocation() - pkMesh->World.GetTranslate();
        break;
    default:
        // ambient light, nothing we can do to handle this
        return;
    }

    // Transform the world light vector into model space.  If the light is
    // directional, then kMLight is a vector (M,0).  If the light is
    // positional (point/spot), then kMLight is a point (M,1).
    Vector3f kMLight = pkMesh->World.InvertVector(kWDir);

    // Get the triangle mesh vertices, normals, and texture coordinates.
    int iVQuantity = pkMesh->Vertices->GetQuantity();
    Vector3f* akVertex = pkMesh->Vertices->GetData();
    Vector3f* akNormal = pkMesh->Normals->GetData();
    Vector2f* akUV = UVs[0]->GetData();
    int iTQuantity = pkMesh->Indices->GetQuantity()/3;
    int* piIndex = pkMesh->Indices->GetData();

    // Set the light vectors to (0,0,0) as a flag that the quantity has not
    // yet been computed.  The probability that a light vector is actually
    // (0,0,0) should be small, so the flag system should save computation
    // time overall.
    ColorRGB* akLVec = ColorRGBs->GetData();
    memset(akLVec,0,iVQuantity*sizeof(ColorRGB));

    for (int iT = 0; iT < iTQuantity; iT++)
    {
        // get the triangle vertices and attributes
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        Vector3f* apkV[3] = {&akVertex[iV0], &akVertex[iV1], &akVertex[iV2]};
        Vector3f* apkN[3] = {&akNormal[iV0], &akNormal[iV1], &akNormal[iV2]};
        Vector2f* apkUV[3] = {&akUV[iV0], &akUV[iV1], &akUV[iV2]};
        ColorRGB* apkC[3] = {&akLVec[iV0], &akLVec[iV1], &akLVec[iV2]};

        for (int i = 0; i < 3; i++)
        {
            ColorRGB& rkColor = *apkC[i];
            if ( rkColor != ColorRGB::BLACK )
                continue;

            int iP = (i == 0) ? 2 : i - 1;
            int iN = (i + 1) % 3;

            // compute edge vectors at the specified vertex
            Vector3f kDP1 = *apkV[iN] - *apkV[i];
            Vector3f kDP2 = *apkV[iP] - *apkV[i];

            // estimate a tangent surface vector
            Vector3f kTangent;
            bool fDegenerate = false;

            if ( Mathf::FAbs(kDP1.Length()) < Mathf::EPSILON
            ||   Mathf::FAbs(kDP2.Length()) < Mathf::EPSILON )
            {
                // The triangle is very small, call it degenerate.
                fDegenerate = true;
            }
            else
            {
                // difference of surface parameters along triangle edge
                float fDU1 = apkUV[iN]->X() - apkUV[i]->X();
                float fDV1 = apkUV[iN]->Y() - apkUV[i]->Y();

                if ( Mathf::FAbs(fDV1) < Mathf::EPSILON )
                {
                    // The triangle effectively has no variation in the v
                    // texture coordinate.
                    if ( Mathf::FAbs(fDU1) < Mathf::EPSILON )
                    {
                        // The triangle effectively has no variation in the u
                        // coordinate.  Since the texture coordinates do not
                        // effectively vary on this triangle, treat it as a
                        // degenerate parametric surface.
                        fDegenerate = true;
                    }
                    else
                    {
                        // The variation is effectively all in u, so set the
                        // tangent T = dP/du.
                        kTangent = kDP1/fDU1;
                    }
                }
                else
                {
                    // difference of surface parameters along triangle edge
                    float fDU2 = apkUV[iP]->X() - apkUV[i]->X();
                    float fDV2 = apkUV[iP]->Y() - apkUV[i]->Y();
                    float fDet = fDV1*fDU2 - fDV2*fDU1;

                    if ( Mathf::FAbs(fDet) >= Mathf::EPSILON )
                    {
                        // The triangle vertices form three collinear points
                        // in parameter space, so
                        //   dP/du = (dv1*dP2-dv2*dP1)/(dv1*du2-dv2*du1)
                        kTangent = (fDV1*kDP2-fDV2*kDP1)/fDet;
                    }
                    else
                    {
                        // The triangle vertices are collinear in parameter
                        // space.
                        fDegenerate = true;
                    }
                }
            }

            if ( fDegenerate )
            {
                // The texture coordinate mapping is not properly defined for
                // this.  Just say that the tangent space light vector points
                // in the same direction as the surface normal.
                rkColor.R() = apkN[i]->X();
                rkColor.G() = apkN[i]->Y();
                rkColor.B() = apkN[i]->Z();
                continue;
            }

            // Project T into the tangent plane by projecting out the surface
            // normal, then make it unit length.
            kTangent -= apkN[i]->Dot(kTangent)*(*apkN[i]);
            kTangent.Normalize();

            // compute the binormal B, another tangent perpendicular to T
            Vector3f kBinormal = apkN[i]->UnitCross(kTangent);

            // When generating bump/normal maps, folks usually work in a
            // left-handed screen space with the origin at the upper right,
            // u to the right, and v down, while we apply the textures with
            // the origin at the lower left, u right, v up, so we need to flip
            // the binormal (v-axis) to get a proper transformation to the
            // surface local texture space.
            Matrix3f kRotTS(
                kTangent.X(),   kTangent.Y(),   kTangent.Z(),
              -kBinormal.X(), -kBinormal.Y(), -kBinormal.Z(),
                apkN[i]->X(),   apkN[i]->Y(),   apkN[i]->Z());

            // compute the tangent space light direction
            Vector3f kTSDir;
            if ( m_spkLight->Type == Light::LT_DIRECTIONAL )
                kTSDir = kMLight;
            else  // light is positional
                kTSDir = kMLight - *apkV[i];

            kTSDir.Normalize();
            kTSDir = kRotTS*kTSDir;

            // Transform the light vector into [0,1]^3 to make it a valid
            // ColorRGB object.
            rkColor.R() = 0.5f*(kTSDir.X() + 1.0f);
            rkColor.G() = 0.5f*(kTSDir.Y() + 1.0f);
            rkColor.B() = 0.5f*(kTSDir.Z() + 1.0f);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* BumpMapEffect::GetObjectByName (const String& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_spkLight )
    {
        pkFound = m_spkLight->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void BumpMapEffect::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkLight )
        m_spkLight->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* BumpMapEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_spkLight )
    {
        pkFound = m_spkLight->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BumpMapEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkLight
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(BumpMapEffect);
}
//----------------------------------------------------------------------------
void BumpMapEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkLight = (Light*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool BumpMapEffect::Register (Stream& rkStream) const
{
    if ( !Effect::Register(rkStream) )
        return false;

    if ( m_spkLight )
        m_spkLight->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void BumpMapEffect::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // link data
    rkStream.Write(m_spkLight);

    WM3_END_DEBUG_STREAM_SAVE(BumpMapEffect);
}
//----------------------------------------------------------------------------
StringTree* BumpMapEffect::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(1,2);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Effect::SaveStrings());
    pkTree->SetChild(1,m_spkLight->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int BumpMapEffect::GetMemoryUsed () const
{
    return sizeof(BumpMapEffect) - sizeof(Effect) + Effect::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int BumpMapEffect::GetDiskUsed () const
{
    return Effect::GetDiskUsed() +
        sizeof(m_spkLight);
}
//----------------------------------------------------------------------------
