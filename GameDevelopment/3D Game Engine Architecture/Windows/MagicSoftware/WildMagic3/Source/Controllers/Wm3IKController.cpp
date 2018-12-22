// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IKController.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,IKController,Controller);
WM3_IMPLEMENT_STREAM(IKController);

//----------------------------------------------------------------------------
IKController::IKController (int iJointQuantity, IKJointPtr* aspkJoint,
    int iGoalQuantity, IKGoalPtr* aspkGoal)
{
#ifdef _DEBUG
    assert( iJointQuantity > 0 && aspkJoint );
    assert( iGoalQuantity > 0 && aspkGoal );
    int i;
    for (i = 0; i < iJointQuantity; i++)
    {
        assert( aspkJoint[i] );
    }
    for (i = 0; i < iGoalQuantity; i++)
    {
        assert( aspkGoal[i] );
    }
#endif

    m_iJointQuantity = iJointQuantity;
    m_aspkJoint = aspkJoint;
    m_iGoalQuantity = iGoalQuantity;
    m_aspkGoal = aspkGoal;
    Iterations = 128;
    OrderEndToRoot = true;
}
//----------------------------------------------------------------------------
IKController::IKController ()
{
    m_iJointQuantity = 0;
    m_aspkJoint = 0;
    m_iGoalQuantity = 0;
    m_aspkGoal = 0;
    Iterations = 0;
    OrderEndToRoot = false;
}
//----------------------------------------------------------------------------
IKController::~IKController ()
{
    delete[] m_aspkJoint;
    delete[] m_aspkGoal;
}
//----------------------------------------------------------------------------
bool IKController::Update (double dAppTime)
{
    if ( !Controller::Update(dAppTime) )
        return false;

    // Make sure effectors are all current in world space.  It is assumed
    // that the joints form a chain, so the world transforms of joint I
    // are the parent transforms for the joint I+1.
    int iJoint;
    for (iJoint = 0; iJoint < m_iJointQuantity; iJoint++)
        m_aspkJoint[iJoint]->UpdateWorldSRT();

    // Update joints one-at-a-time to meet goals.  As each joint is updated,
    // the nodes occurring in the chain after that joint must be made current
    // in world space.
    int iIter, i, j;
    IKJoint* pkJoint;
    if ( OrderEndToRoot )
    {
        for (iIter = 0; iIter < Iterations; iIter++)
        {
            for (iJoint = 0; iJoint < m_iJointQuantity; iJoint++)
            {
                int iRJoint = m_iJointQuantity - 1 - iJoint;
                pkJoint = m_aspkJoint[iRJoint];

                for (i = 0; i < 3; i++)
                {
                    if ( pkJoint->AllowTranslation[i] )
                    {
                        if ( pkJoint->UpdateLocalT(i) )
                        {
                            for (j = iRJoint; j < m_iJointQuantity; j++)
                                m_aspkJoint[j]->UpdateWorldRT();
                        }
                    }
                }

                for (i = 0; i < 3; i++)
                {
                    if ( pkJoint->AllowRotation[i] )
                    {
                        if ( pkJoint->UpdateLocalR(i) )
                        {
                            for (j = iRJoint; j < m_iJointQuantity; j++)
                                m_aspkJoint[j]->UpdateWorldRT();
                        }
                    }
                }
            }
        }
    }
    else  // m_eOrder == PO_ROOT_TO_END
    {
        for (iIter = 0; iIter < Iterations; iIter++)
        {
            for (iJoint = 0; iJoint < m_iJointQuantity; iJoint++)
            {
                pkJoint = m_aspkJoint[iJoint];

                for (i = 0; i < 3; i++)
                {
                    if ( pkJoint->AllowTranslation[i] )
                    {
                        if ( pkJoint->UpdateLocalT(i) )
                        {
                            for (j = iJoint; j < m_iJointQuantity; j++)
                                m_aspkJoint[j]->UpdateWorldRT();
                        }
                    }
                }

                for (i = 0; i < 3; i++)
                {
                    if ( pkJoint->AllowRotation[i] )
                    {
                        if ( pkJoint->UpdateLocalR(i) )
                        {
                            for (j = iJoint; j < m_iJointQuantity; j++)
                                m_aspkJoint[j]->UpdateWorldRT();
                        }
                    }
                }
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* IKController::GetObjectByName (const String& rkName)
{
    Object* pkFound = Controller::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkFound = m_aspkJoint[i]->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        pkFound = m_aspkGoal[i]->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void IKController::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Controller::GetAllObjectsByName(rkName,rkObjects);

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
        m_aspkJoint[i]->GetAllObjectsByName(rkName,rkObjects);

    for (i = 0; i < m_iGoalQuantity; i++)
        m_aspkGoal[i]->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* IKController::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Controller::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkFound = m_aspkJoint[i]->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        pkFound = m_aspkGoal[i]->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void IKController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iJointQuantity);
    rkStream.Read(m_iGoalQuantity);
    rkStream.Read(Iterations);
    rkStream.Read(OrderEndToRoot);

    // link data
    Object* pkObject;
    int i;
    m_aspkJoint = new IKJointPtr[m_iJointQuantity];
    for (i = 0; i < m_iJointQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_aspkJoint[i]
        pkLink->Add(pkObject);
    }

    m_aspkGoal = new IKGoalPtr[m_iGoalQuantity];
    for (i = 0; i < m_iGoalQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_aspkGoal[i]
        pkLink->Add(pkObject);
    }

    WM3_END_DEBUG_STREAM_LOAD(IKController);
}
//----------------------------------------------------------------------------
void IKController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);

    Object* pkLinkID;
    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkJoint[i] = (IKJoint*)rkStream.GetFromMap(pkLinkID);
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkGoal[i] = (IKGoal*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool IKController::Register (Stream& rkStream) const
{
    if ( !Controller::Register(rkStream) )
        return false;

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
        m_aspkJoint[i]->Register(rkStream);

    for (i = 0; i < m_iGoalQuantity; i++)
        m_aspkGoal[i]->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void IKController::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(m_iJointQuantity);
    rkStream.Write(m_iGoalQuantity);
    rkStream.Write(Iterations);
    rkStream.Write(OrderEndToRoot);

    // link data
    int i;
    for (i = 0; i < m_iJointQuantity; i++)
        rkStream.Write(m_aspkJoint[i]);

    for (i = 0; i < m_iGoalQuantity; i++)
        rkStream.Write(m_aspkGoal[i]);

    WM3_END_DEBUG_STREAM_SAVE(IKController);
}
//----------------------------------------------------------------------------
StringTree* IKController::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(5,3);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("joint quantity =",
        m_iJointQuantity));
    pkTree->SetString(2,StringTree::Format("goal quantity =",
        m_iGoalQuantity));
    pkTree->SetString(3,StringTree::Format("iterations =",Iterations));
    pkTree->SetString(4,StringTree::Format("end to root =",OrderEndToRoot));

    // children
    pkTree->SetChild(0,Controller::SaveStrings());

    // joints
    StringTree* pkJTree = new StringTree(1,m_iJointQuantity);
    pkJTree->SetString(0,StringTree::Format("joints"));
    int i;
    for (i = 0; i < m_iJointQuantity; i++)
        pkJTree->SetChild(i,m_aspkJoint[i]->SaveStrings());

    pkTree->SetChild(1,pkJTree);

    // goals
    StringTree* pkGTree = new StringTree(1,m_iGoalQuantity);
    pkGTree->SetString(0,StringTree::Format("goals"));
    for (i = 0; i < m_iGoalQuantity; i++)
        pkGTree->SetChild(i,m_aspkGoal[i]->SaveStrings());

    pkTree->SetChild(2,pkGTree);

    return pkTree;
}
//----------------------------------------------------------------------------
int IKController::GetMemoryUsed () const
{
    return sizeof(IKController) - sizeof(Controller) +
        Controller::GetMemoryUsed() +
        m_iJointQuantity*sizeof(m_aspkJoint[0]) +
        m_iGoalQuantity*sizeof(m_aspkGoal[0]);
}
//----------------------------------------------------------------------------
int IKController::GetDiskUsed () const
{
    return Controller::GetDiskUsed() +
        sizeof(m_iJointQuantity) +
        sizeof(m_iGoalQuantity) +
        sizeof(Iterations) +
        sizeof(OrderEndToRoot) +
        m_iJointQuantity*sizeof(m_aspkJoint[0]) +
        m_iGoalQuantity*sizeof(m_aspkGoal[0]);
}
//----------------------------------------------------------------------------
