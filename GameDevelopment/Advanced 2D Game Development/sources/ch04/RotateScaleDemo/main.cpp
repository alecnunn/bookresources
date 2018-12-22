/*
  Name: RotateScaleDemo
  Date: 03/2008
  Description: Demonstrates sprite rotation and scaling
*/
#include "..\Engine\Advanced2D.h"

using namespace Advanced2D;

Sprite *ship;

bool game_preload() 
{
	g_engine->setAppTitle("SPRITE ROTATION AND SCALING DEMO");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(800);
	g_engine->setScreenHeight(600);
	g_engine->setColorDepth(32);	
	return 1;
}

bool game_init(HWND) 
{
	//load sprite
	ship = new Sprite();
	ship->loadImage("fatship.tga");
	ship->setSize(640,480);
    return true;
}

void game_update() 
{
	static float scale = 0.01f;
	float r,s;

	//set position
	ship->setPosition(400,300);

	//set rotation
	ship->setRotation(timeGetTime()/600.0f);

	//set scale
	s = ship->getScale() + scale;
	if (s < 0.01 || s > 1.5f) scale *= -1;
	ship->setScale(s);


	//exit when escape key is pressed
	if (KEY_DOWN(VK_ESCAPE)) g_engine->Close();
}

void game_end() 
{
	delete ship;
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
    g_engine->SetIdentity();
}

void game_render2d()
{
	ship->draw();
}




