#include <windows.h>
#include "mytypes.h"
#include "syncdebug.h"

// This is the tiniest program ever, it simply produces an example syncreport.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SYNC_GAMETURN(0);
	SYNC_MSG("This is an example sync report\n");
	SYNC_MSG("Let's print out some numbers:");
	SYNC_INT(4);
	SYNC_MSG(0.2f);
	SYNC_CR();
	SYNC_GAMETURN(1);
	SYNC_MSG("And here goes another gameturn");
	SYNC_CR();
	SYNC_GAMETURN(2);

	// And let's output a coordinate using a shorthand that we've added to the system.
	M31 coordinate_of_some_sort = {.5f, .3f, .7f};
	SYNC_MSG(coordinate_of_some_sort);
	SYNC_CR();

	// Let's pretend the game's gone out of sync, we estimate it went out of sync in
	// gameturn 1. We'll ask for the data up to gameturn 3 which should also give
	// us a warning.

	// This will produce an out-of-sync report
	SyncDebug::OutputToFile(0, 3, 1);

	return 0;
}
