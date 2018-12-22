#include "CCmpPuzzleLogic.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "ICmpCollectable.h"
#include "ICmpDescription.h"
#include "ICmpEntity.h"
#include "ComponentStructs.h"
#include <stdio.h>

CCmpPuzzleLogic::CCmpPuzzleLogic()
{
}

CCmpPuzzleLogic::~CCmpPuzzleLogic()
{
}

void CCmpPuzzleLogic::RegisterComponentType()
{
	ICmpPuzzleLogic::RegisterInterface(CID_PUZZLE_LOGIC);
	Globals::GetObjectManager().RegisterComponentType(CID_PUZZLE_LOGIC, CCmpPuzzleLogic::CreateMe, CCmpPuzzleLogic::DestroyMe, CHash("PuzzleLogic"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_PUZZLE_LOGIC, MT_DESCRIBE_OBJECT);
	Globals::GetObjectManager().SubscribeToMessageType(CID_PUZZLE_LOGIC, MT_OBJECT_CREATED);
	Globals::GetObjectManager().SubscribeToMessageType(CID_PUZZLE_LOGIC, MT_USE);
}

IComponent *CCmpPuzzleLogic::CreateMe()
{
	return new CCmpPuzzleLogic;
}

bool CCmpPuzzleLogic::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpPuzzleLogic::Init(CObjectId oid, CParameterNode &paramNode)
{
	mCurrentState = paramNode.GetHash("InitialState");

	int i = 0;
	char nodeName[64];
	// Read in interaction names
	while (true)
	{
		sprintf(nodeName, "FromState%d", i);
		CHash fromState = paramNode.GetHash(nodeName);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;
		sprintf(nodeName, "ToState%d", i);
		CHash toState = paramNode.GetHash(nodeName);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;
		sprintf(nodeName, "RequiredObject%d", i);
		CObjectId requiredObject = paramNode.GetHash(nodeName);

		AddStateTransition(fromState, toState, requiredObject);
		++i;
	}

	i = 0;
	// Read in the info for each state
	while (true)
	{
		CHash stateName;
		sprintf(nodeName, "State%d", i);
		stateName = paramNode.GetHash(nodeName);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;

		StateInfo stateInfo;
		sprintf(nodeName, "ShortDescState%d", i);
		strncpy(stateInfo.mShortDesc, paramNode.GetString(nodeName), MAX_STR_LEN);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;
		
		sprintf(nodeName, "LongDescState%d", i);
		strncpy(stateInfo.mLongDesc, paramNode.GetString(nodeName), MAX_STR_LEN);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;

		sprintf(nodeName, "ChangeToState%d", i);
		strncpy(stateInfo.mChangeToStateString, paramNode.GetString(nodeName), MAX_STR_LEN);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;

		AddStateInfo(stateName, stateInfo);
		++i;
	}
	return true;
}

void CCmpPuzzleLogic::Deinit(void)
{
	mStateInfoMap.clear();
	mStateTransitionMap.clear();
}

EMessageResult CCmpPuzzleLogic::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_OBJECT_CREATED:
		{
			SetDescriptionForState(mCurrentState);
			return MR_TRUE;
		}
		case MT_DESCRIBE_OBJECT:
		{
			CHash containingObject = *static_cast<CHash *>(msg.mpData);
			DescribeObject(containingObject);
			return MR_TRUE;
		}
		case MT_USE:
		{
			UseInfo *pUseInfo = static_cast<UseInfo *>(msg.mpData);
			if (pUseInfo == NULL)
				return MR_ERROR;
			if (GetEntity()->IsInteractionName(pUseInfo->useObjectInteractionName))
			{ // The user is talking to me. Check if this object can be used by the user
				if (GetEntity()->CanThisObjectBeSeenBy(pUseInfo->userId, true))
				{
					if (pUseInfo->useWithInteractionName.IsValid())
					{
						UseWith(pUseInfo->userId, pUseInfo->useWithInteractionName);
					}
					else
					{
						Use();
					}
				}
				else
				{
					ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
					if (pDesc)
					{
						Globals::PrintWithType(TTResponse, "You can not see the %s from here\n", pDesc->GetShortDescription());
					}
					else
					{
						Globals::PrintWithType(TTResponse, "You can not see that object from here\n");
					}
				}
			}
			return MR_TRUE;
		}
	}
	return MR_IGNORED;
}

void CCmpPuzzleLogic::DescribeObject(CObjectId viewer) const
{
	ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
	if (pDesc == NULL)
	{ // We have nothing to describe
		return;
	}
	if (GetEntity()->CanThisObjectBeSeenBy(viewer, false))
	{
		Globals::PrintWithType(TTObject, "%s\n", pDesc->GetShortDescription());
	}
}

EComponentTypeId CCmpPuzzleLogic::GetComponentTypeId(void)
{
	return CID_PUZZLE_LOGIC;
}

CHash CCmpPuzzleLogic::GetState()
{
	return mCurrentState;
}

bool CCmpPuzzleLogic::Use()
{
	StateTransitionMap::iterator iter = mStateTransitionMap.find(mCurrentState);
	if (iter == mStateTransitionMap.end())
		return false; // There is no transition out of this state
	CHash targetState = (*iter).second.mEndState;
	if ((*iter).second.mNeededObject.IsValid())
		return false; // We need an object to get to the next state
	return SetState(targetState, false);
}

bool CCmpPuzzleLogic::SetState(CHash targetState, bool silent)
{
	if (!targetState.IsValid())
	{
		return false;
	}
	
	if (!SetDescriptionForState(targetState))
	{
		return false;
	}
	if (!silent)
	{
		StateInfo *pInfo = GetStateInfo(targetState);
		if (pInfo == NULL)
		{
			return false;
		}
		Globals::PrintWithType(TTStateChange, "%s\n", pInfo->mChangeToStateString);
	}
	mCurrentState = targetState;
	EventInfo evInfo(CHash("StateChange"), GetObjectId());
	Globals::GetObjectManager().BroadcastMessage(CMessage(MT_EVENT, &evInfo));
	return true;
}

bool CCmpPuzzleLogic::UseWith(CObjectId userId, CHash object)
{
	StateTransitionMap::iterator iter = mStateTransitionMap.find(mCurrentState);
	if (iter == mStateTransitionMap.end())
		return false; // There is no transition out of this state
	StateTransitionInfo transitionInfo = (*iter).second;

	ICmpEntity *pNeededObjectEntity = static_cast<ICmpEntity *>(Globals::GetObjectManager().QueryInterface(transitionInfo.mNeededObject, IID_ENTITY));
	if (pNeededObjectEntity->IsInteractionName(object) && pNeededObjectEntity->CanThisObjectBeSeenBy(userId, true))
	{
		return SetState(transitionInfo.mEndState, false);
	}
	return false;
}


bool CCmpPuzzleLogic::AddStateTransition(CHash fromState, CHash toState, CObjectId neededObject)
{
	StateTransitionInfo transitionInfo;
	transitionInfo.mEndState = toState;
	transitionInfo.mNeededObject = neededObject;
	return mStateTransitionMap.insert(StateTransitionMap::value_type(fromState, transitionInfo)).second;
}

bool CCmpPuzzleLogic::AddStateInfo(CHash state, StateInfo &stateInfo)
{
	return mStateInfoMap.insert(StateInfoMap::value_type(state, stateInfo)).second;
}

bool CCmpPuzzleLogic::SetDescriptionForState(CHash stateName)
{
	ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
	if (pDesc == NULL)
	{
		return false;
	}

	StateInfo *pInfo = GetStateInfo(stateName);
	if (pInfo == NULL)
	{
		return false;
	}

	pDesc->SetShortDescription(pInfo->mShortDesc);
	pDesc->SetLongDescription(pInfo->mLongDesc);

	return true;
}

CCmpPuzzleLogic::StateInfo *CCmpPuzzleLogic::GetStateInfo(CHash state)
{
	StateInfoMap::iterator iter = mStateInfoMap.find(state);
	if (iter != mStateInfoMap.end())
	{
		return &((*iter).second);
	}
	return NULL;
}