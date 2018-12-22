// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Spatial.h"
#include "Wm3Camera.h"
#include "Wm3Light.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Spatial,Object);
WM3_IMPLEMENT_ABSTRACT_STREAM(Spatial);

//----------------------------------------------------------------------------
Spatial::Spatial ()
    :
    WorldBound(BoundingVolume::Create())
{
    ForceCull = false;
    WorldIsCurrent = false;
    WorldBoundIsCurrent = false;
    m_pkParent = 0;
    m_pkGlobalList = 0;
    m_pkLightList = 0;
}
//----------------------------------------------------------------------------
Spatial::~Spatial ()
{
    RemoveAllGlobalStates();
    RemoveAllLights();
}
//----------------------------------------------------------------------------
void Spatial::UpdateGS (double dAppTime, bool bInitiator)
{
    UpdateWorldData(dAppTime);
    UpdateWorldBound();
    if ( bInitiator )
        PropagateBoundToRoot();
}
//----------------------------------------------------------------------------
void Spatial::UpdateBS ()
{
    UpdateWorldBound();
    PropagateBoundToRoot();
}
//----------------------------------------------------------------------------
void Spatial::UpdateWorldData (double dAppTime)
{
    // update any controllers associated with this object
    UpdateControllers(dAppTime);

    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        pkGList->Item()->UpdateControllers(dAppTime);

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        pkLList->Item()->UpdateControllers(dAppTime);

    // update world transforms
    if ( !WorldIsCurrent )
    {
        if ( m_pkParent )
            World.Product(m_pkParent->World,Local);
        else
            World = Local;
    }
}
//----------------------------------------------------------------------------
void Spatial::PropagateBoundToRoot ()
{
    if ( m_pkParent )
    {
        m_pkParent->UpdateWorldBound();
        m_pkParent->PropagateBoundToRoot();
    }
}
//----------------------------------------------------------------------------
void Spatial::SetGlobalState (GlobalState* pkState)
{
    assert( pkState );

    // check if type of state already exists
    TList<GlobalStatePtr>* pkList;
    for (pkList = m_pkGlobalList; pkList; pkList = pkList->Next())
    {
        if ( pkList->Item()->GetGlobalStateType() ==
             pkState->GetGlobalStateType() )
        {
            // type of state exists, replace it
            pkList->Item() = pkState;
            return;
        }
    }

    // type of state not in current list, add state
    pkList = new TList<GlobalStatePtr>;
    pkList->Item() = pkState;
    pkList->Next() = m_pkGlobalList;
    m_pkGlobalList = pkList;
}
//----------------------------------------------------------------------------
GlobalState* Spatial::GetGlobalState (int eType) const
{
    // check if type of state already exists
    TList<GlobalStatePtr>* pkList;
    for (pkList = m_pkGlobalList; pkList; pkList = pkList->Next())
    {
        if ( pkList->Item()->GetGlobalStateType() == eType )
        {
            // type of state exists, return it
            return pkList->Item();
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
void Spatial::RemoveGlobalState (int eType)
{
    // check if type of state already exists
    TList<GlobalStatePtr>* pkList = m_pkGlobalList;
    TList<GlobalStatePtr>* pkPrev = 0;
    for (/**/; pkList; pkPrev = pkList, pkList = pkList->Next())
    {
        if (  pkList->Item()->GetGlobalStateType() == eType )
        {
            // type of state exists, remove it
            if ( pkPrev )
            {
                // render state not at front of list
                pkPrev->Next() = pkList->Next();
            }
            else
            {
                // render state at front of list
                assert( pkList == m_pkGlobalList );
                m_pkGlobalList = pkList->Next();
            }
            pkList->Next() = 0;
            delete pkList;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::RemoveAllGlobalStates ()
{
    while ( m_pkGlobalList )
    {
        m_pkGlobalList->Item() = 0;
        TList<GlobalStatePtr>* pkSave = m_pkGlobalList->Next();
        delete m_pkGlobalList;
        m_pkGlobalList = pkSave;
    }
}
//----------------------------------------------------------------------------
void Spatial::SetLight (Light* pkLight)
{
    assert( pkLight );

    // check if light is already in the list
    TList<LightPtr>* pkList;
    for (pkList = m_pkLightList; pkList; pkList = pkList->Next())
    {
        if ( pkList->Item() == pkLight )
        {
            // light already exists, nothing to do
            return;
        }
    }

    // light not in current list, add it
    pkList = new TList<LightPtr>;
    pkList->Item() = pkLight;
    pkList->Next() = m_pkLightList;
    m_pkLightList = pkList;
}
//----------------------------------------------------------------------------
int Spatial::GetLightQuantity () const
{
    int iLightQuantity = 0;

    TList<LightPtr>* pkList;
    for (pkList = m_pkLightList; pkList; pkList = pkList->Next())
        iLightQuantity++;

    return iLightQuantity;
}
//----------------------------------------------------------------------------
Light* Spatial::GetLight (int i) const
{
    assert( i >= 0 );

    TList<LightPtr>* pkList = m_pkLightList;
    for (int j = 0; j < i; j++, pkList = pkList->Next())
    {
        if ( !pkList )
        {
            // i >= iLightQuantity
            return 0;
        }
    }
    return ( pkList ? pkList->Item() : 0 );
}
//----------------------------------------------------------------------------
void Spatial::RemoveLight (Light* pkLight)
{
    // check if light is in list
    TList<LightPtr>* pkList = m_pkLightList;
    TList<LightPtr>* pkPrev = 0;
    for (/**/; pkList; pkPrev = pkList, pkList = pkList->Next())
    {
        if ( pkList->Item() == pkLight )
        {
            // light exists, remove it
            if ( pkPrev )
            {
                // light not at front of list
                pkPrev->Next() = pkList->Next();
            }
            else
            {
                // light at front of list
                assert( pkList == m_pkLightList );
                m_pkLightList = pkList->Next();
            }
            pkList->Next() = 0;
            delete pkList;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::RemoveAllLights ()
{
    while ( m_pkLightList )
    {
        m_pkLightList->Item() = 0;
        TList<LightPtr>* pkSave = m_pkLightList->Next();
        delete m_pkLightList;
        m_pkLightList = pkSave;
    }
}
//----------------------------------------------------------------------------
void Spatial::UpdateRS (TStack<GlobalState*>* akGStack,
    TStack<Light*>* pkLStack)
{
    bool bInitiator = (akGStack == 0);

    if ( bInitiator )
    {
        // stack initialized to contain the default global states
        akGStack = new TStack<GlobalState*>[GlobalState::MAX_STATE];
        for (int i = 0; i < GlobalState::MAX_STATE; i++)
            akGStack[i].Push(GlobalState::Default[i]);

        // stack has no lights initially
        pkLStack = new TStack<Light*>;

        // traverse to root and push states from root to this node
        PropagateStateFromRoot(akGStack,pkLStack);
    }
    else
    {
        // push states at this node
        PushState(akGStack,pkLStack);
    }

    // propagate the new state to the subtree rooted here
    UpdateState(akGStack,pkLStack);

    if ( bInitiator )
    {
        delete[] akGStack;
        delete pkLStack;
    }
    else
    {
        // pop states at this node
        PopState(akGStack,pkLStack);
    }
}
//----------------------------------------------------------------------------
void Spatial::PropagateStateFromRoot (TStack<GlobalState*>* akGStack,
    TStack<Light*>* pkLStack)
{
    // traverse to root to allow downward state propagation
    if ( m_pkParent )
        m_pkParent->PropagateStateFromRoot(akGStack,pkLStack);

    // push states onto current render state stack
    PushState(akGStack,pkLStack);
}
//----------------------------------------------------------------------------
void Spatial::PushState (TStack<GlobalState*>* akGStack,
    TStack<Light*>* pkLStack)
{
    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        int eType = pkGList->Item()->GetGlobalStateType();
        akGStack[eType].Push(pkGList->Item());
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        pkLStack->Push(pkLList->Item());
}
//----------------------------------------------------------------------------
void Spatial::PopState (TStack<GlobalState*>* akGStack,
    TStack<Light*>* pkLStack)
{
    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        int eType = pkGList->Item()->GetGlobalStateType();
        GlobalState* pkDummy;
        akGStack[eType].Pop(pkDummy);
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
    {
        Light* pkDummy;
        pkLStack->Pop(pkDummy);
    }
}
//----------------------------------------------------------------------------
void Spatial::OnDraw (Renderer& rkRenderer, bool bNoCull)
{
    if ( ForceCull )
        return;

    CameraPtr spkCamera = rkRenderer.GetCamera();
    unsigned int uiState = spkCamera->GetPlaneState();

    if ( bNoCull || !spkCamera->Culled(WorldBound) )
        Draw(rkRenderer,bNoCull);

    spkCamera->SetPlaneState(uiState);
}
//----------------------------------------------------------------------------
Spatial::PickRecord::PickRecord (Spatial* pkIObject, float fT)
    :
    IObject(pkIObject)
{
    T = fT;
}
//----------------------------------------------------------------------------
Spatial::PickRecord::~PickRecord ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
void Spatial::DoPick (const Ray3f&, PickArray&)
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
Spatial::PickRecord* Spatial::GetClosest (PickArray& rkResults)
{
    if ( rkResults.GetQuantity() == 0 )
        return 0;

    float fTMin = rkResults[0]->T;
    int iMin = 0;
    for (int i = 1; i < rkResults.GetQuantity(); i++)
    {
        float fT = rkResults[i]->T;
        if ( fT < fTMin )
        {
            fTMin = fT;
            iMin = i;
        }
    }
    return rkResults[iMin];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Spatial::GetObjectByName (const String& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( WorldBound )
    {
        pkFound = WorldBound->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        GlobalState* pkGState = pkGList->Item();
        if ( pkGState )
        {
            pkFound = pkGState->GetObjectByName(rkName);
            if ( pkFound )
                return pkFound;
        }
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
    {
        Light* pkLight = pkLList->Item();
        if ( pkLight )
        {
            pkFound = pkLight->GetObjectByName(rkName);
            if ( pkFound )
                return pkFound;
        }
    }

    if ( m_spkEffect )
    {
        pkFound = m_spkEffect->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    // To avoid cycles in a recursive search of a scene graph for an object
    // with the specified name, the member m_pkParent is not checked.
    return 0;
}
//----------------------------------------------------------------------------
void Spatial::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if ( WorldBound )
        WorldBound->GetAllObjectsByName(rkName,rkObjects);

    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        GlobalState* pkGState = pkGList->Item();
        if ( pkGState )
            pkGState->GetAllObjectsByName(rkName,rkObjects);
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
    {
        Light* pkLight = pkLList->Item();
        if ( pkLight )
            pkLight->GetAllObjectsByName(rkName,rkObjects);
    }

    if ( m_spkEffect )
        m_spkEffect->GetAllObjectsByName(rkName,rkObjects);

    // To avoid cycles in a recursive search of a scene graph for an object
    // with the specified name, the member m_pkParent is not checked.
}
//----------------------------------------------------------------------------
Object* Spatial::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( WorldBound )
    {
        pkFound = WorldBound->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        GlobalState* pkGState = pkGList->Item();
        if ( pkGState )
        {
            pkFound = pkGState->GetObjectByID(uiID);
            if ( pkFound )
                return pkFound;
        }
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
    {
        Light* pkLight = pkLList->Item();
        if ( pkLight )
        {
            pkFound = pkLight->GetObjectByID(uiID);
            if ( pkFound )
                return pkFound;
        }
    }

    if ( m_spkEffect )
    {
        pkFound = m_spkEffect->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    // To avoid cycles in a recursive search of a scene graph for an object
    // with the specified id, the member m_pkParent is not checked.
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Spatial::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Local);
    rkStream.Read(World);
    rkStream.Read(ForceCull);
    rkStream.Read(WorldIsCurrent);
    rkStream.Read(WorldBoundIsCurrent);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkWorldBound
    pkLink->Add(pkObject);

    int i, iQuantity;
    rkStream.Read(iQuantity);
    m_pkGlobalList = 0;
    for (i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);
        pkLink->Add((GlobalState*)pkObject);

        // build global state list, to be filled in by Link
        TList<GlobalStatePtr>* pkList = new TList<GlobalStatePtr>;
        pkList->Item() = 0;
        pkList->Next() = m_pkGlobalList;
        m_pkGlobalList = pkList;
    }

    rkStream.Read(iQuantity);
    m_pkLightList = 0;
    for (i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);
        pkLink->Add((Light*)pkObject);

        // build light list, to be filled in by Link
        TList<LightPtr>* pkList = new TList<LightPtr>;
        pkList->Item() = 0;
        pkList->Next() = m_pkLightList;
        m_pkLightList = pkList;
    }

    rkStream.Read(pkObject);  // m_spkEffect
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(Spatial);
}
//----------------------------------------------------------------------------
void Spatial::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    WorldBound = (BoundingVolume*)rkStream.GetFromMap(pkLinkID);

    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        pkLinkID = pkLink->GetLinkID();
        pkGList->Item() = (GlobalState*)rkStream.GetFromMap(pkLinkID);
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
    {
        pkLinkID = pkLink->GetLinkID();
        pkLList->Item() = (Light*)rkStream.GetFromMap(pkLinkID);
    }

    pkLinkID = pkLink->GetLinkID();
    m_spkEffect = (Effect*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Spatial::Register (Stream& rkStream) const
{
    if ( !Object::Register(rkStream) )
        return false;

    if ( WorldBound )
        WorldBound->Register(rkStream);

    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
    {
        GlobalState* pkGState = pkGList->Item();
        if ( pkGState )
            pkGState->Register(rkStream);
    }

    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
    {
        Light* pkLight = pkLList->Item();
        if ( pkLight )
            pkLight->Register(rkStream);
    }

    if ( m_spkEffect )
        m_spkEffect->Register(rkStream);

    // m_pkParent need not be registered since the parent itself must have
    // initiated the Register call to its children, 'this' being one of them.
    return true;
}
//----------------------------------------------------------------------------
void Spatial::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(Local);
    rkStream.Write(World);
    rkStream.Write(ForceCull);
    rkStream.Write(WorldIsCurrent);
    rkStream.Write(WorldBoundIsCurrent);

    // link data
    rkStream.Write(WorldBound);

    int iQuantity = 0;
    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        iQuantity++;

    rkStream.Write(iQuantity);

    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        rkStream.Write(pkGList->Item());

    iQuantity = 0;
    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        iQuantity++;

    rkStream.Write(iQuantity);

    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        rkStream.Write(pkLList->Item());

    rkStream.Write(m_spkEffect);

    // m_pkParent need not be saved since 'this' will be attached as a
    // child in Node::Link.

    WM3_END_DEBUG_STREAM_SAVE(Spatial);
}
//----------------------------------------------------------------------------
StringTree* Spatial::SaveStrings (const char*)
{
    int iCQuantity = 1;

    // count the global render states
    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        iCQuantity++;

    // count the lights
    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        iCQuantity++;

    // count the effect
    if ( m_spkEffect )
        iCQuantity++;

    StringTree* pkTree = new StringTree(9,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("local rotate =",
        Local.GetRotate()));
    pkTree->SetString(2,StringTree::Format("local trans =",
        Local.GetTranslate()));
    pkTree->SetString(3,StringTree::Format("local scale =",
        Local.GetScale()));
    pkTree->SetString(4,StringTree::Format("world rotate =",
        World.GetRotate()));
    pkTree->SetString(5,StringTree::Format("world trans =",
        World.GetTranslate()));
    pkTree->SetString(6,StringTree::Format("world scale =",
        World.GetScale()));
    pkTree->SetString(7,StringTree::Format("world bound =",WorldBound));
    pkTree->SetString(8,StringTree::Format("force cull =",ForceCull));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    // global render states
    iCQuantity = 1;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        pkTree->SetChild(iCQuantity++,pkGList->Item()->SaveStrings());

    // lights
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        pkTree->SetChild(iCQuantity++,pkLList->Item()->SaveStrings());

    // effect
    if ( m_spkEffect )
        pkTree->SetChild(iCQuantity,m_spkEffect->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Spatial::GetMemoryUsed () const
{
    int iBaseSize = sizeof(Spatial) - sizeof(Object);
    int iDynaSize = 0;

    int iQuantity = 0;
    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        iQuantity++;
    iDynaSize += iQuantity*sizeof(TList<GlobalStatePtr>);

    iQuantity = 0;
    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        iQuantity++;
    iDynaSize += iQuantity*sizeof(TList<LightPtr>);

    int iTotalSize = iBaseSize + iDynaSize + Object::GetMemoryUsed();
    return iTotalSize;
}
//----------------------------------------------------------------------------
int Spatial::GetDiskUsed () const
{
    int iSize = Object::GetDiskUsed() +
        Transformation::DISK_USED +  // Local
        Transformation::DISK_USED +  // World
        sizeof(char) + // ForceCull
        sizeof(char) + // WorldIsCurrent
        sizeof(char) + // WorldBoundIsCurrent
        sizeof(WorldBound) +
        sizeof(m_spkEffect);

    int iQuantity = 0;
    TList<GlobalStatePtr>* pkGList;
    for (pkGList = m_pkGlobalList; pkGList; pkGList = pkGList->Next())
        iQuantity++;
    iSize += sizeof(int) + iQuantity*sizeof(GlobalStatePtr);

    iQuantity = 0;
    TList<LightPtr>* pkLList;
    for (pkLList = m_pkLightList; pkLList; pkLList = pkLList->Next())
        iQuantity++;
    iSize += sizeof(int) + iQuantity*sizeof(LightPtr);

    return iSize;
}
//----------------------------------------------------------------------------
