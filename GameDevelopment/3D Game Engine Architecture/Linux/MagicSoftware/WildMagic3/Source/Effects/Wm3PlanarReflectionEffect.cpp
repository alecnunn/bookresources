// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3PlanarReflectionEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,PlanarReflectionEffect,Effect);
WM3_IMPLEMENT_STREAM(PlanarReflectionEffect);

//----------------------------------------------------------------------------
PlanarReflectionEffect::PlanarReflectionEffect (int iQuantity)
{
    Draw = &Renderer::DrawPlanarReflection;

    assert( iQuantity > 0 );
    m_iQuantity = iQuantity;
    m_aspkPlane = new TriMeshPtr[m_iQuantity];
    m_afReflectance = new float[m_iQuantity];
}
//----------------------------------------------------------------------------
PlanarReflectionEffect::PlanarReflectionEffect ()
{
    Draw = &Renderer::DrawPlanarShadow;
    m_iQuantity = 0;
    m_aspkPlane = 0;
    m_afReflectance = 0;
}
//----------------------------------------------------------------------------
PlanarReflectionEffect::~PlanarReflectionEffect ()
{
    delete[] m_aspkPlane;
    delete[] m_afReflectance;
}
//----------------------------------------------------------------------------
Effect* PlanarReflectionEffect::Clone ()
{
    PlanarReflectionEffect* pkClone =
        new PlanarReflectionEffect(m_iQuantity);
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);

    PlanarReflectionEffect* pkDClone = (PlanarReflectionEffect*)pkClone;
    for (int i = 0; i < m_iQuantity; i++)
    {
        pkDClone->m_aspkPlane[i] = m_aspkPlane[i];
        pkDClone->m_afReflectance[i] = m_afReflectance[i];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* PlanarReflectionEffect::GetObjectByName (const String& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_aspkPlane )
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
            {
                pkFound = m_aspkPlane[i]->GetObjectByName(rkName);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    if ( m_aspkPlane )
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
                m_aspkPlane[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }
}
//----------------------------------------------------------------------------
Object* PlanarReflectionEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_aspkPlane )
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
            {
                pkFound = m_aspkPlane[i]->GetObjectByID(uiID);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iQuantity);

    if ( m_iQuantity > 0 )
    {
        m_aspkPlane = new TriMeshPtr[m_iQuantity];
        m_afReflectance = new float[m_iQuantity];

        // native data
        rkStream.Read(m_iQuantity,m_afReflectance);

        // link data
        for (int i = 0; i < m_iQuantity; i++)
        {
            Object* pkObject;
            rkStream.Read(pkObject);  // m_aspkPlane[i]
            pkLink->Add(pkObject);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(PlanarReflectionEffect);
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    for (int i = 0; i < m_iQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_aspkPlane[i] = (TriMesh*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool PlanarReflectionEffect::Register (Stream& rkStream) const
{
    if ( !Effect::Register(rkStream) )
        return false;

    if ( m_aspkPlane )
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
                m_aspkPlane[i]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // native data
    rkStream.Write(m_iQuantity);
    if ( m_iQuantity > 0 )
    {
        rkStream.Write(m_iQuantity,m_afReflectance);

        // link data
        ObjectPtr* aspkArray = (ObjectPtr*)m_aspkPlane;
        rkStream.Write(m_iQuantity,aspkArray);
    }

    WM3_END_DEBUG_STREAM_SAVE(PlanarReflectionEffect);
}
//----------------------------------------------------------------------------
StringTree* PlanarReflectionEffect::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,3);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("quantity =",m_iQuantity));

    // children
    pkTree->SetChild(0,Effect::SaveStrings());

    // planes
    StringTree* pkAttrTree = new StringTree(1,m_iQuantity);
    pkAttrTree->SetString(0,StringTree::Format("planes"));
    for (int i = 0; i < m_iQuantity; i++)
        pkAttrTree->SetChild(i,m_aspkPlane[i]->SaveStrings());

    pkTree->SetChild(1,pkAttrTree);

    // reflectances
    pkTree->SetChild(2,StringTree::Format("reflectances",m_iQuantity,
        m_afReflectance));

    return pkTree;
}
//----------------------------------------------------------------------------
int PlanarReflectionEffect::GetMemoryUsed () const
{
    int iSize = sizeof(PlanarReflectionEffect) - sizeof(Effect) +
        Effect::GetMemoryUsed();

    if ( m_iQuantity > 0 )
    {
        iSize += m_iQuantity*sizeof(m_afReflectance[0]);
        iSize += m_iQuantity*sizeof(m_aspkPlane[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
int PlanarReflectionEffect::GetDiskUsed () const
{
    int iSize = Effect::GetDiskUsed() + sizeof(m_iQuantity);

    if ( m_iQuantity > 0 )
    {
        iSize += m_iQuantity*sizeof(m_afReflectance[0]);
        iSize += m_iQuantity*sizeof(m_aspkPlane[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
