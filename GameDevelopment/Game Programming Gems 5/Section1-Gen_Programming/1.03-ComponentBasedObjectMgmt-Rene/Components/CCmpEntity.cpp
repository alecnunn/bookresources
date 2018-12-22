#include "CCmpEntity.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "ICmpDescription.h"
#include "ICmpCollectable.h"

CCmpEntity::CCmpEntity()
{
}

CCmpEntity::~CCmpEntity()
{
}

void CCmpEntity::RegisterComponentType()
{
	ICmpEntity::RegisterInterface(CID_ENTITY);
	Globals::GetObjectManager().RegisterComponentType(CID_ENTITY, CCmpEntity::CreateMe, CCmpEntity::DestroyMe, CHash("Entity"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_ENTITY, MT_OBJECT_CREATED);
	Globals::GetObjectManager().SubscribeToMessageType(CID_ENTITY, MT_HIDE);
	Globals::GetObjectManager().SubscribeToMessageType(CID_ENTITY, MT_SHOW);
}

IComponent *CCmpEntity::CreateMe()
{
	return new CCmpEntity;
}

bool CCmpEntity::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpEntity::Init(CObjectId oid, CParameterNode &paramNode)
{
	// Get Position
	mRoom = paramNode.GetHash("Room");

	mVisible = paramNode.GetBool("Visible");
	if (paramNode.GetInt() == EPR_FIELD_NOT_FOUND)
	{
		mVisible = true;
	}
	int i = 0;
	char nodeName[64];
	// Read in interaction names
	while (true)
	{
		sprintf(nodeName, "InteractionName%d", i);
		CHash interactionName = paramNode.GetHash(nodeName);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;
		AddInteractionName(interactionName);
		++i;
	}

	return true;
}

void CCmpEntity::Deinit(void)
{
	mInteractionNameSet.clear();
}

EMessageResult CCmpEntity::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_OBJECT_CREATED:
		{
			// Add the object's short name as an interaction name
			ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
			if (pDesc && !IsInteractionName(pDesc->GetShortDescription()))
			{
				AddInteractionName(pDesc->GetShortDescription());
			}
			return MR_TRUE;
		}
		case MT_HIDE:
		{
			SetVisible(false);
			return MR_TRUE;
		}
		case MT_SHOW:
		{
			SetVisible(true);
			return MR_TRUE;
		}
	}
	return MR_IGNORED;
}

EComponentTypeId CCmpEntity::GetComponentTypeId(void)
{
	return CID_ENTITY;
}

void CCmpEntity::SetPosition(CHash room)
{
	mRoom = room;
}

CHash CCmpEntity::GetPosition() const
{
	return mRoom;
}

bool CCmpEntity::GetVisible() const
{
	return mVisible;
}

void CCmpEntity::SetVisible(bool visible)
{
	mVisible = visible;
}

bool CCmpEntity::AddInteractionName(CHash name)
{
	return mInteractionNameSet.insert(name).second;
}

bool CCmpEntity::IsInteractionName(CHash name) const
{
	InteractionNameSet::const_iterator iter = mInteractionNameSet.find(name);
	if (iter == mInteractionNameSet.end())
	{
		return false;
	}
	return true;
}

bool CCmpEntity::CanThisObjectBeSeenBy(CObjectId viewer, bool includeInventoryObject) const
{
	ICmpCollectable *pCollectable = static_cast<ICmpCollectable *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_COLLECTABLE));
	if (pCollectable == NULL || !pCollectable->IsCollected())
	{ // The object is not collectable or it is collectable but not collected. Check if it's in the same room as the viewer.
		if (GetVisible() == false)
			return false; // Can't be seen by anyone

		ICmpEntity *pViewerEntity = static_cast<ICmpEntity *>(Globals::GetObjectManager().QueryInterface(viewer, IID_ENTITY));
		if (pViewerEntity == NULL)
		{
			return false;
		} 
		if (GetPosition() == pViewerEntity->GetPosition())
		{
			return true;
		}
	}
	if (pCollectable && pCollectable->IsCollected())
	{
		if (pCollectable->GetHolder() == viewer)
		{ // This object is collected by the viewer. We're fine
			return includeInventoryObject;
		}
	}
	return false;
}


