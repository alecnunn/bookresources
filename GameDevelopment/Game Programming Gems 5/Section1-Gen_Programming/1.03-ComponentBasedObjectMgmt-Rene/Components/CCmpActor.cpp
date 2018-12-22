#include "CCmpActor.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "ICmpDescription.h"
#include "ICmpEntity.h"
#include "ICmpPuzzleLogic.h"
#include "ComponentStructs.h"
#include "CTextAdventureEngine.h"
#include "CTimer.h"
#include <stdio.h>

CCmpActor::CCmpActor()
{
}

CCmpActor::~CCmpActor()
{
}

void CCmpActor::RegisterComponentType()
{
	ICmpActor::RegisterInterface(CID_ACTOR);
	Globals::GetObjectManager().RegisterComponentType(CID_ACTOR, CCmpActor::CreateMe, CCmpActor::DestroyMe, CHash("Actor"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_ACTOR, MT_DESCRIBE_CHARACTER);
	Globals::GetObjectManager().SubscribeToMessageType(CID_ACTOR, MT_EVENT);
	Globals::GetObjectManager().SubscribeToMessageType(CID_ACTOR, MT_UPDATE);
}

IComponent *CCmpActor::CreateMe()
{
	return new CCmpActor;
}

bool CCmpActor::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpActor::Init(CObjectId oid, CParameterNode &paramNode)
{
	mBossState = IN_OFFICE;
	mThermostatHot = false;
	return true;
}

void CCmpActor::Deinit(void)
{
}

EMessageResult CCmpActor::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_EVENT:
		{
			EventInfo evInfo = *static_cast<EventInfo*>(msg.mpData);
			HandleEvent(evInfo);
			return MR_TRUE;
		}
		case MT_UPDATE:
		{
			Update();
			return MR_TRUE;
		}
		case MT_DESCRIBE_CHARACTER:
		{
			CHash containingObject = *static_cast<CHash *>(msg.mpData);
			DescribeCharacter(containingObject);
			return MR_TRUE;
		}
	}
	return MR_IGNORED;
}

// In the real world, CCmpActor would obviously not be so hard coded, this will do fine for this example though.
void CCmpActor::Update()
{
	const uint32 currentTime = Globals::GetTimer().GetSeconds();
	switch (mBossState)
	{
		case IN_OFFICE:
		{
			// The thermostat being hot sets off the sequence of events
			if (mThermostatHot)
			{
				mBossState = ON_WAY_TO_COOLER;
				mNextStateTime = currentTime + 5;
			}
			break;
		}
		case ON_WAY_TO_COOLER:
		{
			if (currentTime >= mNextStateTime)
			{
				GotoRoom("Corridor");
				// Next state
				mBossState = DRINK;
				mNextStateTime = currentTime + 2;
			}
			break;
		}
		case DRINK:
		{
			if (currentTime >= mNextStateTime)
			{
				ICmpPuzzleLogic *pWaterCoolerPuzzle = static_cast<ICmpPuzzleLogic *>(Globals::GetObjectManager().QueryInterface(CObjectId("WaterCooler"), IID_PUZZLE_LOGIC));
				if (pWaterCoolerPuzzle)
				{
					Globals::PrintWithType(TTEvent,"The boss takes a drink from the water cooler.\n");
					if (pWaterCoolerPuzzle->GetState() == CHash("normal"))
					{
						mBossState = ON_WAY_TO_THERMOSTAT;
						mNextStateTime = currentTime + 5;
					}
					else if (pWaterCoolerPuzzle->GetState() == CHash("LaxativeInWater"))
					{
						Globals::PrintWithType(TTGameOver, "With a walk like John Wayne, the boss hurriedly makes his way into the toilet. By the sounds of it, he'll be occupied for quite some time. You now have ample opportunity to make that GDC pass yours! Well done!\n");
						Globals::GetTextAdventureEngine().EndGame();
					}
				}
			}
			break;
		}
		case ON_WAY_TO_THERMOSTAT:
		{
			if (currentTime >= mNextStateTime)
			{
				GotoRoom("ProgrammingRoom");
				// Next state
				mBossState = SET_THERMOSTAT;
				mNextStateTime = currentTime + 1;
				TellRoom("Boss: Who turned this thermostat up? It's way too hot in here.");
			}
			break;
		}
		case SET_THERMOSTAT:
		{
			if (currentTime >= mNextStateTime)
			{
				ICmpPuzzleLogic *pThermoPuzzle = static_cast<ICmpPuzzleLogic *>(Globals::GetObjectManager().QueryInterface("Thermostat", IID_PUZZLE_LOGIC));
				if (pThermoPuzzle)
				{
					pThermoPuzzle->SetState("normal", true);
					Globals::PrintWithType(TTEvent, "The boss sets the thermostat back to normal.\n");
					mBossState = ON_WAY_BACK_THROUGH_CORRIDOR;
					mNextStateTime = currentTime + 2;
				}
			}
			break;
		}
		case ON_WAY_BACK_THROUGH_CORRIDOR:
		{
			if (currentTime >= mNextStateTime)
			{
				GotoRoom("Corridor");
				mBossState = ON_WAY_TO_OFFICE;
				mNextStateTime = currentTime + 2;			
			}
			break;
		}
		case ON_WAY_TO_OFFICE:
		{
			if (currentTime >= mNextStateTime)
			{
				GotoRoom("BossOffice");
				mBossState = IN_OFFICE;
			}
			break;
		}
	}
}

// In the real world, this would be written in some kind of scripting language. For this little example game, this does nicely.
void CCmpActor::HandleEvent(EventInfo &evInfo)
{
	if (evInfo.mEventName == CHash("StateChange"))
	{
		if (evInfo.mTargetId == CObjectId("Thermostat"))
		{
			ICmpPuzzleLogic *pThermoPuzzle = static_cast<ICmpPuzzleLogic *>(Globals::GetObjectManager().QueryInterface(evInfo.mTargetId, IID_PUZZLE_LOGIC));
			if (pThermoPuzzle)
			{ // Go get a drink
				if (pThermoPuzzle->GetState() == CHash("hot"))
				{
					mThermostatHot = true;
				}
				else if (pThermoPuzzle->GetState() == CHash("normal"))
				{
					mThermostatHot = false;
				}
			}
		}
	}
}

void CCmpActor::DescribeCharacter(CObjectId viewer) const
{
	ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
	if (pDesc == NULL)
	{ // We have nothing to describe
		return;
	}
	if (GetEntity()->CanThisObjectBeSeenBy(viewer, false))
	{
		Globals::PrintWithType(TTCharacter, "%s\n", pDesc->GetShortDescription());
	}
}

EComponentTypeId CCmpActor::GetComponentTypeId(void)
{
	return CID_ACTOR;
}

void CCmpActor::TellRoom(const char *pMsg)
{
	TellRoomInfo trInfo;
	trInfo.room = GetEntity()->GetPosition();
	trInfo.pMessage = pMsg;
	Globals::GetObjectManager().BroadcastMessage(CMessage(MT_TELL_ROOM, &trInfo));
}

void CCmpActor::GotoRoom(CObjectId room)
{
	char tmpStr[MAX_STR_LEN];
	ICmpDescription *pEnteredRoomDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(room, IID_DESCRIPTION));
	if (pEnteredRoomDesc)
	{
		sprintf(tmpStr, "The boss goes to the %s.", pEnteredRoomDesc->GetShortDescription());
		TellRoom(tmpStr);
	}
	ICmpDescription *pLeftRoomDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetEntity()->GetPosition(), IID_DESCRIPTION));

	GetEntity()->SetPosition(room);
	if (pLeftRoomDesc)
	{
		sprintf(tmpStr, "The boss enters from the %s.", pLeftRoomDesc->GetShortDescription());
		TellRoom(tmpStr);
	}
}