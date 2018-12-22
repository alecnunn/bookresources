#include "ICmpRoom.h"

#include "Globals.h"
#include "CObjectManager.h"

void ICmpRoom::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_ROOM, compId);
}