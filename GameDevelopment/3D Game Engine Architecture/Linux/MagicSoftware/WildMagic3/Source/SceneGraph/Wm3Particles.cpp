// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Particles.h"
#include "Wm3Camera.h"
#include "Wm3Light.h"
#include "Wm3Texture.h"
#include "Wm3Vector2Array.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Particles,TriMesh);
WM3_IMPLEMENT_STREAM(Particles);

//----------------------------------------------------------------------------
Particles::Particles (Vector3fArrayPtr spkLocations, FloatArrayPtr spkSizes,
    bool bWantNormals)
    :
    Locations(spkLocations),
    Sizes(spkSizes)
{
    // create storage for model space quad vertices
    int iLQuantity = Locations->GetQuantity();
    int iVQuantity = 4*iLQuantity;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    memset(akVertex,0,iVQuantity*sizeof(Vector3f));
    Vertices = new Vector3fArray(iVQuantity,akVertex);

    // create storage for model space normals
    if ( bWantNormals )
    {
        Vector3f* akNormal = new Vector3f[iVQuantity];
        memset(akNormal,0,iVQuantity*sizeof(Vector3f));
        Normals = new Vector3fArray(iVQuantity,akNormal);
    }

    // generate connectivity
    int iIQuantity = 6*iLQuantity;
    int* aiIndex = new int[iIQuantity];
    for (int i = 0, j = 0; i < iLQuantity; i++)
    {
        int iFI = 4*i, iFIp1 = iFI+1, iFIp2 = iFI+2, iFIp3 = iFI+3;
        aiIndex[j++] = iFI;
        aiIndex[j++] = iFIp1;
        aiIndex[j++] = iFIp2;
        aiIndex[j++] = iFI;
        aiIndex[j++] = iFIp2;
        aiIndex[j++] = iFIp3;
    }
    Indices = new IntArray(iIQuantity,aiIndex);

    // compute a bounding sphere based only on the particle locations
    ModelBound->ComputeFromData(Locations);

    SizeAdjust = 1.0f;
    m_iActiveQuantity = iLQuantity;
}
//----------------------------------------------------------------------------
Particles::Particles ()
{
    SizeAdjust = 0.0f;
    m_iActiveQuantity = 0;
}
//----------------------------------------------------------------------------
Particles::~Particles ()
{
}
//----------------------------------------------------------------------------
void Particles::SetActiveQuantity (int iActiveQuantity)
{
    int iLQuantity = Locations->GetQuantity();
    if ( 0 <= iActiveQuantity && iActiveQuantity <= iLQuantity )
        m_iActiveQuantity = iActiveQuantity;
    else
        m_iActiveQuantity = iLQuantity;
}
//----------------------------------------------------------------------------
void Particles::GenerateColorRGBs ()
{
    int iLQuantity = Locations->GetQuantity();
    int iVQuantity = 4*iLQuantity;
    ColorRGB* akPColor = m_spkParticleEffect->ColorRGBs->GetData();
    ColorRGB* akMColor = new ColorRGB[iVQuantity];
    for (int i = 0, j = 0; i < iLQuantity; i++)
    {
        // get the particle color
        ColorRGB& rkColor = akPColor[i];

        // assign it as the quad color, applied to all four vertices
        akMColor[j++] = rkColor;
        akMColor[j++] = rkColor;
        akMColor[j++] = rkColor;
        akMColor[j++] = rkColor;
    }

    m_spkEffect->ColorRGBs = new ColorRGBArray(iVQuantity,akMColor);
}
//----------------------------------------------------------------------------
void Particles::GenerateColorRGBAs ()
{
    int iLQuantity = Locations->GetQuantity();
    int iVQuantity = 4*iLQuantity;
    ColorRGBA* akPColor = m_spkParticleEffect->ColorRGBAs->GetData();
    ColorRGBA* akMColor = new ColorRGBA[iVQuantity];
    for (int i = 0, j = 0; i < iLQuantity; i++)
    {
        // get the particle color
        ColorRGBA& rkColor = akPColor[i];

        // assign it as the quad color, applied to all four vertices
        akMColor[j++] = rkColor;
        akMColor[j++] = rkColor;
        akMColor[j++] = rkColor;
        akMColor[j++] = rkColor;
    }

    m_spkEffect->ColorRGBAs = new ColorRGBAArray(iVQuantity,akMColor);
}
//----------------------------------------------------------------------------
void Particles::GenerateUVs ()
{
    int iLQuantity = Vertices->GetQuantity();
    int iVQuantity = 4*iLQuantity;
    Vector2f* akUV = new Vector2f[iVQuantity];
    int i, j;
    for (i = 0, j = 0; i < iLQuantity; i++)
    {
        akUV[j++] = Vector2f(0.0f,0.0f);
        akUV[j++] = Vector2f(0.0f,1.0f);
        akUV[j++] = Vector2f(1.0f,1.0f);
        akUV[j++] = Vector2f(1.0f,0.0f);
    }
    Vector2fArrayPtr spkUVs = new Vector2fArray(iVQuantity,akUV);
    for (i = 0; i < m_spkEffect->Textures.GetQuantity(); i++)
        m_spkEffect->UVs[i] = spkUVs;
}
//----------------------------------------------------------------------------
void Particles::GenerateParticles (const Camera* pkCamera)
{
    int i, j;

    // get camera axis directions in model space of particles
    Vector3f kUpR = (pkCamera->GetWorldUVector() +
        pkCamera->GetWorldRVector())*World.GetRotate();
    Vector3f kUmR = (pkCamera->GetWorldUVector() -
        pkCamera->GetWorldRVector())*World.GetRotate();

    // normal vectors are towards the viewer
    if ( Normals )
    {
        Vector3f* akNormal = Normals->GetData();
        Vector3f kDir = -pkCamera->GetWorldDVector()*World.GetRotate();
        for (j = 0; j < 4*m_iActiveQuantity; j++)
            akNormal[j] = kDir;
    }

    // generate particle quadrilaterals as pairs of triangles
    Vector3f* akLocation = Locations->GetData();
    float* afSize = Sizes->GetData();
    Vector3f* akVertex = Vertices->GetData();
    for (i = 0, j = 0; i < m_iActiveQuantity; i++)
    {
        Vector3f& rkCenter = akLocation[i];

        float fTrueSize = SizeAdjust*afSize[i];
        Vector3f kScaledUpR = fTrueSize*kUpR;
        Vector3f kScaledUmR = fTrueSize*kUmR;

        akVertex[j++] = rkCenter - kScaledUmR;
        akVertex[j++] = rkCenter + kScaledUpR;
        akVertex[j++] = rkCenter + kScaledUmR;
        akVertex[j++] = rkCenter - kScaledUpR;
    }

    UpdateMS();
}
//----------------------------------------------------------------------------
void Particles::SetEffect (Effect* pkEffect)
{
    m_spkParticleEffect = pkEffect;
    RefreshEffect();
}
//----------------------------------------------------------------------------
void Particles::RefreshEffect ()
{
    if ( !m_spkParticleEffect )
        return;

    // clone an effect for the triangle mesh representing the particles
    m_spkEffect = m_spkParticleEffect->Clone();

    // quadruple the RGB colors
    if ( m_spkParticleEffect->ColorRGBs )
        GenerateColorRGBs();

    // quadruple the RGBA colors
    if ( m_spkParticleEffect->ColorRGBAs )
        GenerateColorRGBAs();

    // generate textures and UVs for all active textures of m_spkEffect
    if ( m_spkParticleEffect->Textures.GetQuantity() > 0 )
        GenerateUVs();
}
//----------------------------------------------------------------------------
void Particles::Draw (Renderer& rkRenderer, bool bNoCull)
{
    GenerateParticles(rkRenderer.GetCamera());
    TriMesh::Draw(rkRenderer,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Particles::GetObjectByName (const String& rkName)
{
    Object* pkFound = TriMesh::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( Locations )
    {
        pkFound = Locations->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( Sizes )
    {
        pkFound = Sizes->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Particles::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    TriMesh::GetAllObjectsByName(rkName,rkObjects);

    if ( Locations )
        Locations->GetAllObjectsByName(rkName,rkObjects);

    if ( Sizes )
        Sizes->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* Particles::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = TriMesh::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( Locations )
    {
        pkFound = Locations->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( Sizes )
    {
        pkFound = Sizes->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Particles::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(SizeAdjust);
    rkStream.Read(m_iActiveQuantity);

    // link data
    Object* pkObject;

    rkStream.Read(pkObject);  // Locations
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // Sizes
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // m_spkParticleEffect
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(Particles);
}
//----------------------------------------------------------------------------
void Particles::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    Locations = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    Sizes = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    m_spkParticleEffect = (Effect*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Particles::Register (Stream& rkStream) const
{
    if ( !TriMesh::Register(rkStream) )
        return false;

    if ( Locations )
        Locations->Register(rkStream);

    if ( Sizes )
        Sizes->Register(rkStream);

    if ( m_spkParticleEffect )
        m_spkParticleEffect->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void Particles::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(SizeAdjust);
    rkStream.Write(m_iActiveQuantity);

    // link data
    rkStream.Write(Locations);
    rkStream.Write(Sizes);
    rkStream.Write(m_spkParticleEffect);

    WM3_END_DEBUG_STREAM_SAVE(Particles);
}
//----------------------------------------------------------------------------
StringTree* Particles::SaveStrings (const char*)
{
    int iCQuantity = ( m_spkParticleEffect ? 4 : 3 );
    StringTree* pkTree = new StringTree(3,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("active =",m_iActiveQuantity));
    pkTree->SetString(2,StringTree::Format("size adjust =",SizeAdjust));

    // children
    pkTree->SetChild(0,TriMesh::SaveStrings());
    pkTree->SetChild(1,Locations->SaveStrings("locations"));
    pkTree->SetChild(2,Sizes->SaveStrings("sizes"));
    if ( m_spkParticleEffect )
        pkTree->SetChild(3,m_spkParticleEffect->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Particles::GetMemoryUsed () const
{
    return sizeof(Particles) - sizeof(TriMesh) + TriMesh::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int Particles::GetDiskUsed () const
{
    return TriMesh::GetDiskUsed() +
        sizeof(SizeAdjust) +
        sizeof(m_iActiveQuantity) +
        sizeof(Locations) +
        sizeof(Sizes) +
        sizeof(m_spkParticleEffect);
}
//----------------------------------------------------------------------------
