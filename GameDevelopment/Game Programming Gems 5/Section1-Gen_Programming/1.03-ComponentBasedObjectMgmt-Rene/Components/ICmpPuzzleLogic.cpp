#include "ICmpPuzzleLogic.h"
#include "Globals.h"
#include "CObjectManager.h"

void ICmpPuzzleLogic::RegisterInterface(EComponentTypeId compId)
{
	Globals::GetObjectManager().RegisterInterfaceWithComponent(IID_PUZZLE_LOGIC, compId);
}