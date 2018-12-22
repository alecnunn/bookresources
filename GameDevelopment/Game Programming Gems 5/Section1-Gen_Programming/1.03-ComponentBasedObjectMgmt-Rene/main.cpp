#include "types.h"
#include "CTextAdventureEngine.h"
#include "Globals.h"

int main(int argc, char *argv[])
{
	CTextAdventureEngine &engine = Globals::GetTextAdventureEngine();

	engine.Init();
	engine.RunGame();
	engine.Deinit();

	return 0;
}

