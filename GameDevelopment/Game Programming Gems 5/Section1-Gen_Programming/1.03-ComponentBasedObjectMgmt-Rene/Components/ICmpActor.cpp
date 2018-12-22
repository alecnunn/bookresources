#include "ICmpActor.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpActor::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_ACTOR, compId);
}