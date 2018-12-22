#include "types.h"
#include "ICmpDescription.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpDescription::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_DESCRIPTION, compId);
}