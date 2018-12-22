/*
  Name: RotateAnimDemo
  Date: 03/2008
  Description: Demonstrates animation with rotation and scaling
*/
#include "..\Engine\Advanced2D.h"

using namespace Advanced2D;

Sprite *asteroid;

bool game_preload() 
{
    g_engine->setAppTitle("SPRITE ANIMATE/ROTATE/SCALE DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(800);
    g_engine->setScreenHeight(600);
    g_engine->setColorDepth(32);
    return true;
}

bool game_init(HWND) 
{
    //load sprite
    asteroid = new Sprite();
    asteroid->loadImage("asteroid.tga");
    asteroid->setTotalFrames(64);
    asteroid->setColumns(8);
    asteroid->setSize(60,60);
    asteroid->setFrameTimer(30);

    return true;
}

void game_update() 
{
    static float scale = 0.005f;
    float r,s;

    //set position
    asteroid->setPosition(400,300);

    //set rotation
    asteroid->setRotation(timeGetTime()/600.0f);

    //set scale
    s = asteroid->getScale() + scale;
    if (s < 0.25 || s > 5.0f) scale *= -1;
    asteroid->setScale(s);


    //exit when escape key is pressed
    if (KEY_DOWN(VK_ESCAPE)) g_engine->Close();
}

void game_end() 
{
    delete asteroid;
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
}

void game_render2d()
{
    asteroid->animate();
    asteroid->draw();
}




