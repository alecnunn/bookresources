#include <iostream>
#include "..\Engine\Advanced2D.h"

int game_preload() 
{
	//display engine version in a message box
	g_engine->message(g_engine->getVersionText(), "TEST ENGINE");
	
	//return fail to terminate the engine
	return 0;
}

int game_init(HWND) { return 0;}
void game_update() {}
void game_end() {}
