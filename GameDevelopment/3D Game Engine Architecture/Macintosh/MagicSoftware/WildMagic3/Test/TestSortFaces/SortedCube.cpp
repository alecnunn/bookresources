// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "SortedCube.h"
#include "Wm3TriMesh.h"
#include "Wm3TextureEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SortedCube,Node);
WM3_IMPLEMENT_STREAM(SortedCube);

//----------------------------------------------------------------------------
SortedCube::SortedCube (Camera* pkCamera, Image* pkXpImage, Image* pkXmImage,
    Image* pkYpImage, Image* pkYmImage, Image* pkZpImage, Image* pkZmImage)
    :
    Node(6,0),
    m_spkCamera(pkCamera),
    m_iBackFacing(0)
{
    // no culling (all faces drawn)
    CullState* pkCS = new CullState;
    pkCS->Enabled = false;
    SetGlobalState(pkCS);

    // depth buffering writes, but no reads
    ZBufferState* pkZS = new ZBufferState;
    pkZS->Enabled = false;
    pkZS->Writable = true;
    SetGlobalState(pkZS);

    // the child nodes have textures with an alpha channel of 1/2
    AlphaState* pkAS = new AlphaState;
    pkAS->BlendEnabled = true;
    SetGlobalState(pkAS);

    // vertices for shared face
    int iVQuantity = 4;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    akVertex[0] = Vector3f(-1.0f,-1.0f,0.0f);
    akVertex[1] = Vector3f( 1.0f,-1.0f,0.0f);
    akVertex[2] = Vector3f( 1.0f, 1.0f,0.0f);
    akVertex[3] = Vector3f(-1.0f, 1.0f,0.0f);
    Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);

    // triangles
    int iIQuantity = 6;
    int* aiIndex = new int[iIQuantity];
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    IntArray* pkIndices = new IntArray(iIQuantity,aiIndex);

    // texture coordinates
    Vector2f* akUV = new Vector2f[iVQuantity];
    akUV[0] = Vector2f(0.0f,0.0f);
    akUV[1] = Vector2f(1.0f,0.0f);
    akUV[2] = Vector2f(1.0f,1.0f);
    akUV[3] = Vector2f(0.0f,1.0f);
    Vector2fArray* pkUVs = new Vector2fArray(iVQuantity,akUV);

    // The six TriMesh children of this object are initially stored in the
    // order: x=1 face, x=-1 face, y=1 face, y=-1 face, z=1 face, z=-1 face.

    // xp face (x = 1)
    m_aspkFace[0] = new TriMesh(pkVertices,pkIndices,false);
    Texture* pkTextureXp = new Texture(pkXpImage);
    TextureEffect* pkEffectXp = new TextureEffect(pkTextureXp,pkUVs);
    m_aspkFace[0]->SetEffect(pkEffectXp);
    m_aspkFace[0]->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Y,
        Mathf::HALF_PI));
    m_aspkFace[0]->Local.SetTranslate(Vector3f::UNIT_X);
    AttachChild(m_aspkFace[0]);

    // xm face (x = -1)
    m_aspkFace[1] = new TriMesh(pkVertices,pkIndices,false);
    Texture* pkTextureXm = new Texture(pkXmImage);
    TextureEffect* pkEffectXm = new TextureEffect(pkTextureXm,pkUVs);
    m_aspkFace[1]->SetEffect(pkEffectXm);
    m_aspkFace[1]->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Y,
        -Mathf::HALF_PI));
    m_aspkFace[1]->Local.SetTranslate(-Vector3f::UNIT_X);
    AttachChild(m_aspkFace[1]);

    // yp face (y = 1)
    m_aspkFace[2] = new TriMesh(pkVertices,pkIndices,false);
    Texture* pkTextureYp = new Texture(pkYpImage);
    TextureEffect* pkEffectYp = new TextureEffect(pkTextureYp,pkUVs);
    m_aspkFace[2]->SetEffect(pkEffectYp);
    m_aspkFace[2]->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_X,
        -Mathf::HALF_PI));
    m_aspkFace[2]->Local.SetTranslate(Vector3f::UNIT_Y);
    AttachChild(m_aspkFace[2]);

    // ym face (y = -1)
    m_aspkFace[3] = new TriMesh(pkVertices,pkIndices,false);
    Texture* pkTextureYm = new Texture(pkYmImage);
    TextureEffect* pkEffectYm = new TextureEffect(pkTextureYm,pkUVs);
    m_aspkFace[3]->SetEffect(pkEffectYm);
    m_aspkFace[3]->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_X,
        Mathf::HALF_PI));
    m_aspkFace[3]->Local.SetTranslate(-Vector3f::UNIT_Y);
    AttachChild(m_aspkFace[3]);

    // zp face (z = 1)
    m_aspkFace[4] = new TriMesh(pkVertices,pkIndices,false);
    Texture* pkTextureZp = new Texture(pkZpImage);
    TextureEffect* pkEffectZp = new TextureEffect(pkTextureZp,pkUVs);
    m_aspkFace[4]->SetEffect(pkEffectZp);
    m_aspkFace[4]->Local.SetTranslate(Vector3f::UNIT_Z);
    AttachChild(m_aspkFace[4]);

    // zm face (z = -1)
    m_aspkFace[5] = new TriMesh(pkVertices,pkIndices,false);
    Texture* pkTextureZm = new Texture(pkZmImage);
    TextureEffect* pkEffectZm = new TextureEffect(pkTextureZm,pkUVs);
    m_aspkFace[5]->SetEffect(pkEffectZm);
    m_aspkFace[5]->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Y,
        Mathf::PI));
    m_aspkFace[5]->Local.SetTranslate(-Vector3f::UNIT_Z);
    AttachChild(m_aspkFace[5]);
}
//----------------------------------------------------------------------------
SortedCube::SortedCube ()
{
}
//----------------------------------------------------------------------------
SortedCube::~SortedCube ()
{
}
//----------------------------------------------------------------------------
void SortedCube::SetCamera (Camera* pkCamera)
{
    m_spkCamera = pkCamera;
}
//----------------------------------------------------------------------------
void SortedCube::SortFaces ()
{
    // must have a camera for sorting
    if ( !m_spkCamera )
        return;

    // Inverse transform the camera world view direction into the model space
    // of the cube.
    Vector3f kMDir = World.InvertVector(m_spkCamera->GetWorldDVector());

    // test for back faces
    int iNewBackFacing = 0, iBackQuantity = 0;

    // test xp face
    if ( kMDir.X() > 0.0f )
    {
        iNewBackFacing |=  1;
        iBackQuantity++;
    }

    // test xm face
    if ( kMDir.X() < 0.0f )
    {
        iNewBackFacing |=  2;
        iBackQuantity++;
    }

    // test yp face
    if ( kMDir.Y() > 0.0f )
    {
        iNewBackFacing |=  4;
        iBackQuantity++;
    }

    // test ym face
    if ( kMDir.Y() < 0.0f )
    {
        iNewBackFacing |=  8;
        iBackQuantity++;
    }

    // test zp face
    if ( kMDir.Z() > 0.0f )
    {
        iNewBackFacing |= 16;
        iBackQuantity++;
    }

    // test zm face
    if ( kMDir.Z() < 0.0f )
    {
        iNewBackFacing |= 32;
        iBackQuantity++;
    }

    if ( iNewBackFacing != m_iBackFacing )
    {
        // Resorting is needed.  Reassign the sorted children to the node
        // parent.  Back facing children go first, front facing second.
        m_iBackFacing = iNewBackFacing;

        // detach old children
        int i;
        for (i = 0; i < 6; i++)
            SetChild(i,0);

        // attach new children
        int iBStart = 0, iFStart = iBackQuantity, iMask;
        for (i = 0, iMask = 1; i < 6; i++, iMask <<= 1)
        {
            if ( m_iBackFacing & iMask )
                SetChild(iBStart++,m_aspkFace[i]);
            else
                SetChild(iFStart++,m_aspkFace[i]);
        }
    }
}
//----------------------------------------------------------------------------
void SortedCube::Draw (Renderer& rkRenderer, bool bNoCull)
{
    SortFaces();
    Node::Draw(rkRenderer,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* SortedCube::GetObjectByName (const String& rkName)
{
    Object* pkFound = Node::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_spkCamera )
    {
        pkFound = m_spkCamera->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void SortedCube::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Node::GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkCamera )
        m_spkCamera->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* SortedCube::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Node::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_spkCamera )
    {
        pkFound = m_spkCamera->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SortedCube::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkCamera
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(SortedCube);
}
//----------------------------------------------------------------------------
void SortedCube::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkCamera = (Camera*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool SortedCube::Register (Stream& rkStream) const
{
    if ( !Node::Register(rkStream) )
        return false;

    if ( m_spkCamera )
        m_spkCamera->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void SortedCube::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // link data
    rkStream.Write(m_spkCamera);

    WM3_END_DEBUG_STREAM_SAVE(SortedCube);
}
//----------------------------------------------------------------------------
StringTree* SortedCube::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(1,2);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Node::SaveStrings());
    pkTree->SetChild(0,m_spkCamera->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int SortedCube::GetMemoryUsed () const
{
    return sizeof(SortedCube) - sizeof(Node) + Node::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int SortedCube::GetDiskUsed () const
{
    return Node::GetDiskUsed() + sizeof(m_spkCamera);
}
//----------------------------------------------------------------------------
