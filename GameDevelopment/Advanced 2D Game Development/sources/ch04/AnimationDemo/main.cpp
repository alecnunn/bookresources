/*
  Name: AnimationDemo
  Date: 03/2008
  Description: Demonstrates sprite animation
*/
#include "..\Engine\Advanced2D.h"

using namespace Advanced2D;

Sprite *explosion;

bool game_preload() 
{
	g_engine->setAppTitle("SPRITE ANIMATION DEMO");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(800);
	g_engine->setScreenHeight(600);
	g_engine->setColorDepth(32);	
	return 1;
}

bool game_init(HWND) 
{
	//load sprite
	explosion = new Sprite();
	explosion->loadImage("explosion_30_128.tga");
	explosion->setTotalFrames(30);
	explosion->setColumns(6);
	explosion->setSize(128,128);
	explosion->setFrameTimer(40);

    return true;
}

void game_update() 
{
	int cx,cy;

	explosion->animate();
	if (explosion->getCurrentFrame() == explosion->getTotalFrames() - 1) 
	{
		//get new random location
		cx = rand()%(g_engine->getScreenWidth()-128);
		cy = rand()%(g_engine->getScreenHeight()-128);
		explosion->setPosition(cx,cy);
	}

	//exit when escape key is pressed
	if (KEY_DOWN(VK_ESCAPE)) g_engine->Close();
}

void game_end() 
{
	delete explosion;
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));

}

void game_render2d()
{


	explosion->draw();
}




