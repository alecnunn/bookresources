/*
  Name: ParticleDemo
  Date: 03/2008
  Description: Demonstrates a particle system using alpha channel
*/
#include "..\Engine\Advanced2D.h"

using namespace Advanced2D;

ParticleEmitter *pa;
ParticleEmitter *pb;
ParticleEmitter *pc;
ParticleEmitter *pd;
ParticleEmitter *pe;

bool game_preload() 
{
    g_engine->setAppTitle("PARTICLE DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(1024);
    g_engine->setScreenHeight(768);
    g_engine->setColorDepth(32);
    return 1;
}

bool game_init(HWND) 
{
    g_engine->setMaximizeProcessor(true);
    
    pa = new ParticleEmitter();
    pa->loadImage("particle16.tga");
    pa->setPosition(100,300);
    pa->setDirection(0);
    pa->setMax(500);
    pa->setAlphaRange(100,255);
    pa->setSpread(30);
    pa->setVelocity(2.0);
    pa->setLength(250);

    pb = new ParticleEmitter();
    pb->loadImage("particle16.tga");
    pb->setPosition(300,100);
    pb->setDirection(180);
    pb->setScale(0.6);
    pb->setMax(500);
    pb->setAlphaRange(0,100);
    pb->setColorRange(200,0,0,255,10,10);
    pb->setVelocity(2.0);
    pb->setSpread(40);
    pb->setLength(200);

    pc = new ParticleEmitter();
    pc->loadImage("particle16.tga");
    pc->setPosition(250,525);
    pc->setDirection(0);
    pc->setScale(0.5);
    pc->setMax(2000);
    pc->setAlphaRange(100,150);
    pc->setColorRange(0,0,200,10,10,255);
    pc->setVelocity(0.2);
    pc->setSpread(5);
    pc->setLength(180);

    pd = new ParticleEmitter();
    pd->loadImage("particle16.tga");
    pd->setPosition(750,650);
    pd->setScale(0.75);
    pd->setMax(10);
    pd->setAlphaRange(50,100);
    pd->setColorRange(210,50,0,255,255,1);
    pd->setVelocity(2.0);
    pd->setDirection(0);
    pd->setSpread(40);
    pd->setLength(100);

    pe = new ParticleEmitter();
    pe->loadImage("particle16.tga");
    pe->setPosition(730,575);
    pe->setScale(4.0f);
    pe->setMax(1000);
    pe->setAlphaRange(1,20);
    pe->setColorRange(250,250,250,255,255,255);
    pe->setVelocity(2.0);
    pe->setDirection(0);
    pe->setSpread(80);
    pe->setLength(800);

    return true;
}

void game_update()
{
    //move particles
    pa->update();
    
    pb->update();

    //update the circular emitter
    float dir = pc->getDirection() + 0.2f;
    pc->setDirection(dir);
    pc->update();

    //update the rotating emitter
    static double unit = 3.1415926535 / 36000.0;
    static double angle = 0.0;
    static double radius = 150.0;
    angle += unit;
    if (angle > 360) angle = 360 - angle;
    float x = 750 + cos(angle) * radius;
    float y = 500 + sin(angle) * radius;
    pd->setPosition(x,y);
    pd->update();

    //update smoke emitter
    pe->setPosition(x,y);
    pe->update();
    
    //exit when escape key is pressed
    if (KEY_DOWN(VK_ESCAPE)) g_engine->Close();
}

void game_end() 
{
    delete pa;
    delete pb;
    delete pc;
    delete pd;
    delete pe;
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,0));
}

void game_render2d()
{
    pa->draw();
    pb->draw();
    pc->draw();
    pd->draw();
    pe->draw();
}




