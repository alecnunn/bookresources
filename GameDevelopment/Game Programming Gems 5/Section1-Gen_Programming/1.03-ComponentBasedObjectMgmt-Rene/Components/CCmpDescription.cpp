#include "CCmpDescription.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include <string.h>
#include "CMessage.h"
#include "ComponentStructs.h"
#include "ICmpEntity.h"
#include <stdio.h>

CCmpDescription::CCmpDescription()
{
}

CCmpDescription::~CCmpDescription()
{
}

void CCmpDescription::RegisterComponentType()
{
	ICmpDescription::RegisterInterface(CID_DESCRIPTION);
	Globals::GetObjectManager().RegisterComponentType(CID_DESCRIPTION, CCmpDescription::CreateMe, CCmpDescription::DestroyMe, CHash("Description"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_DESCRIPTION, MT_EXAMINE);
}

IComponent *CCmpDescription::CreateMe()
{
	return new CCmpDescription;
}

bool CCmpDescription::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpDescription::Init(CObjectId oid, CParameterNode &paramNode)
{
	strncpy(mShortDesc, paramNode.GetString("ShortDesc"), MAX_STR_LEN);
	strncpy(mLongDesc, paramNode.GetString("LongDesc"), MAX_STR_LEN);
	return true;
}

void CCmpDescription::Deinit(void)
{
}

EMessageResult CCmpDescription::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_EXAMINE:
		{
			ExamineInfo *examineInfo = static_cast<ExamineInfo *>(msg.mpData);
			if (GetEntity()->IsInteractionName(examineInfo->examineObjectInteractionName))
			{ // The user is looking at me. Check if this object can be seen by the user
				if (GetEntity()->CanThisObjectBeSeenBy(examineInfo->userId, true))
				{
					ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION));
					if (pDesc)
					{
						Globals::PrintWithType(TTInventoryItem, "%s\n", pDesc->GetLongDescription());
					}
				}
			}
			return MR_TRUE;
		}
	}

	return MR_IGNORED;
}

EComponentTypeId CCmpDescription::GetComponentTypeId(void)
{
	return CID_DESCRIPTION;
}

void CCmpDescription::SetShortDescription(const char *desc)
{
	strncpy(mShortDesc, desc, MAX_STR_LEN);
}

void CCmpDescription::SetLongDescription(const char *desc)
{
	strncpy(mLongDesc, desc, MAX_STR_LEN);
}

const char *CCmpDescription::GetShortDescription() const
{
	return mShortDesc;
}

const char *CCmpDescription::GetLongDescription() const
{
	return mLongDesc;
}
