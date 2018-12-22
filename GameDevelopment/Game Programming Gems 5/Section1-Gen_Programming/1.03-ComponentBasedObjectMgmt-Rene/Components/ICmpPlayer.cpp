#include "ICmpPlayer.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpPlayer::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_PLAYER, compId);
}