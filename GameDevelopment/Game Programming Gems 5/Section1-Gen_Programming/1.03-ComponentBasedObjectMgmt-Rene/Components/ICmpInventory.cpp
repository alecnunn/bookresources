#include "ICmpInventory.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpInventory::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_INVENTORY, compId);
}