/*
  Name: ScriptDemo
  Date: 04/2008
  Description: Demonstrates the use of Lua library
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

Font *font;
Script script;

std::string title;
int width;
int height;
int depth;
bool fullscreen;


bool game_preload() 
{
	script.loadScript("script.lua");
	title = script.getGlobalString("PROGRAMTITLE");
	width = (int)script.getGlobalNumber("SCREENWIDTH");
	height = (int)script.getGlobalNumber("SCREENHEIGHT");
	depth = (int)script.getGlobalNumber("COLORDEPTH");
	fullscreen = script.getGlobalBoolean("FULLSCREEN");

	g_engine->setAppTitle(title);
	g_engine->setScreenWidth(width);
	g_engine->setScreenHeight(height);
	g_engine->setColorDepth(depth);
	g_engine->setFullscreen(fullscreen);
	return 1;
}

bool game_init(HWND) 
{
	//load the Verdana10 font
	font = new Font();
	if (!font->loadImage("verdana10.tga")) {
		g_engine->message("Error loading verdana10.tga");
		return false;
	}
	if (!font->loadWidthData("verdana10.dat")) {
		g_engine->message("Error loading verdana10.dat");
		return false;
	}
	font->setColumns(16);
	font->setCharSize(20,16);

    return true;
}

void game_render2d()
{
	std::ostringstream ostr;

	font->Print(10,20,title);

	ostr << "Screen Width: " << width;
	font->Print(10,40,ostr.str());

	ostr.str("");
	ostr << "Screen Height: " << height;
	font->Print(10,60,ostr.str());

	ostr.str("");
	ostr << "Color Depth: " << depth;
	font->Print(10,80,ostr.str());

	ostr.str("");
	ostr << "Fullscreen: " << fullscreen;
	font->Print(10,100,ostr.str());

}

void game_end() 
{
	delete font;
}

void game_keyRelease(int key) 
{ 
	switch (key) {
		case DIK_ESCAPE:
			g_engine->Close();
			break;
	}
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
}

void game_update() { }
void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }
void game_entityUpdate(Advanced2D::Entity* entity) { }
void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2) { }
