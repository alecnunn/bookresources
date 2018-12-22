// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3PlanarShadowEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,PlanarShadowEffect,Effect);
WM3_IMPLEMENT_STREAM(PlanarShadowEffect);

//----------------------------------------------------------------------------
PlanarShadowEffect::PlanarShadowEffect (int iQuantity)
{
    Draw = &Renderer::DrawPlanarShadow;

    assert( iQuantity > 0 );
    m_iQuantity = iQuantity;
    m_aspkPlane = new TriMeshPtr[m_iQuantity];
    m_aspkProjector = new LightPtr[m_iQuantity];
    m_akShadowColor = new ColorRGBA[m_iQuantity];
}
//----------------------------------------------------------------------------
PlanarShadowEffect::PlanarShadowEffect ()
{
    Draw = &Renderer::DrawPlanarShadow;
    m_iQuantity = 0;
    m_aspkPlane = 0;
    m_aspkProjector = 0;
    m_akShadowColor = 0;
}
//----------------------------------------------------------------------------
PlanarShadowEffect::~PlanarShadowEffect ()
{
    delete[] m_aspkPlane;
    delete[] m_aspkProjector;
    delete[] m_akShadowColor;
}
//----------------------------------------------------------------------------
Effect* PlanarShadowEffect::Clone ()
{
    PlanarShadowEffect* pkClone = new PlanarShadowEffect(m_iQuantity);
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);

    PlanarShadowEffect* pkDClone = (PlanarShadowEffect*)pkClone;
    for (int i = 0; i < m_iQuantity; i++)
    {
        pkDClone->m_aspkPlane[i] = m_aspkPlane[i];
        pkDClone->m_aspkProjector[i] = m_aspkProjector[i];
        pkDClone->m_akShadowColor[i] = m_akShadowColor[i];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* PlanarShadowEffect::GetObjectByName (const String& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    int i;

    if ( m_aspkPlane )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
            {
                pkFound = m_aspkPlane[i]->GetObjectByName(rkName);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    if ( m_aspkProjector )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkProjector[i] )
            {
                pkFound = m_aspkProjector[i]->GetObjectByName(rkName);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    int i;

    if ( m_aspkPlane )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
                m_aspkPlane[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }

    if ( m_aspkProjector )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkProjector[i] )
                m_aspkProjector[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }
}
//----------------------------------------------------------------------------
Object* PlanarShadowEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    int i;

    if ( m_aspkPlane )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
            {
                pkFound = m_aspkPlane[i]->GetObjectByID(uiID);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    if ( m_aspkProjector )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkProjector[i] )
            {
                pkFound = m_aspkProjector[i]->GetObjectByID(uiID);
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
void PlanarShadowEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iQuantity);

    if ( m_iQuantity > 0 )
    {
        m_aspkPlane = new TriMeshPtr[m_iQuantity];
        m_aspkProjector = new LightPtr[m_iQuantity];
        m_akShadowColor = new ColorRGBA[m_iQuantity];

        // native data
        rkStream.Read(m_iQuantity,m_akShadowColor);

        // link data
        Object* pkObject;
        int i;
        for (i = 0; i < m_iQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkPlane[i]
            pkLink->Add(pkObject);
        }
        for (i = 0; i < m_iQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkProjector[i]
            pkLink->Add(pkObject);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(PlanarShadowEffect);
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    int i;
    for (i = 0; i < m_iQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_aspkPlane[i] = (TriMesh*)rkStream.GetFromMap(pkLinkID);
    }
    for (i = 0; i < m_iQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_aspkProjector[i] = (Light*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool PlanarShadowEffect::Register (Stream& rkStream) const
{
    if ( !Effect::Register(rkStream) )
        return false;

    int i;

    if ( m_aspkPlane )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkPlane[i] )
                m_aspkPlane[i]->Register(rkStream);
        }
    }

    if ( m_aspkProjector )
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if ( m_aspkProjector[i] )
                m_aspkProjector[i]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // native data
    rkStream.Write(m_iQuantity);
    if ( m_iQuantity > 0 )
    {
        rkStream.Write(m_iQuantity,m_akShadowColor);

        // link data
        ObjectPtr* aspkArray = (ObjectPtr*)m_aspkPlane;
        rkStream.Write(m_iQuantity,aspkArray);
        aspkArray = (ObjectPtr*)m_aspkProjector;
        rkStream.Write(m_iQuantity,aspkArray);
    }

    WM3_END_DEBUG_STREAM_SAVE(PlanarShadowEffect);
}
//----------------------------------------------------------------------------
StringTree* PlanarShadowEffect::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,4);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("quantity =",m_iQuantity));

    // children
    pkTree->SetChild(0,Effect::SaveStrings());

    // planes
    StringTree* pkAttrTree = new StringTree(1,m_iQuantity);
    pkAttrTree->SetString(0,StringTree::Format("planes"));
    int i;
    for (i = 0; i < m_iQuantity; i++)
        pkAttrTree->SetChild(i,m_aspkPlane[i]->SaveStrings());

    pkTree->SetChild(1,pkAttrTree);

    // projectors
    pkAttrTree = new StringTree(1,m_iQuantity);
    pkAttrTree->SetString(0,StringTree::Format("projectors"));
    for (i = 0; i < m_iQuantity; i++)
        pkAttrTree->SetChild(i,m_aspkProjector[i]->SaveStrings());

    pkTree->SetChild(2,pkAttrTree);

    // colors
    pkTree->SetChild(3,StringTree::Format("colors",m_iQuantity,
        m_akShadowColor));

    return pkTree;
}
//----------------------------------------------------------------------------
int PlanarShadowEffect::GetMemoryUsed () const
{
    int iSize = sizeof(PlanarShadowEffect) - sizeof(Effect) +
        Effect::GetMemoryUsed();

    if ( m_iQuantity > 0 )
    {
        iSize += m_iQuantity*sizeof(m_akShadowColor[0]);
        iSize += m_iQuantity*sizeof(m_aspkPlane[0]);
        iSize += m_iQuantity*sizeof(m_aspkProjector[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
int PlanarShadowEffect::GetDiskUsed () const
{
    int iSize = Effect::GetDiskUsed() + sizeof(m_iQuantity);

    if ( m_iQuantity > 0 )
    {
        iSize += m_iQuantity*sizeof(m_akShadowColor[0]);
        iSize += m_iQuantity*sizeof(m_aspkPlane[0]);
        iSize += m_iQuantity*sizeof(m_aspkProjector[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
