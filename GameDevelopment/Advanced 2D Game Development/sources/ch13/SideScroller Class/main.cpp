/*
  Name: BitmapScrolling
  Date: 04/2008
  Description: Demonstrates bitmap-based scrolling
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;
using namespace std;

#include "SideScroller.h"

Sprite *background;

SideScroller *scroller1;
SideScroller *scroller2;

double velocity=0.0;


bool game_preload() 
{
	g_engine->setAppTitle("SIDE SCROLLER");
	g_engine->setScreenWidth(800);
	g_engine->setScreenHeight(600);
	g_engine->setColorDepth(32);
	g_engine->setFullscreen(false);
	return true;
}

bool game_init(HWND) 
{
	//load the background
	background = new Sprite();
	if (!background->loadImage("space1.bmp")) {
		g_engine->message("Error loading background");
		return false;
	}

	//load layer 1
	scroller1 = new SideScroller();
	if (!scroller1->load("layer1.tga")) {
		g_engine->message("Error loading layer1");
		return false;
	}

	//load layer 2
	scroller2 = new SideScroller();
	if (!scroller2->load("layer2.tga")) {
		g_engine->message("Error loading layer2");
		return false;
	}


    return true;
}



void game_update() 
{ 
}

void game_render2d()
{
	background->draw();

	//draw layer 1
	scroller1->setViewport(0,0,800,600);
	scroller1->update(velocity);
	scroller1->setColor(0xDDFFFFFF);
	scroller1->draw();

	//draw layer 2
	scroller2->setViewport(0,0,800,600);
	scroller2->update(velocity * 2.0);
	scroller2->setColor(0x99FFFFFF);
	scroller2->draw();

}

void game_end() 
{
	delete background;
	delete scroller1;
	delete scroller2;
}

void game_keyPress(int key) 
{ 
	switch (key) {
		case DIK_LEFT:
			velocity -= 0.05;
			break;

		case DIK_RIGHT:
			velocity += 0.05;
			break;
	}

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

void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }
void game_entityUpdate(Advanced2D::Entity* entity) { }
void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2) { }
