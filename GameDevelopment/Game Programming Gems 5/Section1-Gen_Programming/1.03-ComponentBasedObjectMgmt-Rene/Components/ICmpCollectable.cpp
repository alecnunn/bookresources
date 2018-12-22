#include "ICmpCollectable.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpCollectable::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_COLLECTABLE, compId);
}