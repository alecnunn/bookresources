#include "ICmpEntity.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpEntity::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_ENTITY, compId);
}