#include "CCmpInventory.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CParameterNode.h"
#include "CMessage.h"
#include "ICmpDescription.h"
#include "ICmpEntity.h"

CCmpInventory::CCmpInventory()
{
}

CCmpInventory::~CCmpInventory()
{
}

void CCmpInventory::RegisterComponentType()
{
	ICmpInventory::RegisterInterface(CID_INVENTORY);
	Globals::GetObjectManager().RegisterComponentType(CID_INVENTORY, CCmpInventory::CreateMe, CCmpInventory::DestroyMe, CHash("Inventory"));
	Globals::GetObjectManager().SubscribeToMessageType(CID_INVENTORY, MT_PICK_UP);
	Globals::GetObjectManager().SubscribeToMessageType(CID_INVENTORY, MT_EXAMINE_INVENTORY);
	Globals::GetObjectManager().SubscribeToMessageType(CID_INVENTORY, MT_SET_INVENTORY_ITEM_POS);
}

IComponent *CCmpInventory::CreateMe()
{
	return new CCmpInventory;
}

bool CCmpInventory::DestroyMe(IComponent *pComponent)
{
	delete pComponent;
	return true;
}

// Virtual IComponent methods
bool CCmpInventory::Init(CObjectId oid, CParameterNode &paramNode)
{

	return true;
}

void CCmpInventory::Deinit(void)
{
	mInventorySet.clear();
}

EMessageResult CCmpInventory::HandleMessage(const CMessage &msg)
{
	switch (msg.mType)
	{
		case MT_PICK_UP:
		{ // An object is telling us it wants to be picked up
			CObjectId pickupObject = *static_cast<CObjectId *>(msg.mpData);
			if (InsertItem(pickupObject))
			{ // Successfully picked up the object
				ICmpDescription *pPickupDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface(pickupObject, IID_DESCRIPTION));
				if (pPickupDesc)
					Globals::PrintWithType(TTResponse, "You picked up the %s.\n", pPickupDesc->GetShortDescription());
				else
					Globals::PrintWithType(TTResponse, "You picked up the object.\n");

				// Tell the object it has been picked up. Use a message so that any component in the picked up object gets a chance
				// to perform an action when this happens.
				CObjectId myObjectId = GetObjectId();
				Globals::GetObjectManager().PostMessage(pickupObject, CMessage(MT_PICK_UP_SUCCESSFUL, &myObjectId));
				return MR_TRUE;
			}
			return MR_FALSE;
		}
		case MT_EXAMINE_INVENTORY:
		{ // Print our inventory to the screen.
			ExamineInventory();
			return MR_TRUE;
		}
		case MT_SET_INVENTORY_ITEM_POS:
		{ // Set the positions of all the items in the inventory (to make them be at the same place as this object)
			CHash *newPos = static_cast<CHash *>(msg.mpData);
			if (newPos->IsValid())
				SetItemPositions(*newPos);
			return MR_TRUE;
		}
	}
	return MR_IGNORED;
}

void CCmpInventory::SetItemPositions(CHash newPos)
{
	for (InventorySet::const_iterator iter = mInventorySet.begin() ; iter != mInventorySet.end() ; ++iter)
	{
		ICmpEntity *pItemEntity = static_cast<ICmpEntity *>(Globals::GetObjectManager().QueryInterface((*iter), IID_ENTITY));
		pItemEntity->SetPosition(newPos);
	}
}

void CCmpInventory::ExamineInventory() const
{
	if (mInventorySet.size() == 0)
	{
		Globals::PrintWithType(TTInventoryItem, "The inventory is empty.\n");
		return;
	}
	for (InventorySet::const_iterator iter = mInventorySet.begin() ; iter != mInventorySet.end() ; ++iter)
	{
		ICmpDescription *pItemDesc = static_cast<ICmpDescription *>(Globals::GetObjectManager().QueryInterface((*iter), IID_DESCRIPTION));
		if (pItemDesc)
		{
			//printf("%s\n", pItemDesc->GetShortDescription());
			Globals::PrintWithType(TTInventoryItem, "%s\n", pItemDesc->GetShortDescription());
		}
	}
}

EComponentTypeId CCmpInventory::GetComponentTypeId(void)
{
	return CID_INVENTORY;
}

int32 CCmpInventory::GetNumItems() const
{
	return static_cast<int32>(mInventorySet.size());
}

CObjectId CCmpInventory::GetItem(int index) const
{
	CObjectId itemId;

	// Is index valid?
	if (index >= 0 && index < GetNumItems())
	{
		InventorySet::const_iterator iter = mInventorySet.begin();
		for (int i = 0 ; i < index ; ++i)
		{
			++iter;
		}
		
		itemId = (*iter);
	}
	return itemId;
}

bool CCmpInventory::IsItemInInventory(CObjectId itemId) const
{
	InventorySet::const_iterator iter;
	iter = mInventorySet.find(itemId);
	if (iter == mInventorySet.end())
		return false;
	return true;
}

bool CCmpInventory::InsertItem(CObjectId itemId)
{
	return mInventorySet.insert(itemId).second;
}

bool CCmpInventory::RemoveItem(CObjectId itemId)
{
	return (mInventorySet.erase(itemId) > 0);
}

