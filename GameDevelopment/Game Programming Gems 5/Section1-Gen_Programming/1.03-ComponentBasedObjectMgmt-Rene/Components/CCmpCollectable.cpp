#include "CCmpCollectable.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "ICmpInventory.h"
#include "ICmpDescription.h"
#include "ICmpEntity.h"
#include "ComponentStructs.h"
#include "stdio.h"

CCmpCollectable::CCmpCollectable()
{
}

CCmpCollectable::~CCmpCollectable()
{
}

void CCmpCollectable::RegisterComponentType()
{
	ICmpCollectable::RegisterInterface(CID_COLLECTABLE);
	Globals::GetObjectManager().RegisterComponentType(CID_COLLECTABLE, CCmpCollectable::CreateMe, CCmpCollectable::DestroyMe, CHash("Collectable"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_COLLECTABLE, MT_BE_PICKED_UP);
	Globals::GetObjectManager().SubscribeToMessageType(CID_COLLECTABLE, MT_PICK_UP_SUCCESSFUL);
}

IComponent *CCmpCollectable::CreateMe()
{
	return new CCmpCollectable;
}

bool CCmpCollectable::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpCollectable::Init(CObjectId oid, CParameterNode &paramNode)
{
	return true;
}

void CCmpCollectable::Deinit(void)
{
}

EMessageResult CCmpCollectable::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_BE_PICKED_UP:
		{
			PickupInfo	*pPickupInfo = static_cast<PickupInfo *>(msg.mpData);
			if (GetEntity()->IsInteractionName(pPickupInfo->collectedObjectInteractionName))
			{ // You talkin' to me? You talkin' to me? Yes you are...
				// Can this object be picked up?
				CObjectId collectorId = pPickupInfo->collectorId;
				ICmpDescription *pDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(GetObjectId(), IID_DESCRIPTION)); 
				if (IsCollected())
				{ // It's already collected, so we won't be picking it up.
					if (GetHolder() == collectorId)
					{
						if (pDesc == NULL)
							Globals::PrintWithType(TTResponse, "You've already got that object in your inventory.\n");
						else
							Globals::PrintWithType(TTResponse,"You've already got the %s in your inventory.\n", pDesc->GetShortDescription());
						return MR_TRUE;
					}
					else
					{
						if (pDesc == NULL)
							Globals::PrintWithType(TTResponse,"You can't pick up that object.\n");
						else
							Globals::PrintWithType(TTResponse,"You can't pick up the %s.\n", pDesc->GetShortDescription());
						return MR_FALSE;
					}
				}
				// The object isn't collected. Can it be seen by the collector
				if (GetEntity()->CanThisObjectBeSeenBy(collectorId, true))
				{ // Go and be picked up. There's nothing stopping us now.
					CObjectId myObjectId = GetObjectId();
					Globals::GetObjectManager().PostMessage(collectorId, CMessage(MT_PICK_UP, &myObjectId));
					Globals::GetObjectManager().PostMessage(GetObjectId(), MT_HIDE);
					return MR_TRUE;
				}
				else
				{
					if (pDesc == NULL)
						Globals::PrintWithType(TTResponse,"You can't see that object.\n");
					else
						Globals::PrintWithType(TTResponse,"You can't see the %s from here.\n", pDesc->GetShortDescription());
					return MR_FALSE;
				}
			}
			return MR_FALSE; // If we succeeded, we should have returned already
		}
		case MT_PICK_UP_SUCCESSFUL:
		{ // Make sure we know that we're being held (and who's doing the holding)
			mHolder = *static_cast<CObjectId *>(msg.mpData);
			return MR_TRUE;
		}
	}
	return MR_IGNORED;
}

EComponentTypeId CCmpCollectable::GetComponentTypeId(void)
{
	return CID_COLLECTABLE;
}

bool CCmpCollectable::IsCollected() const
{
	return mHolder.IsValid();
}

CObjectId CCmpCollectable::GetHolder() const
{
	return mHolder;
}