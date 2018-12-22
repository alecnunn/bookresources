#include "CCmpPlayer.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "string.h"
#include "ICmpRoom.h"
#include "ICmpEntity.h"
#include "CMessage.h"
#include "ComponentStructs.h"
#include <stdio.h>
#include "CTextAdventureEngine.h"

CCmpPlayer::CCmpPlayer()
{
}

CCmpPlayer::~CCmpPlayer()
{
}

void CCmpPlayer::RegisterComponentType()
{
	ICmpPlayer::RegisterInterface(CID_PLAYER);
	Globals::GetObjectManager().RegisterComponentType(CID_PLAYER, CCmpPlayer::CreateMe, CCmpPlayer::DestroyMe, CHash("Player"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_PLAYER, MT_OBJECT_CREATED);
	Globals::GetObjectManager().SubscribeToMessageType(CID_PLAYER, MT_ALL_OBJECTS_CREATED);
	Globals::GetObjectManager().SubscribeToMessageType(CID_PLAYER, MT_COMMAND);
	Globals::GetObjectManager().SubscribeToMessageType(CID_PLAYER, MT_EVENT);
	Globals::GetObjectManager().SubscribeToMessageType(CID_PLAYER, MT_TELL_ROOM);
}

IComponent *CCmpPlayer::CreateMe()
{
	return new CCmpPlayer;
}

bool CCmpPlayer::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpPlayer::Init(CObjectId oid, CParameterNode &paramNode)
{

	return true;
}

void CCmpPlayer::Deinit(void)
{
}

EMessageResult CCmpPlayer::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_OBJECT_CREATED:
		{
			
			return MR_TRUE;
		}
		case MT_ALL_OBJECTS_CREATED:
		{
			Globals::GetObjectManager().PostMessage(GetEntity()->GetPosition(), MT_LOOK);
			return MR_TRUE;
		}
		case MT_COMMAND:
		{
			if (HandleCommand(static_cast<const char *>(msg.mpData)))
			{
				return MR_TRUE;
			}
			return MR_FALSE;
		}
		case MT_EVENT:
		{ // In the real world, this part would live in a script. That seems a bit overkill for this example though.
			EventInfo *pEventInfo = static_cast<EventInfo*>(msg.mpData);
			if (pEventInfo->mEventName == CHash("StateChange") && pEventInfo->mTargetId == CObjectId("Laxative"))
			{
				Globals::PrintWithType(TTGameOver, "Oh no! The laxative has an explosive effect on your digestive system rending you hostage to your own bowels. You'll be lucky to see the outside of a lavatory for the next two weeks. Alas, there's no way you'll be able to make the GDC now. Your journey ends here.\n");
				Globals::GetTextAdventureEngine().EndGame();
			}
			else if (pEventInfo->mEventName == CHash("StateChange") && pEventInfo->mTargetId == CObjectId("WaterCooler"))
			{
				
			}
			return MR_TRUE;
		}
		case MT_TELL_ROOM:
		{
			TellRoomInfo *trInfo = static_cast<TellRoomInfo *>(msg.mpData);
			if (GetEntity()->GetPosition() == trInfo->room)
			{
				Globals::GetTextAdventureEngine().ClearInputStringDisplay();
				Globals::PrintWithType(TTTellRoom, "%s\n", trInfo->pMessage);
				Globals::GetTextAdventureEngine().DisplayInputString();
			}
			return MR_TRUE;
		}
	}
	return MR_IGNORED;
}

bool CCmpPlayer::HandleCommand(const char *commandString)
{
	char tmpStr[MAX_STR_LEN];
	strncpy(tmpStr, commandString, MAX_STR_LEN);
	char *pRemainder;
	char *pToken = Globals::GetFirstToken(tmpStr, " ", &pRemainder);
	if (pToken == NULL)
		return false;

	CHash command(pToken);

	if (command == CHash("Enter"))
	{
		ICmpRoom *pCurrentRoomInterface = static_cast<ICmpRoom *>(Globals::GetObjectManager().QueryInterface(GetEntity()->GetPosition(), IID_ROOM));

		if (pCurrentRoomInterface)
		{
			CObjectId destRoom = pCurrentRoomInterface->GetConnectedRoom(pRemainder);
			if (destRoom.IsValid())
			{
				GetEntity()->SetPosition(destRoom);

				EventInfo	evInfo(CHash("EnterRoom"), destRoom, GetObjectId());
				Globals::GetObjectManager().BroadcastMessage(CMessage(MT_EVENT, &evInfo));

				Globals::GetObjectManager().PostMessage(GetObjectId(), CMessage(MT_SET_INVENTORY_ITEM_POS, &destRoom));
				Globals::GetObjectManager().PostMessage(destRoom, MT_LOOK);
			}
			else
			{
				Globals::PrintWithType(TTResponse, "I don't know how to get to %s from here.\n", pRemainder);
			}
		}
		return true;
	}
	else if (command == CHash("Look"))
	{ // Send a look message to the room we're in
		Globals::GetObjectManager().PostMessage(GetEntity()->GetPosition(), MT_LOOK);
		return true;
	}
	else if (command == CHash("Help"))
	{
		Globals::PrintWithType(TTResponse, "Command list: Enter <room>, Look, Examine <item>, Get <item>, Use <item> [with <item>], Inventory\n");
		return true;
	}
	else if (command == CHash("Examine"))
	{
		CHash remainder(pRemainder);
		ExamineInfo exInfo;
		exInfo.examineObjectInteractionName = remainder;
		exInfo.userId = GetObjectId();
		Globals::GetObjectManager().BroadcastMessage(CMessage(MT_EXAMINE, &exInfo));
		return true;
	}
	else if (command == CHash("Take") || command == CHash("Get"))
	{
		PickupInfo pickupInfo;
		pickupInfo.collectedObjectInteractionName = CHash(pRemainder);
		pickupInfo.collectorId = GetObjectId();

		Globals::GetObjectManager().BroadcastMessage(CMessage(MT_BE_PICKED_UP, &pickupInfo));
		return true;
	}
	else if (command == CHash("Inventory"))
	{
		Globals::PrintWithType(TTInventoryHeading, "Inventory:\n");
		Globals::GetObjectManager().PostMessage(GetObjectId(), MT_EXAMINE_INVENTORY);
		return true;
	}
	else if (command == CHash("Use"))
	{
		char *pBefore;
		char *pAfter;
		char pToken[] = " with ";
		UseInfo useInfo;
		useInfo.userId = GetObjectId();
		if (Globals::SplitOnToken(pRemainder, pToken, &pBefore, &pAfter))
		{ // Use A with B
			useInfo.useObjectInteractionName = CHash(pAfter); // B
			useInfo.useWithInteractionName = CHash(pBefore); // A
			Globals::GetObjectManager().BroadcastMessage(CMessage(MT_USE, &useInfo));			
		}
		else
		{ // Use A
			useInfo.useObjectInteractionName = CHash(pRemainder);
			CHash invalidHash;
			useInfo.useWithInteractionName = invalidHash;
			Globals::GetObjectManager().BroadcastMessage(CMessage(MT_USE, &useInfo));
		}
		return true;
	}
	else if (command == CHash("Quit"))
	{
		Globals::GetTextAdventureEngine().EndGame();
		return true;
	}

	printf("Don't know how to: %s\n", commandString);
	return false;
}

EComponentTypeId CCmpPlayer::GetComponentTypeId(void)
{
	return CID_PLAYER;
}







