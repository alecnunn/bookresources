/*
  Name: InputDemo
  Date: 03/2008
  Description: Demonstrates keyboard and mouse input
*/
#include "..\Engine\Advanced2D.h"

using namespace Advanced2D;

ParticleEmitter *p;

Sprite *cursor;

bool game_preload() 
{
    g_engine->setAppTitle("INPUT DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(1024);
    g_engine->setScreenHeight(768);
    g_engine->setColorDepth(32);
    return 1;
}

bool game_init(HWND) 
{
    p = new ParticleEmitter();
    p->loadImage("particle16.tga");
    p->setMax(0);
    p->setAlphaRange(50,200);
    p->setDirection(0);
    p->setSpread(270);
    p->setScale(1.5f);
    p->setLength(2000);

    //load cursor
    cursor = new Sprite();
    cursor->loadImage("particle16.tga");

    return true;
}

void game_update()
{
    p->update();
}

void game_keyPress(int key)
{
}

void game_keyRelease(int key)
{
    if (key == DIK_ESCAPE) g_engine->Close();
}

void game_mouseButton(int button)
{
    switch(button) {
        case 0: //button 1
            p->setVelocity( (rand() % 10 - 5) / 500.0f );
            p->add();
            break;
    }
}

void game_mouseMotion(int x,int y)
{
}

void game_mouseMove(int x,int y)
{
    float fx = (float)x;
    float fy = (float)y;

    cursor->setPosition(fx,fy);

    p->setPosition(fx,fy);
}

void game_mouseWheel(int wheel)
{
}


void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,0));
}

void game_render2d()
{
    p->draw();
    cursor->draw();
}


void game_end() 
{
    delete p;
    delete cursor;
}



