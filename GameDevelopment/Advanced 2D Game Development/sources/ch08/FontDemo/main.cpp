/*
  Name: FontDemo
  Date: 03/2008
  Description: Demonstrates bitmapped font output
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define OBJECT_BACKGROUND 1
#define OBJECT_SPRITE 100
#define MAX 50

Sprite *panel;
Texture *asteroid_image;
Font *system12;
Font *nonprop;
Font *verdana10;

bool game_preload() 
{
    g_engine->setAppTitle("FONT DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(SCREENW);
    g_engine->setScreenHeight(SCREENH);
    g_engine->setColorDepth(32);    
    return 1;
}

bool game_init(HWND) 
{
    //load background image
    Sprite *background = new Sprite();
    background->loadImage("orion.bmp");
    background->setObjectType(999);
    g_engine->addEntity(background);

    //load asteroid image
    asteroid_image = new Texture();
    asteroid_image->Load("asteroid.tga");

    //create asteroid sprites
    Sprite *asteroid;
    for (int n=0; n < MAX; n++) 
    {
        //create a new asteroid sprite
        asteroid = new Sprite();
        asteroid->setObjectType(OBJECT_SPRITE);
        asteroid->setImage(asteroid_image);
        asteroid->setTotalFrames(64);
        asteroid->setColumns(8);
        asteroid->setSize(60,60);
        asteroid->setPosition( rand() % SCREENW, rand() % SCREENH );
        asteroid->setFrameTimer( rand() % 100 );
        asteroid->setCurrentFrame( rand() % 64 );
        if (rand()%2==0) asteroid->setAnimationDirection(-1);
        asteroid->setVelocity( (float)(rand()%10)/10.0f, (float)(rand()%10)/10.0f );
        //add asteroid to the entity manager
        g_engine->addEntity(asteroid);
    }

    //load the panel
    panel = new Sprite();
    panel->loadImage("panel.tga");
    float scale = SCREENW / 640.0f;
    panel->setScale(scale);
    panel->setColor(0xBBFFFFFF);


    //load the System12 font
    system12 = new Font();
    if (!system12->loadImage("system12.tga")) {
        g_engine->message("Error loading system12.tga");
        return false;
    }
    system12->setColumns(16);
    system12->setCharSize(14,16);

    if (!system12->loadWidthData("system12.dat")) {
        g_engine->message("Error loading system12.dat");
        return false;
    }

    //load System12 without proportional data
    nonprop = new Font();
    nonprop->loadImage("system12.tga");
    nonprop->setColumns(16);
    nonprop->setCharSize(14,16);

    //load the Verdana12 font
    verdana10 = new Font();
    if (!verdana10->loadImage("verdana10.tga")) {
        g_engine->message("Error loading verdana10.tga");
        return false;
    }
    verdana10->setColumns(16);
    verdana10->setCharSize(20,16);

    if (!verdana10->loadWidthData("verdana10.dat")) {
        g_engine->message("Error loading verdana10.dat");
        return false;
    }


    return true;
}


void game_update() 
{
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
    g_engine->SetIdentity();

}

void game_render2d()
{
    std::ostringstream os;
    std::string str;

    panel->draw();

    nonprop->Print(1,1,"This is the SYSTEM 12 font WITHOUT proportional data", 0xFF111111);
    nonprop->Print(1,20, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 0xFF111111);
    nonprop->Print(1,40,"abcdefghijklmnopqrstuvwxyz!@#$%^&*()_+{}|:<>?", 0xFF111111);

    system12->setScale(1.0f);
    system12->Print(1,80,"This is the SYSTEM 12 font WITH proportional data", 0xFF111111);
    system12->Print(1,100, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 0xFF111111);
    system12->Print(1,120,"abcdefghijklmnopqrstuvwxyz!@#$%^&*()_+{}|:<>?", 0xFF111111);

    for (float s=0.5f; s<2.0f; s+=0.25f) {
        verdana10->setScale( s );
        int x = (int)(s * 20);
        int y = (int)(100 + s * 120);
        os.str("");
        os << "VERDANA 10 font scaled at " << s*100 << "%";
        verdana10->Print(x,y, os.str(), 0xFF111111);
    }

    verdana10->setScale( 1.5f );
    verdana10->Print(600,140, g_engine->getVersionText(), 0xFF991111);

    os.str(""); 
    os << "SCREEN : " << (float)(1000.0f/g_engine->getFrameRate_real()) << " ms";
    verdana10->Print(600,180, os.str(), 0xFF119911);

    os.str("");
    os << "CORE : " << (float)(1000.0f/g_engine->getFrameRate_core()) << " ms";
    verdana10->Print(600,220,os.str(),0xFF119911);

}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
    switch(entity->getObjectType()) 
    {
        case OBJECT_SPRITE:
            Sprite* spr = (Sprite*)entity;
            if (spr->getX() < -60) spr->setX(SCREENW);
            if (spr->getX() > SCREENW) spr->setX(-60);
            if (spr->getY() < -60) spr->setY(SCREENH);
            if (spr->getY() > SCREENH) spr->setY(-60);
            break;
    }
}


void game_keyRelease(int key) 
{ 
    if (key == DIK_ESCAPE) g_engine->Close();
}

void game_end() 
{
    delete panel;
    delete asteroid_image;
    delete system12;
    delete nonprop;
    delete verdana10;
}


void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }
