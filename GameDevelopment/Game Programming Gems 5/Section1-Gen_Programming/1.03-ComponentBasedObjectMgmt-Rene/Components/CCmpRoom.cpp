#include "CCmpRoom.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "ICmpDescription.h"
#include "ICmpEntity.h"

CCmpRoom::CCmpRoom()
{
}

CCmpRoom::~CCmpRoom()
{
}

void CCmpRoom::RegisterComponentType()
{
	ICmpRoom::RegisterInterface(CID_ROOM);
	Globals::GetObjectManager().RegisterComponentType(CID_ROOM, CCmpRoom::CreateMe, CCmpRoom::DestroyMe, CHash("Room"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_ROOM, MT_LOOK);
}

IComponent *CCmpRoom::CreateMe()
{
	return new CCmpRoom;
}

bool CCmpRoom::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpRoom::Init(CObjectId oid, CParameterNode &paramNode)
{
	char nodeName[64];
	int i = 0;
	// Read in room connections
	while (true)
	{
		sprintf(nodeName, "ConnectedRoom%d", i);
		CObjectId connectedRoom = paramNode.GetHash(nodeName);
		if (paramNode.GetLastResult() == EPR_FIELD_NOT_FOUND)
			break;
		AddConnectedRoom(connectedRoom);
		++i;
	}

	return true;
}

void CCmpRoom::Deinit(void)
{
	mConnectedRoomsSet.clear();
}

EMessageResult CCmpRoom::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_LOOK:
		{
			PrintLookDescription();
			break;
		}
	}
	return MR_IGNORED;
}

EComponentTypeId CCmpRoom::GetComponentTypeId(void)
{
	return CID_ROOM;
}

bool CCmpRoom::AddConnectedRoom(CObjectId room)
{
	return mConnectedRoomsSet.insert(room).second;
}

bool CCmpRoom::IsRoomConnected(CObjectId room) const
{
	ConnectedRoomSet::const_iterator iter = mConnectedRoomsSet.find(room);
	if (iter == mConnectedRoomsSet.end())
	{
		return false;
	}
	return true;
}

CObjectId CCmpRoom::GetConnectedRoom(CHash interactionName) const
{
	ConnectedRoomSet::const_iterator iter;

	for (iter = mConnectedRoomsSet.begin() ; iter != mConnectedRoomsSet.end() ; ++iter)
	{
//		ICmpRoom *pConnectedRoom = static_cast<ICmpRoom *>(Globals::GetObjectManager().QueryInterface((*iter), IID_ROOM));
		ICmpEntity *pConnectedRoomEntity = static_cast<ICmpEntity *>(Globals::GetObjectManager().QueryInterface((*iter), IID_ENTITY));
		if (pConnectedRoomEntity && pConnectedRoomEntity->IsInteractionName(interactionName))
		{
			return pConnectedRoomEntity->GetObjectId();
		}
	}
	CObjectId invalidId;
	return invalidId;
}

void CCmpRoom::PrintLookDescription()
{
	// Description
	ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
	if (pDesc)
	{
		Globals::PrintWithType(TTShortDesc, "\n%s\n", pDesc->GetShortDescription());
		Globals::PrintWithType(TTRoomDesc, "%s\n", pDesc->GetLongDescription());
	}

	// Characters
	Globals::PrintWithType(TTCharacterHeading, "Characters:\n");
	CMessage describeCharacterMsg;
	describeCharacterMsg.mType = MT_DESCRIBE_CHARACTER;
	CHash containingObjectHash(GetObjectId());
	describeCharacterMsg.mpData = &containingObjectHash;
	Globals::GetObjectManager().BroadcastMessage(describeCharacterMsg);

	// Items
	Globals::PrintWithType(TTObjectHeading, "Items:\n");
	CMessage describeObjectMsg;
	describeObjectMsg.mType = MT_DESCRIBE_OBJECT;
	describeObjectMsg.mpData = &containingObjectHash;
	Globals::GetObjectManager().BroadcastMessage(describeObjectMsg);

	// Exits
	ConnectedRoomSet::const_iterator iter = mConnectedRoomsSet.begin();
	if (iter == mConnectedRoomsSet.end())
	{
		Globals::PrintWithType(TTExit, "There are no exits from this room.\n");
	}
	else
	{
		Globals::PrintWithType(TTExitHeading, "Exits:\n");
		for (iter = mConnectedRoomsSet.begin() ; iter != mConnectedRoomsSet.end() ; ++iter)
		{
			ICmpDescription *pConnectedRoomDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface((*iter), IID_DESCRIPTION));
			if (pConnectedRoomDesc)
			{
				Globals::PrintWithType(TTExit, "%s\n", pConnectedRoomDesc->GetShortDescription());
			}
		}
	}
}
